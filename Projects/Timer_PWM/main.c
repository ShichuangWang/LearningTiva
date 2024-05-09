
/**
 * main.c
 * ʹ�ö�ʱ�����Ƶ��Ϊ16Hz��ռ�ձ�Ϊ50%��PWM���������ΪPF2
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
    //����ϵͳʱ�ӣ�ֱ��ʹ���ⲿ���񣬷�Ƶϵ��16��ϵͳʱ��1Mhz
    SysCtlClockSet(SYSCTL_SYSDIV_16|SYSCTL_USE_OSC|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);       //ʹ������
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);       //ʹ������

    //����PF2����ΪT1CCP0���ţ����PWM
    GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_2);
    GPIOPinConfigure(GPIO_PF2_T1CCP0);
    //���ö�ʱ��ʱ��Դ�������õĻ�Ĭ����ϵͳʱ����Ϊʱ��Դ
    TimerClockSourceSet(TIMER1_BASE,TIMER_CLOCK_SYSTEM);
    //���ö�ʱ������Timer0��ֲ�����TIMERAΪPWM���ģʽ
    TimerConfigure(TIMER1_BASE,TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM);
    //����װ��ֵ
    TimerLoadSet(TIMER1_BASE, TIMER_A,62499);
    //���ñȽ�ֵ
    TimerMatchSet(TIMER1_BASE, TIMER_A,31250);
    //ʹ�ܶ�ʱ��
    TimerEnable( TIMER1_BASE, TIMER_A);
    while(1)
    {

    }
}


