#include "Int_MPU6050.h"
#include "main.h"

//从MPU6050的指定寄存器reg_addr中读取一字节数据卷
uint8_t Int_MPU6050_ReadByte(uint8_t reg_addr,uint8_t *receive_byte){
    /*1.起始信号*/
    Driver_I2C2_Start();
    /*2.AD+W*/
    Driver_I2C_SendAddr(MPU_IIC_ADDR<<1|0X00);
    /*3.发送寄存器地址*/
    Driver_I2C_SendByte(reg_addr);
    /*4.起始信号*/
    Driver_I2C2_Start();
    /*5.AD+R*/
    Driver_I2C_SendAddr(MPU_IIC_ADDR<<1|0X01);
    /*6.先准备NACK和停止信号*/
    Driver_I2C2_NAck();
    Driver_I2C2_Stop();
    /*7.读取一字节数据*/
    *receive_byte=Driver_I2C_ReadByte();

    return 0;
}

//从MPU6050的指定寄存器reg_addr中读取多字节数据
uint8_t Int_MPU6050_ReadBytes(uint8_t reg_addr,uint8_t *receive_buff,uint8_t size){
    /*1.起始信号*/
    Driver_I2C2_Start();
    /*2.AD+W*/
    Driver_I2C_SendAddr(MPU_IIC_ADDR<<1|0X00);
    /*3.发送寄存器地址*/
    Driver_I2C_SendByte(reg_addr);
    /*4.起始信号*/
    Driver_I2C2_Start();
    /*5.AD+R*/
    Driver_I2C_SendAddr(MPU_IIC_ADDR<<1|0X01);
    /*6.连续读取多个字节*/
    for(uint8_t i=0;i<size;i++){
        if(i==size-1){
            /*是最后一个字节，提前准备NACK和STOP*/
            Driver_I2C2_NAck();
            Driver_I2C2_Stop();
        }else{
            /*不是最后一个字节，提前准备ACK*/
            Driver_I2C2_Ack();
        }
        *(receive_buff+i)=Driver_I2C_ReadByte();
    }

    return 0;
}

//向MPU6050的指定寄存器reg_addr中写入一字节数据
uint8_t Int_MPU6050_WriteByte(uint8_t reg_addr,uint8_t write_byte){
    /*1.起始信号*/
    Driver_I2C2_Start();
    /*2.AD+W*/
    Driver_I2C_SendAddr(MPU_IIC_ADDR<<1|0X00);
    /*3.发送寄存器地址*/
    Driver_I2C_SendByte(reg_addr);
    /*4.写入一个字节*/
    Driver_I2C_SendByte(write_byte);
    /*5.停止信号*/
    Driver_I2C2_Stop();

    return 0;
}

//向MPU6050的指定寄存器reg_addr中写入多个字节数据
uint8_t Int_MPU6050_WriteBytes(uint8_t reg_addr,uint8_t *write_bytes,uint8_t size){
    /*1.起始信号*/
    Driver_I2C2_Start();
    /*2.AD+W*/
    Driver_I2C_SendAddr(MPU_IIC_ADDR<<1|0X00);
    /*3.发送寄存器地址*/
    Driver_I2C_SendByte(reg_addr);
    /*4.写入多个字节*/
    for(uint8_t i=0;i<size;i++){
        Driver_I2C_SendByte(*(write_bytes+i));
    }
    /*5.停止信号*/
    Driver_I2C2_Stop();

    return 0;
}

//低通滤波器设置函数
void Int_MPU6050_Set_DLPF_CFG(uint16_t rate){
    uint8_t cfg = 0;
    /* 根据采样定理，采样率>=2*带宽，才不会失真*/
    rate=rate/2;
    if(rate>188){
        cfg = 1;
    }else if(rate>98){
        cfg = 2;
    }else if(rate>42){
        cfg = 3;
    }else if(rate>20){
        cfg = 4;
    }else if(rate>10){
        cfg = 5;
    }else{
        cfg = 6;
    }
    Int_MPU6050_WriteByte(MPU_CFG_REG,cfg<<0);
}

//陀螺仪采样率设置函数，rate是采样率
void Int_MPU6050_SetGyroRate(uint16_t rate){
    /* 采样率=输出频率/(1+分频值)*/
    uint8_t sample_div = 0;
    /*1.采样率限幅,产品说明书中限定输出频率最小为4*/
    if(rate<4){
        rate=4;
    }else if(rate>1000){
        rate=1000;
    }
    /*2.根据期望的采样率，计算出分频值*/
    sample_div=1000/rate-1;
    /*3.将分频值设置到寄存器中*/
    Int_MPU6050_WriteByte(MPU_SAMPLE_RATE_REG,sample_div);
    /*4.根据采样率去设置低通滤波器*/
    Int_MPU6050_Set_DLPF_CFG(rate);
}


//MPU6050初始化
void Int_MPU6050_Init(void){
    uint8_t dev_id=0;
    /*1.初始化IIC*/
    Driver_I2C2_Init();
    /*2.复位->延迟一会->唤醒*/
    Int_MPU6050_WriteByte(MPU_PWR_MGMT1_REG,0X80);
    for_delay_ms(300);
    Int_MPU6050_WriteByte(MPU_PWR_MGMT1_REG,0X00);
    /*3.陀螺仪量程,+-2000°,设置fsr=3*/
    Int_MPU6050_WriteByte(MPU_GYRO_CFG_REG,3<<3);
    /*4.加速度量程*/
    Int_MPU6050_WriteByte(MPU_ACCEL_CFG_REG,0<<3);
    /*5.其他功能设置（可选）：FIFO、第二IIC、中断*/
    Int_MPU6050_WriteByte(MPU_INT_EN_REG,0X00);//关闭所有中断
    Int_MPU6050_WriteByte(MPU_USER_CTRL_REG,0X00);//关闭第二IIC、关闭FIFO使用（这里不让第二IIC使用FIFO，即MPU6050作为主机时）
    Int_MPU6050_WriteByte(MPU_FIFO_EN_REG,0X00);//关闭所有FIFO（MPU6050作为从机时）
    /*6.系统时钟源、陀螺仪采样率、低通滤波的设置*/
    /*配置时钟源之前，确认已经正常工作，读一下id*/
    Int_MPU6050_ReadByte(MPU_DEVICE_ID_REG,&dev_id);
    if(dev_id==MPU_IIC_ADDR){
        //配置时钟源，选择陀螺仪X轴的时钟，精度更高
        Int_MPU6050_WriteByte(MPU_PWR_MGMT1_REG,0X01);
        //配置陀螺仪采样率及低通滤波，MPU6050采集的是三轴加速度，和三轴角速度
        Int_MPU6050_SetGyroRate(100);
        //让两个传感器退出待机模式，进入正常工作状态
        Int_MPU6050_WriteByte(MPU_PWR_MGMT2_REG,0X00);
    }
}

//从MPU6050读取陀螺仪三轴的角速度的数据，gx、gy、gz分别为x、y、z轴的数据
void Int_MPU6050_Get_Gyro(short *gx,short *gy,short *gz){
    uint8_t buff[6]={0};
    // buff[0]：角速度x轴高八位
    // buff[1]：角速度x轴低八位
    // buff[2]：角速度y轴高八位
    // buff[3]：角速度y轴低八位
    // buff[4]：角速度z轴高八位
    // buff[5]：角速度z轴低八位
    Int_MPU6050_ReadBytes(MPU_GYRO_XOUTH_REG,buff,6);
    //获取x、y、z轴角速度
    *gx=((short)(buff[0])<<8)|buff[1];
    *gy=((short)(buff[2])<<8)|buff[3];
    *gz=((short)(buff[4])<<8)|buff[5];
}

//从MPU6050读取加速度计的加速度的数据，ax、ay、az分别为x、y、z轴的数据
void Int_MPU6050_Get_Accel(short *ax,short *ay,short *az){
    uint8_t buff[6]={0};
    // buff[0]：加速度x轴高八位
    // buff[1]：加速度x轴低八位
    // buff[2]：加速度y轴高八位
    // buff[3]：加速度y轴低八位
    // buff[4]：加速度z轴高八位
    // buff[5]：加速度z轴低八位
    Int_MPU6050_ReadBytes(MPU_ACCEL_XOUTH_REG,buff,6);
    //获取x轴角速度
    *ax=((short)(buff[0])<<8)|buff[1];
    *ay=((short)(buff[2])<<8)|buff[3];
    *az=((short)(buff[4])<<8)|buff[5];
}
