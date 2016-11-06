#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lwip_comm.h"
#include "LAN8720.h"
#include "usmart.h"
#include "timer.h"
#include "lcd.h"
#include "sram.h"
#include "malloc.h"
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "udp_demo.h"

#include "recorder.h" 
#include "wm8978.h"
#include "i2s.h"
#include "delay.h"
#include "string.h"  
#include "24cxx.h"

//ALIENTEK ̽����STM32F407������
//����RAW API��UDPʵ��
//����֧�֣�www.openedv.com
//�������������ӿƼ����޹�˾
 
//const u8 TEXT_Buffer[]={"Explorer STM32F4 IIC TEST"};
//#define SIZE sizeof(TEXT_Buffer)	 
extern u8 udp_demo_flag;  //UDP ����ȫ��״̬��Ǳ���
//����UI
//mode:
//bit0:0,������;1,����ǰ�벿��UI
//bit1:0,������;1,���غ�벿��UI
void lwip_test_ui(u8 mode)
{
	u8 speed;
	u8 buf[30]; 
	POINT_COLOR=RED;
	if(mode&1<<0)
	{
		LCD_Fill(30,30,lcddev.width,110,WHITE);	//�����ʾ
		LCD_ShowString(30,30,200,16,16,"Explorer STM32F4");
		LCD_ShowString(30,50,200,16,16,"Ethernet lwIP Test");
		LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
		LCD_ShowString(30,90,200,16,16,"2014/8/15"); 	
	}
	if(mode&1<<1)
	{
		LCD_Fill(30,110,lcddev.width,lcddev.height,WHITE);	//�����ʾ
		LCD_ShowString(30,110,200,16,16,"lwIP Init Successed");
		if(lwipdev.dhcpstatus==2)sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//��ӡ��̬IP��ַ
		else sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//��ӡ��̬IP��ַ
		LCD_ShowString(30,130,210,16,16,buf); 
		speed=LAN8720_Get_Speed();//�õ�����
		if(speed&1<<1)LCD_ShowString(30,150,200,16,16,"Ethernet Speed:100M");
		else LCD_ShowString(30,150,200,16,16,"Ethernet Speed:10M");
		LCD_ShowString(30,170,200,16,16,"PORT: 8087");
		LCD_ShowString(30,170,200,16,16,"KEY1: Send data");
	}
}


int main(void)
{
//		u8 datatemp[SIZE];	
	u8 key;
	delay_init(168);       	//��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�жϷ�������
	uart_init(115200);   	//���ڲ���������
	usmart_dev.init(84); 	//��ʼ��USMART
	LED_Init();  			//LED��ʼ��
	KEY_Init();  			//������ʼ��
	LCD_Init(); 			//LCD��ʼ��
//	FSMC_SRAM_Init();		//��ʼ���ⲿSRAM  
	mymem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
//	mymem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��
	mymem_init(SRAMCCM);	//��ʼ��CCM�ڴ��
	AT24CXX_Init();			//IIC��ʼ�� 
  AT24CXX_Check();//��ⲻ��24c02

	WM8978_Init();				//��ʼ��WM8978
	WM8978_Init2();
	WM8978_HPvol_Set(20,20);	//������������40
	WM8978_HPvol2_Set(20,20);
	WM8978_SPKvol_Set(20);		//������������50
	WM8978_SPKvol2_Set(20);
	
	POINT_COLOR = RED; 		//��ɫ����
	lwip_test_ui(1);		//����ǰ�벿��UI
	TIM3_Int_Init(99,839); //100khz��Ƶ��,����1000Ϊ10ms
	
	while(lwip_comm_init()) 			//lwip��ʼ��
	{
		LCD_ShowString(30,150,200,20,16,"LWIP Init Falied!");
		delay_ms(1200);
		LCD_Fill(30,110,230,130,WHITE); //�����ʾ
		LCD_ShowString(30,110,200,16,16,"Retrying...");  
	}

	LCD_ShowString(30,110,200,20,16,"LWIP Init Success!");
 	LCD_ShowString(30,130,200,16,16,"DHCP IP configing...");  //�ȴ�DHCP��ȡ 
#if LWIP_DHCP
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//�ȴ�DHCP��ȡ�ɹ�/��ʱ���
	{
		lwip_periodic_handle();
	}
#endif
	lwip_test_ui(2);		//���غ�벿��UI 
	delay_ms(500);			//��ʱ1s
	delay_ms(500);
	
	wav_recorder();	
	wav2_recorder();
	
	udp_demo_test();  		//UDP ģʽ
	while(1)
	{
		key = KEY_Scan(0);
		if(key == KEY1_PRES)		//��KEY1����������
		{
			if((udp_demo_flag & 1<<5)) printf("UDP�����Ѿ�����,�����ظ�����\r\n");	//������ӳɹ�,�����κδ���
			else udp_demo_test();		//���Ͽ����Ӻ�,����udp_demo_test()����
		}
//		 if(key==KEY1_PRES)//KEY1����,д��24C02
//		{
//			LCD_Fill(0,170,239,319,WHITE);//�������    
// 			LCD_ShowString(30,170,200,16,16,"Start Write 24C02....");
//			AT24CXX_Write(0,(u8*)TEXT_Buffer,SIZE);
//			LCD_ShowString(30,170,200,16,16,"24C02 Write Finished!");//��ʾ�������
//		}
//		if(key==KEY0_PRES)//KEY0����,��ȡ�ַ�������ʾ
//		{
// 			LCD_ShowString(30,170,200,16,16,"Start Read 24C02.... ");
//			AT24CXX_Read(0,datatemp,SIZE);
//			LCD_ShowString(30,170,200,16,16,"The Data Readed Is:  ");//��ʾ�������
//			LCD_ShowString(30,190,200,16,16,datatemp);//��ʾ�������ַ���
//		}
//		delay_ms(10);
	}
}









