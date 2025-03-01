#include "App_Car.h"
short gx,gy,gz;
short ax,ay,az;

float accel_angle;//通过加速度计算出来的倾角
float gyro_y;//存储y轴角速度的测量数据经转换后的到的实际角度
extern float angle;//卡尔曼滤波以后得到的角度

int16_t ea,eb;

char bat_str[5];//2个整数位+1个小数点+1个小数位+1个字符串\0结尾
char ea_str[7];//1个符号位+5个整数位+1个字符串结尾\0
char eb_str[7];//1个符号位+5个整数位+1个字符串结尾\0
char angle_str[7];//1个符号位+3个整数位+1个小数点+1个小数位+1个字符串结尾\0

//直立环PID参数
float balance_kp=-720.0; //-900出现低频震荡，乘以0.6为-540，最后精调至-720
float balance_kd=0.72;   //+1.5出现低频振荡，乘以0.6为+0.9，最后精调至0.72
float balance_angle=-0.5;//-0.5

//速度环PID参数
float velocity_kp=170.0;//+
float velocity_ki=0.85;//+

//转向环PID参数
float turn_kp=0.5;

//遥控小车运动的标志位
uint8_t flag_up=0,flag_down=0,flag_right=0,flag_left=0;
//遥控前后移动的控制量
int remote_move=0;
//遥控左右移动的控制量
int remote_turn=0;

//该函数用于计算小车倾角
void App_Car_GetAngle(void){
    /*1.读取MPU6050的数据*/
    Int_MPU6050_Get_Accel(&ax,&ay,&az);
    Int_MPU6050_Get_Gyro(&gx,&gy,&gz);

    /*2.通过加速度计算倾角,atan2得到的是弧度，还要进行换算才能得到角度：角度=弧度*180/PI*/
    accel_angle=atan2(ax,az)*180/PI;

    /*3.角速度，量程是+-2000°/S，65536/4000=16.4*/
    gyro_y=-gy/16.4;//注意要统一一下角度方向，经过测试将这里取反可使他们一致

    /*4.通过卡尔曼滤波计算倾角*/
    Com_Filter_Kalman(accel_angle,gyro_y);

    // printf("accel_angle=%.1f\n",accel_angle);
    // printf("gyro_y=%.1f\n",gyro_y);
    // printf("angle=%.1f\n",angle);

    /*将读取编码值的操作也放到获取角度的函数中，这样两类数据的获取就可以同频*/
    ea=Int_Encoder_ReadCounter(2);
    eb=-Int_Encoder_ReadCounter(3);
}


//显示任务：填充电池电压值、两个编码器的值、计算的角度值
void App_Car_Display(void){
    double bat_val=0.0;
    /*1.填充电压值*/
    bat_val=Driver_ADC1_ReadV();
    sprintf(bat_str,"%5.1f",bat_val);
    OLED_ShowString(32,0,bat_str,16,1);

    /*2.填充两个编码器的值*/
    sprintf(ea_str,"%6d",ea);
    sprintf(eb_str,"%6d",eb);
    OLED_ShowString(24,16,ea_str,16,1);//前面有EA:三个字符，x=3*8=24开始；第二行y=16
    OLED_ShowString(24,32,eb_str,16,1);//前面有EB:三个字符，x=3*8=24开始；第三行y=32

    /*3.填充角度值*/
    sprintf(angle_str,"%6.1f",angle);
    OLED_ShowString(48,48,angle_str,16,1);//前面有ANGLE:六个字符，x=6*8=48开始；第四行y=48

    /*4.刷写到显存中*/
    OLED_Refresh();
}

void App_Car_PID(void){
    int balance_out=0;
    int velocity_out=0;
    int turn_out=0;
    int pwma=0,pwmb=0;
    /*1.直立环控制*/
    balance_out=Com_PID_Balance(balance_kp,balance_kd,angle,balance_angle,gy);

    /*2.速度环控制*/
    //添加遥控小车前后移动的逻辑
    if(flag_up){
        remote_move=50;
    }else if(flag_down){
        remote_move=-50;
    }else{
        remote_move=0;
    }
    velocity_out=Com_PID_Velocity(velocity_kp,velocity_ki,ea,eb,remote_move);

    /*3.转向环控制*/
    if(flag_left){
        remote_turn-=20;
    }else if(flag_right){
        remote_turn+=20;
    }else{
        remote_turn=0;
        turn_out=Com_PID_Trun(turn_kp,gz);
    }
    /*一直按住会不断累加，所以要加一个限幅*/
   if(remote_turn>500){
       remote_turn=500;
   }else if(remote_turn<-500){
       remote_turn=-500;
   }
    printf("remote_turn=%d\n",remote_turn);
    /*4.叠加PID结果，作用到电机上*/
    pwma = balance_out+velocity_out+turn_out+remote_turn;
    pwmb = balance_out+velocity_out-turn_out-remote_turn;
    printf("pwma=%d,pwmb=%d\n",pwma,pwmb);
    Int_TB6612_SetPWM(pwma,pwmb);
}

//USART2的中断处理函数，无线遥控相关的逻辑
void USART2_IRQHandler(void)
{
    uint8_t recieve_data=0;
    /* 数据接收寄存器非空 */
    if (USART2->SR & USART_SR_RXNE)
    {
        recieve_data=USART2->DR;
    }
    switch(recieve_data){
        case 'U':
            flag_up=1;flag_down=0;flag_left=0;flag_right=0;
            break;
        case 'D':
            flag_up=0;flag_down=1;flag_left=0;flag_right=0;
            break;
        case 'L':
            flag_up=0;flag_down=0;flag_left=1;flag_right=0;
            break;
        case 'R':
            flag_up=0;flag_down=0;flag_left=0;flag_right=1;
            break;
        case 'S':
            flag_up=0;flag_down=0;flag_left=0;flag_right=0;
            break;
        default:
            flag_up=0;flag_down=0;flag_left=0;flag_right=0;
            break;
    }
}
