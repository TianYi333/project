#include "Bsp_UserTask.h"

/* ������ */

TaskHandle_t Key_Board_Handle = NULL;       //������
TaskHandle_t Display_Handle = NULL;         //��Ļ��ʾ
TaskHandle_t Analyze_Handle = NULL;         //����
TaskHandle_t Action_Handle = NULL;          //����
TaskHandle_t Zigbee_Receive_Handle = NULL;  //zigbee����
TaskHandle_t Uart_Transmit_Handle = NULL;   //���ڷ���
TaskHandle_t Batery_Handle = NULL;          //��ص������

void CreatTask(void)
{
  xTaskCreate((TaskFunction_t )AppTaskCreate,     /* ������ں��� */
                (const char*    )"AppTaskCreate", /* �������� */
                (uint16_t       )512,             /* ����ջ��С */
                (void*          )NULL,            /* ������ں������� */
                (UBaseType_t    )1,               /* ��������ȼ� */
                (TaskHandle_t*  )NULL);           /* ������ƿ�ָ�� */
}
uint16_t ifsu;
void AppTaskCreate(void)
{
  taskENTER_CRITICAL();           //�����ٽ���
  
  Queue_KeyBoard_To_Analyze_Handle = xQueueCreate(4, 1);
  Queue_Clean_Red_Handle = xQueueCreate(8, 8);
  Queue_Step_Stop_Handle = xQueueCreate(1, 1);
  Queue_Analyze_To_Display_Handle = xQueueCreate(8, 8);
  Queue_Zigbee_Receive_Handle = xQueueCreate(8, 8);
  
  xTaskCreate((TaskFunction_t )KeyBoard_Function,   /* ������ں��� */
              (const char*    )"KeyBoard",          /* �������� */
              (uint16_t       )64,                  /* ����ջ��С */
              (void*          )NULL,                /* ������ں������� */
              (UBaseType_t    )6,                   /* ��������ȼ� */
              (TaskHandle_t*  )&Key_Board_Handle);  /* ������ƿ�ָ�� */
                
  xTaskCreate((TaskFunction_t )Display_Function,    /* ������ں��� */
              (const char*    )"Display",           /* �������� */
              (uint16_t       )256,                 /* ����ջ��С */
              (void*          )NULL,                /* ������ں������� */
              (UBaseType_t    )6,                   /* ��������ȼ� */
              (TaskHandle_t*  )&Display_Handle);    /* ������ƿ�ָ�� */
                
  xTaskCreate((TaskFunction_t )Analyze_Function,    /* ������ں��� */
              (const char*    )"Analyze",           /* �������� */
              (uint16_t       )64,                  /* ����ջ��С */
              (void*          )NULL,                /* ������ں������� */
              (UBaseType_t    )6,                   /* ��������ȼ� */
              (TaskHandle_t*  )&Analyze_Handle);    /* ������ƿ�ָ�� */
              
  xTaskCreate((TaskFunction_t )Action_Control_Function,   /* ������ں��� */
              (const char*    )"Action",                  /* �������� */
              (uint16_t       )64,                        /* ����ջ��С */
              (void*          )NULL,                      /* ������ں������� */
              (UBaseType_t    )6,                         /* ��������ȼ� */
              (TaskHandle_t*  )&Action_Handle);           /* ������ƿ�ָ�� */
              
  xTaskCreate((TaskFunction_t )Zigbee_Receive_Function,   /* ������ں��� */
              (const char*    )"Zigbee",                  /* �������� */
              (uint16_t       )64,                        /* ����ջ��С */
              (void*          )NULL,                      /* ������ں������� */
              (UBaseType_t    )6,                         /* ��������ȼ� */
              (TaskHandle_t*  )&Zigbee_Receive_Handle);   /* ������ƿ�ָ�� */
              
  xTaskCreate((TaskFunction_t )Uart_Transmit_Function,    /* ������ں��� */
              (const char*    )"Uart",                    /* �������� */
              (uint16_t       )64,                        /* ����ջ��С */
              (void*          )NULL,                      /* ������ں������� */
              (UBaseType_t    )6,                         /* ��������ȼ� */
              (TaskHandle_t*  )&Uart_Transmit_Handle);    /* ������ƿ�ָ�� */
 ifsu = xTaskCreate((TaskFunction_t )Batery_Function,           /* ������ں��� */
              (const char*    )"Batery",                  /* �������� */
              (uint16_t       )16,                        /* ����ջ��С */
              (void*          )NULL,                      /* ������ں������� */
              (UBaseType_t    )6,                         /* ��������ȼ� */
              (TaskHandle_t*  )&Batery_Handle);           /* ������ƿ�ָ�� */
  
  vTaskDelete(NULL);              //ɾ���Լ�
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

