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

//���Ҷ��������λ��	
#define O1X 23
#define O1Y -25
#define O2X 49	
#define O2Y -25

//�ʲ�λ��
#define ERASEX 73
#define ERASEY 46

//�ֱ۳���
#define L1 35	  //������ĵ���һö��˿
#define L2 57.2 //��һö��˿���ʼ�
#define L3 14.2 //�ʼ⵽�ڶ�ö��˿
#define L4 45 //�ڶ�ö��˿����һö��˿

//Բ����
#define M_PI 3.1415926

//̧��ģʽ
#define LIFT0 65 // on drawing surface
#define LIFT1 80  // between numbers
#define LIFT2 115  // going towards sweeper

//̧���ٶ�
#define LIFTSPEED 60  //higher is slower
//�ƶ��ٶ�
#define WRITESPEED 8//higher is slower
//�����ٶ�
#define SWEEPSPEED 6  //higher is slower

void leftHand_Rotate(float angle);
void rightHand_Rotate(float angle);
void liftHand_Rotate(float angle);
void origin_Attitude(void);
void drawTo(double pX, double pY) ;
void lift(char lift_mode);
void number(float bx, float by, int num, float scale) ;
#endif
