#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "malloc.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ds1302.h"
#include "pwm.h"
#include "write.h"



//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		256  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define WRITE_TASK_PRIO		3
//任务堆栈大小	
#define WRITE_STK_SIZE 		256  
//任务句柄
TaskHandle_t WriteTask_Handler;
//任务函数
void write_task(void *pvParameters);

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

	mem_init(); 	//初始化内部内存池
	TIM8_PWM_Init(719,1999);//2000分频，获得周期约20ms的PWM脉冲
	
	DS1302_GPIOInit();
	Delay_xms(50);
	ds1302_init(); 
	Delay_xms(50);
	ds1302_write_time(); 

	
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
	
    //创建WRITE任务
    xTaskCreate((TaskFunction_t )write_task,             
                (const char*    )"WRITE_task",           
                (uint16_t       )WRITE_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )WRITE_TASK_PRIO,        
                (TaskHandle_t*  )&WriteTask_Handler);   
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

//WRITE任务函数
void write_task(void *pvParameters)
{

   	while(1)
	{
 		vTaskDelay(500);  				 
		leftHand_Rotate(0);
		rightHand_Rotate(180);
		liftHand_Rotate(45);
	} 
}

//DS1302_task任务函数
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
		
		vTaskDelay(1000); 
	}
}




