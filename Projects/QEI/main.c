/**
 * main.c
 * 用信号发生器和电机驱动驱动电机转动，将电机编码器A、B相信号分别接到开发板PD6和PD7引脚，
 * 打开串口助手每个0.1S在串口助手中更新一次QEI0模块位置计数器的值
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/systick.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/qei.h"
#include "utils/uartstdio.h"

//初始化uart串口打印显示函数
void ConfigureUART(void)
{
    // 启用UART使用的GPIO
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // 使能 UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    // 为UART配置GPIO引脚。
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // 初始化控制台I/O的UART
    UARTStdioConfig(0, 115200, SysCtlClockGet());//0-115200-16M
}

int main()
{
    SysCtlClockSet(SYSCTL_SYSDIV_1|SYSCTL_USE_OSC|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);       //系统时钟16M
    //使能QEI与GPIOD
    SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    //解锁PD7引脚
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= 0x80;
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;
    //复用GPIO
    GPIOPinTypeQEI(GPIO_PORTD_BASE,GPIO_PIN_6);
    GPIOPinTypeQEI(GPIO_PORTD_BASE,GPIO_PIN_7);
    GPIOPinConfigure(GPIO_PD6_PHA0);//PD6->编码器A相输入
    GPIOPinConfigure(GPIO_PD7_PHB0);//PD7->编码器B相输入
    //配置QEI0模块，A、B两相信号边沿均产生计数，不开启索引脉冲复位，正交相位模式，A、B两相信号不交换，最大计数值为0xffffffff
    QEIConfigure(QEI0_BASE,QEI_CONFIG_CAPTURE_A_B|QEI_CONFIG_NO_RESET|QEI_CONFIG_QUADRATURE|QEI_CONFIG_NO_SWAP,0xffffffff);
    //禁止QEI0的所有中断
    QEIIntDisable(QEI0_BASE,QEI_INTERROR | QEI_INTDIR | QEI_INTTIMER | QEI_INTINDEX);
    //使能QEI0
    QEIEnable(QEI0_BASE);
    //设置QEI0模块当前位置计数器值为0
    QEIPositionSet(QEI0_BASE,0);
    //串口初始化
    ConfigureUART();
    UARTprintf("It is OK!\n");
    while(1)
    {
        //每隔0.1S在终端输出当前QEI位置计数器的值
        SysCtlDelay(0.1*SysCtlClockGet()/3);
        uint32_t pos=QEIPositionGet(QEI0_BASE);
        UARTprintf("%u\n",pos);
    }
}
