#ifndef __CONFIG_H
#define __CONFIG_H	 
#include "stm32f4xx_hal.h"

#define ACTION_NUM 10           //动作加休息共10组
#define ACTION_TIME_TRAINING 5  //每个动作训练5秒，休息5秒
#define DATA_CHANNEL 8
#define TIME_INTERVAL 300
#define TIME_INCREMENT 100
#define DATA_LENGTH  (ACTION_NUM*ACTION_TIME_TRAINING*1000) //LZX//<?>
#define START 1
#define REST 0
extern  int32_t dur;
extern uint8_t state;
extern uint16_t window_count;
extern uint16_t UNILABEL[6];
extern uint16_t action_serial[10];
#endif
