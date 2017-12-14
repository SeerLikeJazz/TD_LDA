#ifndef __EXIT_H	
#define __EXIT_H	 
#include "sys.h"
#include "config.h"
#include "main.h"
#include "led.h" 


extern uint8_t trainig_flag;
extern uint8_t testing_flag;

extern uint8_t key_fall_flag;
extern uint8_t keyUpFlag;
extern uint16_t key_holdon_ms;
extern uint8_t short_key_flag;
extern uint8_t key_long_down;
extern uint8_t long_key_flag;
extern uint16_t keyupCnt;

void Key_Func(void);




#endif

/******************* (C) COPYRIGHT 2015-2020 Econ-Tech *****END OF FILE****/
