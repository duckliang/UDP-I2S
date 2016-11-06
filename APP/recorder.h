#ifndef __RECORDER_H
#define __RECORDER_H
#include "sys.h"
//#include "ff.h"
//#include "wavplay.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//录音机 应用代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/6/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define I2S_RX_DMA_BUF_SIZE    	1024		//定义RX DMA 数组大小
#define I2S3_RX_DMA_BUF_SIZE    2048		//定义RX DMA 数组大小

void rec_i2s_dma_rx_callback(void);
void recoder_enter_rec_mode(void);
//void recoder_wav_init(__WaveHeader* wavhead);
void wav_recorder(void);

void rec_i2s3_dma_rx_callback(void);
void recoder2_enter_rec_mode(void);
//void recoder2_wav_init(__WaveHeader* wavhead);
void wav2_recorder(void);

#endif












