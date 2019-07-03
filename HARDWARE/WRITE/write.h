#ifndef _WRITE_H
#define _WRITE_H
#include <stdint.h>

#define LEFTHAND_0     655
#define LEFTHAND_90    692
//#define LEFTHAND_180   720

//#define RIGHTHAND_0    630
#define RIGHTHAND_90   658.5
#define RIGHTHAND_180  695

//#define LIFTHAND_0     630
#define LIFTHAND_90    670
#define LIFTHAND_135   688

//左右舵机的轴心位置	
#define O1X 23
#define O1Y -25
#define O2X 49	
#define O2Y -25

//笔擦位置
#define ERASEX 73
#define ERASEY 46

//手臂长度
#define L1 35	  //舵机轴心到第一枚螺丝
#define L2 57.2 //第一枚螺丝到笔尖
#define L3 14.2 //笔尖到第二枚螺丝
#define L4 45 //第二枚螺丝到第一枚螺丝

//圆周率
#define M_PI 3.1415926

//抬臂模式
#define LIFT0 65 // on drawing surface
#define LIFT1 80  // between numbers
#define LIFT2 115  // going towards sweeper

//抬臂速度
#define LIFTSPEED 60  //higher is slower
//移动速度
#define WRITESPEED 8//higher is slower
//擦除速度
#define SWEEPSPEED 6  //higher is slower

void leftHand_Rotate(float angle);
void rightHand_Rotate(float angle);
void liftHand_Rotate(float angle);
void origin_Attitude(void);
void drawTo(double pX, double pY) ;
void lift(char lift_mode);
void number(float bx, float by, int num, float scale) ;
#endif
