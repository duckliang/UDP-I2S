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

//ALIENTEK 探索者STM32F407开发板
//基于RAW API的UDP实验
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
 
//const u8 TEXT_Buffer[]={"Explorer STM32F4 IIC TEST"};
//#define SIZE sizeof(TEXT_Buffer)	 
extern u8 udp_demo_flag;  //UDP 测试全局状态标记变量
//加载UI
//mode:
//bit0:0,不加载;1,加载前半部分UI
//bit1:0,不加载;1,加载后半部分UI
void lwip_test_ui(u8 mode)
{
	u8 speed;
	u8 buf[30]; 
	POINT_COLOR=RED;
	if(mode&1<<0)
	{
		LCD_Fill(30,30,lcddev.width,110,WHITE);	//清除显示
		LCD_ShowString(30,30,200,16,16,"Explorer STM32F4");
		LCD_ShowString(30,50,200,16,16,"Ethernet lwIP Test");
		LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
		LCD_ShowString(30,90,200,16,16,"2014/8/15"); 	
	}
	if(mode&1<<1)
	{
		LCD_Fill(30,110,lcddev.width,lcddev.height,WHITE);	//清除显示
		LCD_ShowString(30,110,200,16,16,"lwIP Init Successed");
		if(lwipdev.dhcpstatus==2)sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印动态IP地址
		else sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印静态IP地址
		LCD_ShowString(30,130,210,16,16,buf); 
		speed=LAN8720_Get_Speed();//得到网速
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
	delay_init(168);       	//延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断分组配置
	uart_init(115200);   	//串口波特率设置
	usmart_dev.init(84); 	//初始化USMART
	LED_Init();  			//LED初始化
	KEY_Init();  			//按键初始化
	LCD_Init(); 			//LCD初始化
//	FSMC_SRAM_Init();		//初始化外部SRAM  
	mymem_init(SRAMIN);		//初始化内部内存池
//	mymem_init(SRAMEX);		//初始化外部内存池
	mymem_init(SRAMCCM);	//初始化CCM内存池
	AT24CXX_Init();			//IIC初始化 
  AT24CXX_Check();//检测不到24c02

	WM8978_Init();				//初始化WM8978
	WM8978_Init2();
	WM8978_HPvol_Set(20,20);	//耳机音量设置40
	WM8978_HPvol2_Set(20,20);
	WM8978_SPKvol_Set(20);		//喇叭音量设置50
	WM8978_SPKvol2_Set(20);
	
	POINT_COLOR = RED; 		//红色字体
	lwip_test_ui(1);		//加载前半部分UI
	TIM3_Int_Init(99,839); //100khz的频率,计数1000为10ms
	
	while(lwip_comm_init()) 			//lwip初始化
	{
		LCD_ShowString(30,150,200,20,16,"LWIP Init Falied!");
		delay_ms(1200);
		LCD_Fill(30,110,230,130,WHITE); //清除显示
		LCD_ShowString(30,110,200,16,16,"Retrying...");  
	}

	LCD_ShowString(30,110,200,20,16,"LWIP Init Success!");
 	LCD_ShowString(30,130,200,16,16,"DHCP IP configing...");  //等待DHCP获取 
#if LWIP_DHCP
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
	{
		lwip_periodic_handle();
	}
#endif
	lwip_test_ui(2);		//加载后半部分UI 
	delay_ms(500);			//延时1s
	delay_ms(500);
	
	wav_recorder();	
	wav2_recorder();
	
	udp_demo_test();  		//UDP 模式
	while(1)
	{
		key = KEY_Scan(0);
		if(key == KEY1_PRES)		//按KEY1键建立连接
		{
			if((udp_demo_flag & 1<<5)) printf("UDP连接已经建立,不能重复连接\r\n");	//如果连接成功,不做任何处理
			else udp_demo_test();		//当断开连接后,调用udp_demo_test()函数
		}
//		 if(key==KEY1_PRES)//KEY1按下,写入24C02
//		{
//			LCD_Fill(0,170,239,319,WHITE);//清除半屏    
// 			LCD_ShowString(30,170,200,16,16,"Start Write 24C02....");
//			AT24CXX_Write(0,(u8*)TEXT_Buffer,SIZE);
//			LCD_ShowString(30,170,200,16,16,"24C02 Write Finished!");//提示传送完成
//		}
//		if(key==KEY0_PRES)//KEY0按下,读取字符串并显示
//		{
// 			LCD_ShowString(30,170,200,16,16,"Start Read 24C02.... ");
//			AT24CXX_Read(0,datatemp,SIZE);
//			LCD_ShowString(30,170,200,16,16,"The Data Readed Is:  ");//提示传送完成
//			LCD_ShowString(30,190,200,16,16,datatemp);//显示读到的字符串
//		}
//		delay_ms(10);
	}
}










