#ifndef __EMG_H
#define __EMG_H	
#include "sys.h"
#include "usart.h"
#include "arm_math.h"

#include "led.h" 
#include "arm_math.h"
#include "config.h"
#include "exti.h"


extern float32_t feat[33];
extern double data[600][8];
extern float32_t LDA_TEMP[32][6];
extern float32_t Bias_Mat[6];
extern void feature_extraction(void);//................................
extern uint8_t LDA_predict(float32_t feat[33]);//........................
extern void LDA_train(float32_t feat_matrix[410][32]);//..........................
extern uint16_t count;
extern float32_t feat_matrix[410][32];
extern void qiuni(void);//...............................
#endif

