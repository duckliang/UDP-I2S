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
const u16 i2splaybuf[2]={0X0000,0X0000};//2��16λ����,����¼��ʱI2S Master����.ѭ������0.


const u16 i2s3playbuf[2]={0X0000,0X0000};//2��16λ����,����¼��ʱI2S Master����.ѭ������0.

////����PCM ¼��ģʽ 		  
//void recoder_enter_rec_mode(void)
//{
//	WM8978_ADDA_Cfg(0,1);		//����ADC
//	WM8978_Input_Cfg(1,1,0);	//��������ͨ��(MIC&LINE IN)
//	WM8978_Output_Cfg(0,1);		//����BYPASS��� 
//	WM8978_MIC_Gain(46);		//MIC�������� 
//	
//	WM8978_I2S_Cfg(2,0);		//�����ֱ�׼,16λ���ݳ���
//	I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);			//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,16λ֡���� 
//	I2S2ext_Init(I2S_Standard_Phillips,I2S_Mode_SlaveRx,I2S_CPOL_Low,I2S_DataFormat_16b);		//�����ֱ�׼,�ӻ�����,ʱ�ӵ͵�ƽ��Ч,16λ֡����	
//	I2S2_SampleRate_Set(48000);	//���ò����� 
// 	I2S2_TX_DMA_Init((u8*)&i2splaybuf[0],(u8*)&i2splaybuf[1],1); 		//����TX DMA 
//	DMA1_Stream4->CR&=~(1<<4);	//�رմ�������ж�(���ﲻ���ж�������) 
//	I2S2ext_RX_DMA_Init(i2srecbuf1,i2srecbuf2,I2S_RX_DMA_BUF_SIZE/2); 	//����RX DMA
//  i2s_rx_callback=rec_i2s_dma_rx_callback;//�ص�����ָwav_i2s_dma_callback
// 	I2S_Play_Start();	//��ʼI2S���ݷ���(����)
//	I2S_Rec_Start(); 	//��ʼI2S���ݽ���(�ӻ�)
//}  
//����PCM ¼��ģʽ 		  
void recoder_enter_rec_mode(void)
{
	WM8978_ADDA_Cfg(0,1);		//����ADC
	WM8978_Input_Cfg(1,1,0);	//��������ͨ��(MIC&LINE IN)
	WM8978_Output_Cfg(0,1);		//����BYPASS��� 
	WM8978_MIC_Gain(46);		//MIC�������� 
	
	WM8978_I2S_Cfg(2,0);		//�����ֱ�׼,16λ���ݳ���
	I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);			//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,16λ֡���� 
	I2S2ext_Init(I2S_Standard_Phillips,I2S_Mode_SlaveRx,I2S_CPOL_Low,I2S_DataFormat_16b);		//�����ֱ�׼,�ӻ�����,ʱ�ӵ͵�ƽ��Ч,16λ֡����	
	I2S2_SampleRate_Set(48000);	//���ò����� 
 	I2S2_TX_DMA_Init((u8*)&i2splaybuf[0],(u8*)&i2splaybuf[1],1); 		//����TX DMA 
	DMA1_Stream4->CR&=~(1<<4);	//�رմ�������ж�(���ﲻ���ж�������) 
	I2S2ext_RX_DMA_Init(i2srecbuf1,i2srecbuf2,I2S_RX_DMA_BUF_SIZE/2); 	//����RX DMA
  i2s_rx_callback=rec_i2s_dma_rx_callback;//�ص�����ָwav_i2s_dma_callback
 	I2S_Play_Start();	//��ʼI2S���ݷ���(����)
	I2S_Rec_Start(); 	//��ʼI2S���ݽ���(�ӻ�)
} 
void recoder2_enter_rec_mode(void)
{
	WM8978_ADDA2_Cfg(0,1);		//����ADC
	WM8978_Input2_Cfg(1,1,0);	//��������ͨ��(MIC&LINE IN)
	WM8978_Output2_Cfg(0,1);		//����BYPASS��� 
	WM8978_MIC2_Gain(46);		//MIC�������� 
	
	WM8978_I2S3_Cfg(2,2);		//�����ֱ�׼,24λ���ݳ���,0Ϊ16λ
	I2S3_Init(I2S_Standard_Phillips,I2S_Mode_SlaveTx,I2S_CPOL_Low,I2S_DataFormat_24b);			//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,16λ֡���� 
	I2S3ext_Init(I2S_Standard_Phillips,I2S_Mode_SlaveRx,I2S_CPOL_Low,I2S_DataFormat_24b);		//�����ֱ�׼,�ӻ�����,ʱ�ӵ͵�ƽ��Ч,16λ֡����	
	//I2S3_SampleRate_Set(48000);	//���ò����� 
 	I2S3_TX_DMA_Init((u8*)&i2s3playbuf[0],(u8*)&i2s3playbuf[1],1); 		//����TX DMA 
	DMA1_Stream5->CR&=~(1<<4);	//�رմ�������ж�(���ﲻ���ж�������) 
	I2S3ext_RX_DMA_Init(i2s3recbuf1,i2s3recbuf2,I2S3_RX_DMA_BUF_SIZE/2); 	//����RX DMA
  i2s3_rx_callback=rec_i2s3_dma_rx_callback;//�ص�����ָwav_i2s_dma_callback
 	I2S3_Play_Start();	//��ʼI2S���ݷ���(����)
	I2S3_Rec_Start(); 	//��ʼI2S���ݽ���(�ӻ�)
} 


//WAV¼�� 
void wav_recorder(void)
{ 
	i2srecbuf1=mymalloc(SRAMIN,I2S_RX_DMA_BUF_SIZE);//I2S¼���ڴ�1����
	i2srecbuf2=mymalloc(SRAMIN,I2S_RX_DMA_BUF_SIZE);//I2S¼���ڴ�2����  	
	recoder_enter_rec_mode();	//����¼��ģʽ,��ʱ��������������ͷ�ɼ�������Ƶ   
}
void wav2_recorder(void)
{ 
	i2s3recbuf1=mymalloc(SRAMIN,I2S3_RX_DMA_BUF_SIZE);//I2S¼���ڴ�1����
	i2s3recbuf2=mymalloc(SRAMIN,I2S3_RX_DMA_BUF_SIZE);//I2S¼���ڴ�2����  	
	recoder2_enter_rec_mode();	//����¼��ģʽ,��ʱ��������������ͷ�ɼ�������Ƶ   
}

					
//¼�� I2S_DMA�����жϷ�����.���ж�����д������
extern	struct udp_pcb *udppcb;  	//����һ��TCP���������ƿ�
extern	struct udp_pcb *udppcb2;  	//����һ��TCP���������ƿ�
void rec_i2s_dma_rx_callback(void) 
{    
		if(DMA1_Stream3->CR&(1<<19))//��ǰĿ��洢��Ϊ�洢��1
		{udp_demo_senddata(udppcb,i2srecbuf1);
//			readbuf1_1=1;
//			tcp_client_flag1|=1<<7;//���Ҫ��������			
			
		}else //��ǰĿ��洢��Ϊ�洢��0
		{udp_demo_senddata(udppcb,i2srecbuf2);
//			readbuf1_2=1;
//			tcp_client_flag1|=1<<7;//���Ҫ��������
			
		}
		
} 
void rec_i2s3_dma_rx_callback(void) 
{   
		if(DMA1_Stream0->CR&(1<<19))//��ǰĿ��洢��Ϊ�洢��1
		{udp_demo_senddata2(udppcb2,i2s3recbuf1);
//			readbuf2_1=1;
//			tcp_client_flag2|=1<<7;//���Ҫ��������
		}else //��ǰĿ��洢��Ϊ�洢��0
		{
			udp_demo_senddata2(udppcb2,i2s3recbuf2);
}
//			readbuf2_2=1;
//			tcp_client_flag2|=1<<7;//���Ҫ��������
		}
		



































