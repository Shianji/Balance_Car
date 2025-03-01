#include "Com_PID.h"


//直立环，角度的PID控制函数，其中kp为P系数，kd为D系数，kalman_angle为卡尔曼滤波测量所得角度值，balance_angle为期望角度值，gy为y轴角速度原始值
int Com_PID_Balance(float kp,float kd,float kalman_angle,float balance_angle,short gy){
    float bias_angle=0.0f;
    /* 1.计算角度偏差*/
    bias_angle=kalman_angle-balance_angle;
    /*2.进行PID计算*/
    return kp*bias_angle+kd*gy;
}

//速度环，速度的PID控制函数，其中kp为P系数，ki为I系数，encoder_a和encoder_b分别为两个编码器输出的值，remove_move是遥控小车前后移动的控制量
int Com_PID_Velocity(float kp,float ki,int encoder_a,int encoder_b,int remove_move){
    int bias_velocity=0;
    static int least_velocity=0;//速度积分制（累加值）
    static int last_velocity=0;//记录上一次速度偏差值
    /* 1.计算速度偏差，速度期望值直接设置为0*/
    bias_velocity=(encoder_a+encoder_b)-0;
    /*2.对速度偏差做一个滤波（一阶低通），因为我们使用叠加的方式，以此来减弱速度环的影响，让其只起到辅助作用*/
    bias_velocity=0.8*last_velocity+0.2*bias_velocity;
    last_velocity=bias_velocity;
    /*3.计算积分（没有乘以时间，回头调整系数KI的大小就可以*/
    least_velocity+=bias_velocity;
    /*遥控小车前后移动时从位移上留出来*/
    least_velocity-=remove_move;

    /*4.积分限幅（一般积分操作，都要考虑限幅的问题）,least_velocity是对速度偏差的累加，速度取值在-32768~32767之间，考虑积分值在1/3的范围内*/
    if(least_velocity>10000){
        least_velocity=10000;
    }else if (least_velocity<-10000)
    {
        least_velocity=-10000;
    }
    /*5.计算PID*/
    return kp*bias_velocity+ki*least_velocity;
}

//转向环P控制（辅助作用），其中kp为P系数，gz为z轴角速度
int Com_PID_Trun(float kp,short gz){
    /*kp*z轴角速度*/
    return kp*(gz-0);
}

