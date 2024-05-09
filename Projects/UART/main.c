
/**
 * main.c
 */

/*
 * ������ʹ�õ�UART0�Ͱ����ϵ��ϴ������USB����������һ��ģ�ֻ��ӵ������ϾͿ���ʹ��������ڣ��ϴ�����󣬰���led����˸��ͬʱ���ô��ڵ�����������ӷ����ַ������ӻὫ���͵��ַ����ͻ���
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
#include "utils/uartstdio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
//�����жϷ�����
void UART0_Handler();

int main(void)
{
    //����ϵͳʱ�ӣ�ʹ���ⲿ��������PLL����Ƶϵ��2.5��ϵͳʱ��80Mhz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    //ʹ������
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    //����PF2���ţ�����������ӵ��˰���LED
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_2);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
    //��PA0��PA1�������óɴ�������
    GPIOPinConfigure(GPIO_PA0_U0RX);//RX=GPIO_PIN_0
    GPIOPinConfigure(GPIO_PA1_U0TX);//TX=GPIO_PIN_1
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    //���ô��ڲ���ʱ��Դ������
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_SYSTEM);
    UARTConfigSetExpClk( UART0_BASE, SysCtlClockGet(),115200,  UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE);
    //ʹ��FIFO������FIFO���
    UARTFIFOEnable(UART0_BASE);
    UARTFIFOLevelSet(UART0_BASE, UART_FIFO_RX1_8,UART_FIFO_TX1_8);
    //���ô����ж�
    UARTIntRegister(UART0_BASE, UART0_Handler);
    UARTIntEnable(UART0_BASE, UART_INT_RX|UART_INT_RT);//�������ڽ����жϺͽ��ճ�ʱ�ж�
    UARTIntClear(UART0_BASE, UART0_BASE);
    //���ô���0���жϲ������������жϿ�����
    IntEnable(INT_UART0);
    IntMasterEnable();
    //ʹ�ܴ���
    UARTEnable( UART0_BASE);

    while(1)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
        SysCtlDelay(SysCtlClockGet()*0.5/3);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
        SysCtlDelay(SysCtlClockGet()*0.5/3);
    }

}

void UART0_Handler()
{
    int32_t rxbuf;
    //��ȡ�ж�״̬
    uint32_t status=UARTIntStatus(UART0_BASE,true);
    //����жϱ�־λ
    UARTIntClear(UART0_BASE, status);
    //���FIFO���Ƿ����ַ������У���ȡ����������
    while(UARTCharsAvail(UART0_BASE))
    {
        rxbuf=UARTCharGetNonBlocking(UART0_BASE);
        UARTCharPutNonBlocking(UART0_BASE, rxbuf);
    }
}

