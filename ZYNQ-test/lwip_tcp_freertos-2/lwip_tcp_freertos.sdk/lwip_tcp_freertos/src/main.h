#ifndef MAIN_H_
#define MAIN_H_

extern void Lwip_Receive_thread(void *p);/* 以太网数据接收处理任务 */
extern QueueHandle_t Lwip_Receive_Queue;/* 以太网数据接收队列 */
extern QueueHandle_t Lwip_Send_Queue;/* 以太网数据接收队列 */





#endif
