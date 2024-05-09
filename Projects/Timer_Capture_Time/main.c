

/**
 * main.c
 * ��������Բⶨ�ź����ڲ�ͨ�����ڴ�ӡ�������źŴ�PF2����
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
        //����ϵͳʱ�ӣ�ʹ���ⲿ��������PLL����Ƶϵ��4��ϵͳʱ��50Mhz
        SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
        // ����Timer1ģ��
        SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
        // ����GPIO_F��Ϊ���岶׽��
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
        // ����GPIO��Ϊʹ��Timer1��׽ģʽ
        GPIOPinConfigure(GPIO_PF2_T1CCP0);
        GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_2);
        // Ϊ�ܽ�����������ģʽ
        GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
        //���ô���
        UART0Configure();
        // ����ʹ��Timer1��TimerAģ��Ϊ�ش����Ӽ���ģʽ,TimerBģ��Ϊ�������¼���ģʽ
        TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME_UP|TIMER_CFG_B_PERIODIC);
        // ʹ���½��ش���
        TimerControlEvent(TIMER1_BASE, TIMER_A, TIMER_EVENT_NEG_EDGE);
        // ����TimerA������ΧΪ0~49999��TimerB������ΧΪ49999~0
        TimerLoadSet(TIMER1_BASE, TIMER_A, 49999);
        TimerLoadSet(TIMER1_BASE, TIMER_B, 49999);
        // ע���жϴ���������Ӧ�����¼�
        TimerIntRegister(TIMER1_BASE, TIMER_A, Timer1AIntHandler);
        TimerIntRegister(TIMER1_BASE, TIMER_B, Timer1BIntHandler);
        // ϵͳ���жϿ�
        IntMasterEnable();
        // ʱ���ж���������TimerA Captureģʽ�в����ź�ָ�����غ�TimerB��ʱʱ�����ж�
        TimerIntEnable(TIMER1_BASE, TIMER_CAPA_EVENT|TIMER_TIMB_TIMEOUT);
        //�趨�ж����ȼ�������ԽС���ȼ�Խ��
        IntPrioritySet(INT_TIMER1A,  1);
        IntPrioritySet(INT_TIMER1B,  0);
        // NVIC������ʱ��A��Bģ���ж�
        IntEnable(INT_TIMER1A);
        IntEnable(INT_TIMER1B);
        // ������ʱ��
        TimerEnable(TIMER1_BASE, TIMER_A|TIMER_B);
        while(1)
        {
            //��ʱ0.1s
            SysCtlDelay(SysCtlClockGet()*0.1/3);
            //��ӡ���źŵ����ڣ���λΪms
            UARTprintf("T:%d\n",T/50000);
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
    uint32_t status;
    // ��ȡ�жϱ�־λ
    status = TimerIntStatus(TIMER1_BASE, true);
    // ����жϱ�־λ
    now_count=TimerValueGet(TIMER1_BASE,TIMER_A)+cnt*50000;
    T=now_count-last_count;
    last_count=now_count;
    TimerIntClear(TIMER1_BASE, status);
}

void Timer1BIntHandler()
{
    uint32_t status;
    // ��ȡ�жϱ�־λ
    status = TimerIntStatus(TIMER1_BASE, true);
    //��ʱ�������cnt++
    cnt++;
    // ����жϱ�־λ
    TimerIntClear(TIMER1_BASE, status);
}
