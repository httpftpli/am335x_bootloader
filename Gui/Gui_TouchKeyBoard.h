#ifndef __Gui_TouchKeyBoard_H
#define __Gui_TouchKeyBoard_H

#define C_ButtonUp1			0xF7BE			//触摸按钮正常状态下上半部的颜色
#define C_ButtonUp2			0xE75D			//触摸按钮正常状态下下半部的颜色
#define C_ButtonDown			0x63D0			//触摸按钮按住状态下的颜色
#define C_ButtonYinYing		0				//触摸按钮阴影的颜色
#define C_ButtonBack			0x1EA			//触摸按钮容器的背景色
#define C_ButtonSide			0x21C4			//触摸键盘边框的颜色

#define MAX_TKeyBoard		6				//模拟触摸键盘的个数

#define TKEY_BoardStand		1				//标准输入触摸键盘	
#define TKEY_BoardStandMove	2				//标准输入触摸键盘(带左右键)
#define TKEY_BoardPara		3				//参数输入触摸键盘(带复制键)
#define TKEY_BoardParaSet	4				//参数输入触摸键盘(带当前值设定键)
#define TKEY_BoardAsc		5				//输入触摸键盘(带字符A~F)
#define TKEY_BoardTest		6				//测试触摸键盘(带自动测试)

void Dis_TouchButton(const char *hz,uint32 x,uint32 y,uint32 width,uint32 height,uint16 key,uint16 num,uint32 color,uint16 font);
void Dis_TouchKeyBoard(uint32 hint);
void TouchKeyBoardClose(void);
void Dis_TouchButtonFlash(void);
void Dis_IconUp(uint32 x,uint32 y,uint32 width,uint32 height,uint16 *bmp);
void Dis_IconDown(uint32 x,uint32 y,uint32 width,uint32 height,uint16 *bmp);
void Dis_TouchIconRef(void);

#endif

