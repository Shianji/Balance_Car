#include "main.h"
#include "freertos.h"
#include "task.h"
#include "Int_TB6612.h"
#include "Int_Encoder.h"
#include "Dri_USART1.h"
#include "Dri_USART2.h"
#include "Int_MPU6050.h"
#include "App_Car.h"
#include "Dri_ADC.h"
#include "oled.h"
#include "App_Task.h"


void for_delay_ms(uint32_t ms)
{
    uint32_t Delay = ms * 72000 / 9; /* 72M时钟频率 */
    do
    {
        __NOP(); /* 空指令（NOP）来占用 CPU 时间 */
    } while (Delay--);
}


int main(void)
{
    Driver_USART1_Init();
    Driver_USART2_Init();

    Int_TB6612_Init();
    Int_Encoder_Init();
    Int_MPU6050_Init();

    Driver_ADC1_Init();

    OLED_Init();
    OLED_Clear();
    /*  第一个参数:x坐标，水平方向
        第二个参数:y坐标，垂直方向
        第三个参数:要显示的字符申
        第四个参数:字体高度(需要字库支持)
        显示模式，0反显(白底黑字)，1正显(黑底白字)第五个参数:*/
    // OLED_ShowString(0,10,"hello world!",16,1);
    OLED_ShowString(0,0,"BAT:      V",16,1);
    OLED_ShowString(0,16,"EA:",16,1);
    OLED_ShowString(0,32,"EB:",16,1);
    OLED_ShowString(0,48,"Angle:",16,1);
    OLED_Refresh();

    /* 进入FreeRTOS开始调度 */
    App_Task_Init();

    // Int_TB6612_SetPWM(3600,-3600);//测试得出A是左轮，B是右轮；正转是向前转，反转是向后转

    while(1){
        /*==================测试编码器读取的值==================*/
        // printf("tim2 cnt=%d\r\n",Int_Encoder_ReadCounter(2));
        // printf("tim3 cnt=%d\r\n",Int_Encoder_ReadCounter(3));
        // for_delay_ms(1000);

        /*==================测试MPU6050==================*/
        // short gx=0,gy=0,gz=0;
        // short ax=0,ay=0,az=0;
        // Int_MPU6050_Get_Accel(&ax,&ay,&az);
        // Int_MPU6050_Get_Gyro(&gx,&gy,&gz);
        // printf("====================================\n");
        // printf("gx=%d\n",gx);
        // printf("gy=%d\n",gy);
        // printf("gz=%d\n",gz);
        // printf("ax=%d\n",ax);
        // printf("ay=%d\n",ay);
        // printf("az=%d\n",az);
        // for_delay_ms(100);

        /*==================测试计算角度==================*/
        // App_Car_GetAngle();
        // for_delay_ms(10);

        /*==================测试ADC驱动显示电源电压==================*/
        // printf("battery voltage=%.1f\n",Driver_ADC1_ReadV());
        // for_delay_ms(1000);

        /*==================测试OLED显示==================*/
        // App_Car_GetAngle();
        // App_Car_Display();
        // for_delay_ms(100);
    }
}

extern void xPortSysTickHandler(void);
void SysTick_Handler(void)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        xPortSysTickHandler();
    }
}
