/**
 ****************************************************************************************************
 * @file        text.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2025-01-01
 * @brief       ������ʾ ����
 *              �ṩtext_show_font��text_show_string��������,������ʾ����
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

#ifndef __TEXT_H
#define __TEXT_H

#include "fonts.h"
#include "esp_partition.h"
#include "spi_flash_mmap.h"
#include "esp_log.h"
#include "ff.h"
#include "lcd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


/* �������� */
void text_show_font(uint16_t x, uint16_t y, uint8_t *font, uint8_t size, uint8_t mode, uint32_t color);                                 /* ��ʾ���� */
void text_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *str, uint8_t size, uint8_t mode, uint32_t color);  /* ��ʾ�����ַ��� */
void text_show_string_middle(uint16_t x, uint16_t y, char *str, uint8_t size, uint16_t width, uint32_t color);                          /* ������ʾ�����ַ��� */

#endif