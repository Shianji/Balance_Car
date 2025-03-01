#include "Dri_TIM.h"

//定时器4初始化：TIM4_CH3、TIM4_CH4输出pwm
void Dri_TIM4_Init(void){
    /*时钟、gpio、定时器基本设置、pwm模式*/
    /* 1.  开启时钟*/
    /* 1.1 定时器4的时钟 */
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    /* 1.2 GPIO的时钟 PB */
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    /* 2. 设置GPIO的复用推挽输出 PB8、PB9，这两个GPIO对应GPIOB->CRH CNF=10 MODE=11*/
    GPIOB->CRH |= GPIO_CRH_MODE8;
    GPIOB->CRH |= GPIO_CRH_CNF8_1;
    GPIOB->CRH &= ~GPIO_CRH_CNF8_0;

    GPIOB->CRH |= GPIO_CRH_MODE9;
    GPIOB->CRH |= GPIO_CRH_CNF9_1;
    GPIOB->CRH &= ~GPIO_CRH_CNF9_0;


    /* 3. 定时器基本通用配置 */
    /* pwm输出频率 = 72M时钟频率/(分频*重载值)=10k，频率设置过低不稳定，设置过高mos管压力太大 */
    /* 3.1 预分频器的配置 */
    TIM4->PSC = 1 - 1;
    /* 3.2 自动重装载寄存器的配置 */
    TIM4->ARR = 7200 - 1;
    /* 3.3 计数器的计数方向 0=向上 1=向下*/
    TIM4->CR1 &= ~TIM_CR1_DIR;


    /* 4. pwm模式配置：TIM4_CH3 */
    /* 4.1 配置通道3的捕获比较寄存器，这个寄存器用来控制占空比 */
    TIM4->CCR3 = 0;
    /* 4.2 把通道3配置为输出  CCMR2_CC3S=00 */
    TIM4->CCMR2 &= ~TIM_CCMR2_CC3S;
    /* 4.3 配置通道的输出比较模式 CCMR2_OC3M=110，设为pwm模式1：在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为有效电平，否则为
			无效电平；在向下计数时，一旦TIMx_CNT>TIMx_CCR1时通道1为无效电平(OC1REF=0)，否则为有效电平(OC1REF=1)。*/
    TIM4->CCMR2 |= TIM_CCMR2_OC3M_2;
    TIM4->CCMR2 |= TIM_CCMR2_OC3M_1;
    TIM4->CCMR2 &= ~TIM_CCMR2_OC3M_0;
    /* 4.4 输出使能通道3  CCER_CC3E=1 */
    TIM4->CCER |= TIM_CCER_CC3E;
    /* 4.5 设置通道的极性 0=高电平有效  1=低电平有效 */
    TIM4->CCER &= ~TIM_CCER_CC3P;

     /* 5. pwm模式配置：TIM4_CH4 */
    /* 5.1 配置通道4的捕获比较寄存器，这个寄存器用来控制占空比 */
    TIM4->CCR4 = 0;
    /* 5.2 把通道4配置为输出  CCMR2_CC4S=00 */
    TIM4->CCMR2 &= ~TIM_CCMR2_CC4S;
    /* 5.3 配置通道的输出比较模式 CCMR2_OC4M=110，设为pwm模式1*/
    TIM4->CCMR2 |= TIM_CCMR2_OC4M_2;
    TIM4->CCMR2 |= TIM_CCMR2_OC4M_1;
    TIM4->CCMR2 &= ~TIM_CCMR2_OC4M_0;
    /* 5.4 输出使能通道4  CCER_CC4E=1 */
    TIM4->CCER |= TIM_CCER_CC4E;
    /* 5.5 设置通道的极性 0=高电平有效  1=低电平有效 */
    TIM4->CCER &= ~TIM_CCER_CC4P;

    /* 6.使能启动计数器 */
    TIM4->CR1 |= TIM_CR1_CEN;
}


//初始化为编码器模式，电机A
void Dri_TIM2_Init(void){
    //时钟、GPIO、定时器通用设置、编码器模式
    /* 1.  开启时钟*/
    /* 1.1 定时器2的时钟 */
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    /* 1.2 GPIO的时钟 PA、PB、AFIO时钟(AFIO用于重映射) */
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN|RCC_APB2ENR_IOPAEN|RCC_APB2ENR_AFIOEN;

    /* 2.GPIO重映射 */
    /* 2.1关闭JTAG功能，注意保留SWD开启！！SWG_cfg=010 */
    AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG_2;
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_1;
    AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG_0;

    /* 2.2将PA15重映射为TIM2-CH1，PB3重映射为TIM2-CH2，TIM2_REMAP=11*/
    AFIO->MAPR |= AFIO_MAPR_TIM2_REMAP_FULLREMAP;

    /* 2.3设置GPIO的浮空输入 CNF=01 MODE=00*/
    GPIOA->CRH &= ~GPIO_CRH_MODE15;
    GPIOA->CRH &= ~GPIO_CRH_CNF15_1;
    GPIOA->CRH |= GPIO_CRH_CNF15_0;

    GPIOB->CRL &= ~GPIO_CRL_MODE3;
    GPIOB->CRL &= ~GPIO_CRL_CNF3_1;
    GPIOB->CRL |= GPIO_CRL_CNF3_0;

    /* 3. 定时器基本通用配置 */
    /* 3.1 预分频器的配置 */
    TIM2->PSC = 1 - 1;
    /* 3.2 自动重装载寄存器的配置 */
    TIM2->ARR = 65536 - 1;

    /* 4.配置编码器模式*/
    /* 4.1配置输入通道映射：IC1-TI1，IC2-TI2，CC1S=01,CC2S=01*/
    TIM2->CCMR1 &= ~TIM_CCMR1_CC1S_1;
    TIM2->CCMR1 |= TIM_CCMR1_CC1S_0;

    TIM2->CCMR1 &= ~TIM_CCMR1_CC2S_1;
    TIM2->CCMR1 |= TIM_CCMR1_CC2S_0;

    /* 4.3配置不反相CC1P=0,CC2P=0*/
    TIM2->CCER &= ~TIM_CCER_CC1P;
    TIM2->CCER &= ~TIM_CCER_CC2P;

    /* 4.3 两路信号都计数，编码器模式3，SMS=011*/
    TIM2->SMCR &= ~TIM_SMCR_SMS_2;
    TIM2->SMCR |= TIM_SMCR_SMS_1;
    TIM2->SMCR |= TIM_SMCR_SMS_0;

    /* 5启动定时器*/
    TIM2->CR1 |= TIM_CR1_CEN;
}

//初始化为编码器模式，电机B
void Dri_TIM3_Init(void){
    //时钟、GPIO、定时器通用设置、编码器模式
    /* 1.  开启时钟*/
    /* 1.1 定时器3的时钟 */
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    /* 1.2 GPIO的时钟PB、AFIO时钟(AFIO用于重映射) */
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN|RCC_APB2ENR_AFIOEN;

    /* 2.GPIO重映射 */
    /* 2.1关闭JTAG功能，注意保留SWD开启！！SWG_cfg=010 */
    AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG_2;
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_1;
    AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG_0;

    /* 2.2将PB4重映射为TIM3-CH1，PB5重映射为TIM3-CH2，TIM3_REMAP=10*/
    AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_PARTIALREMAP;

    /* 2.3设置GPIO的浮空输入 CNF=01 MODE=00*/
    GPIOB->CRL &= ~GPIO_CRL_MODE4;
    GPIOB->CRL &= ~GPIO_CRL_CNF4_1;
    GPIOB->CRL |= GPIO_CRL_CNF4_0;

    GPIOB->CRL &= ~GPIO_CRL_MODE5;
    GPIOB->CRL &= ~GPIO_CRL_CNF5_1;
    GPIOB->CRL |= GPIO_CRL_CNF5_0;

    /* 3. 定时器基本通用配置 */
    /* 3.1 预分频器的配置 */
    TIM3->PSC = 1 - 1;
    /* 3.2 自动重装载寄存器的配置 */
    TIM3->ARR = 65536 - 1;

    /* 4.配置编码器模式*/
    /* 4.1配置输入通道映射：IC1-TI1，IC2-TI2，CC1S=01,CC2S=01*/
    TIM3->CCMR1 &= ~TIM_CCMR1_CC1S_1;
    TIM3->CCMR1 |= TIM_CCMR1_CC1S_0;

    TIM3->CCMR1 &= ~TIM_CCMR1_CC2S_1;
    TIM3->CCMR1 |= TIM_CCMR1_CC2S_0;

    /* 4.3配置不反相CC1P=0,CC2P=0*/
    TIM3->CCER &= ~TIM_CCER_CC1P;
    TIM3->CCER &= ~TIM_CCER_CC2P;

    /* 4.3 两路信号都计数，编码器模式3，SMS=011*/
    TIM3->SMCR &= ~TIM_SMCR_SMS_2;
    TIM3->SMCR |= TIM_SMCR_SMS_1;
    TIM3->SMCR |= TIM_SMCR_SMS_0;

    /* 5启动定时器*/
    TIM3->CR1 |= TIM_CR1_CEN;
}
