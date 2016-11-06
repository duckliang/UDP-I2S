#ifndef __WM8978_H
#define __WM8978_H
#include "sys.h"    										
 
 
//如果AD0脚(4脚)接地,IIC地址为0X4A(不包含最低位).
//如果接V3.3,则IIC地址为0X4B(不包含最低位).
#define WM8978_ADDR				0X1A	//WM8978的器件地址,固定为0X1A 

  
u8 WM8978_Init(void); 
void WM8978_ADDA_Cfg(u8 dacen,u8 adcen);
void WM8978_Input_Cfg(u8 micen,u8 lineinen,u8 auxen);
void WM8978_Output_Cfg(u8 dacen,u8 bpsen);
void WM8978_MIC_Gain(u8 gain);
void WM8978_LINEIN_Gain(u8 gain);
void WM8978_AUX_Gain(u8 gain);
u8 WM8978_Write_Reg(u8 reg,u16 val); 
u16 WM8978_Read_Reg(u8 reg);
void WM8978_HPvol_Set(u8 voll,u8 volr);
void WM8978_SPKvol_Set(u8 volx);
void WM8978_I2S_Cfg(u8 fmt,u8 len);

u8 WM8978_Init2(void); 
void WM8978_ADDA2_Cfg(u8 dacen,u8 adcen);
void WM8978_Input2_Cfg(u8 micen,u8 lineinen,u8 auxen);
void WM8978_Output2_Cfg(u8 dacen,u8 bpsen);
void WM8978_MIC2_Gain(u8 gain);
void WM8978_LINEIN2_Gain(u8 gain);
void WM8978_AUX2_Gain(u8 gain);
u8 WM8978_Write2_Reg(u8 reg,u16 val); 
u16 WM8978_Read2_Reg(u8 reg);
void WM8978_HPvol2_Set(u8 voll,u8 volr);
void WM8978_SPKvol2_Set(u8 volx);
void WM8978_I2S3_Cfg(u8 fmt,u8 len);


#endif





















