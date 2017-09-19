#ifndef __CONFIG_H
#define __CONFIG_H	 
#include "sys.h" 
#define ACTION_NUM 10
#define ACTION_TIME_TRAINING 5
#define DATA_CHANNEL 8
#define TIME_INTERVAL 300
#define TIME_INCREMENT 100
#define DATA_LENGTH  (ACTION_NUM*ACTION_TIME_TRAINING*1000) 
#define START 1
#define REST 0
extern s32 dur;
extern u8 state;
extern u16 window_count;
extern u16 UNILABEL[6];
extern u16 action_serial[10];
#endif
