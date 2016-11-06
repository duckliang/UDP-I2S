#include "wm8978.h"
#include "myiic.h"
#include "delay.h"
#include "stm32f4xx_gpio.h"
 
static u16 WM8978_REGVAL_TBL[58]=
{
	0X0000,0X0000,0X0000,0X0000,0X0050,0X0000,0X0140,0X0000,
	0X0000,0X0000,0X0000,0X00FF,0X00FF,0X0000,0X0100,0X00FF,
	0X00FF,0X0000,0X012C,0X002C,0X002C,0X002C,0X002C,0X0000,
	0X0032,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
	0X0038,0X000B,0X0032,0X0000,0X0008,0X000C,0X0093,0X00E9,
	0X0000,0X0000,0X0000,0X0000,0X0003,0X0010,0X0010,0X0100,
	0X0100,0X0002,0X0001,0X0001,0X0039,0X0039,0X0039,0X0039,
	0X0001,0X0001
}; 
static u16 WM8978_REGVAL2_TBL[58]=
{
	0X0000,0X0000,0X0000,0X0000,0X0050,0X0000,0X0140,0X0000,
	0X0000,0X0000,0X0000,0X00FF,0X00FF,0X0000,0X0100,0X00FF,
	0X00FF,0X0000,0X012C,0X002C,0X002C,0X002C,0X002C,0X0000,
	0X0032,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
	0X0038,0X000B,0X0032,0X0000,0X0008,0X000C,0X0093,0X00E9,
	0X0000,0X0000,0X0000,0X0000,0X0003,0X0010,0X0010,0X0100,
	0X0100,0X0002,0X0001,0X0001,0X0039,0X0039,0X0039,0X0039,
	0X0001,0X0001
};
//WM8978��ʼ��
//����ֵ:0,��ʼ������
//    ����,�������
u8 WM8978_Init(void)
{
	u8 res;

	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);			//ʹ������GPIOB,GPIOCʱ��
     
	//PB12/13 ���ù������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//���� 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	
	//PC2/PC3/PC6���ù������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3|GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//���� 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_SPI2);  //PB12,AF5  I2S_LRCK------LRC
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);	//PB13,AF5  I2S_SCLK------BCLK 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource2,GPIO_AF6_SPI2);	//PC2 ,AF6  I2S_SDOUT-----ADCDAT 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource3,GPIO_AF_SPI2);	  //PC3 ,AF5  I2S_SDIN------DACDAT
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_SPI2);	  //PC6 ,AF5  I2S_MCLK------MCLK

 
	
	IIC_Init();//��ʼ��IIC�ӿ�
	res=WM8978_Write_Reg(0,0);	//��λWM8978
	if(res)return 1;			//����ָ��ʧ��,WM8978�쳣
	//����Ϊͨ������
	WM8978_Write_Reg(1,0X1B);	//R1,MICEN����Ϊ1(MICʹ��),BIASEN����Ϊ1(ģ��������),VMIDSEL[1:0]����Ϊ:11(5K)
	WM8978_Write_Reg(2,0X1B0);	//R2,ROUT1,LOUT1���ʹ��(�������Թ���),BOOSTENR,BOOSTENLʹ��
	WM8978_Write_Reg(3,0X6C);	//R3,LOUT2,ROUT2���ʹ��(���ȹ���),RMIX,LMIXʹ��	
	WM8978_Write_Reg(6,0);		//R6,MCLK���ⲿ�ṩ
	WM8978_Write_Reg(43,1<<4);	//R43,INVROUT2����,��������
	WM8978_Write_Reg(47,1<<8);	//R47����,PGABOOSTL,��ͨ��MIC���20������
	WM8978_Write_Reg(48,1<<8);	//R48����,PGABOOSTR,��ͨ��MIC���20������
	WM8978_Write_Reg(49,1<<1);	//R49,TSDEN,�������ȱ��� 
	WM8978_Write_Reg(10,1<<3);	//R10,SOFTMUTE�ر�,128x����,���SNR 
	WM8978_Write_Reg(14,1<<3);	//R14,ADC 128x������
	return 0;
} 
u8 WM8978_Init2(void)
{
	u8 res;

	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);			//ʹ������GPIOB,GPIOCʱ��
     
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4| GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//���� 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//���� 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//���� 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_SPI3); //PA4,AF6  I2S_LRCK--------LRC
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI3);	//PB3,AF6  I2S_SCLK--------BCLK
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF7_SPI3);//PB4 ,AF7  I2S_SDOUT------ADCDAT
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI3);	//PB5 ,AF6  I2S_SDIN-------DACDAT
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_SPI3);	//PC7 ,AF6  I2S_MCLK-------MCLK
 
	
	IIC2_Init();//��ʼ��IIC�ӿ�
	res=WM8978_Write2_Reg(0,0);	//��λWM8978
	if(res)return 1;			//����ָ��ʧ��,WM8978�쳣
	//����Ϊͨ������
	WM8978_Write2_Reg(1,0X1B);	//R1,MICEN����Ϊ1(MICʹ��),BIASEN����Ϊ1(ģ��������),VMIDSEL[1:0]����Ϊ:11(5K)
//	WM8978_Write2_Reg(1,0X3B);	//R1,MICEN����Ϊ1(MICʹ��),BIASEN����Ϊ1(ģ��������),VMIDSEL[1:0]����Ϊ:11(5K)
	WM8978_Write2_Reg(2,0X1B0);	//R2,ROUT1,LOUT1���ʹ��(�������Թ���),BOOSTENR,BOOSTENLʹ��
	WM8978_Write2_Reg(3,0X6C);	//R3,LOUT2,ROUT2���ʹ��(���ȹ���),RMIX,LMIXʹ��	
		
	WM8978_Write2_Reg(6,9);		//4��Ƶ
//	WM8978_Write2_Reg(6,13);		//8��Ƶ

	WM8978_Write2_Reg(43,1<<4);	//R43,INVROUT2����,��������
	WM8978_Write2_Reg(47,1<<8);	//R47����,PGABOOSTL,��ͨ��MIC���20������
	WM8978_Write2_Reg(48,1<<8);	//R48����,PGABOOSTR,��ͨ��MIC���20������
	WM8978_Write2_Reg(49,1<<1);	//R49,TSDEN,�������ȱ��� 
	WM8978_Write2_Reg(10,1<<3);	//R10,SOFTMUTE�ر�,128x����,���SNR 
	WM8978_Write2_Reg(14,1<<3);	//R14,ADC 128x������
	return 0;
}
//WM8978д�Ĵ���
//reg:�Ĵ�����ַ
//val:Ҫд��Ĵ�����ֵ 
//����ֵ:0,�ɹ�;
//    ����,�������
u8 WM8978_Write_Reg(u8 reg,u16 val)
{ 
	IIC_Start(); 
	IIC_Send_Byte((WM8978_ADDR<<1)|0);//����������ַ+д����	 
	if(IIC_Wait_Ack())return 1;	//�ȴ�Ӧ��(�ɹ�?/ʧ��?) 
    IIC_Send_Byte((reg<<1)|((val>>8)&0X01));//д�Ĵ�����ַ+���ݵ����λ
	if(IIC_Wait_Ack())return 2;	//�ȴ�Ӧ��(�ɹ�?/ʧ��?) 
	IIC_Send_Byte(val&0XFF);	//��������
	if(IIC_Wait_Ack())return 3;	//�ȴ�Ӧ��(�ɹ�?/ʧ��?) 
    IIC_Stop();
	WM8978_REGVAL_TBL[reg]=val;	//����Ĵ���ֵ������
	return 0;	
}  
u8 WM8978_Write2_Reg(u8 reg,u16 val)
{ 
	IIC2_Start(); 
	IIC2_Send_Byte((WM8978_ADDR<<1)|0);//����������ַ+д����	 
	if(IIC2_Wait_Ack())return 1;	//�ȴ�Ӧ��(�ɹ�?/ʧ��?) 
    IIC2_Send_Byte((reg<<1)|((val>>8)&0X01));//д�Ĵ�����ַ+���ݵ����λ
	if(IIC2_Wait_Ack())return 2;	//�ȴ�Ӧ��(�ɹ�?/ʧ��?) 
	IIC2_Send_Byte(val&0XFF);	//��������
	if(IIC2_Wait_Ack())return 3;	//�ȴ�Ӧ��(�ɹ�?/ʧ��?) 
    IIC2_Stop();
	WM8978_REGVAL2_TBL[reg]=val;	//����Ĵ���ֵ������
	return 0;	
} 
//WM8978���Ĵ���
//���Ƕ�ȡ���ؼĴ���ֵ�������ڵĶ�Ӧֵ
//reg:�Ĵ�����ַ 
//����ֵ:�Ĵ���ֵ
u16 WM8978_Read_Reg(u8 reg)
{  
	return WM8978_REGVAL_TBL[reg];	
} 
u16 WM8978_Read2_Reg(u8 reg)
{  
	return WM8978_REGVAL2_TBL[reg];	
} 
//WM8978 DAC/ADC����
//adcen:adcʹ��(1)/�ر�(0)
//dacen:dacʹ��(1)/�ر�(0)
void WM8978_ADDA_Cfg(u8 dacen,u8 adcen)
{
	u16 regval;
	regval=WM8978_Read_Reg(3);	//��ȡR3
	if(dacen)regval|=3<<0;		//R3���2��λ����Ϊ1,����DACR&DACL
	else regval&=~(3<<0);		//R3���2��λ����,�ر�DACR&DACL.
	WM8978_Write_Reg(3,regval);	//����R3
	regval=WM8978_Read_Reg(2);	//��ȡR2
	if(adcen)regval|=3<<0;		//R2���2��λ����Ϊ1,����ADCR&ADCL
	else regval&=~(3<<0);		//R2���2��λ����,�ر�ADCR&ADCL.
	WM8978_Write_Reg(2,regval);	//����R2	
}
void WM8978_ADDA2_Cfg(u8 dacen,u8 adcen)
{
	u16 regval;
	regval=WM8978_Read2_Reg(3);	//��ȡR3
	if(dacen)regval|=3<<0;		//R3���2��λ����Ϊ1,����DACR&DACL
	else regval&=~(3<<0);		//R3���2��λ����,�ر�DACR&DACL.
	WM8978_Write2_Reg(3,regval);	//����R3
	regval=WM8978_Read2_Reg(2);	//��ȡR2
	if(adcen)regval|=3<<0;		//R2���2��λ����Ϊ1,����ADCR&ADCL
	else regval&=~(3<<0);		//R2���2��λ����,�ر�ADCR&ADCL.
	WM8978_Write2_Reg(2,regval);	//����R2	
}
//WM8978 ����ͨ������ 
//micen:MIC����(1)/�ر�(0)
//lineinen:Line In����(1)/�ر�(0)
//auxen:aux����(1)/�ر�(0) 
void WM8978_Input_Cfg(u8 micen,u8 lineinen,u8 auxen)
{
	u16 regval;  
	regval=WM8978_Read_Reg(2);	//��ȡR2
	if(micen)regval|=3<<2;		//����INPPGAENR,INPPGAENL(MIC��PGA�Ŵ�)
	else regval&=~(3<<2);		//�ر�INPPGAENR,INPPGAENL.
 	WM8978_Write_Reg(2,regval);	//����R2 
	
	regval=WM8978_Read_Reg(44);	//��ȡR44
	if(micen)regval|=3<<4|3<<0;	//����LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	else regval&=~(3<<4|3<<0);	//�ر�LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	WM8978_Write_Reg(44,regval);//����R44
	
	if(lineinen)WM8978_LINEIN_Gain(5);//LINE IN 0dB����
	else WM8978_LINEIN_Gain(0);	//�ر�LINE IN
	if(auxen)WM8978_AUX_Gain(7);//AUX 6dB����
	else WM8978_AUX_Gain(0);	//�ر�AUX����  
}
void WM8978_Input2_Cfg(u8 micen,u8 lineinen,u8 auxen)
{
	u16 regval;  
	regval=WM8978_Read2_Reg(2);	//��ȡR2
	if(micen)regval|=3<<2;		//����INPPGAENR,INPPGAENL(MIC��PGA�Ŵ�)
	else regval&=~(3<<2);		//�ر�INPPGAENR,INPPGAENL.
 	WM8978_Write2_Reg(2,regval);	//����R2 
	
	regval=WM8978_Read2_Reg(44);	//��ȡR44
	if(micen)regval|=3<<4|3<<0;	//����LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	else regval&=~(3<<4|3<<0);	//�ر�LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	WM8978_Write2_Reg(44,regval);//����R44
	
	if(lineinen)WM8978_LINEIN2_Gain(5);//LINE IN 0dB����
	else WM8978_LINEIN2_Gain(0);	//�ر�LINE IN
	if(auxen)WM8978_AUX2_Gain(7);//AUX 6dB����
	else WM8978_AUX2_Gain(0);	//�ر�AUX����  
}
//WM8978 ������� 
//dacen:DAC���(����)����(1)/�ر�(0)
//bpsen:Bypass���(¼��,����MIC,LINE IN,AUX��)����(1)/�ر�(0) 
void WM8978_Output_Cfg(u8 dacen,u8 bpsen)
{
	u16 regval=0;
	if(dacen)regval|=1<<0;	//DAC���ʹ��
	if(bpsen)
	{
		regval|=1<<1;		//BYPASSʹ��
		regval|=5<<2;		//0dB����
	} 
	WM8978_Write_Reg(50,regval);//R50����
	WM8978_Write_Reg(51,regval);//R51���� 
}
void WM8978_Output2_Cfg(u8 dacen,u8 bpsen)
{
	u16 regval=0;
	if(dacen)regval|=1<<0;	//DAC���ʹ��
	if(bpsen)
	{
		regval|=1<<1;		//BYPASSʹ��
		regval|=5<<2;		//0dB����
	} 
	WM8978_Write2_Reg(50,regval);//R50����
	WM8978_Write2_Reg(51,regval);//R51���� 
}
//WM8978 MIC��������(������BOOST��20dB,MIC-->ADC���벿�ֵ�����)
//gain:0~63,��Ӧ-12dB~35.25dB,0.75dB/Step
void WM8978_MIC_Gain(u8 gain)
{
	gain&=0X3F;
	WM8978_Write_Reg(45,gain);		//R45,��ͨ��PGA���� 
	WM8978_Write_Reg(46,gain|1<<8);	//R46,��ͨ��PGA����
}
void WM8978_MIC2_Gain(u8 gain)
{
	gain&=0X3F;
	WM8978_Write2_Reg(45,gain);		//R45,��ͨ��PGA���� 
	WM8978_Write2_Reg(46,gain|1<<8);	//R46,��ͨ��PGA����
}
//WM8978 L2/R2(Ҳ����Line In)��������(L2/R2-->ADC���벿�ֵ�����)
//gain:0~7,0��ʾͨ����ֹ,1~7,��Ӧ-12dB~6dB,3dB/Step
void WM8978_LINEIN_Gain(u8 gain)
{
	u16 regval;
	gain&=0X07;
	regval=WM8978_Read_Reg(47);	//��ȡR47
	regval&=~(7<<4);			//���ԭ�������� 
 	WM8978_Write_Reg(47,regval|gain<<4);//����R47
	regval=WM8978_Read_Reg(48);	//��ȡR48
	regval&=~(7<<4);			//���ԭ�������� 
 	WM8978_Write_Reg(48,regval|gain<<4);//����R48
} 
void WM8978_LINEIN2_Gain(u8 gain)
{
	u16 regval;
	gain&=0X07;
	regval=WM8978_Read2_Reg(47);	//��ȡR47
	regval&=~(7<<4);			//���ԭ�������� 
 	WM8978_Write2_Reg(47,regval|gain<<4);//����R47
	regval=WM8978_Read2_Reg(48);	//��ȡR48
	regval&=~(7<<4);			//���ԭ�������� 
 	WM8978_Write2_Reg(48,regval|gain<<4);//����R48
}
//WM8978 AUXR,AUXL(PWM��Ƶ����)��������(AUXR/L-->ADC���벿�ֵ�����)
//gain:0~7,0��ʾͨ����ֹ,1~7,��Ӧ-12dB~6dB,3dB/Step
void WM8978_AUX_Gain(u8 gain)
{
	u16 regval;
	gain&=0X07;
	regval=WM8978_Read_Reg(47);	//��ȡR47
	regval&=~(7<<0);			//���ԭ�������� 
 	WM8978_Write_Reg(47,regval|gain<<0);//����R47
	regval=WM8978_Read_Reg(48);	//��ȡR48
	regval&=~(7<<0);			//���ԭ�������� 
 	WM8978_Write_Reg(48,regval|gain<<0);//����R48
} 
void WM8978_AUX2_Gain(u8 gain)
{
	u16 regval;
	gain&=0X07;
	regval=WM8978_Read2_Reg(47);	//��ȡR47
	regval&=~(7<<0);			//���ԭ�������� 
 	WM8978_Write2_Reg(47,regval|gain<<0);//����R47
	regval=WM8978_Read2_Reg(48);	//��ȡR48
	regval&=~(7<<0);			//���ԭ�������� 
 	WM8978_Write2_Reg(48,regval|gain<<0);//����R48
}
//����I2S����ģʽ
//fmt:0,LSB(�Ҷ���);1,MSB(�����);2,�����ֱ�׼I2S;3,PCM/DSP;
//len:0,16λ;1,20λ;2,24λ;3,32λ;  
void WM8978_I2S_Cfg(u8 fmt,u8 len)
{
	fmt&=0X03;
	len&=0X03;//�޶���Χ
	WM8978_Write_Reg(4,(fmt<<3)|(len<<5));	//R4,WM8978����ģʽ����	
}	
void WM8978_I2S3_Cfg(u8 fmt,u8 len)
{
	fmt&=0X03;
	len&=0X03;//�޶���Χ
	WM8978_Write2_Reg(4,(fmt<<3)|(len<<5));	//R4,WM8978����ģʽ����	
}

//���ö���������������
//voll:����������(0~63)
//volr:����������(0~63)
void WM8978_HPvol_Set(u8 voll,u8 volr)
{
	voll&=0X3F;
	volr&=0X3F;//�޶���Χ
	if(voll==0)voll|=1<<6;//����Ϊ0ʱ,ֱ��mute
	if(volr==0)volr|=1<<6;//����Ϊ0ʱ,ֱ��mute 
	WM8978_Write_Reg(52,voll);			//R52,������������������
	WM8978_Write_Reg(53,volr|(1<<8));	//R53,������������������,ͬ������(HPVU=1)
}
void WM8978_HPvol2_Set(u8 voll,u8 volr)
{
	voll&=0X3F;
	volr&=0X3F;//�޶���Χ
	if(voll==0)voll|=1<<6;//����Ϊ0ʱ,ֱ��mute
	if(volr==0)volr|=1<<6;//����Ϊ0ʱ,ֱ��mute 
	WM8978_Write2_Reg(52,voll);			//R52,������������������
	WM8978_Write2_Reg(53,volr|(1<<8));	//R53,������������������,ͬ������(HPVU=1)
}
//������������
//voll:����������(0~63) 
void WM8978_SPKvol_Set(u8 volx)
{ 
	volx&=0X3F;//�޶���Χ
	if(volx==0)volx|=1<<6;//����Ϊ0ʱ,ֱ��mute 
 	WM8978_Write_Reg(54,volx);			//R54,������������������
	WM8978_Write_Reg(55,volx|(1<<8));	//R55,������������������,ͬ������(SPKVU=1)	
}
void WM8978_SPKvol2_Set(u8 volx)
{ 
	volx&=0X3F;//�޶���Χ
	if(volx==0)volx|=1<<6;//����Ϊ0ʱ,ֱ��mute 
 	WM8978_Write2_Reg(54,volx);			//R54,������������������
	WM8978_Write2_Reg(55,volx|(1<<8));	//R55,������������������,ͬ������(SPKVU=1)	
}













