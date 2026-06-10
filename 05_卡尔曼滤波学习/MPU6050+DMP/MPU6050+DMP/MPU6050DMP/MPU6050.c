#include "MPU6050.h"

u8 mpu6050_write(u8 addr, u8 reg, u8 len, u8* buf)//返回值 0：读成功  -1：读失败
{ 
		 unsigned char i;
		 addr=addr<<1;                     //注意dma库地址不包含最后一位，需要移位
		 MPU6050_IIC_Start();              //启动总线
		 MPU6050_IIC_Send_Byte(addr);      //发送器件地址           
		 MPU6050_IIC_Send_Byte(reg);       //发送器件子地址

		 for(i=0;i<len;i++)            
			 MPU6050_IIC_Send_Byte(*buf++);  //发送数据
		 MPU6050_IIC_Stop();               //结束总线

		 return 0;
}

#define MPU6050_ADDRESS		0xD0		//MPU6050的I2C从机地址

/**
  * 函    数：MPU6050读寄存器
  * 参    数：RegAddress 寄存器地址，范围：参考MPU6050手册的寄存器描述
  * 返 回 值：读取寄存器的数据，范围：0x00~0xFF
  */
//uint8_t MPU6050_ReadReg(uint8_t RegAddress)
//{
//uint8_t Data;
//MPU6050_IIC_Start();							//启动总线 
//MPU6050_IIC_Send_Byte(MPU6050_ADDRESS);			//发送器件地址
//MPU6050_IIC_Send_Byte(RegAddress);				//发送器件地址

//MPU6050_IIC_Start();							//I2C重复起始
//MPU6050_IIC_Send_Byte(MPU6050_ADDRESS | 0x01);	//发送从机地址，读写位为1，表示即将读取
//Data = 
//}


u8 mpu6050_read(u8 addr, u8 reg, u8 len, u8 *buf)//返回值 0：读成功  -1：读失败
{
		 unsigned char i;
		 addr=addr<<1;                     //注意dma库地址不包含最后一位，需要移位
		 MPU6050_IIC_Start();              //启动总线           
		 MPU6050_IIC_Send_Byte(addr);      //发送器件地址            
		 MPU6050_IIC_Send_Byte(reg);       //发送器件子地址

		 MPU6050_IIC_Start();              //重新启动总线
		 MPU6050_IIC_Send_Byte(addr+1);
		 for(i=0;i<len-1;i++)  
			 *buf++=MPU6050_IIC_Read_Byte(0);//发送数据
		 *buf=MPU6050_IIC_Read_Byte(1);
		 MPU6050_IIC_Stop();               //结束总线
	
		 return 0;
}

void mpu6050_write_reg(u8 reg, u8 dat)
{
   mpu6050_write(SlaveAddress,reg,1,&dat);
}

u8   mpu6050_read_reg (u8 reg)
{
	 u8 dat;
   mpu6050_read(SlaveAddress,reg,1,&dat);
	 return dat;
}

void MPU6050_Init(void)
{
    MPU6050_IIC_IO_Init(); 
 	
	mpu6050_write_reg(PWR_MGMT_1,	 0X00);	//唤醒MPU6050 
    mpu6050_write_reg(SMPLRT_DIV,  0x07); //陀螺仪采样率125Hz
    mpu6050_write_reg(CONFIG,      0x06); //低通滤波频率5Hz
	mpu6050_write_reg(GYRO_CONFIG, 0x18); //陀螺仪自检及测量范围:不自检，2000deg/s
	mpu6050_write_reg(ACCEL_CONFIG,0x01); //加速计自检、测量范围及高通滤波频率:不自检，2G，5Hz
}

//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//   	其他,错误代码
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint8_t DataH, DataL;								//定义数据高8位和低8位的变量
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);		//读取陀螺仪Z轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);		//读取陀螺仪Z轴的低8位数据
	*GyroZ = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
}


 
