/**
 * main.c
 * ���źŷ������͵�������������ת���������������A��B���źŷֱ�ӵ�������PD6��PD7���ţ�
 * �򿪴�������ÿ��0.1S�ڴ��������и���һ��QEI0ģ��λ�ü�������ֵ
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/systick.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/qei.h"
#include "utils/uartstdio.h"

//��ʼ��uart���ڴ�ӡ��ʾ����
void ConfigureUART(void)
{
    // ����UARTʹ�õ�GPIO
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // ʹ�� UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    // ΪUART����GPIO���š�
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // ��ʼ������̨I/O��UART
    UARTStdioConfig(0, 115200, SysCtlClockGet());//0-115200-16M
}

int main()
{
    SysCtlClockSet(SYSCTL_SYSDIV_1|SYSCTL_USE_OSC|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);       //ϵͳʱ��16M
    //ʹ��QEI��GPIOD
    SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    //����PD7����
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= 0x80;
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;
    //����GPIO
    GPIOPinTypeQEI(GPIO_PORTD_BASE,GPIO_PIN_6);
    GPIOPinTypeQEI(GPIO_PORTD_BASE,GPIO_PIN_7);
    GPIOPinConfigure(GPIO_PD6_PHA0);//PD6->������A������
    GPIOPinConfigure(GPIO_PD7_PHB0);//PD7->������B������
    //����QEI0ģ�飬A��B�����źű��ؾ������������������������帴λ��������λģʽ��A��B�����źŲ�������������ֵΪ0xffffffff
    QEIConfigure(QEI0_BASE,QEI_CONFIG_CAPTURE_A_B|QEI_CONFIG_NO_RESET|QEI_CONFIG_QUADRATURE|QEI_CONFIG_NO_SWAP,0xffffffff);
    //��ֹQEI0�������ж�
    QEIIntDisable(QEI0_BASE,QEI_INTERROR | QEI_INTDIR | QEI_INTTIMER | QEI_INTINDEX);
    //ʹ��QEI0
    QEIEnable(QEI0_BASE);
    //����QEI0ģ�鵱ǰλ�ü�����ֵΪ0
    QEIPositionSet(QEI0_BASE,0);
    //���ڳ�ʼ��
    ConfigureUART();
    UARTprintf("It is OK!\n");
    while(1)
    {
        //ÿ��0.1S���ն������ǰQEIλ�ü�������ֵ
        SysCtlDelay(0.1*SysCtlClockGet()/3);
        uint32_t pos=QEIPositionGet(QEI0_BASE);
        UARTprintf("%u\n",pos);
    }
}
