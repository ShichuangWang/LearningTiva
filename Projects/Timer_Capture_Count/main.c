

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
        //����ϵͳʱ�ӣ�ʹ���ⲿ��������PLL����Ƶϵ��4��ϵͳʱ��50Mhz
        SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
        // ����Timer1ģ��
        SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
        // ����GPIO_M��Ϊ���岶׽��
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
        // ����GPIO��Ϊʹ��Timer1��׽ģʽ
        GPIOPinConfigure(GPIO_PF2_T1CCP0);
        GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_2);
        // Ϊ�ܽ�����������ģʽ
        GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
        //���ô���
        UART0Configure();
        // ����ʹ��Timer1��TimerAģ��Ϊ�ش����Ӽ���ģʽ
        TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_COUNT_UP);
        // ʹ���½��ش���
        TimerControlEvent(TIMER1_BASE, TIMER_A, TIMER_EVENT_NEG_EDGE);
        // ���ü�����ΧΪ0~0x8FFA
        TimerMatchSet(TIMER1_BASE, TIMER_A, 10);
        // ע���жϴ���������Ӧ�����¼�
        TimerIntRegister(TIMER1_BASE, TIMER_A, Timer1AIntHandler);
        // ϵͳ���жϿ�
        IntMasterEnable();
        // ʱ���ж���������Captureģʽ�дﵽ����ֵ�ﵽƥ��ֵMatch�����ж�
        TimerIntEnable(TIMER1_BASE, TIMER_CAPA_MATCH);
        // NVIC������ʱ��Aģ���ж�
        IntEnable(INT_TIMER1A);
        // ������׽ģ��
        TimerEnable(TIMER1_BASE, TIMER_A);
        while(1)
        {
               SysCtlDelay(SysCtlClockGet()/3);
               //���ڴ�ӡ����ǰ����ֵ
               UARTprintf("count:%d\n",TimerValueGet(TIMER1_BASE,TIMER_A));
        }
}


void UART0Configure()
{
    //ʹ��UART0��GPIOF
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //��PA0��PA1�������óɴ�������
    GPIOPinConfigure(GPIO_PA0_U0RX);//RX=GPIO_PIN_0
    GPIOPinConfigure(GPIO_PA1_U0TX);//TX=GPIO_PIN_1
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    //���ô��ڲ���ʱ��Դ
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    //UART��printf��������
    UARTStdioConfig(0, 115200, 16000000);

}

void Timer1AIntHandler()
{
    uint32_t ulstatus;
    // ��ȡ�жϱ�־λ
    ulstatus = TimerIntStatus(TIMER1_BASE, TIMER_CAPA_MATCH);
    // ����жϱ�־λ
    TimerIntClear(TIMER1_BASE, ulstatus);
    //���¿����жϣ��ڼӼ���ģʽ�¿���ʡ�ԣ������ڼ�����ģʽ�£�һ��Ҫ����������ģʽ�´����жϺ��ֱ�ӹر��жϣ���Ҫ�ֶ����¿���
    TimerEnable(TIMER1_BASE, TIMER_A);
}
