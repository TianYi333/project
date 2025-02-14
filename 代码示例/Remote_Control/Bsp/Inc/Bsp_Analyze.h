#ifndef __BSP_ANALYZE_H__
#define __BSP_ANALYZE_H__

#include "main.h"

#define STATE_IDEL        0x00    //空闲状态
#define STATE_SELECTION   0x01    //选中架号状态
#define STATE_SINGLE      0x02    //单动状态
#define STATE_GROUP       0x03    //成组状态

#define MESSAGE_CONNECT       0xC0    //连接指定架号
#define GROUP_CONNECT         0xC2    //连接指定架号
#define SIGNAL_SEND_MESSAGE   0xC1    //单动标志
#define GROUP_SEND_MESSAGE    0xC5    //成组标志

/*最小/最大架号*/
#define NUM_MIN 1
#define NUM_MAX 102

/*动作最小/最大编号*/
#define ACTION_MIN 1
#define ACTION_MAX 18       //暂定为18个动作
/*成组最小/最大编号*/
#define G_ACTION_MIN 1
#define G_ACTION_MAX 6      //暂定为6个成组动作

extern QueueHandle_t Queue_Step_Stop_Handle;    //停止单动消息

//用于显示的数字
extern int16_t print_num;
//当前界面动作
extern uint8_t choose_action;
//成组方向
extern uint8_t m_dir;
//成组时光标所在位置
extern uint8_t group_choose;

/*记录当前状态
  0：空闲状态；
  1：选中架号状态；
*/
extern uint8_t current_state;

extern struct group_msg group_para;

struct group_msg
{
  uint8_t dir; //地址增向    1：左；2：右
  int8_t qua[6]; //动作支架数量
};


/* 任务函数 */
void Analyze_Function(void);		//解析函数

/*成组按键被按下*/
void Key_Group(void);


#endif
