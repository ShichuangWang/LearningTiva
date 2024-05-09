/**
 * main.c
 *
 * 本程序使用定时器定时实现LED固定周期闪烁
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
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
//中断服务函数
void TIMER0_IRQHandler();
int led=0;
uint32_t clk=0;
int main(void)
{
    //配置系统时钟，使用外部晶振驱动PLL，分频系数10，系统时钟20Mhz
    SysCtlClockSet(SYSCTL_SYSDIV_10|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);       //使能外设
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);       //使能外设
    clk=SysCtlClockGet();
    //配置PF3引脚为输出模式,PF3连接到了板载LED
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_3|GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3|GPIO_PIN_0, 0);

    //配置定时器，将Timer0拆分并配置TIMERA为连续向下计数模式
    TimerConfigure(TIMER0_BASE,TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PERIODIC);
    //设置预分频值
    TimerPrescaleSet(TIMER0_BASE, TIMER_A,199);
    //设置装载值
    TimerLoadSet(TIMER0_BASE, TIMER_A,49999);
    //注册中断服务函数

    TimerIntRegister(TIMER0_BASE,TIMER_A, TIMER0_IRQHandler);
    //开启定时器A超时中断
    TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
    //设置中断优先级
    IntPrioritySet(INT_TIMER0A,  0);
    //使能中断
    IntEnable(INT_TIMER0A);
    IntMasterEnable();
    //使能定时器
    TimerEnable( TIMER0_BASE, TIMER_A);
    while(1)
    {

    }
}
void TIMER0_IRQHandler()
{
    //获取中断状态
    uint32_t status=TimerIntStatus(TIMER0_BASE, true);
    //清除中断标志位
    TimerIntClear(TIMER0_BASE,status);
    if(led)
        GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3|GPIO_PIN_0, GPIO_PIN_3|GPIO_PIN_0);
    else
        GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3|GPIO_PIN_0, 0);
    led=!led;
}
