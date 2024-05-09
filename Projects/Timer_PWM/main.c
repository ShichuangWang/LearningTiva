
/**
 * main.c
 * 使用定时器输出频率为16Hz，占空比为50%的PWM，输出引脚为PF2
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
#include "driverlib/pin_map.h"

int main(void)
{
    //配置系统时钟，直接使用外部晶振，分频系数16，系统时钟1Mhz
    SysCtlClockSet(SYSCTL_SYSDIV_16|SYSCTL_USE_OSC|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);       //使能外设
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);       //使能外设

    //配置PF2引脚为T1CCP0引脚，输出PWM
    GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_2);
    GPIOPinConfigure(GPIO_PF2_T1CCP0);
    //配置定时器时钟源，不配置的话默认是系统时钟作为时钟源
    TimerClockSourceSet(TIMER1_BASE,TIMER_CLOCK_SYSTEM);
    //配置定时器，将Timer0拆分并配置TIMERA为PWM输出模式
    TimerConfigure(TIMER1_BASE,TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM);
    //设置装载值
    TimerLoadSet(TIMER1_BASE, TIMER_A,62499);
    //设置比较值
    TimerMatchSet(TIMER1_BASE, TIMER_A,31250);
    //使能定时器
    TimerEnable( TIMER1_BASE, TIMER_A);
    while(1)
    {

    }
}


