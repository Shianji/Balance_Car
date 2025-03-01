#ifndef __DRI_USART1_H
#define __DRI_USART1_H

#include "stm32f10x.h"
#include "stdio.h"
#include "Dri_USART2.h"

void Driver_USART1_Init(void);

void Driver_USART1_SendChar(uint8_t byte);

void Driver_USART1_SendString(uint8_t *str, uint16_t len);

uint8_t Driver_USART1_ReceiveChar(void);

void Driver_USART1_ReceiveString(uint8_t buff[], uint8_t *len);

#endif
