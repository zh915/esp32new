/**
 ****************************************************************************************************
 * @file        wavplay.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-12-01
 * @brief       wav解码 代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 ESP32-S3 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "wavplay.h"
/*FreeRTOS*********************************************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/******************************************************************************************************/
/*FreeRTOS配置*/

/* MUSIC 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define MUSIC_PRIO      4                   /* 任务优先级 */
#define MUSIC_STK_SIZE  5*1024              /* 任务堆栈大小 */
TaskHandle_t            MUSICTask_Handler;  /* 任务句柄 */
void music(void *pvParameters);             /* 任务函数 */

static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;

/******************************************************************************************************/

__wavctrl wavctrl;                          /* WAV控制结构体 */


esp_err_t i2s_play_end = ESP_FAIL;
esp_err_t i2s_play_next_prev = ESP_FAIL;

/**
 * @brief       WAV解析初始化
 * @param       fname : 文件路径+文件名
 * @param       wavx  : 信息存放结构体指针
 * @retval      0,打开文件成功
 *              1,打开文件失败
 *              2,非WAV文件
 *              3,DATA区域未找到
 */
uint8_t wav_decode_init(uint8_t *fname, __wavctrl *wavx)
{
    FIL *ftemp;
    uint8_t *buf; 
    uint32_t br = 0;
    uint8_t res = 0;

    ChunkRIFF *riff;
    ChunkFMT *fmt;
    ChunkFACT *fact;
    ChunkDATA *data;
    
    ftemp = (FIL*)malloc(sizeof(FIL));
    buf = malloc(512);
    
    if (ftemp && buf)                                           /* 内存申请成功 */
    {
        res = f_open(ftemp, (TCHAR*)fname, FA_READ);            /* 打开文件 */
        
        if (res == FR_OK)
        {
            f_read(ftemp, buf, 512, (UINT *)&br);               /* 读取512字节在数据 */
            riff = (ChunkRIFF *)buf;                            /* 获取RIFF块 */
            
            if (riff->Format == 0x45564157)                     /* 是WAV文件 */
            {
                fmt = (ChunkFMT *)(buf + 12);                   /* 获取FMT块 */
                fact = (ChunkFACT *)(buf + 12 + 8 + fmt->ChunkSize);                    /* 读取FACT块 */
                
                if (fact->ChunkID == 0x74636166 || fact->ChunkID == 0x5453494C)
                {
                    wavx->datastart = 12 + 8 + fmt->ChunkSize + 8 + fact->ChunkSize;    /* 具有fact/LIST块的时候(未测试) */
                }
                else
                {
                    wavx->datastart = 12 + 8 + fmt->ChunkSize;
                }
                
                data = (ChunkDATA *)(buf + wavx->datastart);    /* 读取DATA块 */
                
                if (data->ChunkID == 0x61746164)                /* 解析成功! */
                {
                    wavx->audioformat = fmt->AudioFormat;       /* 音频格式 */
                    wavx->nchannels = fmt->NumOfChannels;       /* 通道数 */
                    wavx->samplerate = fmt->SampleRate;         /* 采样率 */
                    wavx->bitrate = fmt->ByteRate * 8;          /* 得到位速 */
                    wavx->blockalign = fmt->BlockAlign;         /* 块对齐 */
                    wavx->bps = fmt->BitsPerSample;             /* 位数,16/24/32位 */
                    
                    wavx->datasize = data->ChunkSize;           /* 数据块大小 */
                    wavx->datastart = wavx->datastart + 8;      /* 数据流开始的地方. */
                     
                    printf("wavx->audioformat:%d\r\n", wavx->audioformat);
                    printf("wavx->nchannels:%d\r\n", wavx->nchannels);
                    printf("wavx->samplerate:%ld\r\n", wavx->samplerate);
                    printf("wavx->bitrate:%ld\r\n", wavx->bitrate);
                    printf("wavx->blockalign:%d\r\n", wavx->blockalign);
                    printf("wavx->bps:%d\r\n", wavx->bps);
                    printf("wavx->datasize:%ld\r\n", wavx->datasize);
                    printf("wavx->datastart:%ld\r\n", wavx->datastart);  
                }
                else
                {
                    res = 3;                                    /* data区域未找到. */
                }
            }
            else
            {
                res = 2;        /* 非wav文件 */
            }
        }
        else
        {
            res = 1;            /* 打开文件错误 */
        }
    }
    
    f_close(ftemp);             /* 关闭文件 */
    free(ftemp);                /* 释放内存 */
    free(buf); 
    
    return 0;
}

/**
 * @brief       获取当前播放时间
 * @param       fx    : 文件指针
 * @param       wavx  : wavx播放控制器
 * @retval      无
 */
void wav_get_curtime(FIL *fx, __wavctrl *wavx)
{
    long long fpos;

    wavx->totsec = wavx->datasize / (wavx->bitrate / 8);    /* 歌曲总长度(单位:秒) */
    fpos = fx->fptr-wavx->datastart;                        /* 得到当前文件播放到的地方 */
    wavx->cursec = fpos * wavx->totsec / wavx->datasize;    /* 当前播放到第多少秒了? */
}

/**
 * @brief       music任务
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void music(void *pvParameters)
{
    pvParameters = pvParameters;
    uint32_t nr;
    FSIZE_t res = 0;
    uint8_t n = 0;
    volatile long long int i2s_table_size = 0;

    while(1)
    {
        if ((g_audiodev.status & 0x0F) == 0x03)
        {
            f_lseek(g_audiodev.file, n ? res : wavctrl.datastart);                          /* 跳过文件头 */

            for (uint16_t readTimes = 0; readTimes < (wavctrl.datasize / WAV_TX_BUFSIZE); readTimes++)
            {
                if ((g_audiodev.status & 0x0F) == 0x00)                                     /* 暂停播放 */
                {
                    res = f_tell(g_audiodev.file);                                          /* 记录暂停位置 */
                    n = 1;
                    break;
                }

                if (i2s_table_size >= wavctrl.datasize || i2s_play_next_prev == ESP_OK)   /* 是否播放完成 */
                {
                    n = 0;
                    i2s_table_size = 0;
                    i2s_play_end = ESP_OK;
                    audio_stop();
                    vTaskDelay(10);
                    break;
                }

                f_read(g_audiodev.file,g_audiodev.tbuf, WAV_TX_BUFSIZE, (UINT*)&nr);        /* 读文件 */
                i2s_table_size = i2s_table_size + i2s_tx_write(g_audiodev.tbuf, WAV_TX_BUFSIZE);
                vTaskDelay(1);
            }

        }
        else
        {
            vTaskDelay(10);
        }
    }
}

/**
 * @brief       播放某个wav文件
 * @param       fname : 文件路径+文件名
 * @retval      KEY0_PRES,错误
 *              KEY1_PRES,打开文件失败
 *              其他,非WAV文件
 */
uint8_t wav_play_song(uint8_t *fname)
{
    uint8_t key = 0;
    uint8_t t = 0;
    uint8_t res;
    i2s_play_end = ESP_FAIL;
    i2s_play_next_prev = ESP_FAIL;
    g_audiodev.file = (FIL*)malloc(sizeof(FIL));
    g_audiodev.tbuf = malloc(WAV_TX_BUFSIZE);
    
    if (g_audiodev.file || g_audiodev.tbuf)
    {
        res = wav_decode_init(fname, &wavctrl);                                 /* 得到文件的信息 */

        if (res == 0)                                                           /* 解析文件成功 */
        {
            if (wavctrl.bps == 16)
            {
                es8388_sai_cfg(0, 3);                                           /* 飞利浦标准,16位数据长度 */
                i2s_set_samplerate_bits_sample(wavctrl.samplerate,I2S_BITS_PER_SAMPLE_16BIT);
            }
            else if (wavctrl.bps == 24)
            {
                es8388_sai_cfg(0, 0);                                           /* 飞利浦标准,24位数据长度 */
                i2s_set_samplerate_bits_sample(wavctrl.samplerate,I2S_BITS_PER_SAMPLE_24BIT);
             }

            audio_stop();

            if (MUSICTask_Handler == NULL)
            {
                taskENTER_CRITICAL(&my_spinlock);
                /* 创建任务1 */
                xTaskCreatePinnedToCore((TaskFunction_t )music,                 /* 任务函数 */
                                        (const char*    )"music",               /* 任务名称 */
                                        (uint16_t       )MUSIC_STK_SIZE,        /* 任务堆栈大小 */
                                        (void*          )NULL,                  /* 传入给任务函数的参数 */
                                        (UBaseType_t    )MUSIC_PRIO,            /* 任务优先级 */
                                        (TaskHandle_t*  )&MUSICTask_Handler,    /* 任务句柄 */
                                        (BaseType_t     ) 0);                   /* 该任务哪个内核运行 */
                taskEXIT_CRITICAL(&my_spinlock);
            }

            res = f_open(g_audiodev.file, (TCHAR*)fname, FA_READ);              /* 打开文件 */

            if (res == 0)
            {
                audio_start();                                                  /* 开始音频播放 */
                vTaskDelay(100);
                audio_start();
                vTaskDelay(100);

                while (res == 0)
                { 
                    while (1)
                    {
                        if (i2s_play_end == ESP_OK)
                        {
                            res = KEY0_PRES;
                            break;
                        }

                        key = xl9555_key_scan(0);
                        
                        if (key == KEY3_PRES)                                   /* 暂停 */
                        {
                            if ((g_audiodev.status & 0x0F) == 0x03)
                            {
                                audio_stop();
                                vTaskDelay(100);
                            }
                            else if ((g_audiodev.status & 0x0F) == 0x00)
                            {
                                audio_start();
                                vTaskDelay(100);
                            }
                        }
                        
                        if (key == KEY2_PRES || key == KEY0_PRES)               /* 下一曲/上一曲 */
                        {
                            i2s_play_next_prev = ESP_OK;
                            vTaskDelay(100);
                            res = KEY0_PRES;
                            break;
                        }
                        
                        if ((g_audiodev.status & 0x0F) == 0x03)                 /* 暂停不刷新时间 */
                        {
                            wav_get_curtime(g_audiodev.file, &wavctrl);         /* 得到总时间和当前播放的时间 */
                            audio_msg_show(wavctrl.totsec, wavctrl.cursec, wavctrl.bitrate);
                        }
                        
                        t++;
                        if (t == 20)
                        {
                            t = 0 ;
                            LED_TOGGLE();
                        }
                        
                        if ((g_audiodev.status & 0x01) == 0)
                        {
                            vTaskDelay(10);
                        }
                        else
                        {
                            break;
                        }
                    }

                    if (key == KEY2_PRES || key == KEY0_PRES)                   /* 退出切换歌曲 */
                    {
                        res = key;
                        break;
                    }
                }
                audio_stop();
            }
            else
            {
                res = 0xFF;
            }
        }
        else
        {
            res = 0xFF;
        }
    }
    else
    {
        res = 0xFF;
    }
    
    free(g_audiodev.tbuf);                                                      /* 释放内存 */
    free(g_audiodev.file);                                                      /* 释放内存 */
    
    return res;
}
