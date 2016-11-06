#include "udp_demo.h" 
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "malloc.h"
#include "stdio.h"
#include "string.h" 
#include "24cxx.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//UDP ���Դ���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/8/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
//UDP�������ݻ�����
u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];	//UDP�������ݻ����� 
u8 udp_demo_recvbuf2[UDP_DEMO_RX_BUFSIZE];	//UDP�������ݻ����� 
//UDP������������
u8 *tcp_demo_sendbuf;

//UDP ����ȫ��״̬��Ǳ���
//bit7:û���õ�
//bit6:0,û���յ�����;1,�յ�������.
//bit5:0,û��������;1,��������.
//bit4~0:����
u8 udp_demo_flag;
u8 udp_demo_flag2;
extern u8 tcp_client_flag1;	 
extern u8 tcp_client_flag2;	
extern u8 readbuf1_1;
extern u8 readbuf1_2;
extern u8 readbuf2_1;
extern u8 readbuf2_2;
extern u8 * i2srecbuf1;
extern u8 * i2srecbuf2;
extern u8 * i2s3recbuf1;
extern u8 * i2s3recbuf2;

//����Զ��IP��ַ
void udp_demo_set_remoteip(void)
{
	u8 *tbuf;
	u16 xoff;
	u8 key;
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	LCD_ShowString(30,30,200,16,16,"Explorer STM32F4");
	LCD_ShowString(30,50,200,16,16,"UDP Test");
	LCD_ShowString(30,70,200,16,16,"Remote IP Set");  
	LCD_ShowString(30,90,200,16,16,"KEY0:+  KEY2:-");  
	LCD_ShowString(30,110,200,16,16,"KEY_UP:OK");  
	tbuf=mymalloc(SRAMIN,100);	//�����ڴ�
	if(tbuf==NULL)return;
	//ǰ����IP���ֺ�DHCP�õ���IPһ��
	lwipdev.remoteip[0]=lwipdev.ip[0];
	lwipdev.remoteip[1]=lwipdev.ip[1];
	lwipdev.remoteip[2]=lwipdev.ip[2]; 
	sprintf((char*)tbuf,"Remote IP:%d.%d.%d.",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2]);//Զ��IP
	LCD_ShowString(30,150,210,16,16,tbuf); 
	POINT_COLOR=BLUE;
	xoff=strlen((char*)tbuf)*8+30;
	LCD_ShowxNum(xoff,150,lwipdev.remoteip[3],3,16,0); 
	while(1)
	{
		key=KEY_Scan(0);
		if(key==WKUP_PRES)break;
		else if(key)
		{
			if(key==KEY0_PRES)lwipdev.remoteip[3]++;//IP����
			if(key==KEY2_PRES)lwipdev.remoteip[3]--;//IP����
			LCD_ShowxNum(xoff,150,lwipdev.remoteip[3],3,16,0X80);//��ʾ��IP
		}
	}
	myfree(SRAMIN,tbuf); 
} 
extern u8 local_port[2];
extern u8 remote_port[2];
//UDP����
	struct udp_pcb *udppcb;  	//����һ��TCP���������ƿ�
	struct udp_pcb *udppcb2;  	//����һ��TCP���������ƿ�
void udp_demo_test(void)
{
 	err_t err;
//	struct udp_pcb *udppcb;  	//����һ��TCP���������ƿ�
//	struct udp_pcb *udppcb2;  	//����һ��TCP���������ƿ�
	struct ip_addr rmtipaddr;  	//Զ��ip��ַ
 	
	u8 *tbuf;
 	u8 key;
	u8 res=0;		
	u8 t=0; 
//	u16 i;
// 	tcp_demo_sendbuf=mymalloc(SRAMIN,TCP_CLIENT_TX_BUFSIZE);
	udp_demo_set_remoteip();//��ѡ��IP
	LCD_Clear(WHITE);	//����
	POINT_COLOR=RED; 	//��ɫ����
	LCD_ShowString(30,30,200,16,16,"Explorer STM32F4");
	LCD_ShowString(30,50,200,16,16,"UDP Test");
	LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");  
	LCD_ShowString(30,90,200,16,16,"KEY0:Send data");  
	LCD_ShowString(30,110,200,16,16,"KEY_UP:Quit"); 
	LCD_ShowString(30,130,200,16,16,"KEY1:Connect");
	tbuf=mymalloc(SRAMIN,200);	//�����ڴ�
	if(tbuf==NULL)return ;		//�ڴ�����ʧ����,ֱ���˳�
	sprintf((char*)tbuf,"Local IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//������IP
	LCD_ShowString(30,150,210,16,16,tbuf);  
	sprintf((char*)tbuf,"Remote IP:%d.%d.%d.%d",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);//Զ��IP
	LCD_ShowString(30,170,210,16,16,tbuf);  
	sprintf((char*)tbuf,"Remote Port:%d.%d",remote_port[0],remote_port[1]);//����˶˿ں�
	LCD_ShowString(30,190,210,16,16,tbuf);
	sprintf((char*)tbuf,"local Port:%d.%d",local_port[0],local_port[1]);//�ͻ��˶˿ں�
	LCD_ShowString(30,210,210,16,16,tbuf);
	udppcb=udp_new();
	udppcb2=udp_new();
	if(udppcb)//�����ɹ�
	{ 
		IP4_ADDR(&rmtipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);
		err=udp_connect(udppcb,&rmtipaddr,remote_port[0]);//UDP�ͻ������ӵ�ָ��IP��ַ�Ͷ˿ںŵķ�����
		if(err==ERR_OK)
		{
			err=udp_bind(udppcb,IP_ADDR_ANY,local_port[0]);//�󶨱���IP��ַ��˿ں�
			if(err==ERR_OK)	//�����
			{
				udp_recv(udppcb,udp_demo_recv,NULL);//ע����ջص����� 
//				LCD_ShowString(30,210,210,16,16,"STATUS:Connected   ");//�����������(UDP�Ƿǿɿ�����,���������ʾ����UDP�Ѿ�׼����)
				udp_demo_flag |= 1<<5;			//����Ѿ�������
//				POINT_COLOR=RED;
//				LCD_ShowString(30,230,lcddev.width-30,lcddev.height-190,16,"Receive Data:");//��ʾ��Ϣ		
//				POINT_COLOR=BLUE;//��ɫ����
			}else res=1;
		}else res=1;		
	}else res=1;
	if(udppcb2)//�����ɹ�
	{ 
		IP4_ADDR(&rmtipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);
		err=udp_connect(udppcb2,&rmtipaddr,remote_port[1]);//UDP�ͻ������ӵ�ָ��IP��ַ�Ͷ˿ںŵķ�����
		if(err==ERR_OK)
		{
			err=udp_bind(udppcb2,IP_ADDR_ANY,local_port[1]);//�󶨱���IP��ַ��˿ں�
			if(err==ERR_OK)	//�����
			{
				udp_recv(udppcb2,udp_demo_recv2,NULL);//ע����ջص����� 
//				LCD_ShowString(30,210,210,16,16,"STATUS:Connected   ");//�����������(UDP�Ƿǿɿ�����,���������ʾ����UDP�Ѿ�׼����)
//				udp_demo_flag2 |= 1<<5;			//����Ѿ�������
//				POINT_COLOR=RED;
//				LCD_ShowString(30,230,lcddev.width-30,lcddev.height-190,16,"Receive Data:");//��ʾ��Ϣ		
//				POINT_COLOR=BLUE;//��ɫ����
			}else res=1;
		}else res=1;		
	}else res=1;
//	for(i=0;i<TCP_CLIENT_TX_BUFSIZE;i++)
//	{
//	  tcp_demo_sendbuf[i]=0x55;
//	}
	while(res==0)
	{
//		udp_demo_senddata(udppcb);
		key=KEY_Scan(0);
		if(key==WKUP_PRES)break;
		if(udp_demo_flag&1<<6)//�Ƿ��յ�����?
		{		
			if((udp_demo_recvbuf[0]==0X55)&&(udp_demo_recvbuf[7]==0X88))
			{
			AT24CXX_Write(0,&(udp_demo_recvbuf[1]),UDP_DEMO_RX_BUFSIZE-2);
			}
//			LCD_Fill(30,250,lcddev.width-1,lcddev.height-1,WHITE);//����һ������
//			LCD_ShowString(30,250,lcddev.width-30,lcddev.height-230,16,udp_demo_recvbuf);//��ʾ���յ�������	
//			USART_SendData(USART1,udp_demo_recvbuf[0]);
			udp_demo_flag&=~(1<<6);//��������Ѿ���������
		} 
		if(udp_demo_flag2&1<<6)//�Ƿ��յ�����?
		{
			if((udp_demo_recvbuf2[0]==0x55)&&(udp_demo_recvbuf2[7]==0x88))
			{
			AT24CXX_Write(10,&(udp_demo_recvbuf2[1]),UDP_DEMO_RX_BUFSIZE-2);
			}
//			LCD_Fill(30,250,lcddev.width-1,lcddev.height-1,WHITE);//����һ������
//			LCD_ShowString(30,250,lcddev.width-30,lcddev.height-230,16,udp_demo_recvbuf2);//��ʾ���յ�������			
			udp_demo_flag2&=~(1<<6);//��������Ѿ���������.
		} 
//			udp_demo_senddata2(udppcb2);
			lwip_periodic_handle();



//			delay_ms(1);

		}
	udp_demo_connection_close(udppcb); 
	udp_demo_connection_close(udppcb2); 
	myfree(SRAMIN,tbuf);
} 

//UDP�ص�����
void udp_demo_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port)
{
	u32 data_len = 0;
	struct pbuf *q;
	if(p!=NULL)	//���յ���Ϊ�յ�����ʱ
	{
		memset(udp_demo_recvbuf,0,UDP_DEMO_RX_BUFSIZE);  //���ݽ��ջ���������
		for(q=p;q!=NULL;q=q->next)  //����������pbuf����
		{
			//�ж�Ҫ������UDP_DEMO_RX_BUFSIZE�е������Ƿ����UDP_DEMO_RX_BUFSIZE��ʣ��ռ䣬�������
			//�Ļ���ֻ����UDP_DEMO_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
			if(q->len > (UDP_DEMO_RX_BUFSIZE-data_len)) memcpy(udp_demo_recvbuf+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));//��������
			else memcpy(udp_demo_recvbuf+data_len,q->payload,q->len);
			data_len += q->len;  	
			if(data_len > UDP_DEMO_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
		}
		upcb->remote_ip=*addr; 				//��¼Զ��������IP��ַ
		upcb->remote_port=port;  			//��¼Զ�������Ķ˿ں�
		lwipdev.remoteip[0]=upcb->remote_ip.addr&0xff; 		//IADDR4
		lwipdev.remoteip[1]=(upcb->remote_ip.addr>>8)&0xff; //IADDR3
		lwipdev.remoteip[2]=(upcb->remote_ip.addr>>16)&0xff;//IADDR2
		lwipdev.remoteip[3]=(upcb->remote_ip.addr>>24)&0xff;//IADDR1 
		udp_demo_flag|=1<<6;	//��ǽ��յ�������
		pbuf_free(p);//�ͷ��ڴ�
	}else
	{
		udp_disconnect(upcb); 
		LCD_Clear(WHITE);			//����
		POINT_COLOR = RED;
		LCD_ShowString(30,30,200,16,16,"Explorer STM32F4");
		LCD_ShowString(30,50,200,16,16,"UDP Test");
		LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
		
		POINT_COLOR=BLUE;
		LCD_ShowString(30,90,200,16,16,"Connect break��");  
		LCD_ShowString(30,110,200,16,16,"KEY1:Connect");
		udp_demo_flag &= ~(1<<5);	//������ӶϿ�
	} 
} 
void udp_demo_recv2(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port)
{
	u32 data_len = 0;
	struct pbuf *q;
	if(p!=NULL)	//���յ���Ϊ�յ�����ʱ
	{
		memset(udp_demo_recvbuf2,0,UDP_DEMO_RX_BUFSIZE);  //���ݽ��ջ���������
		for(q=p;q!=NULL;q=q->next)  //����������pbuf����
		{
			//�ж�Ҫ������UDP_DEMO_RX_BUFSIZE�е������Ƿ����UDP_DEMO_RX_BUFSIZE��ʣ��ռ䣬�������
			//�Ļ���ֻ����UDP_DEMO_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
			if(q->len > (UDP_DEMO_RX_BUFSIZE-data_len)) memcpy(udp_demo_recvbuf2+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));//��������
			else memcpy(udp_demo_recvbuf2+data_len,q->payload,q->len);
			data_len += q->len;  	
			if(data_len > UDP_DEMO_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
		}
		upcb->remote_ip=*addr; 				//��¼Զ��������IP��ַ
		upcb->remote_port=port;  			//��¼Զ�������Ķ˿ں�
		lwipdev.remoteip[0]=upcb->remote_ip.addr&0xff; 		//IADDR4
		lwipdev.remoteip[1]=(upcb->remote_ip.addr>>8)&0xff; //IADDR3
		lwipdev.remoteip[2]=(upcb->remote_ip.addr>>16)&0xff;//IADDR2
		lwipdev.remoteip[3]=(upcb->remote_ip.addr>>24)&0xff;//IADDR1 
		udp_demo_flag2|=1<<6;	//��ǽ��յ�������
		pbuf_free(p);//�ͷ��ڴ�
	}else
	{
		udp_disconnect(upcb); 
		LCD_Clear(WHITE);			//����
		POINT_COLOR = RED;
		LCD_ShowString(30,30,200,16,16,"Explorer STM32F4");
		LCD_ShowString(30,50,200,16,16,"UDP Test");
		LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
		
		POINT_COLOR=BLUE;
		LCD_ShowString(30,90,200,16,16,"Connect break��");  
		LCD_ShowString(30,110,200,16,16,"KEY1:Connect");
		udp_demo_flag2 &= ~(1<<5);	//������ӶϿ�
	} 
} 

//UDP��������������
void udp_demo_senddata(struct udp_pcb *upcb,u8 *sendbuf)
{
	struct pbuf *ptr;
//	if(tcp_client_flag1&(1<<7))	//�ж��Ƿ�������Ҫ���� 
//	{	
//	ptr=pbuf_alloc(PBUF_TRANSPORT,strlen((char*)tcp_demo_sendbuf),PBUF_POOL); //�����ڴ�
		ptr=pbuf_alloc(PBUF_TRANSPORT,TCP_CLIENT_TX_BUFSIZE,PBUF_POOL); //�����ڴ�
		if(ptr)
		{
		  pbuf_take(ptr,(u8*)sendbuf,TCP_CLIENT_TX_BUFSIZE); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
//			if(readbuf1_1==1){
//			pbuf_take(ptr,(u8*)i2srecbuf1,TCP_CLIENT_TX_BUFSIZE); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
//			readbuf1_1=0;
//			}
//			else if(readbuf1_2==1){
//	    pbuf_take(ptr,(u8*)i2srecbuf2,TCP_CLIENT_TX_BUFSIZE); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
//			readbuf1_2=0;
//			}		
//		pbuf_take(ptr,(char*)tcp_demo_sendbuf,strlen((char*)tcp_demo_sendbuf)); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
			udp_send(upcb,ptr);	//udp�������� 
			pbuf_free(ptr);//�ͷ��ڴ�
//			tcp_client_flag1&=~(1<<7);	//������ݷ��ͱ�־
		}
//	} 
} 
void udp_demo_senddata2(struct udp_pcb *upcb,u8 * sendbuf)
{
	struct pbuf *ptr;
//	if(tcp_client_flag2&(1<<7))	//�ж��Ƿ�������Ҫ���� 
//	{	
//	ptr=pbuf_alloc(PBUF_TRANSPORT,strlen((char*)tcp_demo_sendbuf),PBUF_POOL); //�����ڴ�
		ptr=pbuf_alloc(PBUF_TRANSPORT,TCP_CLIENT_TX_BUFSIZE,PBUF_POOL); //�����ڴ�
		if(ptr)
		{
		  pbuf_take(ptr,(u8*)sendbuf,TCP_CLIENT_TX_BUFSIZE); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
//			if(readbuf2_1==1){
//			pbuf_take(ptr,(u8*)sendbuf,TCP_CLIENT_TX_BUFSIZE); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
//			readbuf2_1=0;
//			}
//			else if(readbuf2_2==1){
//	    pbuf_take(ptr,(u8*)sendbuf,TCP_CLIENT_TX_BUFSIZE); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
//			readbuf2_2=0;
//			}		
//		  pbuf_take(ptr,(char*)tcp_demo_sendbuf,strlen((char*)tcp_demo_sendbuf)); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
			udp_send(upcb,ptr);	//udp�������� 
			pbuf_free(ptr);//�ͷ��ڴ�
//			tcp_client_flag2&=~(1<<7);	//������ݷ��ͱ�־
		}
//	} 
}
//�ر�UDP����
void udp_demo_connection_close(struct udp_pcb *upcb)
{
	udp_disconnect(upcb); 
	udp_remove(upcb);			//�Ͽ�UDP���� 
	udp_demo_flag &= ~(1<<5);	//������ӶϿ�
	LCD_Clear(WHITE);			//����
	POINT_COLOR = RED;
	LCD_ShowString(30,30,200,16,16,"Explorer STM32F4");
	LCD_ShowString(30,50,200,16,16,"UDP Test");
	LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
	
	POINT_COLOR=BLUE;
	LCD_ShowString(30,90,200,16,16,"Connect break��");  
	LCD_ShowString(30,110,200,16,16,"KEY1:Connect");
}

























