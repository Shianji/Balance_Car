#include "Int_Encoder.h"

void Int_Encoder_Init(void){
    //初始化定时器为编码器模式
    Dri_TIM2_Init();
    Dri_TIM3_Init();
}

//该函数用于读取编码器模式的计数值，处理成带符号的值
//timx是要读取的定时器的编号
//返回值为读到的带符号的计数值
int16_t Int_Encoder_ReadCounter(uint8_t timx){
    int encoder_value = 0;
    switch(timx){
        case 2:
            //读取定时器2的计数值，并转换成带符号类型
            encoder_value = (int16_t)TIM2->CNT;
            TIM2->CNT=0;
            break;
        case 3:
            //读取定时器2的计数值，并转换成带符号类型
            encoder_value = (int16_t)TIM3->CNT;
            TIM3->CNT=0;
            break;
        default:
            break;
    }
    /*由于电机安装是相差180度，后续读取时，要手动统一方向符号*/
    return encoder_value;
}
