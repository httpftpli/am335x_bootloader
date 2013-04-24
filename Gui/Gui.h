#ifndef	_Gui_H_
#define 	_Gui_H_

#define ASC_Head 			32		//半角字符对应ASC表的首地址
#define ASC_Num				16		//数字在ASC点阵的偏移位置

#define LCD_SIGNAL_Fan		0				// 1表示电平信号反(0x0000=白,0xffff=黑)(夏普设为1)

#define LCD_FilterColor		0xfc1f			//显示图片时指定的过滤色(画图软件最右上角的颜色)
#define LCD_FilterNo			0xffffffff		//显示图片时不进行过滤

#define C_Back				C_Gray

//窗体标题栏颜色类型定义(有渐变效果)--------
#define MAX_FormTitle		3				//窗体标题栏样式最大为3
#define FORM_Red			0				//窗体标题栏为红色
#define FORM_Blue		1				//窗体标题栏为蓝色
#define FORM_Gray		2				//窗体标题栏为灰色

//触摸键的背景色定义(有渐变效果)------------------
#define BTN_Gray			0				
#define BTN_Yellow		1

#define FONT16			16				//字体为16*16点阵	
#define FONT24			24				//字体为24*24点阵	

#define FRAME_Hight		28				//文本框的高度

#define TITLE_H16		24				//标题栏为16点阵字体的高度
#define TITLE_H24		28				//标题样为24点阵字体的高度
#define TITLE_H35		35				//标题栏高度为35个象素


#define PTEXT	(const char *)
#define GET_ScrCenterX(width)	((LCD_XSize-width)>>1)	//显示在屏幕中心的X值
#define GET_ScrCenterY(height)	((LCD_YSize-height)>>1)	//显示在屏幕中心的Y值

//单个图标部件结构----------------
typedef __packed struct _S_GuiIcon{
	const char *Text;			//图标下显示的文本指针
	uint16 *Bmp;				//ICON图标的首地址指针
	uint16 X;					//图标部件的起始X坐标(按提示框算)
	uint16 Y;					//图标部件的起始Y坐标(按提示框算)
	uint16 Width;				//图标部件的宽度(即提示框的宽度)
	uint16 Height;				//图标部件的高度(即提示框的高度)
	uint16 BmpW;				//图片的宽度
	uint16 BmpH;				//图片的高度
	uint16 Sel;				//不为0表示光标正在本图标上
	uint16 TextColor;			//提示文本颜色
	uint16 RectColor;			//选中框的颜色
	uint16 BackColor;			//背景色
}S_GuiIcon;

//图标显示坐标定位结构--------
typedef struct _S_GuiXY{
	uint16 StartX;				//起始的X坐标
	uint16 StartY;				//起始的Y坐标
	uint16 RowSpace;			//上下两项间的行间距
	uint16 ColSpace;			//左右两项间的列间距
	uint16 RowIcons;			//每行显示的图标数
	uint16 MaxIcons;			//总的图标数

	uint16 BtnStartX;			//触摸按钮的起始X坐标
	uint16 BtnPartWidth;		//触摸按钮部件宽度
	uint16 BtnWidth;			//触摸按钮实际宽度
}S_GuiXY;

//显示内容备份及恢复结构--------------
typedef __packed struct _S_LcdMem{
	uint16 *Mem[10];			//每块显存的地址指针(最大10块)
	uint32 Point;				//当前指向的操作显存块(从1开始)
	uint32 Size[10];			//每块显存占用的字数
	uint32 X[10];				//起始X坐标
	uint32 Y[10];				//起始Y坐标
	uint32 Width[10];			//象素宽度
	uint32 Height[10];			//象素高度
}S_LcdMem;

//窗体信息结构定义------------------------------
typedef __packed struct _S_Form{
	uint16 X;					//起始的X坐标
	uint16 Y;					//起始的Y坐标
	uint16 Width;				//窗体宽度
	uint16 Height;				//窗体高度
	const char *TitleText;		//标题栏文本	
	uint16 TitleFont;			//标题栏文本的字体类型
	uint16 TitleColor;			//标题栏颜色类型(固定为三种)
	uint16 TitleHeight;			//标题栏高度	
	uint16 TextColor;			//标题栏文字颜色
	uint16 BackColor;			//背景色(窗体填充的背景色)
	uint16 FillBack;			//不为0表示填充背景色
}S_Form;

extern const uint8 Asc8x16[];
extern const uint8 Asc12x24[];
extern const uint16 FormTitleColor[3][24];
extern S_LcdMem LcdMem;
extern uint32 Lang;
extern uint8 Hz_Lib[];							
extern uint8 Hz_Lib24[];						

void ClearScreen(uint8 dat,uint8 layer);
void Lcd_Init(void);

uint32 KeyIconMove(S_GuiXY *guixy,uint32 k);
void LcdMemInit(void);
void LcdMemPush(uint32 x,uint32 y,uint32 width,uint32 height);
void LcdMemPop(void);

void Dis_Rect(uint16 x,uint16 y,uint16 width,uint16 heigh,uint16 color);
void Dis_RectFill(uint16 x,uint16 y,uint16 width,uint16 heigh,uint16 color);
void Dis_RectSome(uint32 x,uint32 y,uint32 width,uint32 heigh,uint32 line,uint32 color);
void Dis_CircleRect(int16 x,int16 y,uint32 width,uint32 height,uint32 r,uint16 color);
void Dis_CircleRectFill(int16 x,int16 y,uint32 width,uint32 height,uint32 r,uint16 color);
void Dis_CircleRectFillShade(int16 x,int16 y,uint32 width,uint32 height,uint32 r,uint16 color1,uint16 color2);
void Dis_Circle(uint32 x,uint32 y,uint32 r,uint16 color);
void Dis_CircleFill(uint32 x,uint32 y,uint32 r,uint16 color);
void Dis_Ellispse(int x,int y,int a,int b,uint16 color);
void Dis_EllispseFill(int x,int y,int a,int b,uint16 color);

void Dis_IconSelect(S_GuiIcon *icon);

extern void Dis_Picture(uint32 x,uint32 y,uint32 width,uint32 height,uint16 *bmp);
extern void Dis_PictureFilter(uint32 x,uint32 y,uint32 width,uint32 height,uint16 *bmp,uint32 filter);

extern void Dis_8x16(uint8 num,uint16 x,uint16 y,uint16 color_f,uint16 color_b);
extern void Dis_Asc12(uint8 num,uint16 x,uint16 y,uint16 color_f,uint16 color_b);
extern void Dis_Asc(uint8 num,uint16 x,uint16 y,uint16 color_f,uint16 color_b);
extern void Dis_Hex(uint8 num,uint16 x,uint16 y,uint16 color_f,uint16 color_b);

extern void Dis_Strings(const char *hz,uint16 x,uint16 y,uint16 font,uint16 color_f,uint16 color_b);
extern void Dis_StringsAligen(const char *hz,uint32 aligen,uint16 x,uint16 y,uint32 width,uint16 font,uint16 color_f,uint16 color_b);
extern void Dis_NumFilterTitle(uint16 dat,uint16 x,uint16 y,uint8 weisu,uint16 font,uint16 color_f,S_Form *form);
extern void Dis_Chinese(uint16 hz,uint16 x,uint16 y,uint16 color_f,uint16 color_b);
extern void Dis_StringsFilterTitle(const char *hz,uint16 x,uint16 y,uint32 font,uint16 color_f,S_Form *form);
extern void Dis_StringsLogo(const char *hz,uint16 x,uint16 y,uint16 color_f);
extern void Dis_StringsFilter(const char *hz,uint16 x,uint16 y,uint16 font,uint16 color_f);
extern void Dis_StringsAligenFilter(const char *hz,uint32 aligen,uint16 x,uint16 y,uint32 width,uint16 font,uint16 color_f);

extern void Dis_NumHide(uint16 dat,uint16 x,uint16 y,uint8 weisu,uint16 font,uint16 color_f,uint16 color_b);
extern void Dis_NumAll(uint16 dat,uint16 x,uint16 y,uint8 weisu,uint16 font,uint16 color_f,uint16 color_b);
extern void Dis_NumAllLogo(uint16 dat,uint16 x,uint16 y,uint8 weisu,uint16 color_f);
void Dis_NumFloat(uint32 dat,uint16 x,uint16 y,uint8 weisu,uint8 xiaosu,uint16 font,uint16 color_f,uint16 color_b);
void Dis_NumPoint5(uint32 dat,uint32 five,uint16 x,uint16 y,uint8 weisu,uint16 font,uint16 color_f,uint16 color_b);
extern void Dis_NumFuHao(uint16 dat,uint8 zf,uint16 x,uint16 y,uint16 font,uint16 color_f,uint16 color_b);
void Dis_NumInt(int16 dat,uint16 x,uint16 y,uint8 weisu,uint16 font,uint16 color_f,uint16 color_b);
extern void Dis_WindowsForm(S_Form *form);
extern void Dis_WindowsFormLeft(S_Form *form);
extern void Dis_TextFrame(uint16 x,uint16 y,uint16 width,uint16 height,uint16 color);
extern void Dis_BoxUp(uint16 x,uint16 y,uint16 width,uint16 height,uint16 color);
extern void Dis_BoxDown(uint16 x,uint16 y,uint16 width,uint16 height,uint16 color);
extern void Dis_FrameUp(uint16 x,uint16 y,uint16 width,uint16 height,uint16 color);
extern void Dis_FrameDown(uint16 x,uint16 y,uint16 width,uint16 height,uint16 color);
extern void Dis_HintBox(const char *title,const char *text,uint32 push,uint16 color_f,uint16 color_b);
extern void Dis_PictureFilterBottom(uint32 x,uint32 y,uint32 width,uint32 height,uint16 *bmp,uint32 filter);
extern void Dis_HintForm(const char *title,const char *text,uint16 color_f,uint16 color_b);
extern uint32 Dis_InputDialog(uint32 max,uint32 dat,const char *text,uint16 color_b);
uint32 Dis_InputStrDialog(char *str,uint32 len,const char *text,uint16 color_b);
extern uint32 Dis_SureDialog(const char *title,const char *text,uint16 back_color);
extern uint32 Dis_SelectDialog(const char *text[][LANG_MAX],uint32 item,uint16 inx,uint16 iny,uint16 inw,uint16 inh,uint32 k,uint32 b_color);
extern void Dis_PercentSlider(uint32 dat,uint32 tol,uint32 x,uint32 y,uint32 height,uint32 width,uint32 color_f,uint32 color_b);

extern void Dis_SpeedInfo(const char *title,uint32 dat,uint32 tol,uint32 width,uint32 color);
//extern void Dis_TouchBottom(const char *hz,uint32 x,uint32 y,uint32 width,uint32 height,uint32 color,uint32 btncolor,uint16 key,uint16 num,uint16 font);

uint32 Dis_PassWord(uint32 mima);
uint32 Dis_SuperPassWord(uint8 *code,uint32 x,uint32 y);

void Get_IconFormPlace(S_Form *form,S_GuiXY *guixy,S_GuiIcon *icon,uint32 ext_h);
void Dis_IconRef(S_GuiXY *guixy,S_GuiIcon *icon,const char *text[][LANG_MAX],uint16 *bmp,uint32 k,uint32 tch_k);
void Dis_FileName(uint8 *fdt,uint32 ext,uint16 x,uint16 y,uint16 color_f,uint16 color_b);
void Dis_FileTime(uint8 *fdt,uint16 x,uint16 y,uint16 color_f,uint16 color_b);
void File_SortByFloder(uint32 tol,uint8 *file_list);
uint32 Dis_FileSelectOne(const char *title,uint8 *file_fdt,uint32 device);
uint32 Dis_FileSelectMany(const char *title,uint8 *file_fdt,uint8 *sel_list);

#endif

