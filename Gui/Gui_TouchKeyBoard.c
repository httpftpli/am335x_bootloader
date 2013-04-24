/********************************************************************
功能:				触摸屏的模拟键盘
修改时间:		2013-3-6
********************************************************************/
#include "Head.h"

//===================================================================
//功能:			显示一个正常的命令按钮
//参数:
//返回:
//说明:
//修改时间:		2013-1-23
//===================================================================
void Dis_TouchButtonUp(const char *str,uint16 x,uint16 y,uint16 width,uint16 height,uint32 color,uint32 font)
{
uint32 len;
	Dis_CircleRectFillShade(x,y,width,height,6,C_ButtonUp1,C_ButtonUp2);

	//计算字符显示的起始x坐标-----------
	len=strlen(str);
	len=len*(font>>1);
	if(width>len)
	{
		x=x+((width-len)>>1);
	}
	
	//计算字符显示的起始y坐标-----------
	y=y+((height-font)>>1);
	
	Dis_StringsFilter(str,x,y,font,color);
}

//===================================================================
//功能:			显示一个按住的命令按钮
//参数:
//返回:
//说明:
//修改时间:		2013-1-23
//===================================================================
void Dis_TouchButtonDown(const char *str,uint16 x,uint16 y,uint16 width,uint16 height,uint32 font)
{
uint32 len;
	Dis_CircleRectFill(x,y,width,height,6,C_ButtonDown);

	//计算字符显示的起始x坐标-----------
	len=strlen(str);
	len=len*(font>>1);
	if(width>len) 
	{
		x=x+((width-len)>>1);
	}
	
	//计算字符显示的起始y坐标-----------
	y=y+((height-font)>>1);
	
	Dis_StringsFilter(str,x,y,font,C_Yellow);
}

//===================================================================
//功能: 			显示一个触摸按钮并定义触摸键值
//入口参数:
//		hz:			按钮上显示的字符
//		key:			触摸区域对应的键值
//		color: 		字符的颜色
//		num:		触摸键下标号
//修改时间:		2013-1-28
//===================================================================
void Dis_TouchButton(const char *hz,uint32 x,uint32 y,uint32 width,uint32 height,uint16 key,uint16 num,uint32 color,uint16 font)
{
	KeyT.Status=KEY_No;
	KeyT.DisStatus=KEY_No;
	KeyT.Key=KEY_No;

	TchKey[0][num].x=x;		
	TchKey[0][num].y=y;
	TchKey[0][num].width=width;
	TchKey[0][num].height=height;
	TchKey[0][num].key=key;
	TchKey[0][num].Flash=1;
	TchKey[0][num].String=hz;
	TchKey[0][num].Font=font;

	Dis_CircleRectFill(TchKey[0][num].x-1,TchKey[0][num].y+2,TchKey[0][num].width+3,TchKey[0][num].height,6,C_ButtonYinYing);
	Dis_TouchButtonUp(hz,x,y,width,height,color,font);
}

//===================================================================
//功能:			显示一个模拟键盘
//参数:
//返回:
//说明:			标准模拟键盘
//修改时间:		2013-3-6
//===================================================================
void Dis_TKeyBoardStandard(void)
{
const char *Hz_KeyBoard[][LANG_MAX]=
{
	"1","1","2","2","3","3",
	"4","4","5","5","6","6",
	"7","7","8","8","9","9",
	"+/-","+/-","0","0",".",".",
	"退格","Del","取消","Esc","确定","Ent",
};
enum 
{
	FONT=FONT24,
	MAX=15,
	WIDTH=290,HEIGHT=185,
	START_X=5,START_Y=5,
	KEY_XJG=65,KEY_YJG=45,KEYF_YJG=60,
	KEY_W=60,KEYF_W=85,KEY_H=40,KEYF_H=55
};
uint32 tk[MAX]=
{
	KEY_1,KEY_2,KEY_3,
	KEY_4,KEY_5,KEY_6,
	KEY_7,KEY_8,KEY_9,
	KEY_ZF,KEY_0,KEY_POINT,
	KEY_QUK,KEY_ESC,KEY_OK
};
uint32 i;
uint16 x,y;
	Touch_BoxGetXY(WIDTH,HEIGHT,FIRST_Down);
	Touch_KeyPush();

	LcdMemPush(TchVal.PicX,TchVal.PicY,WIDTH,HEIGHT);
	Dis_CircleRectFill(TchVal.PicX,TchVal.PicY,WIDTH,HEIGHT,6,C_ButtonBack);
	Dis_CircleRect(TchVal.PicX,TchVal.PicY,WIDTH,HEIGHT,6,C_ButtonSide);

	//触摸键定义-------------
	for(i=0;i<MAX;i++)
	{
		//前面4行的显示位置-----
		if(i<12)
		{
			x=START_X+KEY_XJG*(i%3);
			y=START_Y+KEY_YJG*(i/3);
		}
		else
		{
			x=START_X+KEY_XJG*3;
			y=START_Y+KEYF_YJG*(i-12);			
		}

		TchKey[0][i].x=TchVal.PicX+x;		
		TchKey[0][i].y=TchVal.PicY+y;
		TchKey[0][i].height=KEY_H;
		TchKey[0][i].key=tk[i];
		TchKey[0][i].Flash=1;
		TchKey[0][i].String=Hz_KeyBoard[i][Lang];
		TchKey[0][i].Font=FONT;

		if(i<12)
		{
			TchKey[0][i].width=KEY_W;
		}
		else
		{
			TchKey[0][i].width=KEYF_W;
			TchKey[0][i].height=KEYF_H;
		}

		Dis_CircleRectFill(TchKey[0][i].x-1,TchKey[0][i].y+3,TchKey[0][i].width+4,TchKey[0][i].height,6,C_ButtonYinYing);
		Dis_TouchButtonUp(TchKey[0][i].String,TchKey[0][i].x,TchKey[0][i].y,TchKey[0][i].width,TchKey[0][i].height,C_Black,FONT);
	}
	
	TchVal.ButtonMax[0]=MAX;
	TchVal.KeyNum[0]=MAX;		//允许使用的键数(前面的优先)
}

//===================================================================
//功能:			显示一个模拟键盘
//参数:
//返回:
//说明:			标准模拟键盘(带左右键)
//修改时间:		2013-3-6
//===================================================================
void Dis_TKeyBoardStandardMove(void)
{
const char *Hz_KeyBoardPara[][LANG_MAX]=
{
	"1","1","2","2","3","3",
	"4","4","5","5","6","6",
	"7","7","8","8","9","9",
	"+/-","+/-","0","0",".",".",
	"←","←","→","→",
	"退格","Del","取消","Esc","确定","Ent",
};
enum 
{
	FONT=FONT24,
	MAX=17,
	WIDTH=305,HEIGHT=185,
	START_X=5,START_Y=5,
	KEY_XJG=65,KEY_YJG=45,
	KEY_W=60,KEYF_W=100,KEY_H=40
};
uint32 tk[MAX]=
{
	KEY_1,KEY_2,KEY_3,
	KEY_4,KEY_5,KEY_6,
	KEY_7,KEY_8,KEY_9,
	KEY_ZF,KEY_0,KEY_POINT,
	KEY_LEFT,KEY_RIGHT,
	KEY_QUK,KEY_ESC,KEY_OK
};
uint32 i;
uint16 x,y;
	Touch_BoxGetXY(WIDTH,HEIGHT,FIRST_Down);
	Touch_KeyPush();

	LcdMemPush(TchVal.PicX,TchVal.PicY,WIDTH,HEIGHT);
	Dis_CircleRectFill(TchVal.PicX,TchVal.PicY,WIDTH,HEIGHT,6,C_ButtonBack);
	Dis_CircleRect(TchVal.PicX,TchVal.PicY,WIDTH,HEIGHT,6,C_ButtonSide);

	//触摸键定义-------------
	for(i=0;i<MAX;i++)
	{
		//前面4行的显示位置-----
		if(i<12)
		{
			x=START_X+KEY_XJG*(i%3);
			y=START_Y+KEY_YJG*(i/3);
		}
		else
		{
			if(i==13) x=START_X+KEY_XJG*3+KEYF_W/2+1;		//右移键
			else x=START_X+KEY_XJG*3;

			if(i<14) y=START_Y;
			else y=START_Y+KEY_YJG*(i-13);
		}

		TchKey[0][i].x=TchVal.PicX+x;		
		TchKey[0][i].y=TchVal.PicY+y;
		TchKey[0][i].height=KEY_H;
		TchKey[0][i].key=tk[i];
		TchKey[0][i].Flash=1;
		TchKey[0][i].String=Hz_KeyBoardPara[i][Lang];
		TchKey[0][i].Font=FONT;

		if(i<12)
		{
			TchKey[0][i].width=KEY_W;
		}
		else
		{
			if(i<14) TchKey[0][i].width=KEYF_W/2-2;
			else TchKey[0][i].width=KEYF_W;
		}

		Dis_CircleRectFill(TchKey[0][i].x-1,TchKey[0][i].y+2,TchKey[0][i].width+3,TchKey[0][i].height,6,C_ButtonYinYing);
		Dis_TouchButtonUp(TchKey[0][i].String,TchKey[0][i].x,TchKey[0][i].y,TchKey[0][i].width,TchKey[0][i].height,C_Black,FONT);
	}
	
	TchVal.ButtonMax[0]=MAX;
	TchVal.KeyNum[0]=MAX;		//允许使用的键数(前面的优先)
}

//===================================================================
//功能:			显示一个模拟键盘
//参数:
//返回:
//说明:			供参数输入调用(带复制键)
//修改时间:		2013-3-6
//===================================================================
void Dis_TKeyBoardPara(void)
{
const char *Hz_KeyBoardPara[][LANG_MAX]=
{
	"1","1","2","2","3","3",
	"4","4","5","5","6","6",
	"7","7","8","8","9","9",
	"+/-","+/-","0","0",".",".",
	"退格","Del","复制","Copy","取消","Esc","确定","Ent",
};
enum 
{
	FONT=FONT24,
	MAX=16,
	WIDTH=295,HEIGHT=185,
	START_X=5,START_Y=5,
	KEY_XJG=65,KEY_YJG=45,
	KEY_W=60,KEYF_W=90,KEY_H=40
};
uint32 tk[MAX]=
{
	KEY_1,KEY_2,KEY_3,
	KEY_4,KEY_5,KEY_6,
	KEY_7,KEY_8,KEY_9,
	KEY_ZF,KEY_0,KEY_POINT,
	KEY_QUK,KEY_CE,KEY_ESC,KEY_OK
};
uint32 i;
uint16 x,y;
	Touch_BoxGetXY(WIDTH,HEIGHT,FIRST_Down);
	Touch_KeyPush();

	LcdMemPush(TchVal.PicX,TchVal.PicY,WIDTH,HEIGHT);
	Dis_CircleRectFill(TchVal.PicX,TchVal.PicY,WIDTH,HEIGHT,6,C_ButtonBack);
	Dis_CircleRect(TchVal.PicX,TchVal.PicY,WIDTH,HEIGHT,6,C_ButtonSide);

	//触摸键定义-------------
	for(i=0;i<MAX;i++)
	{
		//前面4行的显示位置-----
		if(i<12)
		{
			x=START_X+KEY_XJG*(i%3);
			y=START_Y+KEY_YJG*(i/3);
		}
		else
		{
			x=START_X+KEY_XJG*3;
			y=START_Y+KEY_YJG*(i-12);			
		}

		TchKey[0][i].x=TchVal.PicX+x;		
		TchKey[0][i].y=TchVal.PicY+y;
		TchKey[0][i].height=KEY_H;
		TchKey[0][i].key=tk[i];
		TchKey[0][i].Flash=1;
		TchKey[0][i].String=Hz_KeyBoardPara[i][Lang];
		TchKey[0][i].Font=FONT;

		if(i<12)
		{
			TchKey[0][i].width=KEY_W;
		}
		else
		{
			TchKey[0][i].width=KEYF_W;
		}

		Dis_CircleRectFill(TchKey[0][i].x-1,TchKey[0][i].y+2,TchKey[0][i].width+3,TchKey[0][i].height,6,C_ButtonYinYing);
		Dis_TouchButtonUp(TchKey[0][i].String,TchKey[0][i].x,TchKey[0][i].y,TchKey[0][i].width,TchKey[0][i].height,C_Black,FONT);
	}
	
	TchVal.ButtonMax[0]=MAX;
	TchVal.KeyNum[0]=MAX;		//允许使用的键数(前面的优先)
}

//===================================================================
//功能:			显示一个模拟键盘
//参数:
//返回:
//说明:			供参数输入调用(带当前值设定键)
//修改时间:		2013-1-24
//===================================================================
void Dis_TKeyBoardParaSet(void)
{
const char *Hz_KeyBoardPara[][LANG_MAX]=
{
	"1","1","2","2","3","3",
	"4","4","5","5","6","6",
	"7","7","8","8","9","9",
	"+/-","+/-","0","0",".",".",
	"退格","Del","设定","Set","取消","Esc","确定","Ent",
};
enum 
{
	FONT=FONT24,
	MAX=16,
	WIDTH=295,HEIGHT=185,
	START_X=5,START_Y=5,
	KEY_XJG=65,KEY_YJG=45,
	KEY_W=60,KEYF_W=90,KEY_H=40
};
uint32 tk[MAX]=
{
	KEY_1,KEY_2,KEY_3,
	KEY_4,KEY_5,KEY_6,
	KEY_7,KEY_8,KEY_9,
	KEY_ZF,KEY_0,KEY_POINT,
	KEY_QUK,KEY_F1,KEY_ESC,KEY_OK
};

uint32 i;
uint16 x,y;
	Touch_BoxGetXY(WIDTH,HEIGHT,FIRST_Down);
	Touch_KeyPush();

	LcdMemPush(TchVal.PicX,TchVal.PicY,WIDTH,HEIGHT);
	Dis_CircleRectFill(TchVal.PicX,TchVal.PicY,WIDTH,HEIGHT,6,C_ButtonBack);
	Dis_CircleRect(TchVal.PicX,TchVal.PicY,WIDTH,HEIGHT,6,C_ButtonSide);

	//触摸键定义-------------
	for(i=0;i<MAX;i++)
	{
		//前面4行的显示位置-----
		if(i<12)
		{
			x=START_X+KEY_XJG*(i%3);
			y=START_Y+KEY_YJG*(i/3);
		}
		else
		{
			x=START_X+KEY_XJG*3;
			y=START_Y+KEY_YJG*(i-12);				
		}

		TchKey[0][i].x=TchVal.PicX+x;		
		TchKey[0][i].y=TchVal.PicY+y;
		TchKey[0][i].height=KEY_H;
		TchKey[0][i].key=tk[i];
		TchKey[0][i].Flash=1;
		TchKey[0][i].String=Hz_KeyBoardPara[i][Lang];
		TchKey[0][i].Font=FONT;
		
		if(i<12)
		{
			TchKey[0][i].width=KEY_W;
		}
		else
		{
			TchKey[0][i].width=KEYF_W;
		}

		Dis_CircleRectFill(TchKey[0][i].x-1,TchKey[0][i].y+2,TchKey[0][i].width+3,TchKey[0][i].height,6,C_ButtonYinYing);
		Dis_TouchButtonUp(TchKey[0][i].String,TchKey[0][i].x,TchKey[0][i].y,TchKey[0][i].width,TchKey[0][i].height,C_Black,FONT);
	}
	
	TchVal.ButtonMax[0]=MAX;
	TchVal.KeyNum[0]=MAX;		//允许使用的键数(前面的优先)
}

//===================================================================
//功能:			显示一个模拟键盘
//参数:
//返回:
//说明:			供参数输入调用
//修改时间:		2013-1-24
//===================================================================
void Dis_TKeyBoardPassWord(void)
{
const char *Hz_KeyBoardPara[][LANG_MAX]=
{
	"1","1","2","2","3","3","A","A","B","B",
	"4","4","5","5","6","6","C","C","D","D",
	"7","7","8","8","9","9","E","E","F","F",
	"0","0","退格","Del","取消","Esc","确定","Ent",
};
enum 
{
	FONT=FONT24,
	MAX=19,
	WIDTH=330,HEIGHT=185,
	START_X=5,START_Y=5,
	KEY_XJG=65,KEYF_XJG=81,KEY_YJG=45,
	KEY_W=60,KEYF_W=76,KEY_H=40
};
uint32 tk[MAX]=
{
	KEY_1,KEY_2,KEY_3,KEY_A,KEY_B,
	KEY_4,KEY_5,KEY_6,KEY_C,KEY_D,
	KEY_7,KEY_8,KEY_9,KEY_E,KEY_F,
	KEY_0,KEY_QUK,KEY_ESC,KEY_OK
};
uint32 i;
uint16 x,y;
	Touch_BoxGetXY(WIDTH,HEIGHT,FIRST_Down);
	Touch_KeyPush();

	LcdMemPush(TchVal.PicX,TchVal.PicY,WIDTH,HEIGHT);
	Dis_CircleRectFill(TchVal.PicX,TchVal.PicY,WIDTH,HEIGHT,6,C_ButtonBack);
	Dis_CircleRect(TchVal.PicX,TchVal.PicY,WIDTH,HEIGHT,6,C_ButtonSide);

	//触摸键定义-------------
	for(i=0;i<MAX;i++)
	{
		//前面3行的显示位置-----
		if(i<15)
		{
			x=START_X+KEY_XJG*(i%5);
			y=START_Y+KEY_YJG*(i/5);
		}
		else
		{
			x=START_X+KEYF_XJG*(i-15);
			y=START_Y+KEY_YJG*3;				
		}

		TchKey[0][i].x=TchVal.PicX+x;		
		TchKey[0][i].y=TchVal.PicY+y;
		TchKey[0][i].height=KEY_H;
		TchKey[0][i].key=tk[i];
		TchKey[0][i].Flash=1;
		TchKey[0][i].String=Hz_KeyBoardPara[i][Lang];
		TchKey[0][i].Font=FONT;

		if(i<15)
		{
			TchKey[0][i].width=KEY_W;
		}
		else
		{
			TchKey[0][i].width=KEYF_W;
		}

		Dis_CircleRectFill(TchKey[0][i].x-1,TchKey[0][i].y+2,TchKey[0][i].width+3,TchKey[0][i].height,6,C_ButtonYinYing);
		Dis_TouchButtonUp(TchKey[0][i].String,TchKey[0][i].x,TchKey[0][i].y,TchKey[0][i].width,TchKey[0][i].height,C_Black,FONT);
	}
	
	TchVal.ButtonMax[0]=MAX;
	TchVal.KeyNum[0]=MAX;		//允许使用的键数(前面的优先)
}

//===================================================================
//功能:			显示一个模拟键盘
//参数:
//返回:
//说明:			带自动测试键
//修改时间:		2013-3-6
//===================================================================
void Dis_TKeyBoardTest(void)
{
const char *Hz_KeyBoard[][LANG_MAX]=
{
	"1","1","2","2","3","3",
	"4","4","5","5","6","6",
	"7","7","8","8","9","9",
	"+/-","+/-","0","0",".",".",
	"自动","Auto","退出","Esc",
};
enum 
{
	FONT=FONT24,
	MAX=14,
	WIDTH=290,HEIGHT=185,
	START_X=5,START_Y=5,
	KEY_XJG=65,KEY_YJG=45,KEYF_YJG=90,
	KEY_W=60,KEYF_W=85,KEY_H=40,KEYF_H=85
};
uint32 tk[MAX]=
{
	KEY_1,KEY_2,KEY_3,
	KEY_4,KEY_5,KEY_6,
	KEY_7,KEY_8,KEY_9,
	KEY_ZF,KEY_0,KEY_POINT,
	KEY_QUK,KEY_ESC
};
uint32 i;
uint16 x,y;
	Touch_BoxGetXY(WIDTH,HEIGHT,FIRST_Down);
	Touch_KeyPush();

	LcdMemPush(TchVal.PicX,TchVal.PicY,WIDTH,HEIGHT);
	Dis_CircleRectFill(TchVal.PicX,TchVal.PicY,WIDTH,HEIGHT,6,C_ButtonBack);
	Dis_CircleRect(TchVal.PicX,TchVal.PicY,WIDTH,HEIGHT,6,C_ButtonSide);

	//触摸键定义-------------
	for(i=0;i<MAX;i++)
	{
		//前面4行的显示位置-----
		if(i<12)
		{
			x=START_X+KEY_XJG*(i%3);
			y=START_Y+KEY_YJG*(i/3);
		}
		else
		{
			x=START_X+KEY_XJG*3;
			y=START_Y+KEYF_YJG*(i-12);			
		}

		TchKey[0][i].x=TchVal.PicX+x;		
		TchKey[0][i].y=TchVal.PicY+y;
		TchKey[0][i].height=KEY_H;
		TchKey[0][i].key=tk[i];
		TchKey[0][i].Flash=1;
		TchKey[0][i].String=Hz_KeyBoard[i][Lang];
		TchKey[0][i].Font=FONT;

		if(i<12)
		{
			TchKey[0][i].width=KEY_W;
		}
		else
		{
			TchKey[0][i].width=KEYF_W;
			TchKey[0][i].height=KEYF_H;
		}

		Dis_CircleRectFill(TchKey[0][i].x-1,TchKey[0][i].y+3,TchKey[0][i].width+4,TchKey[0][i].height,6,C_ButtonYinYing);
		Dis_TouchButtonUp(TchKey[0][i].String,TchKey[0][i].x,TchKey[0][i].y,TchKey[0][i].width,TchKey[0][i].height,C_Black,FONT);
	}
	
	TchVal.ButtonMax[0]=MAX;
	TchVal.KeyNum[0]=MAX;		//允许使用的键数(前面的优先)
}

//===================================================================
//功能:			显示一个模拟触摸键盘
//参数:
//		hint:			表示显示哪种类型的触摸键盘
//返回:
//说明:
//修改时间:		2013-3-7
//===================================================================
void Dis_TouchKeyBoard(uint32 hint)
{
	//若触摸屏被关闭或模拟键盘已显示则直接返回-----
	if((Sys.TouchEn!=0x55)||((TchVal.KeyBoard>0)&&(TchVal.KeyBoard<=MAX_TKeyBoard)))
	{
		return;
	}

	TchVal.KeyBoard=hint;
	Key.ReInput=1;
	KeyT.Status=KEY_No;
	KeyT.DisStatus=KEY_No;
	KeyT.Key=KEY_No;
	
	switch(hint)
	{
		case TKEY_BoardStand:
			Dis_TKeyBoardStandard();
			break;
		case TKEY_BoardStandMove:
			Dis_TKeyBoardStandardMove();
			break;
		case TKEY_BoardPara:
			Dis_TKeyBoardPara();
			break;
		case TKEY_BoardParaSet:
			Dis_TKeyBoardParaSet();
			break;
		case TKEY_BoardAsc:
			Dis_TKeyBoardPassWord();
			break;
		case TKEY_BoardTest:
			Dis_TKeyBoardTest();
			break;
		default:
			break;
	}
}

//===================================================================
//功能:			刷新触摸按键的压住及释放状态
//参数:
//		hint:			表示显示哪种类型的
//返回:
//说明:
//修改时间:		2013-3-6
//===================================================================
void TouchKeyBoardClose(void)
{
	//若触摸屏被关闭则直接返回-----
	if(Sys.TouchEn!=0x55)
	{
		return;
	}

	TchVal.KeyBoard=0;
	LcdMemPop();
	Touch_KeyPop();
}

//===================================================================
//功能:			刷新触摸按键的压住及释放状态
//参数:
//返回:
//说明:
//修改时间:		2013-3-6
//===================================================================
void Dis_TouchButtonFlash(void)
{
uint32 i;
	//若触摸屏被关闭则直接返回-----
	if(Sys.TouchEn!=0x55)
	{
		return;
	}

	KeyT.Status=KeyT.Value;

	if(KeyT.Status==KeyT.DisStatus) return;

	i=KeyT.PushIndex;

	if(TchKey[0][i].Flash==0) return;		//若不显示按钮效果则直接返回
	
	if(KeyT.Value!=KEY_No)
	{
		KeyT.Key=KeyT.Value;
		Dis_TouchButtonDown(TchKey[0][i].String,TchKey[0][i].x,TchKey[0][i].y,TchKey[0][i].width,TchKey[0][i].height,TchKey[0][i].Font);			
	}
	else
	{
		Dis_TouchButtonUp(TchKey[0][i].String,TchKey[0][i].x,TchKey[0][i].y,TchKey[0][i].width,TchKey[0][i].height,C_Black,TchKey[0][i].Font);
		KeyT.Key=KEY_No;
	}

	KeyT.DisStatus=KeyT.Status;		
}

//===================================================================
//功能: 			显示一个未被按下的图标
//入口参数:		
//		guixy:		菜单项参数结构指针	
//		k:			菜单项的光标位置项
//返回:			处理后的菜单项光标位置项
//说明: 			
//修改时间:		2010-10-30
//===================================================================
void Dis_IconUp(uint32 x,uint32 y,uint32 width,uint32 height,uint16 *bmp)
{
uint32 i,j;
uint16 color;
	//显示图标--------------------
	for (i=y;i<(y+height);i++) 
	{
		for (j=x;j<(x+width);j++)
		{
			color=*bmp;
			if(color!=LCD_FilterColor)
			{
		       		LCD_SetPixel(j,i,color);
			}
			bmp++;
 	   	}
	}
}

//===================================================================
//功能: 			显示一个未被按下的图标
//入口参数:		
//		guixy:		菜单项参数结构指针	
//		k:			菜单项的光标位置项
//返回:			处理后的菜单项光标位置项
//说明: 			
//修改时间:		2010-10-30
//===================================================================
void Dis_IconDown(uint32 x,uint32 y,uint32 width,uint32 height,uint16 *bmp)
{
enum {C_Chang=15};
uint32 i,j;
uint16 color;
uint32 c_r,c_g,c_b;
	//显示图标--------------------
	for (i=y;i<(y+height);i++) 
	{
		for (j=x;j<(x+width);j++)
		{
			color=*bmp;
			if(color!=LCD_FilterColor)
			{
				c_r=color>>11;
				c_g=(color>>5)&0x3f;
				c_b=color&0x1f;
				if(c_r>=C_Chang) c_r-=C_Chang;
				else c_r=0;
				if(c_g>=(C_Chang*2)) c_g-=C_Chang*2;
				else c_g=0;
				if(c_b>=C_Chang) c_b-=C_Chang;
				else c_b=0;

				color=(c_r<<11)+(c_g<<5)+c_b;
			
		       		LCD_SetPixel(j,i,color);
			}
			bmp++;
 	   	}
	}	
}

//===================================================================
//功能:			刷新模拟键盘当前按键的显示状态
//参数:
//返回:
//说明:
//修改时间:		2013-3-7
//===================================================================
void Dis_TouchIconRef(void)
{
uint32 i;
	//若触摸屏被关闭则直接返回-----
	if(Sys.TouchEn!=0x55)
	{
		return;
	}

	KeyT.Status=KeyT.Value;

	if(KeyT.Status==KeyT.DisStatus) return;

	i=KeyT.PushIndex;

	if(TchKey[0][i].Flash==0) return;		//若不显示按钮效果则直接返回
	
	if(KeyT.Value!=KEY_No)
	{
		KeyT.Key=KeyT.Value;
		Dis_IconDown(TchKey[0][i].x,TchKey[0][i].y,TchKey[0][i].width,TchKey[0][i].height,TchKey[0][i].Bmp);
	}
	else
	{
		Dis_IconUp(TchKey[0][i].x,TchKey[0][i].y,TchKey[0][i].width,TchKey[0][i].height,TchKey[0][i].Bmp);
		KeyT.Key=KEY_No;
	}

	KeyT.DisStatus=KeyT.Status;
}

