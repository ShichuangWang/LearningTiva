
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
//����sw1�жϷ�����
void key_handler();
volatile uint16_t led=1;
int main(void)
{
    //����ϵͳʱ�ӣ�ʹ���ⲿ��������PLL����Ƶϵ��2.5��ϵͳʱ��80Mhz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);       //ʹ������

    //����PF1��PF2��PF3����Ϊ���ģʽ
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);

    //����PF4����Ϊ���룬����ģʽ���ڲ�����
    GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_4, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    //����PF4����Ϊ�ⲿ�ж����ţ��½��ش���
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);
    //�����жϷ��������
    GPIOIntRegister(GPIO_PORTF_BASE, key_handler);
    //ʹ��GPIOF�ж�
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4);
    //ʹ���жϿ�������GPIOF�ж�
    IntEnable(INT_GPIOF);
    //ʹ�ܴ������ж�
    IntMasterEnable();
    //����жϱ�־
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
    //�����ж����ȼ�
    IntPrioritySet(INT_GPIOF, 0xE0);

    while(1)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, led);
    }
}
void key_handler()
{
    uint32_t status;
    //��ȡ�жϱ�־λ
    status=GPIOIntStatus(GPIO_PORTF_BASE, true);

    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
    if(status==GPIO_PIN_4)
    {
        if(led<8)
        led=led*2;
        else
        led=2;
    }

}
