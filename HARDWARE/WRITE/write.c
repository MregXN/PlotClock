#include "pwm.h"
#include "write.h"


void leftHand_Rotate(int angle)
{
	angle = LEFTHAND_0 + (LEFTHAND_90 - LEFTHAND_0)*angle/90;
	TIM_SetCompare1(TIM8,angle);
	
}

void rightHand_Rotate(int angle)
{
	angle = RIGHTHAND_90 + (RIGHTHAND_180 - RIGHTHAND_90)*(angle-90)/90;
	TIM_SetCompare2(TIM8,angle);
	
}

void liftHand_Rotate(int angle)
{
	angle = LIFTHAND_90 +	(LIFTHAND_135 - LIFTHAND_90)*(angle-90)/45;
	TIM_SetCompare3(TIM8,angle);
	
}




