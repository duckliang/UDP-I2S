#ifndef __UDP_DEMO_H
#define __UDP_DEMO_H
#include "sys.h"
#include "lwip_comm.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
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
 
#define UDP_DEMO_RX_BUFSIZE		8	//����udp���������ݳ��� 
#define UDP_REMOTE_PORT			1  //8087	//����udp���ӵĶ˿� 
#define UDP_REMOTE_PORT2		2	//8088	//����udp���ӵĶ˿� 
#define UDP_LOCAL_PORT      1//1010
#define UDP_LOCAL_PORT2     2//1011

#define TCP_CLIENT_TX_BUFSIZE	1024		//����tcp client��������ݳ���
#define LWIP_SEND_DATA			0X80    //���������ݷ���


 
void udp_demo_test(void);
void udp_demo_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port);
void udp_demo_recv2(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port);
void udp_demo_senddata(struct udp_pcb *upcb,u8 *sendbuf);
void udp_demo_senddata2(struct udp_pcb *upcb,u8 *sendbuf);
void udp_demo_connection_close(struct udp_pcb *upcb);

#endif

