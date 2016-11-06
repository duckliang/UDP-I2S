#include "recorder.h" 
#include "malloc.h"
#include "usart.h"
#include "wm8978.h"
#include "i2s.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "string.h" 
#include "udp_demo.h" 

u8 tcp_client_flag1;	
u8 tcp_client_flag2;	
u8 readbuf1_1=0;
u8 readbuf1_2=0;
u8 readbuf2_1=0;
u8 readbuf2_2=0;
u8 * i2srecbuf1;
u8 * i2srecbuf2; 
u8 * i2s3recbuf1;
u8 * i2s3recbuf2; 
const u16 i2splaybuf[2]={0X0000,0X0000};//2个16位数据,用于录音时I2S Master发送.循环发送0.


const u16 i2s3playbuf[2]={0X0000,0X0000};//2个16位数据,用于录音时I2S Master发送.循环发送0.

////进入PCM 录音模式 		  
//void recoder_enter_rec_mode(void)
//{
//	WM8978_ADDA_Cfg(0,1);		//开启ADC
//	WM8978_Input_Cfg(1,1,0);	//开启输入通道(MIC&LINE IN)
//	WM8978_Output_Cfg(0,1);		//开启BYPASS输出 
//	WM8978_MIC_Gain(46);		//MIC增益设置 
//	
//	WM8978_I2S_Cfg(2,0);		//飞利浦标准,16位数据长度
//	I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);			//飞利浦标准,主机发送,时钟低电平有效,16位帧长度 
//	I2S2ext_Init(I2S_Standard_Phillips,I2S_Mode_SlaveRx,I2S_CPOL_Low,I2S_DataFormat_16b);		//飞利浦标准,从机接收,时钟低电平有效,16位帧长度	
//	I2S2_SampleRate_Set(48000);	//设置采样率 
// 	I2S2_TX_DMA_Init((u8*)&i2splaybuf[0],(u8*)&i2splaybuf[1],1); 		//配置TX DMA 
//	DMA1_Stream4->CR&=~(1<<4);	//关闭传输完成中断(这里不用中断送数据) 
//	I2S2ext_RX_DMA_Init(i2srecbuf1,i2srecbuf2,I2S_RX_DMA_BUF_SIZE/2); 	//配置RX DMA
//  i2s_rx_callback=rec_i2s_dma_rx_callback;//回调函数指wav_i2s_dma_callback
// 	I2S_Play_Start();	//开始I2S数据发送(主机)
//	I2S_Rec_Start(); 	//开始I2S数据接收(从机)
//}  
//进入PCM 录音模式 		  
void recoder_enter_rec_mode(void)
{
	WM8978_ADDA_Cfg(0,1);		//开启ADC
	WM8978_Input_Cfg(1,1,0);	//开启输入通道(MIC&LINE IN)
	WM8978_Output_Cfg(0,1);		//开启BYPASS输出 
	WM8978_MIC_Gain(46);		//MIC增益设置 
	
	WM8978_I2S_Cfg(2,0);		//飞利浦标准,16位数据长度
	I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);			//飞利浦标准,主机发送,时钟低电平有效,16位帧长度 
	I2S2ext_Init(I2S_Standard_Phillips,I2S_Mode_SlaveRx,I2S_CPOL_Low,I2S_DataFormat_16b);		//飞利浦标准,从机接收,时钟低电平有效,16位帧长度	
	I2S2_SampleRate_Set(48000);	//设置采样率 
 	I2S2_TX_DMA_Init((u8*)&i2splaybuf[0],(u8*)&i2splaybuf[1],1); 		//配置TX DMA 
	DMA1_Stream4->CR&=~(1<<4);	//关闭传输完成中断(这里不用中断送数据) 
	I2S2ext_RX_DMA_Init(i2srecbuf1,i2srecbuf2,I2S_RX_DMA_BUF_SIZE/2); 	//配置RX DMA
  i2s_rx_callback=rec_i2s_dma_rx_callback;//回调函数指wav_i2s_dma_callback
 	I2S_Play_Start();	//开始I2S数据发送(主机)
	I2S_Rec_Start(); 	//开始I2S数据接收(从机)
} 
void recoder2_enter_rec_mode(void)
{
	WM8978_ADDA2_Cfg(0,1);		//开启ADC
	WM8978_Input2_Cfg(1,1,0);	//开启输入通道(MIC&LINE IN)
	WM8978_Output2_Cfg(0,1);		//开启BYPASS输出 
	WM8978_MIC2_Gain(46);		//MIC增益设置 
	
	WM8978_I2S3_Cfg(2,2);		//飞利浦标准,24位数据长度,0为16位
	I2S3_Init(I2S_Standard_Phillips,I2S_Mode_SlaveTx,I2S_CPOL_Low,I2S_DataFormat_24b);			//飞利浦标准,主机发送,时钟低电平有效,16位帧长度 
	I2S3ext_Init(I2S_Standard_Phillips,I2S_Mode_SlaveRx,I2S_CPOL_Low,I2S_DataFormat_24b);		//飞利浦标准,从机接收,时钟低电平有效,16位帧长度	
	//I2S3_SampleRate_Set(48000);	//设置采样率 
 	I2S3_TX_DMA_Init((u8*)&i2s3playbuf[0],(u8*)&i2s3playbuf[1],1); 		//配置TX DMA 
	DMA1_Stream5->CR&=~(1<<4);	//关闭传输完成中断(这里不用中断送数据) 
	I2S3ext_RX_DMA_Init(i2s3recbuf1,i2s3recbuf2,I2S3_RX_DMA_BUF_SIZE/2); 	//配置RX DMA
  i2s3_rx_callback=rec_i2s3_dma_rx_callback;//回调函数指wav_i2s_dma_callback
 	I2S3_Play_Start();	//开始I2S数据发送(主机)
	I2S3_Rec_Start(); 	//开始I2S数据接收(从机)
} 


//WAV录音 
void wav_recorder(void)
{ 
	i2srecbuf1=mymalloc(SRAMIN,I2S_RX_DMA_BUF_SIZE);//I2S录音内存1申请
	i2srecbuf2=mymalloc(SRAMIN,I2S_RX_DMA_BUF_SIZE);//I2S录音内存2申请  	
	recoder_enter_rec_mode();	//进入录音模式,此时耳机可以听到咪头采集到的音频   
}
void wav2_recorder(void)
{ 
	i2s3recbuf1=mymalloc(SRAMIN,I2S3_RX_DMA_BUF_SIZE);//I2S录音内存1申请
	i2s3recbuf2=mymalloc(SRAMIN,I2S3_RX_DMA_BUF_SIZE);//I2S录音内存2申请  	
	recoder2_enter_rec_mode();	//进入录音模式,此时耳机可以听到咪头采集到的音频   
}

					
//录音 I2S_DMA接收中断服务函数.在中断里面写入数据
extern	struct udp_pcb *udppcb;  	//定义一个TCP服务器控制块
extern	struct udp_pcb *udppcb2;  	//定义一个TCP服务器控制块
void rec_i2s_dma_rx_callback(void) 
{    
		if(DMA1_Stream3->CR&(1<<19))//当前目标存储器为存储器1
		{udp_demo_senddata(udppcb,i2srecbuf1);
//			readbuf1_1=1;
//			tcp_client_flag1|=1<<7;//标记要发送数据			
			
		}else //当前目标存储器为存储器0
		{udp_demo_senddata(udppcb,i2srecbuf2);
//			readbuf1_2=1;
//			tcp_client_flag1|=1<<7;//标记要发送数据
			
		}
		
} 
void rec_i2s3_dma_rx_callback(void) 
{   
		if(DMA1_Stream0->CR&(1<<19))//当前目标存储器为存储器1
		{udp_demo_senddata2(udppcb2,i2s3recbuf1);
//			readbuf2_1=1;
//			tcp_client_flag2|=1<<7;//标记要发送数据
		}else //当前目标存储器为存储器0
		{
			udp_demo_senddata2(udppcb2,i2s3recbuf2);
}
//			readbuf2_2=1;
//			tcp_client_flag2|=1<<7;//标记要发送数据
		}
		



































