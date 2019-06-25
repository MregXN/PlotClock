#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "lcd.h"
#include "key.h"
#include "malloc.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ds1302.h"
#include "pwm.h"



//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		256  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define TASK1_TASK_PRIO		3
//任务堆栈大小	
#define TASK1_STK_SIZE 		256  
//任务句柄
TaskHandle_t Task1Task_Handler;
//任务函数
void task1_task(void *pvParameters);

//任务优先级
#define DS1302_TASK_PRIO 2
//任务堆栈大小	
#define DS1302_STK_SIZE  256 
//任务句柄
TaskHandle_t DS1302_Handler;
//任务函数
void DS1302_task(void *pvParameters);



int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	delay_init();	    				//延时函数初始化	 
	uart_init(115200);					//初始化串口
	LED_Init();		  					//初始化LED
	KEY_Init();							//初始化按键
	LCD_Init();							//初始化LCD
	mem_init(); 	//初始化内部内存池
	TIM1_PWM_Init(899,0);//不分频。PWM频率=72000/(899+1)=80Khz 
	
	DS1302_GPIOInit();
	Delay_xms(50);
	ds1302_init(); 
	Delay_xms(50);
	ds1302_write_time(); 

	POINT_COLOR=RED;
  LCD_ShowString(10,10,200,16,16,"Plot Clock ");	

	
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
	
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )task1_task,             
                (const char*    )"task1_task",           
                (uint16_t       )TASK1_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TASK1_TASK_PRIO,        
                (TaskHandle_t*  )&Task1Task_Handler);   
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )DS1302_task,     
                (const char*    )"ds1302_task",   
                (uint16_t       )DS1302_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )DS1302_TASK_PRIO,
                (TaskHandle_t*  )&DS1302_Handler); 
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//task1任务函数
void task1_task(void *pvParameters)
{
	u16 led0pwmval=0;    
	u8 dir=1;	
		while(1)
	{
 		vTaskDelay(10);	 
		if(dir)led0pwmval++;
		else led0pwmval--;	 
 		if(led0pwmval>300)dir=0;
		if(led0pwmval==0)dir=1;	   					 
		TIM_SetCompare1(TIM1,led0pwmval);	   
	} 
}

//DS1302_task函数
void DS1302_task(void *pvParameters)
{

	while(1)
	{
		ds1302_read_time();  
		
		printf("%d%d%d%d-%d%d-%d%d  %d%d.%d%d.%d%d \r\n",\
				readtime[0],readtime[1],readtime[2],readtime[3],\
				readtime[4],readtime[5],\
				readtime[6],readtime[7],\
				readtime[8],readtime[9],\
				readtime[10],readtime[11],\
				readtime[12],readtime[13]);
		
		vTaskDelay(6000); 
	}
}




