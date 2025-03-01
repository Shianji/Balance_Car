#include "App_Task.h"
//启动任务的配置
TaskHandle_t start_task_handle;
#define START_TASK_STACK 128
#define START_TASK_PRIORITY 1
void App_Task_Start(void *pvParameters);

//姿态数据获取任务的配置
TaskHandle_t data_task_handle;
#define DATA_TASK_STACK 128
#define DATA_TASK_PRIORITY 1
void App_Task_GetData(void *pvParameters);

//PID控制任务的配置
TaskHandle_t pid_task_handle;
#define PID_TASK_STACK 128
#define PID_TASK_PRIORITY 1
void App_Task_PID(void *pvParameters);

//显示任务的配置
TaskHandle_t display_task_handle;
#define DISPLAY_TASK_STACK 128
#define DISPLAY_TASK_PRIORITY 1
void App_Task_Display(void *pvParameters);

void App_Task_Init(void){
    /*1.动态创建启动任务*/
    xTaskCreate((TaskFunction_t)App_Task_Start,
                (char *)"App_Task_Start", 
                (configSTACK_DEPTH_TYPE)START_TASK_STACK,
                (void *)NULL,
                (UBaseType_t)START_TASK_PRIORITY,
                (TaskHandle_t *)&start_task_handle );

    /*2.启动调度器*/
    vTaskStartScheduler();
}

//启动任务对应的任务函数，用于创建其他任务
void App_Task_Start(void *pvParameters){

    taskENTER_CRITICAL();

    xTaskCreate((TaskFunction_t)App_Task_GetData,
                (char *)"App_Task_GetData", 
                (configSTACK_DEPTH_TYPE)DATA_TASK_STACK,
                (void *)NULL,
                (UBaseType_t)DATA_TASK_PRIORITY,
                (TaskHandle_t *)&data_task_handle );
    
    xTaskCreate((TaskFunction_t)App_Task_PID,
                (char *)"App_Task_PID", 
                (configSTACK_DEPTH_TYPE)PID_TASK_STACK,
                (void *)NULL,
                (UBaseType_t)PID_TASK_PRIORITY,
                (TaskHandle_t *)&pid_task_handle );

    xTaskCreate((TaskFunction_t)App_Task_Display,
                (char *)"App_Task_Display", 
                (configSTACK_DEPTH_TYPE)DISPLAY_TASK_STACK,
                (void *)NULL,
                (UBaseType_t)DISPLAY_TASK_PRIORITY,
                (TaskHandle_t *)&display_task_handle );

    vTaskDelete(NULL);

    taskEXIT_CRITICAL();
}

//该函数用于获取姿态数据、角度、编码器的值
void App_Task_GetData(void *pvParameters){
    TickType_t pxPreviousWakeTime=xTaskGetTickCount();
    while(1){
        App_Car_GetAngle();
        //获取完数据之后通知PID控制任务使用数据
        xTaskNotifyGive(pid_task_handle);
        //延时FreeRTOS中的10个Tick，FreeRTOS中的一个Tick延时的是1ms，所以这里是10ms的延迟，而之前MPU的采样频率是100HZ，二者刚好相等
        vTaskDelayUntil(&pxPreviousWakeTime,10);
    }

}

//该函数用于PID控制
void App_Task_PID(void *pvParameters){
    while(1){
        //等待获取数据任务data_task_handle的通知，无限阻塞等待
        ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
        App_Car_PID();
    }
}

//该函数用于执行OLED显示
void App_Task_Display(void *pvParameters){
    TickType_t pxPreviousWakeTime=xTaskGetTickCount();
    while(1){
        App_Car_Display();
        vTaskDelayUntil(&pxPreviousWakeTime,50);
    }
}

