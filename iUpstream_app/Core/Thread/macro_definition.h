/**
******************************************************************************
* @file    		macro_definition.h
* @brief   		系统参数宏定义
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-8-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MACRO_DEFINITION_H__
#define __MACRO_DEFINITION_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/


/*========================================== <main.h> macro ====================================================*/

//******************  调试模式 **************************
//#define SYSTEM_DEBUG_MODE					1
#define UART_PRINTF_LOG						1
//#define UART_DEBUG_SEND_CTRL			1
//*******************************************************

/*==============================================================================================================*/
// 串口1 --> 中控Modbus 	(485)
// 串口2 --> wifi 			(ttl)
// 串口3 --> 驱动板 			(ttl)
// 串口4 --> 调试 debug 	(ttl)
// 串口5 --> 蓝牙				(ttl)

#define MACRO_MODBUS_USART								1
#define	MACRO_WIFI_USART									2
#define	MACRO_DRIVER_USART								3
#define	MACRO_DEBUG_USART									4
#define	MACRO_BLUETOOTH_USART							5

//串口总数
#define	MACRO_SYSTEM_USER_USART_MAX				(5)


#define MACRO_POWER_ON_WAITE_TIME_TASK								(4000)			//上电等待时间 （等显示开机界面 机型码）

// 产品机型码
#define	MACRO_SYSTEM_PRODUCT_MODEL_CODE								(1200)		//

// 软件版本
#define	MACRO_SOFTWARE_VERSION_UINT32								0x00000003		//0.3
// 硬件版本
#define	MACRO_HARDWAR_VERSION_UINT32								0x00030000		//3.0
// 软件版本  驱动板
#define	MACRO_DEVICES_VERSION_UINT32								0x00010000		//1.0
// 硬件版本  驱动板
#define	MACRO_DEVICES_HARDWAR_VERSION_UINT32								0x00030000		//3.0
//线程 最小生命周期
#define THREAD_LIFECYCLE_PERIOD_MIX									10						//ms
/*==============================================================================================================*/
/*==============================================================================================================*/
	
/*========================================== <Breath_light.h> macro ============================================*/
/*==============================================================================================================*/
#define BREATH_LIGHT_THREAD_TURN_ON					1

#if(BREATH_LIGHT_THREAD_TURN_ON)
//线程周期
#define BREATH_LIGHT_THREAD_LIFECYCLE				(2*THREAD_LIFECYCLE_PERIOD_MIX)				// ms
//光圈 pwm 通道号
#define BREATH_LIGHT_PWM_CHANNEL						(TIM_CHANNEL_1)

//******************  调试模式 **************************
#ifdef SYSTEM_DEBUG_MODE
#define LIGHT_BRIGHTNESS_MIX					(0)			// 最低亮度  
#define LIGHT_BRIGHTNESS_MAX					(200)			// 最大亮度  0~500
#else
#define LIGHT_BRIGHTNESS_MIX					(0)			// 最低亮度  
#define LIGHT_BRIGHTNESS_MAX					(500)//(*p_Breath_Light_Max)			// 最大亮度  0~500
#endif
//*******************************************************
//档位
#define BREATH_LIGHT_GEAR_POSITION				(1000/BREATH_LIGHT_THREAD_LIFECYCLE)				// 档位 50
//步进
#define LIGHT_BRIGHTNESS_STEP							((LIGHT_BRIGHTNESS_MAX-LIGHT_BRIGHTNESS_MIX)/BREATH_LIGHT_GEAR_POSITION)

#endif
/*==============================================================================================================*/
/*==============================================================================================================*/


/*========================================== <key.h> macro =====================================================*/
/*==============================================================================================================*/
#define KEY_THREAD_TURN_ON					1

#if(KEY_THREAD_TURN_ON)
//线程周期
#define KEY_THREAD_LIFECYCLE								(2*THREAD_LIFECYCLE_PERIOD_MIX)	// 任务生命周期 200ms

#define KEY_LONG_PRESS_TIME									(3000/KEY_THREAD_LIFECYCLE)			//长按时间 3s
#define KEY_LONG_PRESS_TIME_SHORT						(1000/KEY_THREAD_LIFECYCLE)			//短一点的 长按时间  1s
//-------------- 蜂鸣器 长度 -------------------
//******************  调试模式 **************************
#ifdef SYSTEM_DEBUG_MODE
#define KEY_BUZZER_TIME								(1)					//周期  KEY_THREAD_LIFECYCLE 倍数
#else
#define KEY_BUZZER_TIME								(8)					//周期  KEY_THREAD_LIFECYCLE 倍数
#endif

#define KEY_VALUE_SHAKE_TIME					(2)		//去抖动 次数

//屏幕背光 pwm 控制 
#define LCD_BACK_LIGHT_PWM_CTRL		0

//屏幕背光 pwm 通道号
#define LCD_BACK_LIGHT_PWM_CHANNEL						(TIM_CHANNEL_2)
//屏幕背光 最大亮度
#define BACK_LIGHT_BRIGHTNESS_MAX						(500)			// 最大亮度  0~500
//休眠时间(目前无效)
#define KEY_FOR_SLEEP_TIME_SHORT						(3000/KEY_THREAD_LIFECYCLE)			//5 min  300000

#endif
/*==============================================================================================================*/
/*==============================================================================================================*/


/*========================================== <timing.h> macro ==================================================*/
/*==============================================================================================================*/
#define TIMING_THREAD_TURN_ON					1

#if(TIMING_THREAD_TURN_ON)
#define TIMING_THREAD_LIFECYCLE				(50*THREAD_LIFECYCLE_PERIOD_MIX)				// ms

//******************  调试模式 **************************
#ifdef SYSTEM_DEBUG_MODE
//配网时长
#define WIFI_DISTRIBUTION_TIME_CALLOUT				(6*(1000/TIMING_THREAD_LIFECYCLE))				// 6 s
#define BT_DISTRIBUTION_TIME_CALLOUT					(6*(1000/TIMING_THREAD_LIFECYCLE))				// 6 s
//故障自恢复
#define SYSTEM_FAULT_TIME_CALLOUT							(20*(1000/TIMING_THREAD_LIFECYCLE))				// 20 s
#define SYSTEM_FAULT_RECOVERY_MAX							(3)				// 3 次故障
#define SYSTEM_FAULT_RECOVERY_TIME						(60*(1000/TIMING_THREAD_LIFECYCLE))				// 1 分钟  60 s
//自动关机
#define AUTOMATIC_SHUTDOWN_TIME								(600*(1000/TIMING_THREAD_LIFECYCLE))				// 10 min

#else
//配网时长
#define WIFI_DISTRIBUTION_TIME_CALLOUT				(60*(1000/TIMING_THREAD_LIFECYCLE))				// 60 s
#define BT_DISTRIBUTION_TIME_CALLOUT					(60*(1000/TIMING_THREAD_LIFECYCLE))				// 60 s
//故障自恢复
#define SYSTEM_FAULT_TIME_CALLOUT							(120*(1000/TIMING_THREAD_LIFECYCLE))				// 120 s
#define SYSTEM_FAULT_RECOVERY_MAX							(3)				// 3 次故障
#define SYSTEM_FAULT_RECOVERY_TIME						(3600*(1000/TIMING_THREAD_LIFECYCLE))				// 1 小时内  3600 s
//自动关机
#define AUTOMATIC_SHUTDOWN_TIME								(3600*(1000/TIMING_THREAD_LIFECYCLE))				// 1 小时内  3600 s

#endif
//自动关机 时间


//-------------------------------------------------------------------------------------------------
//-------------- 机箱温度报警值 90°C -------------------
#define AMBIENT_TEMP_ALARM_VALUE								80
//-------------- 机箱温度 降速 70°C -------------------
#define AMBIENT_TEMP_REDUCE_SPEED									70		// 降档 温度
#define AMBIENT_TEMP_RESTORE_SPEED								65		// 恢复 温度
//-------------------------------------------------------------------------------------------------

//-------------- 降速检查时间 -------------------
#define TIME_SLOW_DOWN_TIME													(120)		//2 min  120*2 个周期,与线程周期相关
//-------------- 降速 档位 -------------------
#define TIME_SLOW_DOWN_SPEED_01											(10)		//第一档 降速
#define TIME_SLOW_DOWN_SPEED_02											(5)			//第二档 降速
//-------------- 降速 最低速度 -------------------
#define TIME_SLOW_DOWN_SPEED_MIX										(20)		//最低降到 20%
#define TIME_SLOW_DOWN_SPEED_MAX										(100)		//恢复速度最高恢复到 100%

//-------------- 电机不启动 报警时间 --------------
//#define MOTOR_CANNOT_START_TIME											(5)				// 5 秒
//-------------- 电机转速不准 报警时间 --------------
//#define MOTOR_SPEED_ERROR_TIME											(5)				// 5 秒
#endif
/*==============================================================================================================*/
/*==============================================================================================================*/


/*========================================== <modbus.h> macro ==================================================*/
/*==============================================================================================================*/
#define MODBUS_THREAD_TURN_ON					1

#if(MODBUS_THREAD_TURN_ON)
#define MODBUS_THREAD_LIFECYCLE							(2*THREAD_LIFECYCLE_PERIOD_MIX)				// ms 暂时不用

#endif
/*==============================================================================================================*/
/*==============================================================================================================*/



/*========================================== <motor.h> macro ===================================================*/
/*==============================================================================================================*/
#define MOTOR_THREAD_TURN_ON					1

#if(MOTOR_THREAD_TURN_ON)
#define MOTOR_THREAD_LIFECYCLE						(2*THREAD_LIFECYCLE_PERIOD_MIX)				// 任务生命周期 50ms

// 命令 周期 200ms 
#define MOTOR_POLLING_PERIOD							(200/MOTOR_THREAD_LIFECYCLE)

// 心跳 周期 200ms 
#define MOTOR_HEARTBEAT_CYCLE							(MOTOR_POLLING_PERIOD/3)
// 命令 周期 200ms
#define MOTOR_COMMAND_CYCLE								((MOTOR_POLLING_PERIOD/3)*2)
// 读状态 周期 200ms 
#define MOTOR_READ_STATIC_CYCLE						(0)

//电机极数
#define	MOTOR_POLE_NUMBER									(5)

//最大转速 100%
#define	MOTOR_RPM_SPEED_MAX								(1950*MOTOR_POLE_NUMBER)		//9750
//最低转速 100%
#define	MOTOR_RPM_SPEED_MIX								(700*MOTOR_POLE_NUMBER)			//3500

// 700  1012   1324  1637   1950
//每 1% 转速
#define	MOTOR_RPM_CONVERSION_COEFFICIENT				((MOTOR_RPM_SPEED_MAX - MOTOR_RPM_SPEED_MIX) /80)			//15.6

//每 20% 转速 
#define	MOTOR_RPM_CONVERSION_COEFFICIENT_20				((MOTOR_RPM_SPEED_MAX - MOTOR_RPM_SPEED_MIX) /4)			//312.5     

//电机最低速度  百分比  20%
#define	MOTOR_POWER_SPEED										(20)
//电机最低电流
//#define	MOTOR_CURRENT_MIX									(1000)				//10A
//电机转速误差范围
//#define	MOTOR_SPEED_VIBRATION_RANGE					(20*MOTOR_POLE_NUMBER)				//乘以电机极数
//-------------------------------------------------------------------------------------------------
//-------------- MOS 温度报警值 90°C -------------------
#define MOS_TEMP_ALARM_VALUE								(90)
//-------------- MOS 温度 降速 80°C -------------------
#define MOS_TEMP_REDUCE_SPEED								(80)		// 降档 温度
#define MOS_TEMP_RESTORE_SPEED							(75)		// 恢复 温度
//-------------------------------------------------------------------------------------------------


//通讯故障 报警时间
#define FAULT_MOTOR_LOSS_TIME							(30000/(MOTOR_THREAD_LIFECYCLE))				// 30 秒
//通讯故障 尝试重启 时间
#define FAULT_MOTOR_TRY_RESTAR_TIME				(FAULT_MOTOR_LOSS_TIME/5)								// 6秒

/*------------------- IO define ----------------------------------------------*/
#define	MOTOR_MODULE_HUART				DRIVER_USART		//

#if (MOTOR_MODULE_HUART == 1)
#define MOTOR_RS485_TX_EN_PORT		RS48501_RE_GPIO_Port
#define MOTOR_RS485_TX_EN_PIN			RS48501_RE_Pin
#elif (MOTOR_MODULE_HUART == 4)
#define MOTOR_RS485_TX_EN_PORT		RS48504_RE_GPIO_Port
#define MOTOR_RS485_TX_EN_PIN			RS48504_RE_Pin
#endif

#endif
/*==============================================================================================================*/
/*==============================================================================================================*/
#ifdef __cplusplus
}
#endif

#endif /* __MACRO_DEFINITION_H__ */

