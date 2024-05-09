/**
 * main.c
 * 使用PWM模块输出频率为1Hz，占空比为50%的PWM，输出引脚为PF2
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/systick.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"

int main()
{
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_OSC|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);       //系统时钟4M
    //配置PWM时钟（设置USEPWMDIV分频器）
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);//PWM时钟4M/64
    //使能时钟
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);         //使能PWM模块1时钟
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);        //使能GPIOF时钟
    //使能引脚复用PWM功能
    GPIOPinTypePWM(GPIO_PORTF_BASE,GPIO_PIN_2);
    GPIOPinTypePWM(GPIO_PORTF_BASE,GPIO_PIN_3);
    //PWM信号分配
    GPIOPinConfigure(GPIO_PF2_M1PWM6);                  //PF2->PWM模块1信号6
    //配置PWM发生器
    //模块1->发生器3->向下计数，不同步
    PWMGenConfigure(PWM1_BASE,PWM_GEN_3,PWM_GEN_MODE_DOWN|PWM_GEN_MODE_NO_SYNC);
    //配置PWM周期
    PWMGenPeriodSet(PWM1_BASE,PWM_GEN_3,625);
    //配置PWM占空比
    PWMPulseWidthSet(PWM1_BASE,PWM_OUT_6,(PWMGenPeriodGet(PWM1_BASE, PWM_GEN_3)+1)*0.5);
    //使能PWM模块1通道6输出
    PWMOutputState(PWM1_BASE,PWM_OUT_6_BIT,true);
    //使能PWM发生器
    PWMGenEnable(PWM1_BASE,PWM_GEN_3);
    while(1)
    {

    }
}


