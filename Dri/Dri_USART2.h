#ifndef __DRI_USART2_H
#define __DRI_USART2_H

#include "stm32f10x.h"
#include "stdio.h"
#include "Dri_USART1.h"

void Driver_USART2_Init(void);

void Driver_USART2_SendChar(uint8_t byte);

void Driver_USART2_SendString(uint8_t *str, uint16_t len);

uint8_t Driver_USART2_ReceiveChar(void);

void Driver_USART2_ReceiveString(uint8_t buff[], uint8_t *len);

#endif
