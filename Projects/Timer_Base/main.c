/**
 * main.c
 *
 * ������ʹ�ö�ʱ����ʱʵ��LED�̶�������˸
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
//�жϷ�����
void TIMER0_IRQHandler();
int led=0;
uint32_t clk=0;
int main(void)
{
    //����ϵͳʱ�ӣ�ʹ���ⲿ��������PLL����Ƶϵ��10��ϵͳʱ��20Mhz
    SysCtlClockSet(SYSCTL_SYSDIV_10|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);       //ʹ������
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);       //ʹ������
    clk=SysCtlClockGet();
    //����PF3����Ϊ���ģʽ,PF3���ӵ��˰���LED
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_3|GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3|GPIO_PIN_0, 0);

    //���ö�ʱ������Timer0��ֲ�����TIMERAΪ�������¼���ģʽ
    TimerConfigure(TIMER0_BASE,TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PERIODIC);
    //����Ԥ��Ƶֵ
    TimerPrescaleSet(TIMER0_BASE, TIMER_A,199);
    //����װ��ֵ
    TimerLoadSet(TIMER0_BASE, TIMER_A,49999);
    //ע���жϷ�����

    TimerIntRegister(TIMER0_BASE,TIMER_A, TIMER0_IRQHandler);
    //������ʱ��A��ʱ�ж�
    TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
    //�����ж����ȼ�
    IntPrioritySet(INT_TIMER0A,  0);
    //ʹ���ж�
    IntEnable(INT_TIMER0A);
    IntMasterEnable();
    //ʹ�ܶ�ʱ��
    TimerEnable( TIMER0_BASE, TIMER_A);
    while(1)
    {

    }
}
void TIMER0_IRQHandler()
{
    //��ȡ�ж�״̬
    uint32_t status=TimerIntStatus(TIMER0_BASE, true);
    //����жϱ�־λ
    TimerIntClear(TIMER0_BASE,status);
    if(led)
        GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3|GPIO_PIN_0, GPIO_PIN_3|GPIO_PIN_0);
    else
        GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3|GPIO_PIN_0, 0);
    led=!led;
}
