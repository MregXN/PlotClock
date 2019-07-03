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
#define start_Task_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		256  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_Task(void *pvParameters);

//任务优先级
#define write_Task_PRIO		3
//任务堆栈大小	
#define WRITE_STK_SIZE 		256  
//任务句柄
TaskHandle_t WriteTask_Handler;
//任务函数
void write_Task(void *pvParameters);

//任务优先级
#define DS1302_Task_PRIO 2
//任务堆栈大小	
#define DS1302_STK_SIZE  256 
//任务句柄
TaskHandle_t DS1302_Handler;
//任务函数
void DS1302_Task(void *pvParameters);



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
	xTaskCreate((TaskFunction_t )start_Task,            //任务函数
							(const char*    )"start_Task",          //任务名称
							(uint16_t       )START_STK_SIZE,        //任务堆栈大小
							(void*          )NULL,                  //传递给任务函数的参数
							(UBaseType_t    )start_Task_PRIO,       //任务优先级
							(TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
	vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_Task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
	
    //创建WRITE任务
    xTaskCreate((TaskFunction_t )write_Task,             
                (const char*    )"write_Task",           
                (uint16_t       )WRITE_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )write_Task_PRIO,        
                (TaskHandle_t*  )&WriteTask_Handler);   
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )DS1302_Task,     
                (const char*    )"DS1302_Task",   
                (uint16_t       )DS1302_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )DS1302_Task_PRIO,
                (TaskHandle_t*  )&DS1302_Handler); 
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//WRITE任务函数
void write_Task(void *pvParameters)
{		u32 NotifyValue;
	while(1)
	{
			NotifyValue=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);	//获取任务通知
			if(NotifyValue==1)									//清零之前的任务通知值为1，说明任务通知有效
			{
				number(3, 3, 111, 1);
				
				number(50, 30, readtime[11], 0.6);
				number(38, 30, readtime[10], 0.6);
				
				number(30, 30, 11, 0.6);
				
				number(17, 25, readtime[9], 0.8);
				number(5, 25, readtime[8], 0.8);
				
				lift(2);
				drawTo(ERASEX, ERASEY);
				lift(1);
			} 
			else 
			{
				vTaskDelay(10);      //延时10ms，也就是10个时钟节拍	
			}
	}
}

//DS1302_Task任务函数
void DS1302_Task(void *pvParameters)
{
  BaseType_t xHigherPriorityTaskWoken;
	
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
		
		if( (readtime[12]==0) && (readtime[13]==0) && (WriteTask_Handler!=NULL) )//每分钟发送一次任务通知
		{
			vTaskNotifyGiveFromISR(WriteTask_Handler,&xHigherPriorityTaskWoken);//发送任务通知
		}
	}
}




