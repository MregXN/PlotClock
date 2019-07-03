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



//�������ȼ�
#define start_Task_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		256  
//������
TaskHandle_t StartTask_Handler;
//������
void start_Task(void *pvParameters);

//�������ȼ�
#define write_Task_PRIO		3
//�����ջ��С	
#define WRITE_STK_SIZE 		256  
//������
TaskHandle_t WriteTask_Handler;
//������
void write_Task(void *pvParameters);

//�������ȼ�
#define DS1302_Task_PRIO 2
//�����ջ��С	
#define DS1302_STK_SIZE  256 
//������
TaskHandle_t DS1302_Handler;
//������
void DS1302_Task(void *pvParameters);



int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 
	delay_init();	    				//��ʱ������ʼ��	 
	uart_init(115200);					//��ʼ������

	mem_init(); 	//��ʼ���ڲ��ڴ��
	TIM8_PWM_Init(719,1999);//2000��Ƶ���������Լ20ms��PWM����
	
	DS1302_GPIOInit();
	Delay_xms(50);
	ds1302_init(); 
	Delay_xms(50);
	ds1302_write_time(); 
		
	//������ʼ����
	xTaskCreate((TaskFunction_t )start_Task,            //������
							(const char*    )"start_Task",          //��������
							(uint16_t       )START_STK_SIZE,        //�����ջ��С
							(void*          )NULL,                  //���ݸ��������Ĳ���
							(UBaseType_t    )start_Task_PRIO,       //�������ȼ�
							(TaskHandle_t*  )&StartTask_Handler);   //������              
	vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_Task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
	
    //����WRITE����
    xTaskCreate((TaskFunction_t )write_Task,             
                (const char*    )"write_Task",           
                (uint16_t       )WRITE_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )write_Task_PRIO,        
                (TaskHandle_t*  )&WriteTask_Handler);   
    //����TASK2����
    xTaskCreate((TaskFunction_t )DS1302_Task,     
                (const char*    )"DS1302_Task",   
                (uint16_t       )DS1302_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )DS1302_Task_PRIO,
                (TaskHandle_t*  )&DS1302_Handler); 
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//WRITE������
void write_Task(void *pvParameters)
{		u32 NotifyValue;
	while(1)
	{
			NotifyValue=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);	//��ȡ����֪ͨ
			if(NotifyValue==1)									//����֮ǰ������ֵ֪ͨΪ1��˵������֪ͨ��Ч
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
				vTaskDelay(10);      //��ʱ10ms��Ҳ����10��ʱ�ӽ���	
			}
	}
}

//DS1302_Task������
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
		
		if( (readtime[12]==0) && (readtime[13]==0) && (WriteTask_Handler!=NULL) )//ÿ���ӷ���һ������֪ͨ
		{
			vTaskNotifyGiveFromISR(WriteTask_Handler,&xHigherPriorityTaskWoken);//��������֪ͨ
		}
	}
}




