#ifndef __RECORDER_H
#define __RECORDER_H
#include "sys.h"
//#include "ff.h"
//#include "wavplay.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//¼���� Ӧ�ô���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/6/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define I2S_RX_DMA_BUF_SIZE    	1024		//����RX DMA �����С
#define I2S3_RX_DMA_BUF_SIZE    2048		//����RX DMA �����С

void rec_i2s_dma_rx_callback(void);
void recoder_enter_rec_mode(void);
//void recoder_wav_init(__WaveHeader* wavhead);
void wav_recorder(void);

void rec_i2s3_dma_rx_callback(void);
void recoder2_enter_rec_mode(void);
//void recoder2_wav_init(__WaveHeader* wavhead);
void wav2_recorder(void);

#endif












