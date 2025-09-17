/**
 ****************************************************************************************************
 * @file        videoplay.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2023-12-01
 * @brief       ��Ƶ������ Ӧ�ô���
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ESP32-S3 ������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "videoplay.h"


extern uint8_t frameup;

/**
 * @brief       ��ȡָ��·������Ч��Ƶ�ļ�������
 * @param       path: ָ��·��
 * @retval      ��Ч��Ƶ�ļ�������
 */
static uint16_t video_get_tnum(char *path)
{
    uint8_t res;
    uint16_t rval = 0;
    FF_DIR tdir;
    FILINFO *tfileinfo;
    
    tfileinfo = (FILINFO *)malloc(sizeof(FILINFO));             /* �����ڴ� */
    res = (uint8_t)f_opendir(&tdir, (const TCHAR *)path);       /* ��Ŀ¼ */
    
    if ((res == 0) && tfileinfo)
    {
        while (1)                                               /* ��ѯ�ܵ���Ч�ļ��� */
        {
            res = (uint8_t)f_readdir(&tdir, tfileinfo);         /* ��ȡĿ¼�µ�һ���ļ� */
            
            if ((res != 0) || (tfileinfo->fname[0] == 0))       /* �����ĩβ���˳� */
            {
                break;
            }
            
            res = exfuns_file_type(tfileinfo->fname);
            
            if ((res & 0xF0) == 0x60)                           /* ����Ƶ�ļ� */
            {
                /* ����Ƿ�Ϊ.avi�ļ���ֻ��.avi�ļ��ż��� */
                char *ext = strrchr(tfileinfo->fname, '.');
                if (ext && (strcmp(ext, ".avi") == 0 || strcmp(ext, ".AVI") == 0))
                {
                    rval++;
                }
            }
        }
    }
    
    free(tfileinfo);                                            /* �ͷ��ڴ� */
    
    return rval;
}

/**
 * @brief       ��ʾ��Ƶ������Ϣ
 * @param       name : ��Ƶ����
 * @param       index: ��ǰ����
 * @param       total: ���ļ���
 * @retval      ��
 */
static void video_bmsg_show(uint8_t *name, uint16_t index, uint16_t total)
{
    uint8_t *buf;
    
    buf = malloc(100);                                                /* ����100�ֽ��ڴ� */
    
    sprintf((char *)buf, "�ļ���:%s", name);
//    text_show_string(10, 10, lcd_self.width - 10, 16, (char *)buf, 16, 0, RED);   /* ��ʾ�ļ��� */
    
    sprintf((char *)buf, "����:%d/%d", index, total);
//    text_show_string(10, 30, lcd_self.width - 10, 16, (char *)buf, 16, 0, RED);   /* ��ʾ���� */
    
    free(buf);                                                        /* �ͷ��ڴ� */
}

/**
 * @brief       ��ʾ��ǰ��Ƶ�ļ��������Ϣ
 * @param       aviinfo: avi���ƽṹ��
 * @retval      ��
 */
// static void video_info_show(AVI_INFO *aviinfo)
// {
//     uint8_t *buf;
    
//     buf = malloc(100);                                                /* ����100�ֽ��ڴ� */
    
//     sprintf((char *)buf, "������:%d,������:%ld", aviinfo->Channels, aviinfo->SampleRate * 10);
//     text_show_string(10, 10, lcd_self.width - 10, 16, (char *)buf, 16, 0, RED);   /* ��ʾ�������� */
    
//     sprintf((char *)buf, "֡��:%ld֡", 1000 / (aviinfo->SecPerFrame / 1000));
//     text_show_string(10, 30, lcd_self.width - 10, 16, (char *)buf, 16, 0, RED);   /* ��ʾ�������� */
    
//     free(buf);                                                        /* �ͷ��ڴ� */
// }

/**
 * @brief       ��ʾ��ǰ����ʱ��
 * @param       favi   : ��ǰ���ŵ���Ƶ�ļ�
 * @param       aviinfo: avi���ƽṹ��
 * @retval      ��
 */
// void video_time_show(FIL *favi, AVI_INFO *aviinfo)
// {
//     static uint32_t oldsec;                                         /* ��һ�εĲ���ʱ�� */
    
//     uint8_t *buf;
    
//     uint32_t totsec = 0;                                            /* video�ļ���ʱ�� */
//     uint32_t cursec;                                                /* ��ǰ����ʱ�� */
    
//     totsec = (aviinfo->SecPerFrame / 1000) * aviinfo->TotalFrame;   /* �����ܳ���(��λ:ms) */
//     totsec /= 1000;                                                 /* ������. */
//     cursec = ((double)favi->fptr / favi->obj.objsize) * totsec;     /* ��ȡ��ǰ���ŵ������� */
    
//     if (oldsec != cursec)                                           /* ��Ҫ������ʾʱ�� */
//     {
//         buf = malloc(100);                                          /* ����100�ֽ��ڴ� */
//         oldsec = cursec;
        
//        sprintf((char *)buf, "Play time:%02ld:%02ld:%02ld/%02ld:%02ld:%02ld", cursec / 3600, (cursec % 3600) / 60, cursec % 60, totsec / 3600, (totsec % 3600) / 60, totsec % 60);
//        text_show_string(10, 50, lcd_self.width - 10, 16, (char *)buf, 16, 0, RED);   /* ��ʾ�������� */
        
//         free(buf);
//     }
// }

/**
 * @brief       ����MJPEG��Ƶ
 * @param       pname: ��Ƶ�ļ���
 * @retval      ������ֵ
 *              KEY0_PRES: ��һ����Ƶ
 *              KEY1_PRES: ��һ����Ƶ
 *              ����ֵ   : �������
 */
static uint8_t video_play_mjpeg(uint8_t *pname)
{
    uint8_t *framebuf;
    uint8_t *pbuf;
    uint8_t res = 0;
    uint16_t offset;
    uint32_t nr;
    uint8_t key;
    FIL *favi;

    framebuf = (uint8_t *)malloc(AVI_VIDEO_BUF_SIZE);   /* �����ڴ� */
    favi = (FIL *)malloc(sizeof(FIL));

    if ((framebuf == NULL) || (favi == NULL))
    {
        printf("memory error!\r\n");
        res = 0xFF;
    }

    memset(framebuf, 0, AVI_VIDEO_BUF_SIZE);
    
    while (res == 0)
    {
        res = (uint8_t)f_open(favi, (const TCHAR *)pname, FA_READ);                                                     /* ���ļ� */
        
        if (res == 0)
        {
            pbuf = framebuf;
            res = (uint8_t)f_read(favi, pbuf, AVI_VIDEO_BUF_SIZE, (UINT*)&nr);                                                 /* ��ʼ��ȡ */
            
            if (res != 0)
            {
                printf("fread error:%d\r\n", res);
                break;
            }
            
            res = avi_init(pbuf, AVI_VIDEO_BUF_SIZE);                                                                   /* AVI���� */
            
            if (res != 0)
            {
                printf("avi error:%d\r\n", res);
                break;
            }
            
            // video_info_show(&g_avix);
            esptim_int_init(g_avix.SecPerFrame / 1000, 1000);
            offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "movi");                                                   /* Ѱ��movi ID */
            avi_get_streaminfo(pbuf + offset + 4);                                                                      /* ��ȡ����Ϣ */
            f_lseek(favi, offset + 12);                                                                                 /* ������־ID������ַƫ�Ƶ������ݿ�ʼ�� */
            res = mjpegdec_init(0, 0);/* ��ʼ��JPG���� */
            /* ����ͼ��Ŀ�� */
            Windows_Width = lcd_self.width;
            Windows_Height = lcd_self.height;

            if (g_avix.SampleRate)                                          /* ����Ƶ��Ϣ,�ų�ʼ�� */
            {
                printf("g_avix.SampleRate:%ld\r\n",g_avix.SampleRate);
                es8388_sai_cfg(0, 3);                                       /* �����ֱ�׼,16λ���ݳ��� */
                i2s_set_samplerate_bits_sample(g_avix.SampleRate,I2S_BITS_PER_SAMPLE_16BIT);    /* ���ò����� */
                i2s_start(I2S_NUM);
            }

            while (1)
            {
                if (g_avix.StreamID == AVI_VIDS_FLAG)                       /* ��Ƶ�� */
                {
                    pbuf = framebuf;
                    f_read(favi, pbuf, g_avix.StreamSize + 8, (UINT*)&nr);  /* ��ȡ��֡+��һ֡������ID��Ϣ */
                    res = mjpegdec_decode(pbuf, g_avix.StreamSize);
                    
                    if (res != 0)
                    {
                        printf("decode error!\r\n");
                    }

                    while (frameup == 0);                                   /* �ȴ�����ʱ�䵽�� */
                    frameup = 0;
                }
                else
                {
                    // video_time_show(favi, &g_avix);                         /* ��ʾ��ǰ����ʱ�� */
                    f_read(favi, framebuf, g_avix.StreamSize + 8, &nr);     /* ���psaibuf */
                    pbuf = framebuf;
                    i2s_tx_write(framebuf, g_avix.StreamSize);          /* ����ת��+���͸�DAC */
                }
                
                key = xl9555_key_scan(0);

                if (key == KEY0_PRES)                   /* KEY0����,������һ����Ƶ */
                {
                    res = key;
                    break;
                }
                else if (key == KEY1_PRES)              /* KEY1����,������һ����Ƶ */
                {
                    res = key;
                    break;
                }
                else if (key == KEY2_PRES || key == KEY3_PRES)
                {
                    i2s_stop(I2S_NUM);                                      /* �ر���Ƶ */
                    video_seek(favi, &g_avix, framebuf);
                    pbuf = framebuf;
                    i2s_start(I2S_NUM);                                     /* ����DMA���� */
                }

                /* ���Զ�������һ֡���������ŵ�ǰ֡ */
                if (avi_get_streaminfo(pbuf + g_avix.StreamSize) != 0)      /* ��ȡ��һ֡����־ */
                {
                    /* ���û�и���֡�����¿�ʼ���ŵ�ǰ��Ƶ */
                    f_lseek(favi, offset + 12);  /* ���¶�λ����Ƶ��ʼλ�� */
                    avi_get_streaminfo(pbuf + offset + 4);  /* ���»�ȡ����Ϣ */
                }
            }

            i2s_stop(I2S_NUM);
            esp_timer_stop(esp_tim_handle);
            lcd_set_window(0, 0, lcd_self.width, lcd_self.height);          /* �ָ����� */
            mjpegdec_free();                                                /* �ͷ��ڴ� */
            f_close(favi);                                                  /* �ر��ļ� */
        }
    }

    i2s_zero_dma_buffer(I2S_NUM);
    free(framebuf);
    free(favi);
    
    return res;
}

/**
 * @brief       ת��
 * @param       fs:�ļ�ϵͳ����
 * @param       clst:ת��
 * @retval      =0:�����ţ�0:ʧ��
 */
static LBA_t atk_clst2sect(FATFS *fs, DWORD clst)
{
    clst -= 2;  /* Cluster number is origin from 2 */

    if (clst >= fs->n_fatent - 2)
    {
        return 0;   /* Is it invalid cluster number? */
    }

    return fs->database + (LBA_t)fs->csize * clst;  /* Start sector number of the cluster */
}

/**
 * @brief       ƫ��
 * @param       dp:ָ��Ŀ¼����
 * @param       Offset:Ŀ¼���ƫ����
 * @retval      FR_OK(0):�ɹ���!=0:����
 */
FRESULT atk_dir_sdi(FF_DIR *dp, DWORD ofs)
{
    DWORD clst;
    FATFS *fs = dp->obj.fs;

    if (ofs >= (DWORD)((FF_FS_EXFAT && fs->fs_type == FS_EXFAT) ? 0x10000000 : 0x200000) || ofs % 32)
    {
        /* Check range of offset and alignment */
        return FR_INT_ERR;
    }

    dp->dptr = ofs;         /* Set current offset */
    clst = dp->obj.sclust;  /* Table start cluster (0:root) */

    if (clst == 0 && fs->fs_type >= FS_FAT32)
    {	/* Replace cluster# 0 with root cluster# */
        clst = (DWORD)fs->dirbase;

        if (FF_FS_EXFAT)
        {
            dp->obj.stat = 0;
        }
        /* exFAT: Root dir has an FAT chain */
    }

    if (clst == 0)
    {	/* Static table (root-directory on the FAT volume) */
        if (ofs / 32 >= fs->n_rootdir)
        {
            return FR_INT_ERR;  /* Is index out of range? */
        }

        dp->sect = fs->dirbase;

    }
    else
    {   /* Dynamic table (sub-directory or root-directory on the FAT32/exFAT volume) */
        dp->sect = atk_clst2sect(fs, clst);
    }

    dp->clust = clst;   /* Current cluster# */

    if (dp->sect == 0)
    {
        return FR_INT_ERR;
    }

    dp->sect += ofs / fs->ssize;             /* Sector# of the directory entry */
    dp->dir = fs->win + (ofs % fs->ssize);   /* Pointer to the entry in the win[] */

    return FR_OK;
}

/**
 * @brief       ������Ƶ
 * @param       ��
 * @retval      ��
 */
void video_play(void)
{
    uint8_t res;
    FF_DIR vdir;
    FILINFO *vfileinfo;
    uint8_t *pname;
    uint16_t totavinum;
    uint16_t curindex;
    uint32_t *voffsettbl;
    uint8_t key;
    uint32_t temp;
    
    while (f_opendir(&vdir, "0:/VIDEO") != FR_OK)                               /* ���VIDEO�ļ����Ƿ���� */
    {
        text_show_string(60, 190, 240, 16, "VIDEO�ļ��д���!", 16, 0, RED);
        vTaskDelay(200);
        lcd_fill(60, 190, 240, 206, WHITE);
        vTaskDelay(200);
    }
    
    totavinum = video_get_tnum("0:/VIDEO");                                     /* ����Ƿ�����Ƶ�ļ� */
    
    while (totavinum == 0)
    {
        text_show_string(60, 190, 240, 16, "û����Ƶ�ļ�!", 16, 0, RED);
        vTaskDelay(200);
        lcd_fill(60, 190, 240, 146, WHITE);
        vTaskDelay(200);
    }
    
    vfileinfo = (FILINFO *)malloc(sizeof(FILINFO));                             /* �����ڴ� */
    pname = (uint8_t *)malloc(2 * 255 + 1);
    voffsettbl = (uint32_t *)malloc(totavinum  * 4);
    
    while ((vfileinfo == NULL) || (pname == NULL) || (voffsettbl == NULL))
    {
        text_show_string(60, 190, 240, 16, "�ڴ����ʧ��!", 16, 0, RED);
        vTaskDelay(200);
        lcd_fill(60, 190, 240, 146, WHITE);
        vTaskDelay(200);
    }
    
    res = (uint8_t)f_opendir(&vdir, "0:/VIDEO");                                /* ��Ŀ¼ */
    
    if (res == 0)
    {
        curindex = 0;
        
        while (1)
        {
            temp = vdir.dptr;                                                   /* ��¼��ǰdptrƫ�� */
            res = (uint8_t)f_readdir(&vdir, vfileinfo);                         /* ��ȡ��һ���ļ� */
            
            if ((res != 0) || (vfileinfo->fname[0] == 0))                       /* �����ĩβ���˳� */
            {
                break;
            }
            
            res = exfuns_file_type(vfileinfo->fname);
            
            if ((res & 0xF0) == 0x60)                                           /* ����Ƶ�ļ� */
            {
                /* ����Ƿ�Ϊ.avi�ļ���ֻ��.avi�ļ��Ŵ��� */
                char *ext = strrchr(vfileinfo->fname, '.');
                if (ext && (strcmp(ext, ".avi") == 0 || strcmp(ext, ".AVI") == 0))
                {
                    voffsettbl[curindex] = temp;                                    /* ��¼���� */
                    curindex++;
                }
            }
        }
    }
    
    curindex = 0;
    res = (uint8_t)f_opendir(&vdir, "0:/VIDEO");                                /* ��Ŀ¼ */

    while (res == 0)
    {
        atk_dir_sdi(&vdir, voffsettbl[curindex]);                                   /* �ı䵱ǰĿ¼���� */
        res = (uint8_t)f_readdir(&vdir, vfileinfo);                             /* ��ȡĿ¼����һ���ļ� */
        
        if ((res != 0) || (vfileinfo->fname[0] == 0))                           /* �����ĩβ���˳� */
        {
            break;
        }
        
        strcpy((char *)pname, "0:/VIDEO/");                                     /* ����·����Ŀ¼�� */
        strcat((char *)pname, (const char *)vfileinfo->fname);                  /* ���ļ������ں��� */
        lcd_clear(WHITE);
        // video_bmsg_show((uint8_t *)vfileinfo->fname, curindex + 1, totavinum);  /* ��ʾ��Ƶ������Ϣ */
        
        key = video_play_mjpeg(pname);

        if (key == KEY1_PRES)   /* ��һ����Ƶ */
        {
            if (curindex != 0)
            {
                curindex--;
            }
            else
            {
                curindex = totavinum - 1;
            }
        }
        else if (key == KEY0_PRES)  /* ��һ����Ƶ */
        {
            curindex++;
            if (curindex >= totavinum)
            {
                curindex = 0;
            }
        }
        else
        {
            break;
        }
    }
    
    free(vfileinfo);      /* �ͷ��ڴ� */
    free(pname);
    free(voffsettbl);
}

/**
 * @brief       AVI�ļ�����
 * @param       favi    : AVI�ļ�
 * @param       aviinfo : AVI��Ϣ�ṹ��
 * @param       mbuf    : ���ݻ�����
 * @retval      ִ�н��
 *   @arg       0    , �ɹ�
 *   @arg       ���� , ����
 */
uint8_t video_seek(FIL *favi, AVI_INFO *aviinfo, uint8_t *mbuf)
{
    uint32_t fpos = favi->fptr;
    uint8_t *pbuf;
    uint16_t offset;
    uint32_t br;
    uint32_t delta;
    uint32_t totsec;
    uint8_t key;

    totsec = (aviinfo->SecPerFrame / 1000) * aviinfo->TotalFrame;
    totsec /= 1000;                             /* ������ */
    delta = (favi->obj.objsize / totsec) * 5;   /* ÿ��ǰ��5���ӵ������� */

    while (1)
    {
        key = xl9555_key_scan(1);

        if (key == KEY3_PRES)                   /* ��� */
        {
            if (fpos < favi->obj.objsize)
            {
                fpos += delta;
            }

            if (fpos > (favi->obj.objsize - AVI_VIDEO_BUF_SIZE))
            {
                fpos = favi->obj.objsize - AVI_VIDEO_BUF_SIZE;
            }
        }
        else if (key == KEY1_PRES)              /* ���� */
        {
            if (fpos > delta)
            {
                fpos -= delta;
            }
            else
            {
                fpos = 0;
            }
        }
        else 
        {
            break;
        }

        f_lseek(favi, fpos);
        f_read(favi, mbuf, AVI_VIDEO_BUF_SIZE, &br);       /* ������֡+��һ������ID��Ϣ */
        pbuf = mbuf;
        
        if (fpos == 0)                                     /* ��0��ʼ,����Ѱ��movi ID */
        {
            offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "movi");
        }
        else
        {
            offset = 0;
        }

        offset += avi_srarch_id(pbuf + offset, AVI_VIDEO_BUF_SIZE, g_avix.VideoFLAG); /* Ѱ����Ƶ֡ */
        avi_get_streaminfo(pbuf + offset);                  /* ��ȡ����Ϣ */
        f_lseek(favi, fpos + offset + 8);                   /* ������־ID,����ַƫ�Ƶ������ݿ�ʼ�� */
        
        if (g_avix.StreamID == AVI_VIDS_FLAG)
        {
            f_read(favi, mbuf, g_avix.StreamSize + 8, &br); /* ������֡ */
            mjpegdec_decode(mbuf, g_avix.StreamSize);       /* ��ʾ��Ƶ֡ */
        }
        else
        {
            printf("error flag");
        }

        // video_time_show(favi, &g_avix);
    }

    return 0;
}
