#ifndef MAIN_H_
#define MAIN_H_

extern void Lwip_Receive_thread(void *p);/* ��̫�����ݽ��մ������� */
extern QueueHandle_t Lwip_Receive_Queue;/* ��̫�����ݽ��ն��� */
extern QueueHandle_t Lwip_Send_Queue;/* ��̫�����ݽ��ն��� */





#endif
