#include "config.h"

int32_t dur=-1;
uint8_t state = REST;
uint16_t window_count=0;//LZX//<feat_matrix[410][32]>
uint16_t action_serial[10]={0,5,1,5,2,5,3,5,4,5};
uint16_t UNILABEL[6]={0,1,2,3,4,5};
