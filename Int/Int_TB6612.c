#include "Int_TB6612.h"

//电机驱动芯片TB6612的初始化函数
void Int_TB6612_Init(void){
    //初始化定时器4
    Dri_TIM4_Init();
    
    //初始化GPIO：PB12-PB15，推挽输出mode=11，cnf=00
		GPIOB->CRH |= (GPIO_CRH_MODE12|GPIO_CRH_MODE13|GPIO_CRH_MODE14|GPIO_CRH_MODE15);
    GPIOB->CRH &= ~(GPIO_CRH_CNF12|GPIO_CRH_CNF13|GPIO_CRH_CNF14|GPIO_CRH_CNF15);
}

//控制电机A转动方向的函数，direct为期望电机A转动的方向
void Int_TB6612_MotorA(uint8_t direct){
	if(direct==GO){
    //正转
    AIN1_L;
    AIN2_H;
	}else if(direct==BACK){
    //反转
    AIN1_H;
    AIN2_L;
  }else{
    //刹车
    AIN1_H;
    AIN2_H;
  }
}

//控制电机B转动方向的函数，direct为期望电机B转动的方向
void Int_TB6612_MotorB(uint8_t direct){
	if(direct==GO){
    //正转
    BIN1_L;
    BIN2_H;
	}else if(direct==BACK){
    //反转
    BIN1_H;
    BIN2_L;
  }else{
    //刹车
    BIN1_H;
    BIN2_H;
  }
}


void Int_TB6612_SetPWM(int pwma,int pwmb){
  //根据带符号的pwma值，处理A电机转动方向并设置pwma占空比
  if(pwma>0){
    //正转
    Int_TB6612_MotorA(GO);
  }else if(pwma<0){
    //反转
    Int_TB6612_MotorA(BACK);
    pwma=-pwma;
  }else{
    //刹车
    Int_TB6612_MotorA(STOP);
  }
  //设置pwma对应占空比
  TIM4->CCR4=pwma;

  //根据带符号的pwmb值，处理B电机转动方向并设置pwmb占空比
  if(pwmb>0){
    //正转
    Int_TB6612_MotorB(GO);
  }else if(pwmb<0){
    //反转
    Int_TB6612_MotorB(BACK);
    pwmb=-pwmb;
  }else{
    //刹车
    Int_TB6612_MotorB(STOP);
  }
  //设置pwmb对应占空比
  TIM4->CCR3=pwmb;
}
