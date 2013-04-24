/********************************************************************
功能:				TFT显示GUI驱动文件
修改时间:		2013-3-15
********************************************************************/
#include "Head.h"
#include "Asc8x16_dot.h"

uint32 Lang;									//语言选择 0=中文,1=英文
uint8 Hz_Lib[1024*256];							//汉字库16*16点阵(实际占用256K字节)
uint8 Hz_Lib24[1024*576];						//汉字库24*24点阵(实际占用576K字节)
S_LcdMem LcdMem;

const char *HZ_SuRuMiMa[][LANG_MAX]={"请输入密码:"," Password:"};
const char *Hz_InputDialog[][LANG_MAX]={"输入对话框","Input Dialog"};

//===================================================================
//功能: 			显示备份内存初始化
//说明: 			
//修改时间:		2010-10-29
//===================================================================
void LcdMemInit(void)
{
uint32 i;
	LcdMem.Point=0;
	for(i=0;i<10;i++)
	{
		LcdMem.X[i]=0;
		LcdMem.Y[i]=0;
		LcdMem.Width[i]=0;
		LcdMem.Height[i]=0;
		LcdMem.Size[i]=0;
		LcdMem.Mem[i]=(void *)0;
	}
}

//===================================================================
//功能: 			申请一块备份用的显存
//入口参数:		
//		width:		宽度		
//		height:		高度
//说明: 			最大内存块限制为10块	

//修改时间:		2013-3-6
//===================================================================
uint16 *LcdAppMem(uint32 x,uint32 y,uint32 width,uint32 height)
{
uint32 size;
uint32 addr;
uint32 tol;
	//已申请的显存块数到最大-----
	if(LcdMem.Point>=10)
	{
		return((void *)0);
	}

	size=(width*height)<<1;		//申请备份显存的大小(字节数)
	tol=0;
	addr=(uint32)LcdMemory;
	//根据上一块信息计算当前申请块的绝对起始地址----
	if(LcdMem.Point>0)
	{
		addr=(uint32)LcdMem.Mem[LcdMem.Point-1]+LcdMem.Size[LcdMem.Point-1];
	}

	tol=addr+size;					
	//判断申请的显存块是否超出范围------
	if(tol>(uint32)(LcdMemory+LCD_MAX_DisRam))
	{
		return((void *)0);
	}
	LcdMem.Point++;
	LcdMem.Mem[LcdMem.Point-1]=(uint16 *)(addr);
	LcdMem.X[LcdMem.Point-1]=x;
	LcdMem.Y[LcdMem.Point-1]=y;
	LcdMem.Width[LcdMem.Point-1]=width;
	LcdMem.Height[LcdMem.Point-1]=height;
	LcdMem.Size[LcdMem.Point-1]=size;
	return(LcdMem.Mem[LcdMem.Point-1]);
}

//===================================================================
//功能: 			释放上次备份用的显存
//说明: 			最大内存块限制为10块	
//修改时间:		2010-10-29
//===================================================================
void LcdFreeMem(void)
{
	if(LcdMem.Point>0)
	{
		LcdMem.Point--;
		LcdMem.Size[LcdMem.Point]=0;
		LcdMem.Mem[LcdMem.Point]=(void *)0;
	}
}

//===================================================================
//功能: 			将指定的显示区域数据存入备份显存
//参数:			
//		x,y:			指定显示区域的起始x,y坐标
//		width,height:	显示区域的宽度和高度
//说明: 			
//修改时间:		2013-3-15
//===================================================================
void LcdMemPush(uint32 x,uint32 y,uint32 width,uint32 height)
{
uint32 i;
uint16 *p;
	p=LcdAppMem(x,y,width,height);
	if(p==NULL) 
	{
		return;
	}
	for(i=0;i<height;i++)
	{
		memcpy((uint8 *)p,(uint8 *)&LcdBuffer[y+i][x],sizeof(uint16)*width);
		p+=width;
	}
}

//===================================================================
//功能: 			从备份显存中恢复之前的显示界面
//说明: 			
//修改时间:		2013-3-15
//===================================================================
void LcdMemPop(void)
{
uint32 i;
uint32 x,y,width,height;
uint16 *p;
	if(LcdMem.Point==0) return;

	x=LcdMem.X[LcdMem.Point-1];
	y=LcdMem.Y[LcdMem.Point-1];
	width=LcdMem.Width[LcdMem.Point-1];
	height=LcdMem.Height[LcdMem.Point-1];
	p=LcdMem.Mem[LcdMem.Point-1];
	for(i=0;i<height;i++)
	{
		memcpy((uint8 *)&LcdBuffer[y+i][x],(uint8 *)p,sizeof(uint16)*width);
		p+=width;
	}
	LcdFreeMem();
}

//===================================================================
//功能:			画矩形框函数
//参数:
//		x:			起始横坐标
//		y:			起始纵坐标
//		width:		矩形框的度度
//		heigh:		矩形框的高度
//		color:		颜色
//返回:
//说明:
//修改时间:		2012-10-17
//===================================================================
void Dis_Rect(uint16 x,uint16 y,uint16 width,uint16 heigh,uint16 color)
{
	Dis_HLine(x,y,width,color);
	Dis_HLine(x,y+heigh-1,width,color);
	Dis_VLine(x,y,heigh,color);
	Dis_VLine(x+width-1,y,heigh,color);           
}   

//===================================================================
//功能:			画矩形框函数,线宽可设定
//参数:
//		x:			起始横坐标
//		y:			起始纵坐标
//		width:		矩形框的度度
//		heigh:		矩形框的高度
//		line: 			线的条数
//		color:		颜色
//返回:
//说明:
//修改时间:		2012-10-17
//===================================================================
void Dis_RectSome(uint32 x,uint32 y,uint32 width,uint32 heigh,uint32 line,uint32 color)
{
uint32 i;
	for(i=0;i<line;i++)
	{
		Dis_HLine(x+i,y+i,width-i*2,color);				//上面的横线
		Dis_HLine(x+i,y+heigh-1-i,width-i*2,color);		//下面的横线
		Dis_VLine(x+i,y+i,heigh-i*2,color);				//左边的竖线
		Dis_VLine(x+i+width-1-i*2,y+i,heigh-i*2,color);	//右边的竖线
	}
}   

//===================================================================
//功能:			画填充的矩形函数
//参数:
//		x:			起始X坐标
//		y:			起始Y坐标
//		width:		矩形的度度
//		heigh:		矩形的高度
//		color:		颜色
//返回:
//说明:
//修改时间:		2013-3-15
//===================================================================
void Dis_RectFill(uint16 x,uint16 y,uint16 width,uint16 heigh,uint16 color)
{
uint32 i;    
uint16 dat[LCD_XSize];
	for(i=0;i<width;i++) dat[i]=color;
	for(i=0;i<heigh;i++) 
	{
		memcpy((uint8 *)(&LcdBuffer[SECTION_Y(x, y)][SECTION_X(x, y)]),(uint8 *)dat,(sizeof(uint16))*width);
		y++;
	}  
}  

//=============================================================
//功能:			画空心圆角矩形
//参数: 
//		x :			圆角矩形的起始x坐标
//		y :			圆角矩形的起始y坐标
//		width:		圆角矩形的宽度
//		height:		圆角矩形的高度
//		r :			圆角的半径
//		color:		颜色
//返回: 
//说明:
//修改时间:		2012-10-17
//=============================================================
void Dis_CircleRect(int16 x,int16 y,uint32 width,uint32 height,uint32 r,uint16 color)
{
int a,b;
int di;
uint16 qua2_y0,qua3_y0;
uint16 qua3_x0,qua4_x0;
	x=x+r;
	y=y+r;
	width=width-2*r-1;
	height=height-2*r-1;
	a=0;
	b=r;
	di=3-2*r;
	while(a<=b)
	{
		//第一象限
		LCD_SetPixel(x-a,y-b,color);    
		LCD_SetPixel(x-b,y-a,color);             //3

		//第二象限        
		qua2_y0=y+height;
		LCD_SetPixel(x-a,qua2_y0+b,color);
		LCD_SetPixel(x-b,qua2_y0+a,color);
		
		//第三象限        
		qua3_y0=y+height;
		qua3_x0=x+width;
		LCD_SetPixel(qua3_x0+a,qua3_y0+b,color);           
		LCD_SetPixel(qua3_x0+b,qua3_y0+a,color);             //4

		//第四象限
		qua4_x0=x+width;
		LCD_SetPixel(qua4_x0+a,y-b,color);               
		LCD_SetPixel(qua4_x0+b,y-a,color);        

		a++;
		if(di<0) 
		{
			di+=4*a+6;
		}
		else
		{
			di+=10+4*(a-b);
			b--;
		}
	}

	Dis_VLine(x-r,y,height,color);	
	Dis_VLine(x+r+width,y,height,color);
	Dis_HLine(x,y-r,width,color);
	Dis_HLine(x,y+height+r,width,color);
}

//=============================================================
//功能:			画实心圆角矩形
//参数: 
//		x :			圆角矩形的起始x坐标
//		y :			圆角矩形的起始y坐标
//		width:		圆角矩形的宽度
//		height:		圆角矩形的高度
//		r :			圆角的半径
//		color:		颜色
//返回: 
//说明:
//修改时间:		2012-10-17
//=============================================================
void Dis_CircleRectFill(int16 x,int16 y,uint32 width,uint32 height,uint32 r,uint16 color)
{
int a,b;
int di;
uint32 qua2_y0,qua3_y0;
uint16 qua3_x0,qua4_x0;   
	x=x+r;
	y=y+r;
	width=width-2*r-1;
	height=height-2*r-1;
	a=0;
	b=r;
	di=3-2*r;
	while(a<=b)
	{
		//第一象限
		LCD_SetPixel(x-a,y-b,color);      
		LCD_SetPixel(x-b,y-a,color);             //3

		//第二象限        
		qua2_y0=y+height;
		LCD_SetPixel(x-a,qua2_y0+b,color);     
		LCD_SetPixel(x-b,qua2_y0+a,color);            

		//第三象限        
		qua3_y0=y+height;
		qua3_x0=x+width;
		LCD_SetPixel(qua3_x0+a,qua3_y0+b,color);           
		LCD_SetPixel(qua3_x0+b,qua3_y0+a,color);             //4

		//第四象限
		qua4_x0=x+width;
		LCD_SetPixel(qua4_x0+a,y-b,color);                    
		LCD_SetPixel(qua4_x0+b,y-a,color);           

		Dis_HLine(x-a,y-b,(qua4_x0+a)-(x-a),color);
		Dis_HLine(x-b,y-a,(qua4_x0+b)-(x-b),color);

		Dis_HLine(x-a,qua2_y0+b,(qua3_x0+a)-(x-a),color);
		Dis_HLine(x-b,qua2_y0+a,(qua4_x0+b)-(x-b),color);

		a++;
		if(di<0) 
		{
			di+=4*a+6;
		}
		else
		{
			di+=10+4*(a-b);
			b--;
		}
	}

	Dis_VLine(x-r,y,height,color);
	Dis_VLine(x+r+width,y,height,color);
	Dis_HLine(x,y-r,width,color);
	Dis_HLine(x,y+height+r,width,color);
	Dis_RectFill(x-r+1,y,width+2*r-1,height,color);
}

//=============================================================
//功能:			画有渐变效果的实心圆角矩形
//参数: 
//		x :			圆角矩形的起始x坐标
//		y :			圆角矩形的起始y坐标
//		width:		圆角矩形的宽度
//		height:		圆角矩形的高度
//		r :			圆角的半径
//		color:		颜色
//返回: 
//说明:
//修改时间:		2012-10-17
//=============================================================
void Dis_CircleRectFillShade(int16 x,int16 y,uint32 width,uint32 height,uint32 r,uint16 color1,uint16 color2)
{
int a,b;
int di;
uint32 qua2_y0,qua3_y0;
uint16 qua3_x0,qua4_x0;   
	x=x+r;
	y=y+r;
	width=width-2*r-1;
	height=height-2*r-1;
	a=0;
	b=r;
	di=3-2*r;
	while(a<=b)
	{
		//第一象限
		LCD_SetPixel(x-a,y-b,color1);      
		LCD_SetPixel(x-b,y-a,color1);             //3

		//第二象限        
		qua2_y0=y+height;
		LCD_SetPixel(x-a,qua2_y0+b,color1);     
		LCD_SetPixel(x-b,qua2_y0+a,color1);            

		//第三象限        
		qua3_y0=y+height;
		qua3_x0=x+width;
		LCD_SetPixel(qua3_x0+a,qua3_y0+b,color2);           
		LCD_SetPixel(qua3_x0+b,qua3_y0+a,color2);             //4

		//第四象限
		qua4_x0=x+width;
		LCD_SetPixel(qua4_x0+a,y-b,color2);                    
		LCD_SetPixel(qua4_x0+b,y-a,color2);           

		Dis_HLine(x-a,y-b,(qua4_x0+a)-(x-a),color1);
		Dis_HLine(x-b,y-a,(qua4_x0+b)-(x-b),color1);

		Dis_HLine(x-a,qua2_y0+b,(qua3_x0+a)-(x-a),color2);
		Dis_HLine(x-b,qua2_y0+a,(qua4_x0+b)-(x-b),color2);

		a++;
		if(di<0) 
		{
			di+=4*a+6;
		}
		else
		{
			di+=10+4*(a-b);
			b--;
		}
	}

	Dis_VLine(x-r,y,height,color1);
	Dis_VLine(x+r+width,y,height,color2);
	Dis_HLine(x,y-r,width,color1);
	Dis_HLine(x,y+height+r,width,color2);
//	Dis_RectFill(x-r+1,y,width+2*r-1,height,color1);

	Dis_RectFill(x-r+1,y,width+2*r-1,height/2,color1);
	Dis_RectFill(x-r+1,y+height/2,width+2*r-1,height/2+1,color2);
}

//=============================================================
//功能:			画空心圆
//参数: 
//		x :			圆心的x坐标
//		y :			圆心的y坐标
//		r :			圆的半径
//		color:		颜色
//返回: 
//说明:
//修改时间:		2012-10-10
//=============================================================
void Dis_Circle(uint32 x,uint32 y,uint32 r,uint16 color)
{
int dx,dy;
	if(r==0) return;
	dx=r;
	dy=1;
	
	while(dy<=dx)
	{
		LCD_SetPixel(x+dx,y+dy,color);
		LCD_SetPixel(x-dx,y+dy,color);
		LCD_SetPixel(x+dx,y-dy,color);
		LCD_SetPixel(x-dx,y-dy,color);
		if(dx!=dy)
		{
			LCD_SetPixel(x+dy,y+dx,color);
			LCD_SetPixel(x+dy,y-dx,color);
			LCD_SetPixel(x-dy,y+dx,color);
			LCD_SetPixel(x-dy,y-dx,color);
		}
		dy++;
		if((r*r-dy*dy)<=(dx-1)*dx)
		{
			dx--;
		}
	}
	LCD_SetPixel(x + r, y, color);
	LCD_SetPixel(x - r, y, color);
	LCD_SetPixel(x, y + r, color);
	LCD_SetPixel(x, y - r, color);
}

//=============================================================
//功能:			画实心圆
//参数: 
//		x :			圆心的x坐标
//		y :			圆心的y坐标
//		r :			圆的半径
//		color:		颜色
//返回: 
//说明:
//修改时间:		2012-10-10
//=============================================================
void Dis_CircleFill(uint32 x,uint32 y,uint32 r,uint16 color)
{
int dx,dy,ref;
int Temp;
	if(r==0) return;
	dx=r;
	dy=1;
	ref=(707*r)/1000; 				// 参考值为0.707r

	while(dy<=dx)
	{
		if(dx>ref)
		{
			Dis_Line(x + ref + 1, y + dy, x + dx, y + dy, color);
		}
		
		if(x>=(ref+1)&&(dx>ref))
		{
			if(x < dx)
				Temp = 0;
			else
				Temp = x - dx;
			Dis_Line(Temp, y + dy, x - ref - 1, y + dy, color);
		}

		if((y>=dy)&&(dx>ref))
			Dis_Line(x+ref + 1, y - dy, x + dx, y - dy, color);
		if((x>=(ref+1))&&(y>=dy)&&(dx>ref))
		{
			if(x<dx)
				Temp = 0;
			else
				Temp = x - dx;
			Dis_Line(Temp, y - dy, x - ref - 1, y - dy, color);
		}

		if((dy!=dx)||(dx==ref))
		{
			if(y<dx)
				Temp = 0;
			else
				Temp = y - dx;
			Dis_Line(x + dy, Temp, x + dy, y + dx, color);
		}

		if((x>=dy)&&((dy!= dx)||(dy==ref)))
		{
			if(y < dx)
				Temp = 0;
			else
				Temp = y - dx;
			Dis_Line(x - dy, Temp, x - dy, y + dx, color);
		} 
		dy++;
		if((r*r-dy*dy)<=(dx-1)*dx)
			dx--;
	}

			if(y < dx)
				Temp = 0;
			else
				Temp = y - dx;
			Dis_Line(x-dy,Temp,x-dy,y+dx,color);

	Dis_Line(x+ref+1,y,x+r,y,color);					//填充右侧的横线

	if(x>=(ref+1))
	{
		if(x<r)
			Temp = 0;
		else
			Temp = x - r;
		Dis_Line(Temp, y, x - ref - 1, y, color);		//填充左侧的横线
	}

	if(y< r)
		Temp = 0;
	else
		Temp=y-r;
	Dis_Line(x, Temp, x, y + r, color);				//填充中间的竖线
}

//=============================================================
//功能:			画空心椭圆
//参数: 
//		x:			椭圆心的x坐标
//		y:			椭圆心的y坐标
//		a:			椭圆的宽度
//		b:			椭圆的高度
//		color:		颜色
//返回: 
//说明:
//修改时间:		2012-10-10
//=============================================================
void Dis_Ellispse(int x,int y,int a,int b,uint16 color) 
{
int xx,yy;
float d1,d2;
	xx=0;
	yy=b;
       d1=b*b+a*a*(-b+0.25);   
       LCD_SetPixel(x+xx,y+yy,color);
       LCD_SetPixel(x-xx,y-yy,color);
       LCD_SetPixel(x-xx,y+yy,color);
       LCD_SetPixel(x+xx,y-yy,color);

       while((b*b*(xx+1))<(a*a*(yy-0.5)))
       {
              if(d1<=0)
              {
			d1+=b*b*(2*xx+3);
			xx++;
              }
              else
              {
			d1+=b*b*(2*xx+3)+a*a*(-2*yy+2);
			xx++;
			yy--;
              }
	       LCD_SetPixel(x+xx,y+yy,color);
	       LCD_SetPixel(x-xx,y-yy,color);
	       LCD_SetPixel(x-xx,y+yy,color);
	       LCD_SetPixel(x+xx,y-yy,color);
       }

       d2=b*b*(xx+0.5)*(xx+0.5)+a*a*(yy-1)*(yy-1)-a*a*b*b;

       while(yy>0)
       {
              if(d2<=0)
              {
			d2+=b*b*(2*xx+2)+a*a*(-2*yy+3);
			xx++;
			yy--;
              }
              else
              {
			d2+=a*a*(-2*yy+3);
			yy--;
              }

	       LCD_SetPixel(x+xx,y+yy,color);
	       LCD_SetPixel(x-xx,y-yy,color);
	       LCD_SetPixel(x-xx,y+yy,color);
	       LCD_SetPixel(x+xx,y-yy,color);
       }
}

//=============================================================
//功能:			画实心椭圆
//参数: 
//		x:			椭圆心的x坐标
//		y:			椭圆心的y坐标
//		a:			椭圆的宽度
//		b:			椭圆的高度
//返回: 
//说明:
//修改时间:		2012-10-10
//=============================================================
void Dis_EllispseFill(int x,int y,int a,int b,uint16 color) 
{
int xx,yy;
float d1,d2;
	xx=0;
	yy=b;
       d1=b*b+a*a*(-b+0.25);   //判别式
       LCD_SetPixel(x+xx,y+yy,color);
       LCD_SetPixel(x-xx,y-yy,color);
       LCD_SetPixel(x-xx,y+yy,color);
       LCD_SetPixel(x+xx,y-yy,color);

       while((b*b*(xx+1))<(a*a*(yy-0.5)))
       {
              if(d1<=0)
              {
			d1+=b*b*(2*xx+3);
			xx++;
              }
              else
              {
			d1+=b*b*(2*xx+3)+a*a*(-2*yy+2);
			Dis_Line(x-xx,y-yy,x+xx,y-yy,color);
			Dis_Line(x-xx,y+yy,x+xx,y+yy,color);
			xx++;
			yy--;
              }
       }

	Dis_Line(x-xx,y-yy,x+xx,y-yy,color);
	Dis_Line(x-xx,y+yy,x+xx,y+yy,color);


       d2=b*b*(xx+0.5)*(xx+0.5)+a*a*(yy-1)*(yy-1)-a*a*b*b;

       while(yy>0)
       {
              if(d2<=0)
              {
			d2+=b*b*(2*xx+2)+a*a*(-2*yy+3);
			xx++;
			yy--;
              }
              else
              {
			d2+=a*a*(-2*yy+3);
			yy--;
              }

		Dis_Line(x-xx,y-yy,x+xx,y-yy,color);
		Dis_Line(x-xx,y+yy,x+xx,y+yy,color);
       }
}

//===================================================================
//功能: 显示一幅图片
//x         : 起始横坐标
//y         : 起始纵坐标
//width   : 图片的宽度
//height  : 图片的高度
//*bmp   : 图片数据指针
//===================================================================
void Dis_Picture(uint32 x,uint32 y,uint32 width,uint32 height,uint16 *bmp)
{
uint32 i,j;
uint16 color;
	for (i=y;i<height+y;i++) 
	{
		for (j=x;j<width+x;j++) 
		{
			color=*bmp;
	       		LCD_SetPixel(j,i,color);
			bmp++;
 	   	}
	}
}

//===================================================================
//功能: 显示一幅图片(过滤指定的颜色)
//x         : 起始横坐标
//y         : 起始纵坐标
//width   : 图片的宽度
//height  : 图片的高度
//*bmp   : 图片数据指针
//===================================================================
void Dis_PictureFilter(uint32 x,uint32 y,uint32 width,uint32 height,uint16 *bmp,uint32 filter)
{
uint32 i,j;
uint16 color;
	for (i=y;i<height+y;i++) 
	{
		for (j=x;j<width+x;j++) 
		{
			color=*bmp;
	       	if(color!=filter)
	       	{
		       	LCD_SetPixel(j,i,color);
	       	}
			bmp++;
 	   	}
	}
}

//===================================================================
//功能: 显示一幅图片(过滤指定的颜色,背景色为指定图片)
//x         : 起始横坐标
//y         : 起始纵坐标
//width   : 图片的宽度
//height  : 图片的高度
//*bmp   : 图片数据指针
//===================================================================
void Dis_PictureFilterBottom(uint32 x,uint32 y,uint32 width,uint32 height,uint16 *bmp,uint32 filter)
{
uint32 i,j;
uint16 color;
uint16 colorb;
	for (i=y;i<height+y;i++) 
	{
		for (j=x;j<width+x;j++) 
		{
			color=*bmp;
	       	if(color!=filter)
	       	{
		       	LCD_SetPixel(j,i,color);
	       	}
			else
			{
				colorb=Lcd_BottomLogo[i-375][j];
				LCD_SetPixel(j,i,colorb);
			}
			bmp++;
 	   	}
	}
}

//===================================================================
//功能: 显示半角字符
//x         : 起始横坐标
//y         : 起始纵坐标
//color_f  : 显示的前景色
//color_b : 显示的前景色
//===================================================================
void Dis_8x16(uint8 num,uint16 x,uint16 y,uint16 color_f,uint16 color_b)
{
uint32 i,j;
uint32 addr;
	addr=num*16;
	for(i=0;i<16;i++)
	{
		for(j=0;j<8;j++)
		{
			if(Asc8x16[addr+i]&(0x80>>j))
			{
				LCD_SetPixel(x+j,y+i,color_f);
			}
			else
			{
				LCD_SetPixel(x+j,y+i,color_b);
			}
		}
	}
}

//===================================================================
//功能: 显示半角字符
//x         : 起始横坐标
//y         : 起始纵坐标
//color_f  : 显示的前景色
//color_b : 显示的前景色
//===================================================================
void Dis_8x16Logo(uint8 num,uint16 x,uint16 y,uint16 color_f)
{
uint32 i,j;
uint32 addr;
	addr=num*16;
	for(i=0;i<16;i++)
	{
		for(j=0;j<8;j++)
		{
			if(Asc8x16[addr+i]&(0x80>>j))
			{
				LCD_SetPixel(x+j,y+i,color_f);
			}
			else
			{
				LcdBuffer[y+i][x+j]=Lcd_BottomLogo[y-BOTTOM_LogoY+i][x-BOTTOM_LogoX+j];
			}
		}
	}
}

//===================================================================
//功能: 显示8*16点阵的ASC字符(ASC码从0开始)
//x         : 起始横坐标
//y         : 起始纵坐标
//color_f  : 显示的前景色
//color_b : 显示的前景色
//===================================================================
void Dis_Asc(uint8 num,uint16 x,uint16 y,uint16 color_f,uint16 color_b)
{
	if(num<32) return;
	num-=32;
	Dis_8x16(num,x,y,color_f,color_b);
}

//===================================================================
//画填充的矩形函数
//x:		起始横坐标
//y:		起始纵坐标
//width:	矩形的度度
//heigh:	矩形的高度
//color:	线的颜色
//===================================================================
void Dis_SuZi(uint8 num,uint16 x,uint16 y,uint16 font,uint16 color_f,uint16 color_b)
{
#define ASC_Num		16		//数字在ASC表中的位置
uint32 i,j;
uint32 addr;
	if(font==FONT24) 
	{
		addr=(num+ASC_Num)*48;
		for(i=0;i<24;i++)
		{
			for(j=0;j<8;j++)
			{
				if(Asc12x24[addr+i*2]&(0x80>>j))
				{
					LCD_SetPixel(x+j,y+i,color_f);
				}
				else
				{
					LCD_SetPixel(x+j,y+i,color_b);
				}
			}
			for(j=0;j<4;j++)
			{
				if(Asc12x24[addr+i*2+1]&(0x80>>j))
				{
					LCD_SetPixel(x+j+8,y+i,color_f);
				}
				else
				{
					LCD_SetPixel(x+j+8,y+i,color_b);
				}
			}			
		}				
	}
	else 
	{
		addr=(num+ASC_Num)*16;
		for(i=0;i<16;i++)
		{
			for(j=0;j<8;j++)
			{
				if(Asc8x16[addr+i]&(0x80>>j))
				{
					LCD_SetPixel(x+j,y+i,color_f);
				}
				else
				{
					LCD_SetPixel(x+j,y+i,color_b);
				}
			}
		}		
	}
}

//===================================================================
//画填充的矩形函数
//x:		起始横坐标
//y:		起始纵坐标
//width:	矩形的度度
//heigh:	矩形的高度
//color:	线的颜色
//===================================================================
void Dis_SuZiLogo(uint8 num,uint16 x,uint16 y,uint16 color_f)
{
#define ASC_Num		16		//数字在ASC表中的位置
uint32 i,j;
uint32 addr;
	addr=(num+ASC_Num)*16;
	for(i=0;i<16;i++)
	{
		for(j=0;j<8;j++)
		{
			if(Asc8x16[addr+i]&(0x80>>j))
			{
				LCD_SetPixel(x+j,y+i,color_f);
			}
			else
			{
				LcdBuffer[y+i][x+j]=Lcd_BottomLogo[y-BOTTOM_LogoY+i][x-BOTTOM_LogoX+j];
			}
		}
	}
}

//===================================================================
//功能: 显示十六进制的数值(0~F)
//===================================================================
void Dis_Hex(uint8 num,uint16 x,uint16 y,uint16 color_f,uint16 color_b)
{
	if(num<=9) Dis_SuZi(num,x,y,FONT16,color_f,color_b);
	else if(num<=15) Dis_Asc('A'+num-10,x,y,color_f,color_b);
	else Dis_Asc('.',x,y,color_f,color_b);
}

//===================================================================
//功能:			显示16*16点阵字符
//参数:
//		num:		全角字符的位置
//		x,y:			显示位置的x,y坐标
//		color_f:		显示的前景色
//		color_b:		显示的背景色
//返回:
//说明:
//修改时间:		2013-3-27
//===================================================================
void Dis_16x16(uint32 num,uint16 x,uint16 y,uint16 color_f,uint16 color_b)
{
uint32 i,j;
uint8 dat;
uint8 dot[32];
uint32 addr;
	addr=num*32;
	memcpy(dot,Hz_Lib+addr,32);
	for(i=0;i<16;i++)
	{
		dat=dot[i*2];
		for(j=0;j<8;j++)
		{
			if(dat&(0x80>>j))
			{
				LCD_SetPixel(x+j,y+i,color_f);
			}
			else
			{
				LCD_SetPixel(x+j,y+i,color_b);
			}
		}

		dat=dot[i*2+1];
		for(j=0;j<8;j++)
		{
			if(dat&(0x80>>j))
			{
				LCD_SetPixel(x+8+j,y+i,color_f);
			}
			else
			{
				LCD_SetPixel(x+8+j,y+i,color_b);
			}
		}
	}
}

//===================================================================
//画填充的矩形函数
//x:		起始横坐标
//y:		起始纵坐标
//width:	矩形的度度
//heigh:	矩形的高度
//color:	线的颜色
//===================================================================
void Dis_16x16Logo(uint32 num,uint16 x,uint16 y,uint16 color_f)
{
uint32 i,j;
uint8 dat;
uint8 dot[32];
uint32 addr;
	addr=num*32;
	memcpy(dot,Hz_Lib+addr,32);
	for(i=0;i<16;i++)
	{
		dat=dot[i*2];
		for(j=0;j<8;j++)
		{
			if(dat&(0x80>>j))
			{
				LCD_SetPixel(x+j,y+i,color_f);
			}
			else
			{
				LcdBuffer[y+i][x+j]=Lcd_BottomLogo[y-BOTTOM_LogoY+i][x-BOTTOM_LogoX+j];
			}
		}

		dat=dot[i*2+1];
		for(j=0;j<8;j++)
		{
			if(dat&(0x80>>j))
			{
				LCD_SetPixel(x+8+j,y+i,color_f);
			}
			else
			{
				LcdBuffer[y+i][x+8+j]=Lcd_BottomLogo[y-BOTTOM_LogoY+i][x-BOTTOM_LogoX+8+j];
			}
		}
	}
}

//===================================================================
//功能: 显示半角字符
//x         : 起始横坐标
//y         : 起始纵坐标
//color_f  : 显示的前景色
//color_b : 显示的前景色
//===================================================================
void Dis_12x24(uint8 num,uint16 x,uint16 y,uint16 color_f,uint16 color_b)
{
uint32 i,j;
uint32 addr;
	addr=num*2*24;
	for(i=0;i<24;i++)
	{
		for(j=0;j<8;j++)
		{
			if(Asc12x24[addr+i*2]&(0x80>>j))
			{
				LCD_SetPixel(x+j,y+i,color_f);
			}
			else
			{
				LCD_SetPixel(x+j,y+i,color_b);
			}
		}

		for(j=0;j<4;j++)
		{
			if(Asc12x24[addr+i*2+1]&(0x80>>j))
			{
				LCD_SetPixel(x+j+8,y+i,color_f);
			}
			else
			{
				LCD_SetPixel(x+j+8,y+i,color_b);
			}
		}		
	}
}

//===================================================================
//功能: 			显示半角字符
//x         : 起始横坐标
//y         : 起始纵坐标
//color_f  : 显示的前景色
//color_b : 显示的前景色
//修改时间:		2010-11-10
//===================================================================
void Dis_12x24Filter(uint8 num,uint16 x,uint16 y,uint16 color_f)
{
uint32 i,j;
uint32 addr;
	addr=num*2*24;
	for(i=0;i<24;i++)
	{
		for(j=0;j<8;j++)
		{
			if(Asc12x24[addr+i*2]&(0x80>>j))
			{
				LCD_SetPixel(x+j,y+i,color_f);
			}
		}

		for(j=0;j<4;j++)
		{
			if(Asc12x24[addr+i*2+1]&(0x80>>j))
			{
				LCD_SetPixel(x+j+8,y+i,color_f);
			}
		}		
	}
}

//===================================================================
//功能:			显示24*24点阵字符
//参数:
//		num:		全角字符的位置
//		x,y:			显示位置的x,y坐标
//		color_f:		显示的前景色
//		color_b:		显示的背景色
//返回:
//说明:
//修改时间:		2013-3-27
//===================================================================
void Dis_24x24(uint32 num,uint16 x,uint16 y,uint16 color_f,uint16 color_b)
{
uint32 i,j;
uint8 dat;
uint8 dot[72];
uint32 addr;
	addr=num*72;
	memcpy(dot,Hz_Lib24+addr,72);
	for(i=0;i<24;i++)
	{
		dat=dot[i*3];
		for(j=0;j<8;j++)
		{
			if(dat&(0x80>>j))
			{
				LCD_SetPixel(x+j,y+i,color_f);
			}
			else
			{
				LCD_SetPixel(x+j,y+i,color_b);
			}
		}

		dat=dot[i*3+1];
		for(j=0;j<8;j++)
		{
			if(dat&(0x80>>j))
			{
				LCD_SetPixel(x+8+j,y+i,color_f);
			}
			else
			{
				LCD_SetPixel(x+8+j,y+i,color_b);
			}
		}

		dat=dot[i*3+2];
		for(j=0;j<8;j++)
		{
			if(dat&(0x80>>j))
			{
				LCD_SetPixel(x+16+j,y+i,color_f);
			}
			else
			{
				LCD_SetPixel(x+16+j,y+i,color_b);
			}
		}		
	}
}

//===================================================================
//功能:			显示24*24点阵字符(不刷背景色)
//参数:
//		num:		全角字符的位置
//		x,y:			显示位置的x,y坐标
//		color_f:		显示的前景色
//返回:
//说明:
//修改时间:		2013-3-27
//===================================================================
void Dis_24x24Filter(uint32 num,uint16 x,uint16 y,uint16 color_f)
{
uint32 i,j;
uint8 dat;
uint8 dot[72];
uint32 addr;
	addr=num*72;
	memcpy(dot,Hz_Lib24+addr,72);
	for(i=0;i<24;i++)
	{
		dat=dot[i*3];
		for(j=0;j<8;j++)
		{
			if(dat&(0x80>>j))
			{
				LCD_SetPixel(x+j,y+i,color_f);
			}
		}

		dat=dot[i*3+1];
		for(j=0;j<8;j++)
		{
			if(dat&(0x80>>j))
			{
				LCD_SetPixel(x+8+j,y+i,color_f);
			}
		}

		dat=dot[i*3+2];
		for(j=0;j<8;j++)
		{
			if(dat&(0x80>>j))
			{
				LCD_SetPixel(x+16+j,y+i,color_f);
			}
		}		
	}
}

//===================================================================
//功能:			显示半角字符,背景刷成当前窗体标题栏
//入口参数:		
//		x:			起始X坐标
//		y:			起始纵坐标
//		color_f:		字符的颜色
//		form:		当前窗体结构指针
//说明:			
//修改时间:		2010-11-16
//===================================================================
void Dis_8x16FilterTitle(uint8 num,uint16 x,uint16 y,uint16 color_f,S_Form *form)
{
uint32 i,j;
uint32 m;
uint32 addr;
uint16 titlecolor;
	if(((x+8)>LCD_XSize)||((y+16)>LCD_YSize)) return;

	addr=num*16;
	for(i=0;i<16;i++)
	{
		//取出当前Y坐标的窗体标题栏背景色--------------
		m=y+i-form->Y;		//当前行相对于窗体起始Y坐标的值	
		//标题栏高度大于24时的背景色处理------
		if(form->TitleHeight>24)
		{
			if(m<12)
			{
				titlecolor=FormTitleColor[form->TitleColor][m];
			}
			else if((form->TitleHeight-m)>=12)
			{
				titlecolor=FormTitleColor[form->TitleColor][12];
			}
			else
			{
				titlecolor=FormTitleColor[form->TitleColor][24-(form->TitleHeight-m)];
			}
		}
		else
		{
			titlecolor=FormTitleColor[form->TitleColor][m];
		}		
		
		for(j=0;j<8;j++)
		{
			if(Asc8x16[addr+i]&(0x80>>j))
			{
				LCD_SetPixel(x+j,y+i,color_f);
			}
			else
			{
				LCD_SetPixel(x+j,y+i,titlecolor);
			}
		}
	}
}

//===================================================================
//功能: 			显示半角字符
//x         : 起始横坐标
//y         : 起始纵坐标
//color_f  : 显示的前景色
//color_b : 显示的前景色
//修改时间:		2010-11-10
//===================================================================
void Dis_12x24FilterTitle(uint8 num,uint16 x,uint16 y,uint16 color_f,S_Form *form)
{
uint32 i,j;
uint32 addr;
uint32 m;
uint16 titlecolor;
	addr=num*2*24;
	for(i=0;i<24;i++)
	{
		//取出当前Y坐标的窗体标题栏背景色--------------
		m=y+i-form->Y;		//当前行相对于窗体起始Y坐标的值	
		//标题栏高度大于24时的背景色处理------
		if(form->TitleHeight>24)
		{
			if(m<12)
			{
				titlecolor=FormTitleColor[form->TitleColor][m];
			}
			else if((form->TitleHeight-m)>=12)
			{
				titlecolor=FormTitleColor[form->TitleColor][12];
			}
			else
			{
				titlecolor=FormTitleColor[form->TitleColor][24-(form->TitleHeight-m)];
			}
		}
		else
		{
			titlecolor=FormTitleColor[form->TitleColor][m];
		}		
	
		for(j=0;j<8;j++)
		{
			if(Asc12x24[addr+i*2]&(0x80>>j))
			{
				LCD_SetPixel(x+j,y+i,color_f);
			}
			else
			{
				LCD_SetPixel(x+j,y+i,titlecolor);
			}
		}

		for(j=0;j<4;j++)
		{
			if(Asc12x24[addr+i*2+1]&(0x80>>j))
			{
				LCD_SetPixel(x+j+8,y+i,color_f);
			}
			else
			{
				LCD_SetPixel(x+j+8,y+i,titlecolor);
			}
		}		
	}
}


//===================================================================
//功能:			显示全角字符,背景刷成当前窗体标题栏
//		x:			起始横坐标
//		y:			起始纵坐标
//		width:		矩形的度度
//		heigh:		矩形的高度
//		color:		线的颜色
//修改时间:		2010-11-16
//===================================================================
void Dis_16x16FilterTitle(uint32 num,uint16 x,uint16 y,uint16 color_f,S_Form *form)
{
uint32 i,j;
uint32 m;
uint8 dat;
uint8 dot[32];
uint32 addr;
uint16 titlecolor;
	if(((x+16)>LCD_XSize)||((y+16)>LCD_YSize)) return;

	addr=num*32;
	memcpy(dot,Hz_Lib+addr,32);
	
	for(i=0;i<16;i++)
	{
		//取出当前Y坐标的窗体标题栏背景色--------------
		m=y+i-form->Y;		//当前行相对于窗体起始Y坐标的值	
		//标题栏高度大于24时的背景色处理------
		if(form->TitleHeight>24)
		{
			if(m<12)
			{
				titlecolor=FormTitleColor[form->TitleColor][m];
			}
			else if((form->TitleHeight-m)>=12)
			{
				titlecolor=FormTitleColor[form->TitleColor][12];
			}
			else
			{
				titlecolor=FormTitleColor[form->TitleColor][24-(form->TitleHeight-m)];
			}
		}
		else
		{
			titlecolor=FormTitleColor[form->TitleColor][m];
		}
		
		dat=dot[i*2];
		for(j=0;j<8;j++)
		{
			if(dat&(0x80>>j))
			{
				LCD_SetPixel(x+j,y+i,color_f);
			}
			else
			{
				LCD_SetPixel(x+j,y+i,titlecolor);
			}
		}

		dat=dot[i*2+1];
		for(j=0;j<8;j++)
		{
			if(dat&(0x80>>j))
			{
				LCD_SetPixel(x+8+j,y+i,color_f);
			}
			else
			{
				LCD_SetPixel(x+8+j,y+i,titlecolor);
			}
		}
	}
}

//===================================================================
//功能:			显示24*24点阵字符(背景色用标题栏颜色填充)
//参数:
//		num:		全角字符的位置
//		x,y:			显示位置的x,y坐标
//		color_f:		显示的前景色
//		form:		窗体数据结构指针
//返回:
//说明:
//修改时间:		2013-3-27
//===================================================================
void Dis_24x24FilterTitle(uint32 num,uint16 x,uint16 y,uint16 color_f,S_Form *form)
{
uint32 i,j;
uint8 dat;
uint8 dot[72];
uint32 addr;
uint32 m;
uint16 titlecolor;
	addr=num*72;
	memcpy(dot,Hz_Lib24+addr,72);
	for(i=0;i<24;i++)
	{
		//取出当前Y坐标的窗体标题栏背景色--------------
		m=y+i-form->Y;		//当前行相对于窗体起始Y坐标的值	
		//标题栏高度大于24时的背景色处理------
		if(form->TitleHeight>24)
		{
			if(m<12)
			{
				titlecolor=FormTitleColor[form->TitleColor][m];
			}
			else if((form->TitleHeight-m)>=12)
			{
				titlecolor=FormTitleColor[form->TitleColor][12];
			}
			else
			{
				titlecolor=FormTitleColor[form->TitleColor][24-(form->TitleHeight-m)];
			}
		}
		else
		{
			titlecolor=FormTitleColor[form->TitleColor][m];
		}
	
		dat=dot[i*3];
		for(j=0;j<8;j++)
		{
			if(dat&(0x80>>j))
			{
				LCD_SetPixel(x+j,y+i,color_f);
			}
			else
			{
				LCD_SetPixel(x+j,y+i,titlecolor);
			}
		}

		dat=dot[i*3+1];
		for(j=0;j<8;j++)
		{
			if(dat&(0x80>>j))
			{
				LCD_SetPixel(x+8+j,y+i,color_f);
			}
			else
			{
				LCD_SetPixel(x+8+j,y+i,titlecolor);
			}
		}

		dat=dot[i*3+2];
		for(j=0;j<8;j++)
		{
			if(dat&(0x80>>j))
			{
				LCD_SetPixel(x+16+j,y+i,color_f);
			}
			else
			{
				LCD_SetPixel(x+16+j,y+i,titlecolor);
			}
		}		
	}
}

//===================================================================
//功能:			显示全角字符
//x:		起始横坐标
//y:		起始纵坐标
//width:	矩形的度度
//heigh:	矩形的高度
//color:	线的颜色
//修改时间:		2010-11-16
//===================================================================
void Dis_StringsFilterTitle(const char *hz,uint16 x,uint16 y,uint32 font,uint16 color_f,S_Form *form)
{
uint8 dat;
uint32 qu_ma,wei_ma;
uint32 hz_addr;
uint32 half_step,full_step;
void (*call_full)(uint32 num, uint16 x, uint16 y, uint16 color_f, S_Form *form);
void (*call_half)(uint8 num, uint16 x, uint16 y, uint16 color_f, S_Form *form);
	if(font==FONT24) 
	{
		call_full=Dis_24x24FilterTitle;
		call_half=Dis_12x24FilterTitle;
		full_step=FONT24;
		half_step=FONT24>>1;
	}
	else
	{
		call_full=Dis_16x16FilterTitle;
		call_half=Dis_8x16FilterTitle;
		full_step=FONT16;
		half_step=FONT16>>1;
	}
	while(1)
	{
		dat=*hz;
		if(dat==0) break;		//到字串结束
		//是汉字处理----
		if(dat>=0x80)
		{
			qu_ma=dat;
			hz++;
			wei_ma=*hz;
			if(wei_ma==0) break;
			if((qu_ma>0xa0)&&(wei_ma>0xa0))
			{
				qu_ma-=0xa0;					
				wei_ma-=0xa0;
				hz_addr=(qu_ma-1)*94+wei_ma-1;
				(*call_full)(hz_addr,x,y,color_f,form);
			}
			x+=full_step;
		}
		//ASC码显示处理----
		else
		{
			if(dat<32) return;
			dat-=ASC_Head;
			(*call_half)(dat,x,y,color_f,form);
			x+=half_step;
		}
		hz++;
	}
}

void Dis_NumFilterTitle(uint16 dat,uint16 x,uint16 y,uint8 weisu,uint16 font,uint16 color_f,S_Form *form)
{
enum {NUM_Addr=16};		//数字在字库中的起始下标
uint8 i;
uint8 youxiao;
uint8 buf[5];
uint8 num;
uint32 step;
void (*call_sub)(uint8 num, uint16 x, uint16 y, uint16 color_f, S_Form * form);
	if(font==FONT24) 
	{	
		step=12;
		call_sub=Dis_12x24FilterTitle;
	}
	else 
	{
		step=8;
		call_sub=Dis_8x16FilterTitle;
	}
	
	num=0;
	switch(weisu)
	{
		case 1:
			if(dat>=10) i=0;
			else i=(uint8)dat;
			(*call_sub)(i+NUM_Addr,x,y,color_f,form);
			break;
		case 2:
			if(dat>=100) dat=0;
			buf[0]=dat/10;
			buf[1]=dat%10;
			for(i=0;i<2;i++)
			{
				if((buf[i]>0)||(i==1))
				{
					(*call_sub)(buf[i]+NUM_Addr,x,y,color_f,form);
					x+=step;
					num++;
				}
			}
			if(num!=2)
			{
				Dis_StringsFilterTitle(" ",x,y,font,color_f,form);
			}
			break;
		case 3:
			youxiao=0;
			if(dat>=1000) dat=0;
			buf[0]=dat/100;
			dat%=100;
			buf[1]=dat/10;
			buf[2]=dat%10;
			for(i=0;i<3;i++)
			{
				if((buf[i]>0)||(i==2)||(youxiao>0))
				{
					(*call_sub)(buf[i]+NUM_Addr,x,y,color_f,form);
					x+=step;
					youxiao=1;
					num++;
				}
			}
			if(num!=3)
			{
				for(i=num;i<3;i++)
				{
					Dis_StringsFilterTitle(" ",x,y,font,color_f,form);
					x+=step;
				}
			}
			break;
		case 4:
			youxiao=0;
			if(dat>=10000) dat=0;
			buf[0]=dat/1000;
			dat%=1000;
			buf[1]=dat/100;
			dat%=100;
			buf[2]=dat/10;
			buf[3]=dat%10;
			for(i=0;i<4;i++)
			{
				if((buf[i]>0)||(i==3)||(youxiao>0))
				{
					(*call_sub)(buf[i]+NUM_Addr,x,y,color_f,form);
					x+=step;
					youxiao=1;
					num++;
				}
			}
			if(num!=4)
			{
				for(i=num;i<4;i++)
				{
					Dis_StringsFilterTitle(" ",x,y,font,color_f,form);
					x+=step;
				}
			}
			break;
		case 5:
			youxiao=0;
			buf[0]=dat/10000;
			dat%=10000;
			buf[1]=dat/1000;
			dat%=1000;
			buf[2]=dat/100;
			dat%=100;
			buf[3]=dat/10;
			buf[4]=dat%10;
			for(i=0;i<5;i++)
			{
				if((buf[i]>0)||(i==4)||(youxiao>0))
				{
					(*call_sub)(buf[i]+NUM_Addr,x,y,color_f,form);
					x+=step;
					youxiao=1;
					num++;
				}
			}
			if(num!=5)
			{
				for(i=num;i<5;i++)
				{
					Dis_StringsFilterTitle(" ",x,y,font,color_f,form);
					x+=step;
				}
			}
			break;
		default:
			break;
	}
}

//===================================================================
//功能:			显示ASC字符(ASC码从0开始)
//参数:
//		x:			起始横坐标
//		y:			起始纵坐标
//		font:			字体大小
//		color_f:		显示的前景色
//		color_b:		显示的前景色
//修改时间:		2011-11-30
//===================================================================
void Dis_Char(uint8 num,uint16 x,uint16 y,uint32 font,uint16 color_f,uint16 color_b)
{
	if(num<32) return;
	num-=32;
	if(font==FONT24)
	{
		Dis_12x24(num,x,y,color_f,color_b);
	}
	else
	{
		Dis_8x16(num,x,y,color_f,color_b);
	}
}

//===================================================================
//功能:			显示字符串
//参数:
//		hz:			字符串指针
//		x:			起始x坐标
//		y:			起始y坐标
//		font:			字体点阵大小
//		color_f:		字符串前景色
//		color_b:		字符串背景色
//修改时间:		2010-11-10
//===================================================================
void Dis_Strings(const char *hz,uint16 x,uint16 y,uint16 font,uint16 color_f,uint16 color_b)
{
uint8 dat;
uint32 qu_ma,wei_ma;
uint32 hz_addr;
uint32 full_step;				//全角占用的象素宽度
uint32 half_step;				//半角占用的象素宽度	
void (*call_full)(uint32 num, uint16 x, uint16 y, uint16 color_f, uint16 color_b);
void (*call_half)(uint8 num, uint16 x, uint16 y, uint16 color_f, uint16 color_b);
	//字体为24点阵处理----------
	if(font==FONT24) 
	{
		call_full=Dis_24x24;
		call_half=Dis_12x24;
		full_step=FONT24;
		half_step=FONT24>>1;
	}
	else
	{
		call_full=Dis_16x16;
		call_half=Dis_8x16;
		full_step=FONT16;
		half_step=FONT16>>1;
	}

	while(1)
	{
		dat=*hz;
		if(dat==0) break;		//到字串结束
		//是汉字处理----
		if(dat>=0x80)
		{
			qu_ma=dat-0xa0;
			hz++;
			wei_ma=*hz;
			if(wei_ma==0) break;
			wei_ma-=0xa0;
			hz_addr=(qu_ma-1)*94+wei_ma-1;
			call_full(hz_addr,x,y,color_f,color_b);
			x+=full_step;
		}
		//ASC码显示处理----
		else
		{
			if(dat<32) return;
			dat-=ASC_Head;
			call_half(dat,x,y,color_f,color_b);
			x+=half_step;
		}
		hz++;
	}
}

//===================================================================
//zadd=iAddrZK+32*(((uint32)(zm)-0xa0-1)*94+(uint32)(bm)-0xa0-1);break;
//画填充的矩形函数
//x:		起始横坐标
//y:		起始纵坐标
//width:	矩形的度度
//heigh:	矩形的高度
//color:	线的颜色
//===================================================================
void Dis_StringsLogo(const char *hz,uint16 x,uint16 y,uint16 color_f)
{
uint8 dat;
uint32 qu_ma,wei_ma;
uint32 hz_addr;
	while(1)
	{
		dat=*hz;
		if(dat==0) break;		//到字串结束
		//是汉字处理----
		if(dat>=0x80)
		{
			qu_ma=dat;
			hz++;
			wei_ma=*hz;
			if(wei_ma==0) break;
			if((qu_ma>0xa0)&&(wei_ma>0xa0))
			{
				qu_ma-=0xa0;					
				wei_ma-=0xa0;
				hz_addr=(qu_ma-1)*94+wei_ma-1;
				Dis_16x16Logo(hz_addr,x,y,color_f);
			}
			x+=16;
		}
		//ASC码显示处理----
		else
		{
			if(dat<32) return;
			dat-=ASC_Head;
			Dis_8x16Logo(dat,x,y,color_f);
			x+=8;
		}
		hz++;
	}
}

//===================================================================
//zadd=iAddrZK+32*(((uint32)(zm)-0xa0-1)*94+(uint32)(bm)-0xa0-1);break;
//功能: 显示字符串
//*hz      : 要显示的字符串指针
//aligen   : 显示的对齐方式,0=左对齐,1=居中显示,2=右对齐
//x         : 显示容器的起始横坐标
//y		 : 显示容器的起始纵坐标
//width   : 显示容器的总宽度
//color_f  : 字体颜色
//color_b : 背景色
//===================================================================
void Dis_StringsAligen(const char *hz,uint32 aligen,uint16 x,uint16 y,uint32 width,uint16 font,uint16 color_f,uint16 color_b)
{
uint8 dat;
uint32 tol;					//半角字符的总个数
uint32 i,m;
uint32 step;					//半角字符占用的象素宽度
	//字体为24点阵处理----------
	if(font==FONT24) step=12;
	else step=8;
	tol=0;
	//若不是左对齐显示，则计算显示的起始x坐标----
	if(aligen!=ALIGEN_LEFT)
	{
		for(i=0;i<80;i++)
		{
			dat=*(hz+i);
			//有效的字符----
			if(dat>=32) tol++;
			else 
			{
				//居中显示处理---------
				if(aligen==ALIGEN_MIDDLE)
				{
					m=width-tol*step;
					x=x+m/2;
				}
				//右对齐显示处理----
				else
				{
					x=x+(width-tol*step);
				}
				break;
			}
		}
	}
	Dis_Strings(hz,x,y,font,color_f,color_b);
}

//===================================================================
//功能 : 显示8*16点阵半角字符(不刷新背景色)
//x         : 起始横坐标
//y         : 起始纵坐标
//color_f  : 显示的前景色
//color_b : 显示的前景色
//===================================================================
void Dis_8x16Filter(uint8 num,uint16 x,uint16 y,uint16 color_f)
{
uint32 i,j;
uint32 addr;
	addr=num*16;
	for(i=0;i<16;i++)
	{
		for(j=0;j<8;j++)
		{
			if(Asc8x16[addr+i]&(0x80>>j))
			{
				LCD_SetPixel(x+j,y+i,color_f);
			}
		}
	}
}

//===================================================================
//功能 : 显示16*16点阵全角字符(不刷新背景色)
//x:		起始横坐标
//y:		起始纵坐标
//width:	矩形的度度
//heigh:	矩形的高度
//color:	线的颜色
//===================================================================
void Dis_16x16Filter(uint32 num,uint16 x,uint16 y,uint16 color_f)
{
uint32 i,j;
uint8 dat;
uint8 dot[32];
uint32 addr;
	addr=num*32;
	memcpy(dot,Hz_Lib+addr,32);
	for(i=0;i<16;i++)
	{
		dat=dot[i*2];
		for(j=0;j<8;j++)
		{
			if(dat&(0x80>>j))
			{
				LCD_SetPixel(x+j,y+i,color_f);
			}
		}

		dat=dot[i*2+1];
		for(j=0;j<8;j++)
		{
			if(dat&(0x80>>j))
			{
				LCD_SetPixel(x+8+j,y+i,color_f);
			}
		}
	}
}

//===================================================================
//功能 : 显示字符串(不刷新背景色)
//zadd=iAddrZK+32*(((uint32)(zm)-0xa0-1)*94+(uint32)(bm)-0xa0-1)
//*hz       : 字符串指针
//x          : 起始横坐标
//y          : 起始纵坐标
//color_f  : 显示的前景色
//color:	线的颜色
//===================================================================
void Dis_StringsFilter(const char *hz,uint16 x,uint16 y,uint16 font,uint16 color_f)
{
uint8 dat;
uint32 qu_ma,wei_ma;
uint32 hz_addr;
uint32 full_step;				//全角占用的象素宽度
uint32 half_step;				//半角占用的象素宽度	
void (*call_full)(uint32 num, uint16 x, uint16 y, uint16 color_f);
void (*call_half)(uint8 num, uint16 x, uint16 y, uint16 color_f);
	//字体为24点阵处理----------
	if(font==FONT24) 
	{
		call_full=Dis_24x24Filter;
		call_half=Dis_12x24Filter;
		full_step=FONT24;
		half_step=FONT24>>1;
	}
	else
	{
		call_full=Dis_16x16Filter;
		call_half=Dis_8x16Filter;
		full_step=FONT16;
		half_step=FONT16>>1;
	}

	while(1)
	{
		dat=*hz;
		if(dat==0) break;		//到字串结束
		//是汉字处理----
		if(dat>=0x80)
		{
			qu_ma=dat-0xa0;
			hz++;
			wei_ma=*hz;
			if(wei_ma==0) break;
			wei_ma-=0xa0;
			hz_addr=(qu_ma-1)*94+wei_ma-1;
			(*call_full)(hz_addr,x,y,color_f);
			x+=full_step;
		}
		//ASC码显示处理----
		else
		{
			if(dat<32) return;
			dat-=ASC_Head;
			(*call_half)(dat,x,y,color_f);
			x+=half_step;
		}
		hz++;
	}
}

//===================================================================
//zadd=iAddrZK+32*(((uint32)(zm)-0xa0-1)*94+(uint32)(bm)-0xa0-1);break;
//功能: 显示字符串
//*hz      : 要显示的字符串指针
//aligen   : 显示的对齐方式,0=左对齐,1=居中显示,2=右对齐
//x         : 显示容器的起始横坐标
//y		 : 显示容器的起始纵坐标
//width   : 显示容器的总宽度
//color_f  : 字体颜色
//color_b : 背景色
//修改时间:		2010-11-11
//===================================================================
void Dis_StringsAligenFilter(const char *hz,uint32 aligen,uint16 x,uint16 y,uint32 width,uint16 font,uint16 color_f)
{
uint8 dat;
uint32 qu_ma,wei_ma;
uint32 hz_addr;
uint32 tol;					//半角字符的总个数
uint32 i,m;
uint32 full_step;				//全角占用的象素宽度
uint32 half_step;				//半角占用的象素宽度	
void (*call_full)(uint32 num, uint16 x, uint16 y, uint16 color_f);
void (*call_half)(uint8 num, uint16 x, uint16 y, uint16 color_f);
	//字体为24点阵处理----------
	if(font==FONT24) 
	{
		call_full=Dis_24x24Filter;
		call_half=Dis_12x24Filter;
		full_step=FONT24;
		half_step=FONT24>>1;
	}
	else
	{
		call_full=Dis_16x16Filter;
		call_half=Dis_8x16Filter;
		full_step=FONT16;
		half_step=FONT16>>1;
	}

	tol=0;
	//若不是左对齐显示，则计算显示的起始x坐标----
	if(aligen!=ALIGEN_LEFT)
	{
		for(i=0;i<80;i++)
		{
			dat=*(hz+i);
			//有效的字符----
			if(dat>=32) tol++;
			else 
			{
				//居中显示处理---------
				if(aligen==ALIGEN_MIDDLE)
				{
					m=width-tol*half_step;
					x=x+m/2;
				}
				//右对齐显示处理----
				else
				{
					x=x+(width-tol*half_step);
				}
				break;
			}
		}
	}
	
	while(1)
	{
		dat=*hz;
		if(dat==0) break;		//到字串结束
		//是汉字处理----
		if(dat>=0x80)
		{
			qu_ma=dat;
			hz++;
			wei_ma=*hz;
			if(wei_ma==0) break;
			if((qu_ma>0xa0)&&(wei_ma>0xa0))
			{
				qu_ma-=0xa0;					
				wei_ma-=0xa0;
				hz_addr=(qu_ma-1)*94+wei_ma-1;
				(*call_full)(hz_addr,x,y,color_f);
			}
			x+=full_step;
		}
		//ASC码显示处理----
		else
		{
			if(dat<32) return;
			dat-=ASC_Head;
			(*call_half)(dat,x,y,color_f);
			x+=half_step;
		}
		hz++;
	}
}

//===================================================================
//zadd=iAddrZK+32*(((uint32)(zm)-0xa0-1)*94+(uint32)(bm)-0xa0-1);break;
//画填充的矩形函数
//x:		起始横坐标
//y:		起始纵坐标
//width:	矩形的度度
//heigh:	矩形的高度
//color:	线的颜色
//===================================================================
void Dis_Chinese(uint16 hz,uint16 x,uint16 y,uint16 color_f,uint16 color_b)
{
uint32 qu_ma,wei_ma;
uint32 hz_addr;
	qu_ma=hz>>8;
	wei_ma=hz&0xff;
	if((qu_ma<0xa0)||(wei_ma<0xa0)) return;
	qu_ma-=0xa0;
	wei_ma-=0xa0;
	hz_addr=(qu_ma-1)*94+wei_ma-1;
	Dis_16x16(hz_addr,x,y,color_f,color_b);
}

//===================================================================
//功能:			显示一个数值
//显示一个数值,左对齐,高位0自动消隐
//weisu:  数值的最大位数
//stt:     0为正常显示,1为反白显示
//修改时间:		2010-11-20
//===================================================================
void Dis_NumHide(uint16 dat,uint16 x,uint16 y,uint8 weisu,uint16 font,uint16 color_f,uint16 color_b)
{
uint8 i;
uint8 youxiao;
uint8 buf[5];
uint8 num;
uint32 width;
	if(font==FONT24) width=12;
	else width=8;
	num=0;
	switch(weisu)
	{
		case 1:
			if(dat>=10) i=0;
			else i=(uint8)dat;
			Dis_SuZi(i,x,y,font,color_f,color_b);
			break;
		case 2:
			if(dat>=100) dat=0;
			buf[0]=dat/10;
			buf[1]=dat%10;
			for(i=0;i<2;i++)
			{
				if((buf[i]>0)||(i==1))
				{
					Dis_SuZi(buf[i],x,y,font,color_f,color_b);
					x+=width;
					num++;
				}
			}
			if(num!=2)
			{
				Dis_Strings(" ",x,y,font,color_b,color_b);
			}
			break;
		case 3:
			youxiao=0;
			if(dat>=1000) dat=0;
			buf[0]=dat/100;
			dat%=100;
			buf[1]=dat/10;
			buf[2]=dat%10;
			for(i=0;i<3;i++)
			{
				if((buf[i]>0)||(i==2)||(youxiao>0))
				{
					Dis_SuZi(buf[i],x,y,font,color_f,color_b);
					x+=width;
					youxiao=1;
					num++;
				}
			}
			if(num!=3)
			{
				for(i=num;i<3;i++)
				{
					Dis_Strings(" ",x,y,font,color_b,color_b);
					x+=width;
				}
			}
			break;
		case 4:
			youxiao=0;
			if(dat>=10000) dat=0;
			buf[0]=dat/1000;
			dat%=1000;
			buf[1]=dat/100;
			dat%=100;
			buf[2]=dat/10;
			buf[3]=dat%10;
			for(i=0;i<4;i++)
			{
				if((buf[i]>0)||(i==3)||(youxiao>0))
				{
					Dis_SuZi(buf[i],x,y,font,color_f,color_b);
					x+=width;
					youxiao=1;
					num++;
				}
			}
			if(num!=4)
			{
				for(i=num;i<4;i++)
				{
					Dis_Strings(" ",x,y,font,color_b,color_b);
					x+=width;
				}
			}
			break;
		case 5:
			youxiao=0;
			buf[0]=dat/10000;
			dat%=10000;
			buf[1]=dat/1000;
			dat%=1000;
			buf[2]=dat/100;
			dat%=100;
			buf[3]=dat/10;
			buf[4]=dat%10;
			for(i=0;i<5;i++)
			{
				if((buf[i]>0)||(i==4)||(youxiao>0))
				{
					Dis_SuZi(buf[i],x,y,font,color_f,color_b);
					x+=width;
					youxiao=1;
					num++;
				}
			}
			if(num!=5)
			{
				for(i=num;i<5;i++)
				{
					Dis_Strings(" ",x,y,font,color_b,color_b);
					x+=width;
				}
			}
			break;
		default:
			break;
	}
}

void Dis_NumAll(uint16 dat,uint16 x,uint16 y,uint8 weisu,uint16 font,uint16 color_f,uint16 color_b)
{
uint32 i;
uint8 wei[5];
uint16 step;				//每个字符占用的象素点
	if(font==FONT24) step=12;
	else step=8;
	wei[4]=0;
	wei[3]=0;
	wei[2]=0;
	wei[1]=0;
	if(dat>=10000) 
	{
		wei[4]=dat/10000;
		dat%=10000;
	}
	if(dat>=1000)
	{
		wei[3]=dat/1000;
		dat%=1000;
	}
	if(dat>=100)
	{
		wei[2]=dat/100;
		dat%=100;
	}
	if(dat>=10) wei[1]=dat/10;
	wei[0]=dat%10;
	for(i=weisu;i>0;i--)
	{
		if(wei[i-1]>9) wei[i-1]=0;
		Dis_SuZi(wei[i-1],x,y,font,color_f,color_b);
		x+=step;
	}
}

//===================================================================
//功能: 显示数值(不消隐),背景色为LOGO图片
//===================================================================
void Dis_NumAllLogo(uint16 dat,uint16 x,uint16 y,uint8 weisu,uint16 color_f)
{
uint32 i;
uint8 wei[5];
	wei[4]=0;
	wei[3]=0;
	wei[2]=0;
	wei[1]=0;
	if(dat>=10000) 
	{
		wei[4]=dat/10000;
		dat%=10000;
	}
	if(dat>=1000)
	{
		wei[3]=dat/1000;
		dat%=1000;
	}
	if(dat>=100)
	{
		wei[2]=dat/100;
		dat%=100;
	}
	if(dat>=10) wei[1]=dat/10;
	wei[0]=dat%10;
	for(i=weisu;i>0;i--)
	{
		if(wei[i-1]>9) wei[i-1]=0;
		Dis_SuZiLogo(wei[i-1],x,y,color_f);
		x+=8;
	}
}

//===================================================================
//功能: 显示带小数的数据(最多2位小数)
//dat      : 将小数部分乘以10的N次方后转换成整数的数据
//weisu   : 原先整数部分的位数
//xiaosu  : 原先小数部分的位数
//===================================================================
void Dis_NumFloat(uint32 dat,uint16 x,uint16 y,uint8 weisu,uint8 xiaosu,uint16 font,uint16 color_f,uint16 color_b)
{
uint32 m;
uint32 zensu;
uint32 step;
	if(font==FONT24) step=12;
	else step=8;
	if(xiaosu==1) m=10;
	else if(xiaosu==2) m=100;
	else return;
	zensu=dat/m;
	Dis_NumHide(zensu,x,y,weisu,font,color_f,color_b);
	if(xiaosu==1) Dis_Strings("  ",x+weisu*step,y,font,color_f,color_b);
	else Dis_Strings("   ",x+weisu*step,y,font,color_f,color_b);
	if(zensu<10) x+=step;
	else if(zensu<100) x+=step*2;
	else if(zensu<1000) x+=step*3;
	Dis_Strings(".",x,y,font,color_f,color_b);
	x+=step;
	zensu=dat%m;
	Dis_NumAll(zensu,x,y,xiaosu,font,color_f,color_b);
}

//===================================================================
//功能:			显示带小数的数据(固定为.0或.5)
//参数:			
//		dat:			整数数据
//		five:			不为0表示有0.5
//		weisu:		原先整数部分的位数
//		x,y:			显示的起始x,y坐标
//		color_f:		字体颜色
//		color_b:		背景色
//说明:			整数最大为5位数
//修改时间:		2011-11-28
//===================================================================
void Dis_NumPoint5(uint32 dat,uint32 five,uint16 x,uint16 y,uint8 weisu,uint16 font,uint16 color_f,uint16 color_b)
{
uint32 i;
uint32 m;
uint32 step;
uint32 num;				//整数的实际位数
	if(font==FONT24) step=12;
	else step=8;
	if(dat<10) num=1;
	else if(dat<100) num=2;
	else if(dat<1000) num=3;
	else if(dat<10000) num=4;
	else num=5;
	if(num>weisu) num=weisu;
	Dis_NumHide(dat,x,y,weisu,font,color_f,color_b);
	if(five>0)
	{
		Dis_Strings(".5",x+num*step,y,font,color_f,color_b);
	}
	else 
	{
		Dis_Strings(".0",x+num*step,y,font,color_f,color_b);
	}
	m=weisu-num;
	for(i=0;i<m;i++)
	{
		Dis_Strings(" ",x+(num+i+2)*step,y,font,color_f,color_b);
	}
}

//===================================================================
//功能: 显示16位有符号数的符号
//dat      : 数据(取绝对值)
//zf        : 有符号数的正负标志0=正,1=负
//修改时间:		2010-11-11
//===================================================================
void Dis_NumFuHao(uint16 dat,uint8 zf,uint16 x,uint16 y,uint16 font,uint16 color_f,uint16 color_b)
{
	if(dat==0) 
	{
		Dis_Strings(" ",x,y,font,color_f,color_b);
	}
	else if(zf==0) 
	{
		Dis_Strings("+",x,y,font,color_f,color_b);
	}
	else 
	{
		Dis_Strings("-",x,y,font,color_f,color_b);
	}
}

//===================================================================
//功能:			显示一个有符号数值
//参数:
//		dat:			16位有符号数
//		zf:			不为0表示负数(数值为0时有效)
//		x,y:			显示的起始x,y坐标
//		weisu:		数值的最大位数
//		font:			字体类型
//		color_f:		字体颜色
//		color_b:		字体背景色
//说明:			显示一个数值,左对齐,高位0自动消隐
//修改时间:		2011-11-28
//===================================================================
void Dis_NumInt(int16 dat,uint16 x,uint16 y,uint8 weisu,uint16 font,uint16 color_f,uint16 color_b)
{
uint16 m;
uint32 width;
	if(font==FONT24) width=12;
	else width=8;
	//等于0处理-----
	if(dat==0) 
	{
		m=dat;
		Dis_NumHide(m,x,y,weisu,font,color_f,color_b);
		Dis_Strings(" ",x+weisu*width,y,font,color_f,color_b);
	}
	//大于0处理-----
	else if(dat>0)
	{
		m=dat;
		Dis_Strings("+",x,y,font,color_f,color_b);
		Dis_NumHide(m,x+width,y,weisu,font,color_f,color_b);		
	}
	//小于0处理-----
	else
	{
		m=0-dat;
		Dis_Strings("-",x,y,font,color_f,color_b);
		Dis_NumHide(m,x+width,y,weisu,font,color_f,color_b);		
	}
}


//===================================================================
//功能: 显示一个带标题栏的Windows窗体(标题栏颜色有激变效果)
//             标题栏的文字颜色为白色
//x         : 起始横坐标
//y         : 起始纵坐标
//width   : 窗体的宽度
//heigh   : 窗体的高度
//ref_back: 不为0表示要刷新窗体内的背景色
//head_color: 标题栏的背景色样式0~2
//说明: 顶部标题栏高度24 (右侧显示X表示按标题栏关闭)
//		   标题栏文字居中对齐
//修改时间:		2010-11-10
//===================================================================
void Dis_WindowsForm(S_Form *form)
{
uint32 TOP_H;			//顶部标题栏的高度
uint16 x,y,w,h;
uint16 color;
uint32 i,len;			
uint32 c_x;
uint32 asc_dot;
	x=form->X;
	y=form->Y;
	w=form->Width;
	h=form->Height;
	if(form->TitleColor>MAX_FormTitle) form->TitleColor=FORM_Blue;
	if(form->TitleFont!=FONT24) form->TitleFont=FONT16;
	if((form->TitleHeight<TITLE_H16)||(form->TitleHeight>(TITLE_H24*2)))
	{
		form->TitleHeight=TITLE_H16;
		form->TitleFont=FONT16;
	}

	TOP_H=form->TitleHeight;	
	//标题栏背景色-----
	for(i=0;i<TOP_H;i++)
	{
		//标题栏高度大于24时的背景色处理------
		if(TOP_H>24)
		{
			if(i<12)
			{
				color=FormTitleColor[form->TitleColor][i];
			}
			else if((TOP_H-i)>=12)
			{
				color=FormTitleColor[form->TitleColor][12];
			}
			else
			{
				color=FormTitleColor[form->TitleColor][24-(TOP_H-i)];
			}
		}
		else
		{
			color=FormTitleColor[form->TitleColor][i];
		}
		Dis_HLine(x+1,y+i,w-2,color);
	}

	c_x=x+w-20-4;
	if(form->TitleColor==0) len=BMP_CloseBlue;
	else len=BMP_CloseRed;
	Dis_PictureFilter(c_x,y+((TOP_H-20)>>1),20,20,BmpFunc+len,LCD_FilterColor);		//显示关闭图片

	//左边两条竖线
	Dis_VLine(x,y,h,FormTitleColor[form->TitleColor][23]);
	Dis_VLine(x+1,y+1,h-2,FormTitleColor[form->TitleColor][12]);
	//右边两条竖线
	Dis_VLine(x+w-2,y+1,h-1,FormTitleColor[form->TitleColor][23]);
	Dis_VLine(x+w-1,y,h,FormTitleColor[form->TitleColor][23]);
	//底部两条横线(黑色)
	Dis_HLine(x+2,y+h-2,w-4,FormTitleColor[form->TitleColor][12]);
	Dis_HLine(x,y+h-1,w,FormTitleColor[form->TitleColor][23]);

	len=0;
	asc_dot=(form->TitleFont>>1);
	for(i=0;i<100;i++)
	{
		if(*(form->TitleText+i)!=0) len+=asc_dot;
		else break;
	}
	i=w/2-len/2;
	len=x+i;
	Dis_StringsFilter(form->TitleText,len,y+(TOP_H-form->TitleFont)/2,form->TitleFont,C_White);
	//填充窗体内的背景色-----
	if(form->FillBack>0)
	{
		Dis_RectFill(x+2,y+TOP_H,w-4,h-TOP_H-2,form->BackColor);
	}
}

//===================================================================
//功能:			显示一个带标题栏的Windows窗体
//入口参数:		
//		form:		窗体结构指针
//说明: 			标题栏文字靠左对齐
//修改时间:		2010-11-23
//===================================================================
void Dis_WindowsFormLeft(S_Form *form)
{
uint32 TOP_H;			//顶部标题栏的高度
uint16 x,y,width,height;
uint32 i;		
uint16 color;
uint32 addr;
	if(form->TitleColor>MAX_FormTitle) form->TitleColor=FORM_Blue;
	x=form->X;
	y=form->Y;
	width=form->Width;
	height=form->Height;
	TOP_H=form->TitleHeight;

	//标题栏背景色-----
	for(i=0;i<TOP_H;i++)
	{
		//标题栏高度大于24时的背景色处理------
		if(TOP_H>24)
		{
			if(i<12)
			{
				color=FormTitleColor[form->TitleColor][i];
			}
			else if((TOP_H-i)>=12)
			{
				color=FormTitleColor[form->TitleColor][12];
			}
			else
			{
				color=FormTitleColor[form->TitleColor][24-(TOP_H-i)];
			}
		}
		else
		{
			color=FormTitleColor[form->TitleColor][i];
		}
		Dis_HLine(x+1,y+i,width-2,color);
	}
	
	//显示关闭图标-----
	if(form->TitleColor==FORM_Red) addr=BMP_CloseBlue;
	else addr=BMP_CloseRed;
	x=form->X+width-20-4;
	y=form->Y+((form->TitleHeight-20)>>1);
	Dis_PictureFilter(x,y,20,20,BmpFunc+addr,LCD_FilterColor);		//显示关闭图片

	x=form->X;
	y=form->Y;
	//左边两条竖线
	Dis_VLine(x,y,height,FormTitleColor[form->TitleColor][23]);
	Dis_VLine(x+1,y+1,height-2,FormTitleColor[form->TitleColor][12]);
	//右边两条竖线
	Dis_VLine(x+form->Width-2,y+1,form->Height-1,FormTitleColor[form->TitleColor][23]);
	Dis_VLine(x+form->Width-1,y,form->Height,FormTitleColor[form->TitleColor][23]);
	//底部两条横线(黑色)
	Dis_HLine(x+2,y+form->Height-2,form->Width-4,FormTitleColor[form->TitleColor][12]);
	Dis_HLine(x,y+form->Height-1,form->Width,FormTitleColor[form->TitleColor][23]);
	//显示标题栏文本---------
	Dis_StringsFilter(form->TitleText,x+6,form->Y+((TOP_H-form->TitleFont)>>1),form->TitleFont,C_White);
	if(form->FillBack>0)
	{
		Dis_RectFill(x+2,y+form->TitleHeight,form->Width-4,form->Height-form->TitleHeight-2,form->BackColor);	//刷新窗体内的背景色
	}
}

//===================================================================
//功能:			显示一个文本框
//修改时间:		2010-11-11
//===================================================================
void Dis_TextFrame(uint16 x,uint16 y,uint16 width,uint16 height,uint16 color)
{
	//文本区
	Dis_RectFill(x+2,y+2,width-4,height-4,color);
/*
	//顶部两条横线(黑色)
	Dis_HLine(x,y,width,C_Black);
	Dis_HLine(x,y+1,width-1,C_Black);
	//左边两条竖线(黑色)
	Dis_VLine(x,y,height,C_Black);
	Dis_VLine(x+1,y,height-1,C_Black);	
*/
	Dis_RectSome(x,y,width,height,2,C_Blue);
}

//===================================================================
//功能: 显示一个上凸的框体(刷新框体内的颜色)
//color    : 框体的底色
//===================================================================
void Dis_BoxUp(uint16 x,uint16 y,uint16 width,uint16 height,uint16 color)
{
	//底色
	Dis_RectFill(x,y,width,height,color);
	//顶部两条横线(白色)
	Dis_HLine(x,y,width,C_White);
	Dis_HLine(x,y+1,width-1,C_White);
	//底部两条横线(黑色)
	Dis_HLine(x+2,y+height-2,width-2,C_Black);
	Dis_HLine(x+1,y+height-1,width-1,C_Black);
	//左边两条竖线(白色)
	Dis_VLine(x,y,height,C_White);
	Dis_VLine(x+1,y,height-1,C_White);
	//右边两条竖线(黑色)
	Dis_VLine(x+width-2,y+2,height-2,C_Black);
	Dis_VLine(x+width-1,y+1,height-1,C_Black);
}

//===================================================================
//功能: 显示一个下凹的框体(刷新框体内的颜色)
//color    : 框体的底色
//===================================================================
void Dis_BoxDown(uint16 x,uint16 y,uint16 width,uint16 height,uint16 color)
{
	//底色
	Dis_RectFill(x,y,width,height,color);
	//顶部两条横线(黑色)
	Dis_HLine(x,y,width,C_Black);
	Dis_HLine(x,y+1,width-1,C_Black);
	//底部两条横线(白色)
	Dis_HLine(x+2,y+height-2,width-2,C_White);
	Dis_HLine(x+1,y+height-1,width-1,C_White);
	//左边两条竖线(黑色)
	Dis_VLine(x,y,height,C_Black);
	Dis_VLine(x+1,y,height-1,C_Black);
	//右边两条竖线(白色)
	Dis_VLine(x+width-2,y+2,height-2,C_White);
	Dis_VLine(x+width-1,y+1,height-1,C_White);
}

//===================================================================
//功能: 显示一个上凸的框体(不刷新框体内的颜色)
//color    : 深色线的颜色
//===================================================================
void Dis_FrameUp(uint16 x,uint16 y,uint16 width,uint16 height,uint16 color)
{
	//顶部两条横线(白色)
	Dis_HLine(x,y,width,C_White);
	Dis_HLine(x,y+1,width-1,C_White);
	//底部两条横线(深色)
	Dis_HLine(x+2,y+height-2,width-2,color);
	Dis_HLine(x+1,y+height-1,width-1,color);
	//左边两条竖线(白色)
	Dis_VLine(x,y,height,C_White);
	Dis_VLine(x+1,y,height-1,C_White);
	//右边两条竖线(深色)
	Dis_VLine(x+width-2,y+2,height-2,color);
	Dis_VLine(x+width-1,y+1,height-1,color);
}

//===================================================================
//功能: 显示一个下凹的框体(不刷新框体内的颜色)
//color    : 深色线的颜色
//===================================================================
void Dis_FrameDown(uint16 x,uint16 y,uint16 width,uint16 height,uint16 color)
{
	//顶部两条横线(深色)
	Dis_HLine(x,y,width,color);
	Dis_HLine(x,y+1,width-1,color);
	//底部两条横线(白色)
	Dis_HLine(x+2,y+height-2,width-2,C_White);
	Dis_HLine(x+1,y+height-1,width-1,C_White);
	//左边两条竖线(深色)
	Dis_VLine(x,y,height,color);
	Dis_VLine(x+1,y,height-1,color);
	//右边两条竖线(白色)
	Dis_VLine(x+width-2,y+2,height-2,C_White);
	Dis_VLine(x+width-1,y+1,height-1,C_White);
}

//===================================================================
//功能  : 			显示一个Windows类型的提示框(不作按键处理)
//入口参数:
//		title:		标题栏上的文本信息
//		text:		提示信息文本的内容(居中显示)
//		push:		不为0表示要将窗体区域的背景先入栈再显示
//		color_f:		提示文本的颜色
//		color_b:		窗体的背景色
//说明:			窗体自动居中显示
//					采用24点阵字体
//注意:			当push不为0时,调用本函数结束后应有显存出栈处理
//修改时间:		2010-12-1
//===================================================================
void Dis_HintBox(const char *title,const char *text,uint32 push,uint16 color_f,uint16 color_b)
{
uint32 i;
uint32 x,y;
uint32 len;
S_Form form;
	form.Height=70;
	form.TitleHeight=TITLE_H24;
	form.TitleFont=FONT24;
	form.Y=GET_ScrCenterY(form.Height);
	form.TitleText=PTEXT(title);
	form.TitleColor=FORM_Red;
	form.BackColor=color_b;
	form.FillBack=1;
	//计算字符串的长度(象素值)------
	len=0;
	for(i=0;i<100;i++)
	{
		if(*(text+i)!=0) len+=12;
		else break;
	}
	//根据字符长度确定窗体宽度----
	form.Width=len+60;		//两边各空60个象素点
	form.X=GET_ScrCenterX(form.Width);
	//要求显存入栈处理-------------
	if(push>0) 
	{
		LcdMemPush(form.X,form.Y,form.Width,form.Height);
	}
	Dis_WindowsFormLeft(&form);

	x=(form.Width-len)>>1;
	x=form.X+x;
	y=form.Y+form.TitleHeight+((form.Height-form.TitleHeight-FONT24)>>1);
	Dis_Strings(text,x,y,FONT24,color_f,form.BackColor);	//显示提示文本
}

//===================================================================
//功能  : 			显示一个Windows类型的提示框(按任意键后退出)
//入口参数:
//		title:		标题栏上的文本信息
//		text:		提示信息文本的内容(居中显示)
//		color_f:		提示文本的颜色
//		color_b:		窗体的背景色
//说明  : 			自动装载和恢复调用前的显示信息
//					窗体自动居中显示
//           			根据提示文本的长度自动调整宽度
//					显示字体固定为24点阵
//修改时间:		2013-3-7
//===================================================================
void Dis_HintForm(const char *title,const char *text,uint16 color_f,uint16 color_b)
{
uint16 x,y;
uint32 i;
uint32 len;
uint32 tchnum;							//备份的总触摸键数
S_TchKey tch_backup;						//备份的触摸键信息
S_Form form;
	form.Height=70;
	form.TitleHeight=TITLE_H24;
	form.TitleFont=FONT24;
	form.Y=GET_ScrCenterY(form.Height);
	form.TitleText=PTEXT(title);
	form.TitleColor=FORM_Red;
	form.BackColor=color_b;
	form.FillBack=1;
	//计算字符串的长度(象素值)------
	len=0;
	for(i=0;i<100;i++)
	{
		if(*(text+i)!=0) len+=12;
		else break;
	}
	//根据字符长度确定窗体宽度----
	form.Width=len+60;		//两边各空30个象素点
	form.X=GET_ScrCenterX(form.Width);
	LcdMemPush(form.X,form.Y,form.Width,form.Height);

	Dis_WindowsFormLeft(&form);	
	x=(form.Width-len)>>1;
	x=form.X+x;
	y=form.Y+form.TitleHeight+((form.Height-form.TitleHeight-FONT24)>>1);
	Dis_Strings(text,x,y,FONT24,color_f,color_b);	//显示提示文本
	//保存第1个触摸键信息-----
	tch_backup=TchKey[0][0];
	tchnum=TchVal.KeyNum[0];
	Touch_KeyEn(form.X,form.Y,form.Width,form.Height,KEY_Tch,0);
	TchVal.KeyNum[0]=1;
	WaitAnyKey();
	//恢复之前的第1个触摸键信息----
	TchKey[0][0]=tch_backup;
	TchVal.KeyNum[0]=tchnum;
	LcdMemPop();
}

//===================================================================
//功能:			显示一个Windows类型的输入框
//入口参数:
//		max:			允许输入的最大值
//		dat:			默认的初始数值
//		text:			提示信息文本的内容
//		color_b:		窗体的背景色
//返回:			返回输入值,若取消则返回DO_Cancel
//说明:			采用24点阵字体
//					窗体自动居中,窗体宽度根据内容自动算计
//修改时间:		2011-3-14
//===================================================================
uint32 Dis_InputDialog(uint32 max,uint32 dat,const char *text,uint16 color_b)
{
uint32 i;
uint16 x,y;
uint32 len;
uint32 ret;
uint32 weisu;		
uint32 dat_len;
uint32 old_chang;				//用于恢复先前的Sys.DatChang变量
S_Form form;
	if(dat>max) dat=0;
	old_chang=Sys.DatChang;
	//计算字符串的长度------
	len=0;
	for(i=0;i<100;i++)
	{
		if(*(text+i)!=0) len+=12;
		else break;
	}
	//判断输入值的最大位数-----
	if(max<10) weisu=1;
	else if(max<100) weisu=2;
	else if(max<1000) weisu=3;
	else if(max<10000) weisu=4;
	else weisu=5;
	dat_len=weisu*12;
	
	form.Width=len+dat_len+50;		//自动计算窗体宽度
	if(form.Width<160) form.Width=160;
	form.Height=80;
	form.X=GET_ScrCenterX(form.Width);
	form.Y=GET_ScrCenterY(form.Height);
	form.TitleText=Hz_InputDialog[0][Lang];
	form.TitleHeight=TITLE_H24;
	form.TitleFont=FONT24;
	form.TitleColor=FORM_Blue;
	form.BackColor=color_b;
	form.FillBack=1;

	LcdMemPush(form.X,form.Y,form.Width,form.Height);
	Dis_WindowsFormLeft(&form);

	//显示提示字符串-----------
	x=form.X+10;
	y=form.Y+form.TitleHeight+((form.Height-form.TitleHeight-FONT24)>>1);
	Dis_Strings(text,x,y,FONT24,C_Black,form.BackColor);
	x=x+len+10;
	Dis_TextFrame(x,y-2,dat_len+20,28,C_White);
	Sys.LcdRef=1;
	Key.ReInput=1;
	TchVal.RefX=x;
	TchVal.RefY=y-2;
	TchVal.RefWidth=dat_len+20;
	TchVal.RefHeight=FONT24;
	Dis_TouchKeyBoard(TKEY_BoardStand);
	
	while(1)
	{
		Send_DspReply();		//发送中断中置位的命令回码		
	
		if(Sys.LcdRef>0)
		{
			Sys.LcdRef=0;
			Dis_NumAll(dat,x+10,y,weisu,FONT24,C_Red,C_White);
		}
	
		Read_Key();

		Dis_TouchButtonFlash();			//刷新模拟触摸键盘的变化状态		

		dat=Data_Input(max,0,dat);
		
		if(Key.Value==KEY_ESC)
		{
			ret=DO_Cancel;
			break;
		}
		else if(Key.Value==KEY_OK)
		{
			ret=dat;
			break;
		}
	}
	
	TouchKeyBoardClose();
	Key.Value=KEY_No;
	LcdMemPop();
	Sys.DatChang=old_chang;
	return(ret);
}

//===================================================================
//功能:			显示一个Windows类型的字符串输入框
//参数:
//		dat:			输入的字符指针
//		len:			允许输入的最大字符个数
//		text:			提示信息文本的内容
//		color_b:		窗体的背景色
//返回:			若取消则返回DO_Cancel
//说明:			采用24点阵字体
//					窗体自动居中,窗体宽度根据内容自动算计
//修改时间:		2011-11-30
//===================================================================
uint32 Dis_InputStrDialog(char *str,uint32 len,const char *text,uint16 color_b)
{
enum{ASCW=12};
uint32 i;
uint32 k;
uint16 x,y;
uint32 text_len;
char string[20];
uint16 c_b;
uint32 ret;
S_Form form;
	k=0;
	memcpy(string,str,len);
	//计算字符串的长度------
	text_len=0;
	for(i=0;i<100;i++)
	{
		if(*(text+i)!=0) text_len+=ASCW;
		else break;
	}
	
	form.Width=len*ASCW+text_len+50;			//自动计算窗体宽度
	if(form.Width<160) form.Width=160;
	form.Height=80;
	form.X=GET_ScrCenterX(form.Width);
	form.Y=GET_ScrCenterY(form.Height)-60;		//保证模拟键盘在右下方
	form.TitleText=Hz_InputDialog[0][Lang];
	form.TitleHeight=TITLE_H24;
	form.TitleFont=FONT24;
	form.TitleColor=FORM_Blue;
	form.BackColor=color_b;
	form.FillBack=1;

	LcdMemPush(form.X,form.Y,form.Width,form.Height);
	Dis_WindowsFormLeft(&form);

	//显示提示字符串-----------
	x=form.X+10;
	y=form.Y+form.TitleHeight+((form.Height-form.TitleHeight-FONT24)>>1);
	Dis_Strings(text,x,y,FONT24,C_Black,form.BackColor);
	x=x+text_len+10;
	Dis_TextFrame(x,y-2,len*ASCW+20,28,C_White);
	Sys.LcdRef=1;
	Key.ReInput=1;
	TchVal.RefX=x;
	TchVal.RefY=y-2;
	TchVal.RefWidth=len*ASCW+20;
	TchVal.RefHeight=FONT24;
	Dis_TouchKeyBoard(TKEY_BoardAsc);
	
	while(1)
	{
		Send_DspReply();		//发送中断中置位的命令回码		
	
		if(Sys.LcdRef>0)
		{
			Sys.LcdRef=0;
			for(i=0;i<len;i++)
			{
				if(i==k) c_b=C_Black;
				else c_b=C_White;
				Dis_Char(string[i],x+10+i*ASCW,y,FONT24,C_Red,c_b);
			}
		}
	
		Read_Key();

		Dis_TouchButtonFlash();			//刷新模拟触摸键盘的变化状态		

		if(Key.Value<=KEY_9)
		{
			Sys.LcdRef=REF_Item;
			string[k]=Key.Value+'0';
			k++;
			if(k>=len) k=0;
		}	
		else if((Key.Value>=KEY_A)&&(Key.Value<=KEY_F))
		{
			Sys.LcdRef=REF_Item;
			string[k]=Key.Value-KEY_A+'A';
			k++;
			if(k>=len) k=0;			
		}
		else if(Key.Value==KEY_QUK)
		{	
			Sys.LcdRef=REF_Item;
			string[k]='0';
			if(k>0)
			{
				k--;
			}
		}
		else if(Key.Value==KEY_RIGHT)
		{
			if((k+1)<len)
			{
				Sys.LcdRef=REF_Item;
				k++;
			}
		}
		else if(Key.Value==KEY_LEFT)
		{
			if(k>0)
			{
				Sys.LcdRef=REF_Item;
				k--;
			}
		}

		if(Key.Value==KEY_OK)
		{
			memcpy(str,string,len);
			ret=0;
			break;
		}		
		else if(Key.Value==KEY_ESC)
		{
			ret=DO_Cancel;
			break;
		}
	}
	
	TouchKeyBoardClose();
	Key.Value=KEY_No;
	LcdMemPop();
	return(ret);
}

//===================================================================
//功能: 			显示一个Windows类型的确认框
//入口参数:		
//		title:		标题栏上的文字信息
//		text:		提示信息文本的内容
//返回: 			0xffffffff表示取消(DO_Cancel),其它表示确定
//说明: 			退出时自动恢复被窗体覆盖的显示信息
//					字体为24点阵
//修改时间:		2010-11-29
//===================================================================
uint32 Dis_SureDialog(const char *title,const char *text,uint16 back_color)
{
uint32 i;
uint32 x,y;
uint32 ret;
uint32 len;
uint32 btnw;
S_Form form;
	Touch_KeyPush();
	TchVal.KeyNum[0]=2;
	//计算提示文本的长度(以半角算)-----
	len=0;
	for(i=0;i<100;i++)
	{
		if(*(text+i)!=0) len+=12;
		else break;
	}	
	form.Width=len+30;
	if(form.Width<150) form.Width=150;
	btnw=(form.Width-30*2)>>1;
	form.Height=120;		
	form.X=GET_ScrCenterX(form.Width);
	form.Y=GET_ScrCenterY(form.Height);
	form.TitleText=title;
	form.TitleColor=FORM_Red;
	form.TitleFont=FONT24;
	form.TitleHeight=TITLE_H24;
	form.BackColor=back_color;
	form.FillBack=1;	
	LcdMemPush(form.X,form.Y,form.Width,form.Height);
	Dis_WindowsForm(&form);

	x=(form.Width-len)>>1;
	x+=form.X;
	y=form.Y+form.TitleHeight+10;
	Dis_Strings(text,x,y,FONT24,C_Black,form.BackColor);

	//显示触摸选择框-------------------------------
	x=form.X+15;
	y=form.Y+form.Height-50;
	Dis_TouchButton(HZ_ItemSiFo[0][Lang],x,y,btnw,35,KEY_ESC,0,C_Black,FONT24);

	x=form.X+form.Width-15-btnw;
	Dis_TouchButton(HZ_ItemSiFo[1][Lang],x,y,btnw,35,KEY_OK,1,C_Black,FONT24);

	Key.Value=KEY_No;

	while(1)
	{
		Send_DspReply();		//发送中断中置位的命令回码		
	
		Read_Key();

		Dis_TouchButtonFlash();
		
		if(Key.Value==KEY_OK)
		{
			ret=Key.Value;
			break;
		}
		else if(Key.Value==KEY_ESC)
		{
			ret=DO_Cancel;
			break;
		}
	}
	
	Touch_KeyPop();
	LcdMemPop();
	Key.Value=KEY_No;
	return(ret);
}

//===================================================================
//功能: 			自动计算最优的显示位置
//入口参数:
//		inx/y/w/h:	输入框的X,Y坐标及宽度高度
//		width: 		触摸输入界面的宽度
//		height: 		触摸输入界面的高度
//		first: 		0表示优先显示在下面,1表示优先显示在右边
//修改时间:		2010-11-15
//===================================================================
void Get_AutoXY(uint16 inx,uint16 iny,uint16 inw,uint16 inh,uint32 width,uint32 height,uint32 first,uint16 *rx,uint16 *ry)
{
enum {
	JIANGE_X=5,
	JIANGE_Y=6,
	JIANGE_Side=20		//到最边上的最小允许值
};
uint32 x,y;
uint32 start_x,start_y;
uint32 xy_ok;					//得到正确的位置
uint32 count;
	xy_ok=0;
	count=2;
	while(count--)
	{
		//优先在右边显示的位置处理------------
		if(first==FIRST_Right)
		{
			x=inx+inw+JIANGE_X+width+JIANGE_Side;
			y=iny+height+JIANGE_Side;
			//判断能否显示在右边-------
			if(x<LCD_XSize)
			{
				start_x=inx+inw+JIANGE_X;
				//显示在下面----
				if(y<LCD_YSize)
				{
					start_y=iny;
				}
				//显示在上面----
				else
				{
					start_y=LCD_YSize-height-JIANGE_Side;
				}
				xy_ok=1;
				break;
			}
		}
		
		//优先在下面显示的位置处理------------
		if(first==FIRST_Down)
		{
			//判断能否显示在下方---------------
			x=inx+width+JIANGE_X+JIANGE_Side;
			y=iny+inh+height+JIANGE_Y+JIANGE_Side;
			//判断输入框能否显示在下方----------
			if(y<LCD_YSize)
			{
				start_y=iny+inh+JIANGE_Y;
				//判断输入框能否显示在右方----------
				if(x<LCD_XSize)
				{
					start_x=inx+JIANGE_X;
				}
				else
				{
					start_x=LCD_XSize-width-JIANGE_Side;
				}
				xy_ok=1;
				break;
			}
			
			//判断能否显示在上方----
			x=inx+width+JIANGE_X+JIANGE_Side;
			y=height+JIANGE_Y+JIANGE_Side;
			if(y<iny)
			{
				start_y=iny-height-JIANGE_Y;
				//判断输入框能否显示在右方----------
				if(x<LCD_XSize)
				{
					start_x=inx+JIANGE_X;
				}
				else
				{
					start_x=LCD_XSize-width-JIANGE_Side;
				}
				xy_ok=1;
				break;
			}
		}

		if(first==FIRST_Down) first=FIRST_Right;
		else first=FIRST_Down;
	}

	//显示在左边处理------------------------------
	if(xy_ok==0)
	{
		start_x=inx-width-JIANGE_X;
		y=iny+height+JIANGE_Y;
		if(y<LCD_YSize)
		{
			start_y=iny;
		}
		else 
		{
			start_y=LCD_YSize-height-JIANGE_Y;
		}
	}
	
	*rx=start_x;
	*ry=start_y;
}

//===================================================================
//功能:			显示一个Windows类型的选择框
//参数:
//		text:			选择栏提示信息文本的内容
//		item:		选择框的项数
//		inx/y/w/h:	输入框的起始坐标及宽度高度
//		k:			初始化选项值
//返回: 			0xffffffff表示取消,其它表示选择的项号(从0开始)
//说明: 			退出时自动恢复被窗体覆盖的显示信息
//					自动计算窗体宽度
//修改时间:		2013-3-7
//===================================================================
uint32 Dis_SelectDialog(const char *text[][LANG_MAX],uint32 item,uint16 inx,uint16 iny,uint16 inw,uint16 inh,uint32 k,uint32 b_color)
{
enum {ASCW=12};
const char *Hz_Selcet[][LANG_MAX]={"请选择","Select",};
uint32 i;
uint32 x,y;
uint32 color;
uint16 rx,ry;
uint32 ret;
uint32 step;
uint32 buttonw;				//项目栏的宽度
uint32 ref;
uint32 ref_count;
uint32 kk;
uint32 old;
uint32 len;
char *p;
uint32 asc_num;				//允许最大的字符长度(半角)
S_Form form;
	if((item==0)||(item>9)) return(DO_Cancel);
	step=ASCW;
	old=k;
	Touch_KeyPush();

	len=0;
	//计算最大的字符长度-----
	for(i=0;i<item;i++)
	{
		p=(char *)text[i][Lang];
		asc_num=0;
		while(1)
		{
			if(*p==0) break;
			asc_num++;
			if(asc_num>20) 
			{
				break;
			}
			p++;
		}

		if(asc_num>len)
		{
			len=asc_num;
		}
	}
	
	//窗体信息数据初始化----------
	buttonw=len*step+step;
	form.Width=buttonw+30;
	if(form.Width<130) 
	{
		form.Width=130;
		buttonw=form.Width-30;
	}
	form.Height=TITLE_H24+32*item+15;		//窗体的高度
	Get_AutoXY(inx,iny,inw,inh,form.Width,form.Height,FIRST_Right,&rx,&ry);
	form.Y=ry;
	form.X=rx;
	form.TitleText=Hz_Selcet[0][Lang];
	form.TitleColor=FORM_Red;
	form.TitleHeight=TITLE_H24;
	form.TitleFont=FONT24;
	form.BackColor=b_color;
	form.FillBack=1;	
	LcdMemPush(form.X,form.Y,form.Width,form.Height);	
	Dis_WindowsForm(&form);
	i=(form.Width-buttonw)>>1;
	x=form.X+i;
	Touch_KeyEn(form.X,form.Y,form.Width,form.TitleHeight,KEY_ESC,item);
	Key.Value=KEY_No;
	ref=REF_Item;
	TchVal.KeyNum[0]=item+1;	

	while(1)
	{
		Send_DspReply();		//发送中断中置位的命令回码		
	
		if(ref>0)
		{
			if(ref==REF_Now) ref_count=2;
			else ref_count=1;
			for(i=0;i<item;i++)
			{
				if(ref==REF_Now)
				{
					ref_count--;
					if(ref_count==1) i=kk;
					else i=k;
				}
				
				y=form.Y+form.TitleHeight+8+i*32;	
				if(i==k) color=C_Red;
				else color=C_Black;
				Dis_TouchButton(text[i][Lang],x,y,buttonw,30,i+1,i,color,FONT24);
				if(ref==REF_Now)
				{
					if(ref_count==0) break;
					else i=0;
				}
			}
			ref=0;
			kk=k;
		}
		
		Read_Key();

		Dis_TouchButtonFlash();

		if(TchVal.Down==TCH_KeySlide) Key.Value=KEY_No;			//滑动键无效
		
		if((Key.Value>0)&&(Key.Value<=item))
		{
			ret=Key.Value-1;
			break;
		}
		else if(Key.Value==KEY_DOWN)
		{
			ref=REF_Now;
			k++;
			if(k>=item) k=0;
		}
		else if(Key.Value==KEY_UP)
		{
			ref=REF_Now;
			if(k>0) k--;
			else k=item-1;
		}
		else if(Key.Value==KEY_OK)
		{
			ret=k;
			break;
		}
		else if(Key.Value==KEY_ESC)
		{
			ret=DO_Cancel;
			break;
		}
	}

	if(ret!=DO_Cancel)
	{
		//数据被修改处理------
		if(ret!=old)
		{
			Sys.LcdRef=REF_Now;				
			Sys.DatChang=1;			//表示改变了选项
		}
	}
	Touch_KeyPop();
	LcdMemPop();
	Key.Value=KEY_No;
	return(ret);	
}

/*
//===================================================================
//功能: 显示一个比列条
//dat      : 当前的数值(从1开始)
//tol       : 总数值
//x         : 起始的x坐标
//y         : 起始的y坐标
//height  : 比列条的高度
//color_f : 滑动块的颜色
//color_b : 信息框的背景色
//说明: 比列条占用显示宽度为13
//===================================================================
void Dis_PercentSlider(uint32 dat,uint32 tol,uint32 x,uint32 y,uint32 height,uint32 color_f,uint32 color_b)
{
#define SLIDER_H 	15
#define SLIDER_W	12
uint32 percent;
uint32 m,n;
	if(dat>0) dat--;
	if(tol>0) 
	{
		tol--;
		percent=dat*100/tol;
	}
	else percent=0;
	if(percent>100) percent=100;
	Dis_VLine(x,y,height,C_Gray1);
	m=(height-SLIDER_H)*percent/100;
	Dis_BoxUp(x+1,y+m,SLIDER_W,SLIDER_H,color_f);
	//填充滑动块上部的背景色
	if(m>0)
	{
		Dis_RectFill(x+1,y,SLIDER_W,m,color_b);		
	}
	//填充滑动块下部的背景色
	n=m+SLIDER_H;
	if(n<height)
	{	
		Dis_RectFill(x+1,y+n,SLIDER_W,height-n,color_b);
	}
}
*/

//===================================================================
//功能: 显示一个比列条
//dat      : 当前的数值(从1开始)
//tol       : 总数值
//x         : 起始的x坐标
//y         : 起始的y坐标
//height  : 比列条的高度
//color_f : 滑动块的颜色
//color_b : 信息框的背景色
//说明: 比列条占用显示宽度为13
//===================================================================
void Dis_PercentSlider(uint32 dat,uint32 tol,uint32 x,uint32 y,uint32 height,uint32 width,uint32 color_f,uint32 color_b)
{
#define SLIDER_H 	30	//15
uint32 percent;
uint32 m,n;
	width=width-1;
	if(dat>0) dat--;
	if(tol>0) 
	{
		tol--;
		percent=dat*100/tol;
	}
	else percent=0;
	if(percent>100) percent=100;
	Dis_VLine(x,y,height,C_Gray1);
	m=(height-SLIDER_H)*percent/100;
	Dis_BoxUp(x+1,y+m,width,SLIDER_H,color_f);
	//填充滑动块上部的背景色
	if(m>0)
	{
		Dis_RectFill(x+1,y,width,m,color_b);		
	}
	//填充滑动块下部的背景色
	n=m+SLIDER_H;
	if(n<height)
	{	
		Dis_RectFill(x+1,y+n,width,height-n,color_b);
	}
}

//===================================================================
//功能:			显示进度信息(进度框可设定)
//入口参数:
//		title:			标题样字符串指针
//		dat:			当前处理的进度数值(从1开始)
//		tol:			总的处理数值
//		width:		窗体的宽度
//		color:		窗体的背景色
//说明:			窗体高度固定为90
//修改时间:		2010-11-23
//===================================================================
void Dis_SpeedInfo(const char *title,uint32 dat,uint32 tol,uint32 width,uint32 color)
{
enum {BAR_H=28};
uint32 i;
uint32 percent;
uint32 m;
static uint32 ago_percent=0;		//之前的百分比值
static uint32 ago_width=0;			//之前的宽度
static uint32 now_width=0;			//当前的宽度
S_Form form;
	form.Width=width;
	form.Height=90;				//进度窗体高度固定为90
	form.X=GET_ScrCenterX(form.Width);		//窗体的X坐标固定显示在中间
	form.Y=GET_ScrCenterY(form.Height);		//窗体的Y坐标固定在中间
	while(1)
	{
		if(dat>tol) break;
		percent=dat*100/tol;
		//第一次显示时刷新整个框体----------	
		if(SysFlag.SpeedDis==0)
		{
			SysFlag.SpeedDis=1;
			ago_percent=0;
			ago_width=0;			
			//将显示进度条前的画面复制
			LcdMemPush(form.X,form.Y,form.Width,form.Height);
			form.TitleText=PTEXT(title);
			form.TitleColor=FORM_Red;
			form.TitleHeight=TITLE_H16;
			form.TitleFont=FONT16;
			form.BackColor=color;
			form.FillBack=1;
			Dis_WindowsFormLeft(&form);
			//进度条的底色---------------
			Dis_BoxDown(form.X+10,form.Y+50,form.Width-20,BAR_H,C_Gray1);
			Dis_Strings("%",form.X+form.Width/2+10,form.Y+30,FONT16,C_Blue,color);
		}

		if(percent==ago_percent) break;	//若百分比不变则不刷新显示(节约时间)
		now_width=(form.Width-20-4)*percent/100;
		m=now_width-ago_width;
		for(i=0;i<24;i++)
		{
			Dis_HLine(form.X+12+ago_width,form.Y+52+i,m,FormTitleColor[FORM_Blue][i]);
		}
		Dis_NumHide(percent,form.X+form.Width/2-16,form.Y+30,3,FONT16,C_Blue,color);
		ago_percent=percent;
		ago_width=now_width;		
		break;
	}
	
	if(dat>=tol)
	{
		if(dat==tol) DelayMs(400);
		if(SysFlag.SpeedDis>0)
		{
			LcdMemPop();
		}
		SysFlag.SpeedDis=0;
	}
}

//===================================================================
//功能: 			自动计算图标窗体显示在中心的宽度和起始坐标
//入口参数:		
//		icon:		图标部件结构指针
//说明:			计算占用行数时用循环代替除法运算
//修改时间:		2010-11-3	
//===================================================================
void Get_IconFormPlace(S_Form *form,S_GuiXY *guixy,S_GuiIcon *icon,uint32 ext_h)
{
uint32 i;
uint32 row;
uint32 w,h;
uint32 m;
	w=guixy->RowIcons*guixy->ColSpace+50;
	//计算图标的占用的行数(最大允许5行)--------
	row=0;
	m=guixy->MaxIcons;
	for(i=0;i<5;i++)
	{
		if(m>=guixy->RowIcons)
		{
			row++;
			m-=guixy->RowIcons;
		}
		else
		{	
			if(m>0) row++;		//不足一行按一行算
			break;
		}
	}
	h=guixy->RowSpace*row+form->TitleHeight+40+ext_h;
	form->Width=w;
	form->Height=h;
	form->X=GET_ScrCenterX(form->Width);
	form->Y=GET_ScrCenterY(form->Height);
	guixy->StartX=form->X+25;
	guixy->StartY=form->Y+form->TitleHeight+20;
}

//===================================================================
//功能:			显示一个图标部件(包括提示文本选择框)
//修改时间:		2013-3-7
//===================================================================
void Dis_IconSelect(S_GuiIcon *icon)
{
uint32 x,y,width,height;
uint32 bmp_w,bmp_h;
uint32 bmp_x,bmp_y;
uint32 m;
	x=icon->X;
	y=icon->Y;
	width=icon->Width;
	height=icon->Height;
	bmp_w=icon->BmpW;
	bmp_h=icon->BmpH;
	if((x>=LCD_XSize)||(y>=LCD_YSize)) return;
	if((x+width)>LCD_XSize)
	{
		width=LCD_XSize-x;
	}
	if((y+height)>LCD_YSize)
	{
		height=LCD_YSize-y;
	}

	//计算图片的显示位置----------------
	bmp_x=x+((width-bmp_w)>>1);
	bmp_y=y+5;
	//显示图标--------------------
	Dis_IconUp(bmp_x,bmp_y,bmp_w,bmp_h,icon->Bmp);

	//显示提示文本-------------------
	m=y+bmp_h+(height-bmp_h)/2-8;
	Dis_StringsAligen(icon->Text,ALIGEN_MIDDLE,x,m,width,FONT16,icon->TextColor,icon->BackColor);

	//若光标在当前图标--------
	if(icon->Sel>0)
	{
		Dis_RectSome(x,y,width,height,2,icon->RectColor);
	}
	else
	{
		Dis_RectSome(x,y,width,height,2,icon->BackColor);
	}	
}

//===================================================================
//功能: 			从存储器中读取屏幕保护信息
//入口参数:		
//		guixy:
//		icon:			图标部件结构指针
//		text:			图标文本的二维字符串指针数组
//		bmp:
//		k:			当前的光标位置
//说明:			
//修改时间:		2013-3-7
//===================================================================
void Dis_IconRef(S_GuiXY *guixy,S_GuiIcon *icon,const char *text[][LANG_MAX],uint16 *bmp,uint32 k,uint32 tch_k)
{
uint32 i;
uint16 x,y;
	x=guixy->StartX;
	y=guixy->StartY;
	for(i=0;i<guixy->MaxIcons;i++)
	{
		if((i%guixy->RowIcons)==0)
		{
			x=guixy->StartX;
			if(i>0) y+=guixy->RowSpace;
		}
		else
		{
			x+=guixy->ColSpace;
		}
		
		if(k==i)
		{
			icon->Sel=1;
			icon->TextColor=C_Red;
		}
		else
		{
			icon->Sel=0;
			icon->TextColor=C_White;
		}
		icon->X=x+((guixy->ColSpace-icon->Width)>>1);
		icon->Y=y;
		icon->Bmp=(uint16 *)(bmp+icon->BmpW*icon->BmpH*i);
		icon->Text=text[i][Lang];
		Touch_KeyEn(icon->X+((icon->Width-icon->BmpW)>>1),y+5,icon->BmpW,icon->BmpH,KEY_Tch,tch_k+i);
		TchKey[0][tch_k+i].Flash=1;					//允许显示触摸状态变化
		TchKey[0][tch_k+i].Bmp=(uint16 *)icon->Bmp;

		Dis_IconSelect(icon);		
	}			
}

//===================================================================
//功能: 			图标菜单中的位置移动键处理
//入口参数:		
//		guixy:		菜单项参数结构指针	
//		k:			菜单项的光标位置项
//返回:			处理后的菜单项光标位置项
//说明: 			点击图标后直接进入菜单
//修改时间:		2013-3-7
//===================================================================
uint32 KeyIconMove(S_GuiXY *guixy,uint32 k)
{
uint32 m,n;
	//定位触摸键按下处理------------------
	if(TchVal.Down==TCH_KeyDown)
	{
		if(Key.Value==KEY_Tch)
		{
			TchVal.Down=0;
			k=TchVal.Item[0];
			Sys.LcdRef=REF_Now;
			Key.Value=KEY_OK;
		}
	}

	if(TchVal.Down==TCH_KeySlide) return(k);			//触摸屏的滑动无效

	//右移键按下处理-------------------
	if(Key.Value==KEY_RIGHT)
	{
		Sys.LcdRef=REF_Now;
		k++;
		if(k>=guixy->MaxIcons)
		{
			k=0;
		}
	}
	//左移键按下处理-------------------
	else if(Key.Value==KEY_LEFT)
	{
		Sys.LcdRef=REF_Now;
		if(k>0) k--;
		else k=guixy->MaxIcons-1;
	}
	//下移键按下处理-------------------
	else if(Key.Value==KEY_DOWN)
	{
		Sys.LcdRef=REF_Now;
		k+=guixy->RowIcons;
		if(k>=guixy->MaxIcons)
		{
			k=k%guixy->RowIcons;
		}
	}
	//下移键按下处理-------------------
	else if(Key.Value==KEY_UP)
	{
		Sys.LcdRef=REF_Now;
		if(k>=guixy->RowIcons)
		{
			k-=guixy->RowIcons;
		}
		else
		{
			m=guixy->MaxIcons/guixy->RowIcons;
			n=guixy->MaxIcons%guixy->RowIcons;
			if(n>0) m++;
			if(m>0) m--;
			m*=guixy->RowIcons;			//最后一行的首值
			n=k%guixy->RowIcons;
			m=m+n;
			while(1)
			{
				if(m>=guixy->MaxIcons)
				{
					if(m>=guixy->RowIcons)
					{
						m-=guixy->RowIcons;
					}
					else
					{
						break;
					}
				}
				else
				{
					k=m;
					break;
				}
			}
		}
	}
	return(k);
}

//===================================================================
//功能:			密码框输入处理(只允许数字)
//参数:			
//		mima:		密码值
//返回: 			1表示密码输入正确
//说明:			密码值最大为9999 
//修改时间:		2013-3-6
//===================================================================
uint32 Dis_PassWord(uint32 mima)
{
enum {ASCW=12,FONT=FONT24};
uint8 code[4];
uint8 dat[5]={0,0,0,0,0};
uint32 len;
uint32 i;
uint32 x,y;
uint32 code_ok=0;
uint32 win_x,win_y,win_w,win_h;
	len=0;
	code[0]=mima/1000;
	code[1]=(mima%1000)/100;
	code[2]=(mima%100)/10;
	code[3]=mima%10;
	win_w=240;
	win_h=60;
	if(Sys.TouchEn==0x55)
	{
		win_y=140;
	}
	else
	{
		win_y=GET_ScrCenterY(win_h);
	}	
	win_x=GET_ScrCenterX(win_w);
	LcdMemPush(win_x,win_y,win_w,win_h);
	Dis_RectFill(win_x,win_y,win_w,win_h,C_Black);
	Dis_Rect(win_x,win_y,win_w,win_h,C_Yellow);
	Dis_Rect(win_x+2,win_y+2,win_w-4,win_h-4,C_Yellow);
	Dis_Strings(HZ_SuRuMiMa[0][Lang],win_x+20,win_y+20,FONT,C_Red,C_Black);

	TchVal.RefX=win_x+win_w-60;
	TchVal.RefY=win_y+16;
	TchVal.RefWidth=70;
	TchVal.RefHeight=30;
	Dis_TouchKeyBoard(TKEY_BoardAsc);

	while(1)
	{
		Send_DspReply();		//发送中断中置位的命令回码	
	
		Read_Key();

		Dis_TouchButtonFlash();			//刷新模拟触摸键盘的变化状态		

		if(Sys.LcdRef>0)
		{
			Sys.LcdRef=0;
			x=win_x+20+ASCW*12;
			y=win_y+20;
			for(i=0;i<5;i++)
			{
				if(i<len) Dis_Strings("*",x+i*ASCW,y,FONT24,C_White,C_Black);
				else Dis_Strings(" ",x+i*ASCW,y,FONT,C_Black,C_Black);
			}
		}

		if(Key.Value<=KEY_9)
		{
			if(len<5)
			{
				len++;
				Sys.LcdRef=1;
				dat[len-1]=Key.Value;
			}
		}
		//删除一个数值---------
		else if(Key.Value==KEY_QUK)
		{
			if(len>0)
			{
				Sys.LcdRef=1;
				dat[len-1]=0;
				len--;
			}
		}

		if(Key.Value==KEY_OK)
		{
			if(len!=4) Key.Value=KEY_ESC;
			else
			{
				for(i=0;i<4;i++)
				{
					if(dat[i]!=code[i]) 
					{
						Key.Value=KEY_ESC;
						break;
					}
				}
				if(i>=4) 
				{
					code_ok=1;
					break;
				}
			}
		}
		
		if(Key.Value==KEY_ESC)
		{
			code_ok=0;
			break;
		}
	}

	TouchKeyBoardClose();
	LcdMemPop();
	Key.Value=KEY_No;
	return(code_ok);
}

//===================================================================
//功能:			超级密码输入处理(6位密码,可以为0~F)
//参数:
//		code:		密码字符数据指针
//		x:			密码框的起始X坐标
//		y:			密码框的起始Y坐标
//返回:			1表示密码输入正确
//说明:			密码值最大为A6237F
//修改时间:		2013-3-6
//===================================================================
uint32 Dis_SuperPassWord(uint8 *code,uint32 x,uint32 y)
{
enum {ASCW=12,FONT=FONT24};
uint8 dat[6]={0,0,0,0,0,0};
uint32 len;
uint32 i;
uint32 code_ok=0;
S_Form form;
	Touch_KeyPush();
	len=0;
	form.X=x;
	form.Y=y;
	form.Width=250;
	form.Height=60;
	LcdMemPush(form.X,form.Y,form.Width,form.Height);
	Dis_RectFill(form.X,form.Y,form.Width,form.Height,C_Black);
	Dis_Rect(form.X,form.Y,form.Width,form.Height,C_Yellow);
	Dis_Rect(form.X+2,form.Y+2,form.Width-4,form.Height-4,C_Yellow);
	Dis_Strings(HZ_SuRuMiMa[0][Lang],form.X+20,form.Y+20,FONT,C_Red,C_Black);	
	TchVal.RefX=form.X+form.Width-60;
	TchVal.RefY=form.Y+20;
	TchVal.RefWidth=70;
	TchVal.RefHeight=30;
	Dis_TouchKeyBoard(TKEY_BoardAsc);

	while(1)
	{
		Send_DspReply();		//发送中断中置位的命令回码	
	
		Read_Key();

		Dis_TouchButtonFlash();			//刷新模拟触摸键盘的变化状态		

		if(Sys.LcdRef>0)
		{
			Sys.LcdRef=0;
			x=form.X+20+ASCW*12;
			for(i=0;i<6;i++)
			{
				if(i<len) Dis_Strings("#",x+i*ASCW,form.Y+20,FONT,C_White,C_Black);
				else Dis_Strings(" ",x+i*ASCW,form.Y+20,FONT,C_Black,C_Black);
			}
		}

		if(Key.Value<=KEY_F)
		{
			if(len<6)
			{
				len++;
				Sys.LcdRef=1;
				dat[len-1]=Key.Value;
			}
		}
		//删除一个数值---------
		else if(Key.Value==KEY_QUK)
		{
			if(len>0)
			{
				Sys.LcdRef=1;
				dat[len-1]=0;
				len--;
			}
		}

		if(Key.Value==KEY_OK)
		{
			if(len!=6) Key.Value=KEY_ESC;
			else
			{
				for(i=0;i<6;i++)
				{
					if(dat[i]!=code[i]) 
					{
						Key.Value=KEY_ESC;
						break;
					}
				}
				if(i>=6) 
				{
					code_ok=1;
					break;
				}
			}
		}
		
		if(Key.Value==KEY_ESC)
		{
			code_ok=0;
			break;
		}
	}

	TouchKeyBoardClose();
	LcdMemPop();
	Touch_KeyPop();
	Key.Value=KEY_No;
	return(code_ok);
}

//===================================================================
//功能 : 显示文件名(和扩展名)
//fdt        : 32字节的文件目录项信息
//ext        : 0表示不显示扩展名,1表示显示扩展名
//x          : 起始的x坐标
//y          : 起始的y坐标
//color_f  : 文字的颜色
//color_b : 文字的背景色
//===================================================================
void Dis_FileName(uint8 *fdt,uint32 ext,uint16 x,uint16 y,uint16 color_f,uint16 color_b)
{
uint32 i;
uint32 hz_low;
uint32 hz;
uint32 dat;
uint32 dis_len=0;
	//文件名
	hz_low=0;
	for(i=0;i<8;i++)
	{
		dat=*(fdt+i);
		if(dat==0x20) break;
		else if(dat<0xa0)
		{
			Dis_Asc(dat,x,y,color_f,color_b);
			dis_len+=8;
		}
		else
		{
			if(hz_low==0) 
			{
				hz=dat;
				hz*=256;
				hz_low=1;
			}
			else
			{
				hz+=dat;
				hz_low=0;
				Dis_Chinese(hz,x-8,y,color_f,color_b);
				dis_len+=16;
			}
		}
		x+=8;
	}
	//不显示扩展名---
	if(ext==0) 
	{
		if(dis_len<64)
		{
			Dis_RectFill(x,y,64-dis_len,16,color_b);
		}
		return;				
	}
	//扩展名--------------------------------------
	Dis_Strings(".",x,y,FONT16,color_f,color_b);
	dis_len+=8;
	x+=8;
	for(i=0;i<3;i++)
	{
		dat=*(fdt+8+i);
		if(dat==0x20) break;
		Dis_Asc(dat,x,y,color_f,color_b);
		dis_len+=8;
		x+=8;
	}

	if(dis_len<96)
	{
		Dis_RectFill(x,y,96-dis_len,16,color_b);
	}
}

//===================================================================
//功能 : 显示文件的大小
//fdt        : 32字节的文件目录项信息
//x          : 起始的x坐标
//y          : 起始的y坐标
//color_f  : 文字的颜色
//color_b : 文字的背景色
//===================================================================
void Dis_FileSize(uint8 *fdt,uint16 x,uint16 y,uint16 color_f,uint16 color_b)
{
uint32 i;
uint32 bytes;
uint32 wei[7];
uint32 cusu[7]={1000000,100000,10000,1000,100,10,1};
uint32 start_wei=0;
uint32 dis_len=0;
	bytes=*(fdt+0x1c)+*(fdt+0x1d)*0x100+*(fdt+0x1e)*0x10000+*(fdt+0x1f)*0x10000000;
	if(bytes>9999999) bytes=9999999;
	for(i=0;i<7;i++)
	{
		wei[i]=bytes/cusu[i];
		bytes=bytes%cusu[i];
	}
	start_wei=0;
	for(i=0;i<7;i++)
	{
		if((wei[i]>0)||(start_wei>0)||(i==6))
		{
			start_wei=1;
			Dis_NumHide(wei[i],x,y,1,FONT16,color_f,color_b);
			dis_len+=8;
			x+=8;
		}
	}

	if(dis_len<56)
	{
		Dis_RectFill(x,y,56-dis_len,16,color_b);
	}
}

//===================================================================
//功能 : 显示文件的大小
//fdt        : 32字节的文件目录项信息
//x          : 起始的x坐标
//y          : 起始的y坐标
//color_f  : 文字的颜色
//color_b : 文字的背景色
//===================================================================
void Dis_FileTime(uint8 *fdt,uint16 x,uint16 y,uint16 color_f,uint16 color_b)
{
	Get_FileTime(fdt,&FatTime);
	Dis_NumAll(FatTime.Year,x,y,4,FONT16,color_f,color_b);
	Dis_Strings("-",x+32,y,FONT16,color_f,color_b);
	Dis_NumAll(FatTime.Month,x+40,y,2,FONT16,color_f,color_b);
	Dis_Strings("-",x+56,y,FONT16,color_f,color_b);
	Dis_NumAll(FatTime.Data,x+64,y,2,FONT16,color_f,color_b);
	Dis_RectFill(x+80,y,10,16,color_b);
	x+=90;
	Dis_NumAll(FatTime.Hour,x,y,2,FONT16,color_f,color_b);
	Dis_Strings(":",x+16,y,FONT16,color_f,color_b);
	Dis_NumAll(FatTime.Minute,x+24,y,2,FONT16,color_f,color_b);
	Dis_Strings(":",x+40,y,FONT16,color_f,color_b);
	Dis_NumAll(FatTime.Second,x+48,y,2,FONT16,color_f,color_b);
}

//===================================================================
//功能:			显示文件信息(文件名,扩展名,大小,修改时间)
//入口参数:	
//		fdt: 			文件目录项数据指针(32字节)	
//		ext: 			表示是否要显示扩展名,不为0显示
//		x,y:			显示位置的x,y坐标值
//		color_f:		显示字符的前景色
//		color_b:		显示字符的背景色
//说明:			支持文件夹
//修改时间:		2011-4-28
//===================================================================
void Dis_FileInfoFlord(uint8 *fdt,uint32 ext,uint16 x,uint16 y,uint16 color_f,uint16 color_b)
{
	//当前目录项不是子目录处理-----------------
	if(fdt[FAT_Properties]!=FAT_Dir)
	{
		Dis_FileName(fdt,ext,x,y,color_f,color_b);
		if(ext>0) 
		{
			Dis_RectFill(x+12*8,y,115-12*8,16,color_b);
			x+=115;
		}
		else 
		{
			Dis_RectFill(x+8*8,y,70-8*8,16,color_b);
			x+=70;
		}
		Dis_FileSize(fdt,x,y,color_f,color_b);
		Dis_RectFill(x+8*7,y,80-8*7,16,color_b);
		Dis_FileTime(fdt,x+80,y,color_f,color_b);		
	}
	//是文件夹处理----------------------
	else
	{
		//当前在子目录下处理-----
		if(fdt[0]=='.')
		{
			Dis_RectFill(x,y,115+80+8*20,16, C_Black);
			Dis_Strings(Hz_FatDir[1][Lang],x,y,FONT16,color_f,color_b);
		}
		else
		{
			Dis_RectFill(x,y,115+80+8*20,16, C_Black);
			Dis_FileName(fdt,0,x,y,color_f,color_b);
			x+=115;
			Dis_Strings(Hz_FatDir[0][Lang],x,y,FONT16,color_f,C_Black);
//			color_b=C_Black;
//			Dis_RectFill(x+8*7,y,80-8*7,16,color_b);
//			Dis_FileTime(fdt,x+80,y,color_f,color_b);					
		}
	}
}

//===================================================================
//功能:			将文件列表进行排序(先显示文件夹)
//入口参数:
//		tol:			文件列表中的总文件数
//		file_list:		文件列表的目录项信息(返回排序后的列表)
//说明:			
//修改时间:		2011-5-7
//===================================================================
void File_SortByFloder(uint32 tol,uint8 *file_list)
{
uint32 i,j;
uint32 m;
uint8 fdt[MAX_Files][32];
uint32 floders;					//列表中的文件夹个数
uint32 files;					//列表中的文件个数	
uint32 floder_count;
uint32 file_count;
	if(tol<=1) return;			//若不到2个文件则直接返回

	floders=0;
	files=0;
	//获取文件夹和文件的个数-------
	m=FAT_Properties;
	for(i=0;i<tol;i++)
	{
		if(file_list[m]==FAT_Dir)
		{
			floders++;
		}
		else
		{
			files++;
		}
		m+=32;
	}

	floder_count=0;
	file_count=floders;

	//将原始文件列表排序-------
	m=0;
	for(i=0;i<tol;i++)
	{
		if(file_list[m+FAT_Properties]==FAT_Dir)
		{
			for(j=0;j<32;j++)
			{
				fdt[floder_count][j]=file_list[m+j];
			}
			floder_count++;
		}
		else
		{
			for(j=0;j<32;j++)
			{
				fdt[file_count][j]=file_list[m+j];
			}
			file_count++;
		}
		m+=32;
	}
	
	//将排序后的列表返回-----
	m=0;
	for(i=0;i<tol;i++)
	{
		for(j=0;j<32;j++)
		{
			file_list[m+j]=fdt[i][j];
		}
		m+=32;
	}
}

//===================================================================
//功能:			从文件(指定扩展名)列表框中选择一个文件
//参数:
//		title:			标题栏显示的文本(居中显示)
//		file_fdt:		传递进来的扩展名(前3字节),返回当前选中的文件信息
//		device:		设备类型 0=SD卡,1=U盘,2=NandFlash
//返回:			0xffffffff表示取消操作
//修改时间:		2013-3-7
//===================================================================
uint32 Dis_FileSelectOne(const char *title,uint8 *file_fdt,uint32 device)
{
enum {MAX_Num=10,BOXH=220,SLIDW=25};
uint32 k=0;						//当前光标在本页的序号
uint32 k_start=0;					//光标在最顶上的序号
uint32 i;
uint32 m;
uint32 x,y;
uint8 file_list[32*MAX_Files];
uint32 file_tol;
uint32 color_f,color_b;
uint32 flag;
uint32 ret;
uint32 ref_list;					// 1表示重新读取文件信息
S_Form form;
S_GuiXY guixy;
	Touch_KeyPush();
	ref_list=1;
	file_tol=0;
	TchVal.Item[0]=k;

	form.Width=420;
	form.Height=320;
	form.X=GET_ScrCenterX(form.Width);
	form.Y=GET_ScrCenterY(form.Height);
	form.TitleText=title;
	form.TitleColor=FORM_Red;
	form.TitleHeight=TITLE_H24;
	form.TitleFont=FONT24;
	form.BackColor=C_Back;
	form.FillBack=1;

	guixy.StartX=form.X+20;
	guixy.StartY=form.Y+form.TitleHeight+20;

	Sys.LcdRef=REF_Item;
	Sys.DatChang=0;
	LcdMemPush(form.X,form.Y,form.Width,form.Height);
	Dis_WindowsForm(&form);
	x=form.X+10;y=form.Y+form.Height-40;	
	Dis_Strings(HZ_FileTol[0][Lang],x,y,FONT16,C_Black,C_Back);
	Touch_KeyEn(form.X,form.Y,form.Width,form.TitleHeight,KEY_ESC,10);

	x=form.X+150;
	y=form.Y+form.Height-50;
	Dis_TouchButton(Hz_SelDel[0][Lang],x,y,110,40,KEY_CE,13,C_Black,FONT24);

	x=form.X+280;
	Dis_TouchButton(Hz_SelDel[1][Lang],x,y,110,40,KEY_OK,14,C_Black,FONT24);
	TchVal.KeyNum[0]=MAX_Num+5;

	FatDir.SubDir[device]=0;			//表示在根目录下
	while(1)
	{
		Send_DspReply();		//发送中断中置位的命令回码	
		
		if(ref_list>0)
		{
			ref_list=0;
			Dis_BoxDown(guixy.StartX-10,guixy.StartY-10,form.Width-20,BOXH,C_Black);
			if(device==NAND) i=MAX_MemFiles;
			else i=MAX_Files;
			file_tol=ListFileNameFloder(file_fdt,i,file_list,device);	
			File_SortByFloder(file_tol,file_list);
			Dis_NumHide(file_tol,form.X+10+60,form.Y+form.Height-40,4,FONT16,C_Red,C_Back);
		}
	
		if(Sys.LcdRef>0)
		{
			x=form.X+form.Width-10-SLIDW-2;
			Dis_PercentSlider(k+k_start+1,file_tol,x,guixy.StartY-10+2,BOXH-4,SLIDW,C_Green,C_Gray1);
			Touch_KeyEn(x,guixy.StartY-10+2,SLIDW,60,KEY_PU,11);				//上翻键
			Touch_KeyEn(x,guixy.StartY-10+BOXH-60,SLIDW,60,KEY_PD,12);		//下翻键

			x=guixy.StartX;
			y=guixy.StartY;
			for(i=0;i<10;i++)
			{
				if(k==i) 
				{	
					color_f=C_Red;
					color_b=C_Yellow;
				}
				else 
				{
					color_f=C_White;
					color_b=C_Black;
				}

				m=k_start+i;
				if(m>=file_tol)
				{
					Dis_RectFill(x,y,form.Width-60,16,C_Black);
					TouchKeyDis(i);
				}
				else
				{
					Dis_FileInfoFlord(&file_list[(i+k_start)*32],1,x,y,color_f,color_b);
					Touch_KeyEn(x,y,320,16,KEY_Tch,i);
				}
				y+=20;
			}
			Sys.k=k;
			Sys.LcdRef=0;
		}

		Read_Key();

		Dis_TouchButtonFlash();
		
		//有效的触摸点按下处理(序号)---------
		if((TchVal.Down==TCH_KeyDown)&&(Key.Value==KEY_Tch))
		{
			TchVal.Down=0;
			if(k!=TchVal.Item[0])
			{
				k=TchVal.Item[0];
				Sys.LcdRef=REF_Now;
			}
			//目录项双击当确定键处理---------
			else
			{
				m=k_start+k;
				if((m+1)<=file_tol)
				{
					m*=32;
					if(file_list[m+FAT_Properties]==FAT_Dir)
					{
						Key.Value=KEY_OK;
					}
				}
			}
		}

		//下移一项-----------------
		if(Key.Value==KEY_DOWN)
		{
			if((k<(MAX_Num-1))&&((k_start+k)<(file_tol-1)))
			{	
				Sys.LcdRef=REF_Item;			
				k++;
			}
			else if((k_start+MAX_Num)<file_tol)
			{
				Sys.LcdRef=REF_Item;	
				k_start++;
			}
		}
		//上移一项-----------------
		else if(Key.Value==KEY_UP)
		{
			if(k>0) 
			{
				Sys.LcdRef=REF_Item;			
				k--;
			}
			else if(k_start>0)
			{
				Sys.LcdRef=REF_Item;
				k_start--;
			}
		}
		//下翻一页----------------
		else if(Key.Value==KEY_PD)
		{
			if(file_tol>(k_start+MAX_Num))
			{
				Sys.LcdRef=REF_Item;
				k_start+=MAX_Num;
				if(k>=(file_tol-k_start))
				{
					k=file_tol-k_start-1;		//光标到最后一项
				}
			}
		}
		//上翻一页----------------
		else if(Key.Value==KEY_PU)
		{
			if(k_start>=MAX_Num)
			{
				Sys.LcdRef=REF_Item;
				k_start-=MAX_Num;
			}
			else if(k_start>0)
			{
				Sys.LcdRef=REF_Item;
				k_start=0;
			}
		}	
		else if((Key.Value==KEY_CE)&&(file_tol>0))
		{
			//判断当前文件是否是文件夹----------
			m=(k+k_start)*32;
			if(file_list[m+FAT_Properties]==FAT_Dir)
			{
				Dis_HintForm(HZ_TiSi[0][Lang],HZ_FileOptHint[FILE_DelFloderErr][Lang],C_Black,C_Back);				
			}
			else
			{
				flag=Dis_SureDialog(HZ_TiSi[0][Lang],HZ_SureDelFile[0][Lang],C_Back);
				//按确定键后删除
				if(flag!=DO_Cancel)
				{
					if(FatDir.SubDir[device]!=0x55)
					{
						file_list[m+26]=0;
						file_list[m+27]=0;
						file_list[m+20]=0;
						file_list[m+21]=0;
					}
					flag=DeleteFileFloder(&file_list[m],device);
					if(flag==0)
					{
						Dis_HintForm(HZ_TiSi[0][Lang],HZ_FileOptHint[FILE_DelOk][Lang],C_Black,C_Back);
						//若当前标在最后一项上处理
						if((k+1)==file_tol)
						{
							if(k>0) k-=1;
						}
					}
					ref_list=1;
					Sys.LcdRef=REF_Now;
				}
			}
		}
		else if(Key.Value==KEY_OK)
		{
			m=(k+k_start)*32;
			//文件夹处理------------------------------
			if(file_list[m+FAT_Properties]==FAT_Dir)
			{	
				//返回到上级目录处理------------------
				if(file_list[m]=='.')
				{
					ref_list=1;
					FatDir.CurrentDir[26]=FatDir.ParentDir[26];
					FatDir.CurrentDir[27]=FatDir.ParentDir[27];
					FatDir.CurrentDir[20]=FatDir.ParentDir[20];
					FatDir.CurrentDir[21]=FatDir.ParentDir[21];					
					if((FatDir.CurrentDir[26]==0)&&(FatDir.CurrentDir[27]==0)&&(FatDir.CurrentDir[20]==0)&&(FatDir.CurrentDir[21]==0))
					{
						FatDir.SubDir[device]=0;
					}
					k=0;					//光标移到第1项
					k_start=0;
					Sys.LcdRef=REF_Item;
				}
				//进入文件夹-----------
				else
				{
					ref_list=1;
					FatDir.SubDir[device]=0x55;
					FatDir.CurrentDir[26]=file_list[m+26];
					FatDir.CurrentDir[27]=file_list[m+27];
					FatDir.CurrentDir[20]=file_list[m+20];
					FatDir.CurrentDir[21]=file_list[m+21];
					k=0;					//光标移到第1项
					k_start=0;
					Sys.LcdRef=REF_Item;
				}
			}
			else
			{
				if(file_tol==0) ret=DO_Cancel;
				else
				{
					ret=0;
					for(i=0;i<32;i++)
					{
						file_fdt[i]=file_list[m+i];
					}
					//传递当前子目录的簇号--------
					if(FatDir.SubDir[device]==0x55)
					{
						file_fdt[26]=file_list[26];
						file_fdt[27]=file_list[27];
						file_fdt[20]=file_list[20];
						file_fdt[21]=file_list[21];
					}
					else
					{
						file_fdt[26]=0;
						file_fdt[27]=0;
						file_fdt[20]=0;
						file_fdt[21]=0;
					}
				}
				break;
			}
		}
		if(Key.Value==KEY_ESC)
		{
			ret=DO_Cancel;
			break;
		}
	}
	Key.Value=KEY_No;
	Touch_KeyPop();
	LcdMemPop();
	return(ret);
}

//===================================================================
//功能:			从文件(指定扩展名)列表框中选择一个或多个文件
//入口参数:
//		title:			标题栏显示的文本指针(居中显示)
//		file_fdt:		传递进来的扩展名指针(前3字节)
//		sel_list:		选中的文件目录信息指针
//说明:			按弹出菜单键全部选择
//返回:			返回的个数,0表示没有选择
//修改时间:		2013-3-7
//===================================================================
uint32 Dis_FileSelectMany(const char *title,uint8 *file_fdt,uint8 *sel_list)
{
enum {MAX_Num=10,BOX_H=220,SLID_W=20};
uint32 k=0;						//当前光标在本页的序号
uint32 k_start=0;					//光标在最顶上的序号
uint32 i,j;
uint32 m;
uint32 x,y;
uint8 file_list[32*MAX_Files];			//符合要求的文件列表
uint8 file_sel[MAX_Files];			// 1表示文件被选中
uint32 file_tol;
uint32 sel_tol;						//选中的文件数
uint32 color_f,color_b;
uint32 flag;
uint32 ref_list;					// 1表示重新读取文件信息
S_Form form;
S_GuiXY guixy;
	Touch_KeyPush();
	ref_list=1;
	sel_tol=0;
	form.Width=420;
	form.Height=320;
	form.X=GET_ScrCenterX(form.Width);
	form.Y=GET_ScrCenterY(form.Height);
	form.TitleText=title;
	form.TitleColor=FORM_Red;
	form.TitleHeight=TITLE_H24;
	form.TitleFont=FONT24;
	form.BackColor=C_Back;
	form.FillBack=1;

	guixy.StartX=form.X+32;
	guixy.StartY=form.Y+form.TitleHeight+20;
	guixy.RowSpace=20;

	TchVal.Item[0]=k;
	Sys.LcdRef=REF_Item;
	Sys.DatChang=0;
	for(i=0;i<MAX_Files;i++) file_sel[i]=0;
	LcdMemPush(form.X,form.Y,form.Width,form.Height);
	Dis_WindowsForm(&form);
	Touch_KeyEn(form.X,form.Y,form.Width,form.TitleHeight,KEY_ESC,10);
	Dis_Strings(HZ_FileTol[0][Lang],form.X+10,form.Y+form.Height-40,FONT16,C_Black,C_Back);

	x=form.X+150;
	y=form.Y+form.Height-50;
	Dis_TouchButton(Hz_SelDel[0][Lang],x,y,120,40,KEY_CE,13,C_Black,FONT24);
	x+=130;
	Dis_TouchButton(Hz_SelDel[1][Lang],x,y,120,40,KEY_OK,14,C_Black,FONT24);
	
	TchVal.KeyNum[0]=MAX_Num+5;
	FatDir.SubDir[USB]=0;			//表示在根目录下
	while(1)
	{
		Send_DspReply();		//发送中断中置位的命令回码	
		
		if(ref_list>0)
		{
			ref_list=0;
			Dis_BoxDown(form.X+6,guixy.StartY-7,form.Width-12,BOX_H,C_Black);
			file_tol=ListFileNameFloder(file_fdt,MAX_Files,file_list,USB);	
			File_SortByFloder(file_tol,file_list);
			Dis_NumHide(file_tol,form.X+10+60,form.Y+form.Height-40,4,FONT16,C_Red,C_Back);
		}
	
		if(Sys.LcdRef>0)
		{
			x=form.X+form.Width-SLID_W-6-2;
			y=guixy.StartY-5;
			Dis_PercentSlider(k+k_start+1,file_tol,x,y,BOX_H-4,20,C_Green,C_Black);
			Touch_KeyEn(x,y+2,SLID_W,60,KEY_PU,11);			//上翻键
			Touch_KeyEn(x,y+BOX_H-60-4,20,60,KEY_PD,12);		//下翻键
			x=guixy.StartX;
			y=guixy.StartY;
			for(i=0;i<10;i++)
			{
				if(k==i) 
				{
					color_f=C_Red;
					color_b=C_Yellow;
				}
				else 
				{
					color_f=C_White;
					color_b=C_Black;
				}
				
				m=k_start+i;
				if(m>=file_tol) 
				{
					file_sel[m]=0;	
					Dis_RectFill(form.X+9,y-1,form.Width-40,18,C_Black);
					TouchKeyDis(i);
				}
				else
				{
					//文件夹前不显示选择框----
					if(file_list[m*32+FAT_Properties]==FAT_Dir)
					{
						file_sel[m]=0;			//目录项不能被选中
						Dis_Strings("  ",form.X+10,y,FONT16,C_Black,C_Black);
						Dis_Rect(form.X+9,y-1,18,18,C_Black);				
					}
					else
					{
						if(file_sel[m]==1) Dis_Strings("√",form.X+10,y,FONT16,C_Green,C_Black);
						else Dis_Strings("  ",form.X+10,y,FONT16,C_Black,C_Black);
						Dis_Rect(form.X+9,y-1,18,18,C_Gray1);				
					}
					Dis_FileInfoFlord(&file_list[m*32],1,x,y,color_f,color_b);
					Touch_KeyEn(x,y,330,16,KEY_Tch,i);
				}
				y+=20;
			}
			Sys.k=k;
			Sys.LcdRef=0;
		}

		Read_Key();

		Dis_TouchButtonFlash();

		//有效的触摸点按下处理(序号)---------
		if((TchVal.Down==TCH_KeyDown)&&(Key.Value==KEY_Tch))
		{
			TchVal.Down=0;
			if(k!=TchVal.Item[0])
			{
				k=TchVal.Item[0];
				Sys.LcdRef=REF_Now;
			}
			//目录项双击当确定键处理---------
			else
			{
				m=k_start+k;
				if((m+1)<=file_tol)
				{
					m*=32;
					if(file_list[m+FAT_Properties]==FAT_Dir)
					{
						Key.Value=KEY_OK;
					}
					//不是目录项以击当选择键处理----
					else
					{
						Key.Value=KEY_SEL;
					}
				}
			}						
		}

		//下移一项-----------------
		if(Key.Value==KEY_DOWN)
		{
			if((k<(MAX_Num-1))&&((k_start+k)<(file_tol-1)))
			{	
				Sys.LcdRef=REF_Item;			
				k++;
			}
			else if((k_start+MAX_Num)<file_tol)
			{
				Sys.LcdRef=REF_Item;	
				k_start++;
			}
		}
		//上移一项-----------------
		else if(Key.Value==KEY_UP)
		{
			if(k>0) 
			{
				Sys.LcdRef=REF_Item;			
				k--;
			}
			else if(k_start>0)
			{
				Sys.LcdRef=REF_Item;
				k_start--;
			}
		}
		//下翻一页----------------
		else if(Key.Value==KEY_PD)
		{
			if(file_tol>(k_start+MAX_Num))
			{
				Sys.LcdRef=REF_Item;
				k_start+=MAX_Num;
				if(k>=(file_tol-k_start))
				{
					k=file_tol-k_start-1;		//光标到最后一项
				}
			}
		}
		//上翻一页----------------
		else if(Key.Value==KEY_PU)
		{
			if(k_start>=MAX_Num)
			{
				Sys.LcdRef=REF_Item;
				k_start-=MAX_Num;
			}
			else if(k_start>0)
			{
				Sys.LcdRef=REF_Item;
				k_start=0;
			}
		}				
		//选择键-------------------
		else if(Key.Value==KEY_SEL)
		{
			Sys.LcdRef=REF_Now;
			i=k+k_start;
			if(file_sel[i]==1) file_sel[i]=0;
			else file_sel[i]=1;
		}
		//选择全部文件----------
		else if(Key.Value==KEY_POP)
		{
			Sys.LcdRef=REF_Item;
			for(i=0;i<file_tol;i++)
			{
				file_sel[i]=1;
			}
		}
		else if((Key.Value==KEY_CE)&&(file_tol>0))
		{
			//判断当前文件是否是文件夹----------
			m=(k+k_start)*32;
			if(file_list[m+FAT_Properties]==FAT_Dir)
			{
				Dis_HintForm(HZ_TiSi[0][Lang],HZ_FileOptHint[FILE_DelFloderErr][Lang],C_Black,C_Back);				
			}
			else
			{
				flag=Dis_SureDialog(HZ_TiSi[0][Lang],HZ_SureDelFile[0][Lang],C_Back);
				//按确定键后删除
				if(flag!=DO_Cancel)
				{
					if(FatDir.SubDir[USB]!=0x55)
					{
						file_list[m+26]=0;
						file_list[m+27]=0;
						file_list[m+20]=0;
						file_list[m+21]=0;
					}				
					flag=DeleteFileFloder(&file_list[m],USB);
					if(flag==0)
					{
						Dis_HintForm(HZ_TiSi[0][Lang],HZ_FileOptHint[FILE_DelOk][Lang],C_Black,C_Back);
						//若当前标在最后一项上处理
						if((k+1)==file_tol)
						{
							if(k>0) k-=1;
						}
					}
					ref_list=1;
					Sys.LcdRef=REF_Now;
				}
			}
		}
		else if(Key.Value==KEY_OK)
		{
			m=(k+k_start)*32;
			//文件夹处理------------------------------
			if(file_list[m+FAT_Properties]==FAT_Dir)
			{	
				for(i=0;i<MAX_Files;i++) file_sel[i]=0;
				//返回到上级目录处理------------------
				if(file_list[m]=='.')
				{
					ref_list=1;
					FatDir.CurrentDir[26]=FatDir.ParentDir[26];
					FatDir.CurrentDir[27]=FatDir.ParentDir[27];
					FatDir.CurrentDir[20]=FatDir.ParentDir[20];
					FatDir.CurrentDir[21]=FatDir.ParentDir[21];					
					if((FatDir.CurrentDir[26]==0)&&(FatDir.CurrentDir[27]==0)&&(FatDir.CurrentDir[20]==0)&&(FatDir.CurrentDir[21]==0))
					{
						FatDir.SubDir[USB]=0;
					}
					k=0;					//光标移到第1项
					k_start=0;
					Sys.LcdRef=REF_Item;
				}
				//进入文件夹-----------
				else
				{
					ref_list=1;
					FatDir.SubDir[USB]=0x55;
					FatDir.CurrentDir[26]=file_list[m+26];
					FatDir.CurrentDir[27]=file_list[m+27];
					FatDir.CurrentDir[20]=file_list[m+20];
					FatDir.CurrentDir[21]=file_list[m+21];
					k=0;					//光标移到第1项
					k_start=0;
					Sys.LcdRef=REF_Item;
				}
			}
			else
			{
				if(file_tol>0)
				{
					sel_tol=0;
					for(i=0;i<file_tol;i++)
					{
						m=sel_tol*32;
						if(file_sel[i]==1)
						{
							for(j=0;j<32;j++)
							{
								sel_list[m+j]=file_list[i*32+j];
							}
							//传递当前子目录的簇号--------
							if(FatDir.SubDir[USB]==0x55)
							{
								sel_list[m+26]=file_list[26];
								sel_list[m+27]=file_list[27];
								sel_list[m+20]=file_list[20];
								sel_list[m+21]=file_list[21];
							}
							else
							{
								sel_list[m+26]=0;
								sel_list[m+27]=0;
								sel_list[m+20]=0;
								sel_list[m+21]=0;
							}
							sel_tol++;
						}
					}
				}
				break;
			}
		}
		if(Key.Value==KEY_ESC)
		{
			sel_tol=0;
			break;
		}
	}
	Key.Value=KEY_No;
	Touch_KeyPop();
	LcdMemPop();
	return(sel_tol);
}


