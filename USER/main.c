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
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		256  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define WRITE_TASK_PRIO		3
//�����ջ��С	
#define WRITE_STK_SIZE 		256  
//������
TaskHandle_t WriteTask_Handler;
//������
void write_task(void *pvParameters);

//�������ȼ�
#define DS1302_TASK_PRIO 2
//�����ջ��С	
#define DS1302_STK_SIZE  256 
//������
TaskHandle_t DS1302_Handler;
//������
void DS1302_task(void *pvParameters);



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
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
	
    //����WRITE����
    xTaskCreate((TaskFunction_t )write_task,             
                (const char*    )"WRITE_task",           
                (uint16_t       )WRITE_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )WRITE_TASK_PRIO,        
                (TaskHandle_t*  )&WriteTask_Handler);   
    //����TASK2����
    xTaskCreate((TaskFunction_t )DS1302_task,     
                (const char*    )"ds1302_task",   
                (uint16_t       )DS1302_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )DS1302_TASK_PRIO,
                (TaskHandle_t*  )&DS1302_Handler); 
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//WRITE������
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

//DS1302_task������
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




