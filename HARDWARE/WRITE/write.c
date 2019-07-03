#include "pwm.h"
#include "write.h"
#include "delay.h"
#include <math.h>
#include "FreeRTOS.h"
#include "task.h"

double lastX = ERASEX;
double lastY = ERASEY;

int speed = WRITESPEED;  //write=6 sweep=2

//抬臂初始值
int servoLift = 90;


void leftHand_Rotate(float angle)
{
	angle = LEFTHAND_0 + (LEFTHAND_90 - LEFTHAND_0)*angle/90;
	TIM_SetCompare1(TIM8,angle);
	
}

void rightHand_Rotate(float angle)
{
	angle = RIGHTHAND_90 + (RIGHTHAND_180 - RIGHTHAND_90)*(angle-90)/90;
	TIM_SetCompare2(TIM8,angle);
	
}

void liftHand_Rotate(float angle)
{
	angle = LIFTHAND_90 +	(LIFTHAND_135 - LIFTHAND_90)*(angle-90)/45;
	TIM_SetCompare3(TIM8,angle);
	
}

void origin_Attitude(void)
{
		leftHand_Rotate(0);
		rightHand_Rotate(90);
		liftHand_Rotate(100);
}

//计算a与c之间的角度，返回弧度值
double return_Angle(double a, double b, double c) 
{
  return acos((a * a + c * c - b * b) / (2 * a * c));
}


//移动到（Tx，Ty）
void set_XY(double Tx, double Ty) 
{
  // calculate triangle between pen, servoLeft and arm joint
	double dx, dy, cl, al1, al2,cr, ar1, ar2,aa,bb,Hx,Hy  ;
  
	// cartesian dx/dy
  dx = Tx - O1X;
  dy = Ty - O1Y;
	
  // polar lemgth (c) and angle (a1)
  cl = sqrt(dx * dx + dy * dy); // 
  al1 = atan2(dy, dx); //
  al2 = return_Angle(L1, L2, cl);
	
  // calculate joinr arm point for triangle of the right servo arm
  aa = return_Angle(L2, L1, cl);
	bb =return_Angle(L2, L4, L3);
	
  Hx = Tx + L3 * sin(aa-bb); 
  Hy = Ty - L3 * cos(aa-bb);
	
  // calculate triangle between pen joint, servoRight and arm joint
  dx = Hx - O2X;
  dy = Hy - O2Y;
	
  cr = sqrt(dx * dx + dy * dy);
  ar1 = M_PI-atan2(dy, dx);
  ar2 = return_Angle(L1, L4 , cr);
	
	leftHand_Rotate((M_PI - al2 - al1)*(180/M_PI));
  rightHand_Rotate(((ar2+ar1)*(180/M_PI)));
	
}

void drawTo(double pX, double pY) 
{
  double dx, dy, c;
  int i;

  // dx dy of new point
  dx = pX - lastX;
  dy = pY - lastY;
  //path lenght in mm, times [speed] equals [speed] steps per mm
  c = floor(speed * sqrt(dx * dx + dy * dy));

  if (c < 1) c = 1;

  for (i = 0; i <= c; i++) 
	{
    // draw line point by point
    set_XY(lastX + (i * dx / c), lastY + (i * dy / c));
		vTaskDelay(10); 
  }

  lastX = pX;
  lastY = pY;
}

	
void bogenUZS(float bx, float by, float radius, int start, int ende, float sqee) 
{

  float inkr = -0.05;
  float count = 0;
	
  do 
	{
    drawTo(sqee * radius * cos(start + count) + bx,
    radius * sin(start + count) + by);
    count += inkr;
  } 
  while ((start + count) > ende);
	
}
	
void bogenGZS(float bx, float by, float radius, int start, int ende, float sqee) 
{
	
  float inkr = 0.05;
  float count = 0;
	
  do 
	{
    drawTo(sqee * radius * cos(start + count) + bx,
    radius * sin(start + count) + by);
    count += inkr;
  } 
  while ((start + count) <= ende);
	
}

// Writing numeral with bx by being the bottom left originpoint. Scale 1 equals a 20 mm high font.
	
// The structure follows this principle: move to first startpoint of the numeral, lift down, draw numeral, lift up
	
void number(float bx, float by, int num, float scale) 
{
  switch (num) 
	{

		case 0:
			drawTo(bx + 12 * scale, by + 6 * scale);
			lift(0);
			bogenGZS(bx + 7 * scale, by + 10 * scale, 10 * scale, -0.8, 6.7, 0.5);
			lift(1);
		break;
	
		case 1:
			drawTo(bx + 10 * scale, by + 15 * scale);
			lift(0);
	//		drawTo(bx + 10 * scale, by + 15 * scale);
			drawTo(bx + 10 * scale, by + 5 * scale);
			lift(1);	
    break;
	
		case 2:
			drawTo(bx + 2 * scale, by + 12 * scale);
			lift(0);
			bogenUZS(bx + 8 * scale, by + 14 * scale, 6 * scale, 3, -0.8, 1);
			drawTo(bx + 1 * scale, by + 0 * scale);
			drawTo(bx + 12 * scale, by + 0 * scale);
			lift(1);
    break;
	
		case 3:
			drawTo(bx + 2 * scale, by + 17 * scale);
			lift(0);
			bogenUZS(bx + 5 * scale, by + 15 * scale, 5 * scale, 3, -2, 1);
			bogenUZS(bx + 5 * scale, by + 5 * scale, 5 * scale, 1.57, -3, 1);
			lift(1);
    break;
	
		case 4:
			drawTo(bx + 10 * scale, by + 0 * scale);
			lift(0);
			drawTo(bx + 10 * scale, by + 20 * scale);
			drawTo(bx + 2 * scale, by + 6 * scale);
			drawTo(bx + 12 * scale, by + 6 * scale);
			lift(1);
    break;
	
		case 5:
			drawTo(bx + 2 * scale, by + 5 * scale);
			lift(0);
			bogenGZS(bx + 5 * scale, by + 6 * scale, 6 * scale, -2.5, 2, 1);
			drawTo(bx + 5 * scale, by + 20 * scale);
			drawTo(bx + 12 * scale, by + 20 * scale);
			lift(1);
		break;
	
		case 6:
			drawTo(bx + 2 * scale, by + 10 * scale);
			lift(0);
			bogenUZS(bx + 7 * scale, by + 6 * scale, 6 * scale, 2, -4.4, 1);
			drawTo(bx + 11 * scale, by + 20 * scale);
			lift(1);
    break;
	
		case 7:
			drawTo(bx + 2 * scale, by + 20 * scale);
			lift(0);
			drawTo(bx + 12 * scale, by + 20 * scale);
			drawTo(bx + 2 * scale, by + 0);
			lift(1);
    break;
	
		case 8:
			drawTo(bx + 5 * scale, by + 10 * scale);
			lift(0);
			bogenUZS(bx + 5 * scale, by + 15 * scale, 5 * scale, 4.7, -1.6, 1);
			bogenGZS(bx + 5 * scale, by + 5 * scale, 5 * scale, -4.7, 2, 1);
			lift(1);
    break;
	
 
	
		case 9:
			drawTo(bx + 9 * scale, by + 11 * scale);
			lift(0);
			bogenUZS(bx + 7 * scale, by + 15 * scale, 5 * scale, 4, -0.5, 1);
			drawTo(bx + 5 * scale, by + 0);
			lift(1);
		break;
	
 
	
		case 111:  //erase
			lift(0);	
			speed = SWEEPSPEED;
		
			drawTo(ERASEX, ERASEY);
			drawTo(65, 43);
			
			drawTo(65, 43);
			drawTo(49, 43);
			drawTo(5, 43);
			drawTo(5, 43);
			drawTo(49, 43);
			drawTo(65, 43);
			drawTo(65, 40);
			
			drawTo(5, 40);
			drawTo(5, 35);
			drawTo(49, 35);
			drawTo(65, 35);
			drawTo(65, 30); 
		
			drawTo(5, 30);
			drawTo(5, 25);	
			drawTo(65, 25);	
			drawTo(65, 20);
				
			drawTo(5, 20);	
			drawTo(60, 44);	
			drawTo(ERASEX, ERASEY);
			
			speed = WRITESPEED;
			lift(2);
    break;
	
 
	
		case 11: // :
			drawTo(bx + 5 * scale, by + 15 * scale);
			lift(0);
			vTaskDelay(100); 
			bogenGZS(bx + 5 * scale, by + 15 * scale, 0.1 * scale, 1, -1, 1);
			lift(1);
			drawTo(bx + 5 * scale, by + 5 * scale);
			lift(0);
			vTaskDelay(100); 
			bogenGZS(bx + 5 * scale, by + 5 * scale, 0.1 * scale, 1, -1, 1);
			lift(1);
    break;
  }
}

	
void lift(char lift_mode)
{
  switch (lift_mode) 
	{
		case 0: //writting
			
			if (servoLift >= LIFT0) 
			{
				while (servoLift >= LIFT0) 
				{
		
					servoLift--;
					liftHand_Rotate(servoLift);                                
					vTaskDelay(LIFTSPEED);
				}
			} 
			else 
			{
				while (servoLift <= LIFT0) 
				{
					servoLift++;
					liftHand_Rotate(servoLift);
					vTaskDelay(LIFTSPEED);
				}

			}
			
		break;
			
		case 1: //between number
			
			if (servoLift >= LIFT1) 
			{
				while (servoLift >= LIFT1) 
				{
					servoLift--;
					liftHand_Rotate(servoLift);
					vTaskDelay(LIFTSPEED);
				}
			} 
			else 
			{
				while (servoLift <= LIFT1) 
				{
					servoLift++;
					liftHand_Rotate(servoLift);
					vTaskDelay(LIFTSPEED);
				}
			}
			
		break;
		
		case 2: //go to sweeper
			
			if (servoLift >= LIFT2) 
			{
				while (servoLift >= LIFT2) 
				{
					servoLift--;
					liftHand_Rotate(servoLift);
					vTaskDelay(LIFTSPEED);
				}
			} 
			else 
			{
				while (servoLift <= LIFT2) 
				{
					servoLift++;
					liftHand_Rotate(servoLift);                                
					vTaskDelay(LIFTSPEED);
				}
			}
			
			break;
	
  }
	
}
