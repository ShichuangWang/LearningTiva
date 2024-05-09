

/**
 * main.c
 * 本程序可以测定信号周期并通过串口打印出来，信号从PF2输入
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
void Timer1BIntHandler();

volatile int cnt=0;
uint32_t last_count=0;
uint32_t now_count=0;
uint32_t T=0;
int main(void)
{
        //配置系统时钟，使用外部晶振驱动PLL，分频系数4，系统时钟50Mhz
        SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
        // 启用Timer1模块
        SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
        // 启用GPIO_F作为脉冲捕捉脚
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
        // 配置GPIO脚为使用Timer1捕捉模式
        GPIOPinConfigure(GPIO_PF2_T1CCP0);
        GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_2);
        // 为管脚配置弱上拉模式
        GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
        //配置串口
        UART0Configure();
        // 配置使用Timer1的TimerA模块为沿触发加计数模式,TimerB模块为连续向下计数模式
        TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME_UP|TIMER_CFG_B_PERIODIC);
        // 使用下降沿触发
        TimerControlEvent(TIMER1_BASE, TIMER_A, TIMER_EVENT_NEG_EDGE);
        // 设置TimerA计数范围为0~49999，TimerB计数范围为49999~0
        TimerLoadSet(TIMER1_BASE, TIMER_A, 49999);
        TimerLoadSet(TIMER1_BASE, TIMER_B, 49999);
        // 注册中断处理函数以响应触发事件
        TimerIntRegister(TIMER1_BASE, TIMER_A, Timer1AIntHandler);
        TimerIntRegister(TIMER1_BASE, TIMER_B, Timer1BIntHandler);
        // 系统总中断开
        IntMasterEnable();
        // 时钟中断允许，设置TimerA Capture模式中捕获到信号指定边沿和TimerB超时时触发中断
        TimerIntEnable(TIMER1_BASE, TIMER_CAPA_EVENT|TIMER_TIMB_TIMEOUT);
        //设定中断优先级，数字越小优先级越高
        IntPrioritySet(INT_TIMER1A,  1);
        IntPrioritySet(INT_TIMER1B,  0);
        // NVIC中允许定时器A、B模块中断
        IntEnable(INT_TIMER1A);
        IntEnable(INT_TIMER1B);
        // 启动定时器
        TimerEnable(TIMER1_BASE, TIMER_A|TIMER_B);
        while(1)
        {
            //延时0.1s
            SysCtlDelay(SysCtlClockGet()*0.1/3);
            //打印出信号的周期，单位为ms
            UARTprintf("T:%d\n",T/50000);
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
    uint32_t status;
    // 读取中断标志位
    status = TimerIntStatus(TIMER1_BASE, true);
    // 清除中断标志位
    now_count=TimerValueGet(TIMER1_BASE,TIMER_A)+cnt*50000;
    T=now_count-last_count;
    last_count=now_count;
    TimerIntClear(TIMER1_BASE, status);
}

void Timer1BIntHandler()
{
    uint32_t status;
    // 读取中断标志位
    status = TimerIntStatus(TIMER1_BASE, true);
    //定时器溢出后cnt++
    cnt++;
    // 清除中断标志位
    TimerIntClear(TIMER1_BASE, status);
}
