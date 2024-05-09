/**
 * main.c
 * ʹ��PWMģ�����Ƶ��Ϊ1Hz��ռ�ձ�Ϊ50%��PWM���������ΪPF2
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
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_OSC|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);       //ϵͳʱ��4M
    //����PWMʱ�ӣ�����USEPWMDIV��Ƶ����
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);//PWMʱ��4M/64
    //ʹ��ʱ��
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);         //ʹ��PWMģ��1ʱ��
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);        //ʹ��GPIOFʱ��
    //ʹ�����Ÿ���PWM����
    GPIOPinTypePWM(GPIO_PORTF_BASE,GPIO_PIN_2);
    GPIOPinTypePWM(GPIO_PORTF_BASE,GPIO_PIN_3);
    //PWM�źŷ���
    GPIOPinConfigure(GPIO_PF2_M1PWM6);                  //PF2->PWMģ��1�ź�6
    //����PWM������
    //ģ��1->������3->���¼�������ͬ��
    PWMGenConfigure(PWM1_BASE,PWM_GEN_3,PWM_GEN_MODE_DOWN|PWM_GEN_MODE_NO_SYNC);
    //����PWM����
    PWMGenPeriodSet(PWM1_BASE,PWM_GEN_3,625);
    //����PWMռ�ձ�
    PWMPulseWidthSet(PWM1_BASE,PWM_OUT_6,(PWMGenPeriodGet(PWM1_BASE, PWM_GEN_3)+1)*0.5);
    //ʹ��PWMģ��1ͨ��6���
    PWMOutputState(PWM1_BASE,PWM_OUT_6_BIT,true);
    //ʹ��PWM������
    PWMGenEnable(PWM1_BASE,PWM_GEN_3);
    while(1)
    {

    }
}


