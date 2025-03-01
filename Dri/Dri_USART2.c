#include "Dri_USART2.h"

/**
 * @description: 初始化串口1
 */
void Driver_USART2_Init(void)
{
    /* 1. 开启时钟 */
    /* 1.1 串口2外设的时钟 */
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    /* 1.2 GPIO时钟 */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    /* 2. 配置GPIO引脚的工作模式  PA2=Tx(复用推挽 CNF=10 MODE=11)  PA3=Rx(浮空输入 CNF=01 MODE=00)*/
    GPIOA->CRL |= GPIO_CRL_CNF2_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF2_0;
    GPIOA->CRL |= GPIO_CRL_MODE2;
    
    GPIOA->CRL &= ~GPIO_CRL_CNF3_1;
    GPIOA->CRL |= GPIO_CRL_CNF3_0;
    GPIOA->CRL &= ~GPIO_CRL_MODE3;

    /* 3. 串口的参数配置 */
    /* 3.1 配置波特率 9600 */
    USART2->BRR = 0xEA6;
    /* 3.2 配置一个字的长度 8位 */
    USART2->CR1 &= ~USART_CR1_M;
    /* 3.3 配置不需要校验位 */
    USART2->CR1 &= ~USART_CR1_PCE;
    /* 3.4 配置停止位的长度 */
    USART2->CR2 &= ~USART_CR2_STOP;
    /* 3.5 使能接收和发送 */
    USART2->CR1 |= USART_CR1_TE;
    USART2->CR1 |= USART_CR1_RE;

    /* 3.6 使能串口的各种中断 */
    USART2->CR1 |= USART_CR1_RXNEIE; /* 接收非空中断 */
    /*使用蓝牙时不需要开空闲中断*/
    // USART2->CR1 |= USART_CR1_IDLEIE; /* 空闲中断 */

    /* 4. 配置NVIC */
    /* 4.1 配置优先级组 */
    NVIC_SetPriorityGrouping(3);
    /* 4.2 设置优先级 */
    NVIC_SetPriority(USART2_IRQn, 2);
    /* 4.3 使能串口1的中断 */
    NVIC_EnableIRQ(USART2_IRQn);

    /* 4. 使能串口 */
    USART2->CR1 |= USART_CR1_UE;
}

/**
 * @description: 发送一个字节
 * @param {uint8_t} byte 要发送的字节
 */
void Driver_USART2_SendChar(uint8_t byte)
{
    /* 1. 等待发送寄存器为空 */
    while ((USART2->SR & USART_SR_TXE) == 0)
        ;

    /* 2. 数据写出到数据寄存器 */
    USART2->DR = byte;
}

/**
 * @description: 发送一个字符串
 * @param {uint8_t} *str 要发送的字符串
 * @param {uint16_t} len 字符串中字节的长度
 * @return {*}
 */
void Driver_USART2_SendString(uint8_t *str, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        Driver_USART2_SendChar(str[i]);
    }
}

/**
 * @description: 接收一个字节的数据
 * @return {*} 接收到的字节
 */
uint8_t Driver_USART2_ReceiveChar(void)
{
    /* 等待数据寄存器非空 */
    while ((USART2->SR & USART_SR_RXNE) == 0)
        ;
    return USART2->DR;
}

/**
 * @description: 接收变长数据.接收到的数据存入到buff中
 * @param {uint8_t} buff 存放接收到的数据
 * @param {uint8_t} *len 存放收到的数据的字节的长度
 */
void Driver_USART2_ReceiveString(uint8_t buff[], uint8_t *len)
{
    uint8_t i = 0;
    while (1)
    {
        // 等待接收非空
        while ((USART2->SR & USART_SR_RXNE) == 0)
        {
            // 在等待期间, 判断是否收到空闲帧
            if (USART2->SR & USART_SR_IDLE)
            {
                
                *len = i;
                return;
            }
        }
        buff[i] = USART2->DR;
        i++;
    }
}

// /* 缓冲接收到的数据 */
// uint8_t buff2[100] = {0};
// /* 存储接收到的字节的长度 */
// uint8_t len2 = 0;
// uint8_t isToSend2 = 0;
// void USART2_IRQHandler(void)
// {
//     /* 数据接收寄存器非空 */
//     if (USART2->SR & USART_SR_RXNE)
//     {
//         // 对USART_DR的读操作可以将接收非空的中断位清零。 所以不用单独清除了.
//         //USART2->SR &= ~USART_SR_RXNE;
//         buff2[len2] = USART2->DR;
//         len2++;
//     }
//     else if (USART2->SR & USART_SR_IDLE)
//     {
//         /* 清除空闲中断标志位: 先读sr,再读dr.就可以实现清除了 */
//         USART2->SR;
//         USART2->DR;
//         /* 将接收到的数据转发到串口1 */
//         Driver_USART1_SendString(buff2, len2);
//         isToSend2 = 1;
//         /* 把接收字节的长度清0 */
//         len2 = 0;
//     }
// }
