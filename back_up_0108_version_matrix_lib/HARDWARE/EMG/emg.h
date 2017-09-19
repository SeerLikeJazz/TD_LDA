#ifndef __EMG_H
#define __EMG_H	
#include "sys.h"
#include "usart3.h"
#include "arm_math.h"
extern float32_t feat[33];
extern double data[600][8];
extern float32_t LDA_TEMP[32][6];
extern float32_t Bias_Mat[6];
extern void feature_extraction(void);//................................
extern u8 LDA_predict(float32_t feat[33]);//........................
extern void LDA_train(float32_t feat_matrix[410][32]);//..........................
extern u16 count;
extern float32_t feat_matrix[410][32];
extern void qiuni(void);//...............................
#endif

