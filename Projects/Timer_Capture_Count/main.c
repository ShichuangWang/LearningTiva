

/**
 * main.c
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "utils/uartstdio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"

void UART0Configure();
void Timer1AIntHandler();

int main(void)
{
        //配置系统时钟，使用外部晶振驱动PLL，分频系数4，系统时钟50Mhz
        SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
        // 启用Timer1模块
        SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
        // 启用GPIO_M作为脉冲捕捉脚
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
        // 配置GPIO脚为使用Timer1捕捉模式
        GPIOPinConfigure(GPIO_PF2_T1CCP0);
        GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_2);
        // 为管脚配置弱上拉模式
        GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
        //配置串口
        UART0Configure();
        // 配置使用Timer1的TimerA模块为沿触发加计数模式
        TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_COUNT_UP);
        // 使用下降沿触发
        TimerControlEvent(TIMER1_BASE, TIMER_A, TIMER_EVENT_NEG_EDGE);
        // 设置计数范围为0~0x8FFA
        TimerMatchSet(TIMER1_BASE, TIMER_A, 10);
        // 注册中断处理函数以响应触发事件
        TimerIntRegister(TIMER1_BASE, TIMER_A, Timer1AIntHandler);
        // 系统总中断开
        IntMasterEnable();
        // 时钟中断允许，设置Capture模式中达到计数值达到匹配值Match触发中断
        TimerIntEnable(TIMER1_BASE, TIMER_CAPA_MATCH);
        // NVIC中允许定时器A模块中断
        IntEnable(INT_TIMER1A);
        // 启动捕捉模块
        TimerEnable(TIMER1_BASE, TIMER_A);
        while(1)
        {
               SysCtlDelay(SysCtlClockGet()/3);
               //串口打印出当前计数值
               UARTprintf("count:%d\n",TimerValueGet(TIMER1_BASE,TIMER_A));
        }
}


void UART0Configure()
{
    //使能UART0及GPIOF
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //将PA0、PA1引脚配置成串口引脚
    GPIOPinConfigure(GPIO_PA0_U0RX);//RX=GPIO_PIN_0
    GPIOPinConfigure(GPIO_PA1_U0TX);//TX=GPIO_PIN_1
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    //设置串口波特时钟源
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    //UART的printf操作配置
    UARTStdioConfig(0, 115200, 16000000);

}

void Timer1AIntHandler()
{
    uint32_t ulstatus;
    // 读取中断标志位
    ulstatus = TimerIntStatus(TIMER1_BASE, TIMER_CAPA_MATCH);
    // 清除中断标志位
    TimerIntClear(TIMER1_BASE, ulstatus);
    //重新开启中断，在加计数模式下可以省略，但是在减计数模式下，一定要开，减计数模式下触发中断后会直接关闭中断，需要手动重新开启
    TimerEnable(TIMER1_BASE, TIMER_A);
}
