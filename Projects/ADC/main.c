#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_adc.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "utils/uartstdio.h"

unsigned char flag=1;
int value;

/************************************************************************************
 *����������ڴ洢��ADC FIFO�ж�ȡ�����ݡ�������������ʹ�õ���������FIFOһ����
 *�������ʹ������3����FIFO���Ϊ1�������һ������ʹ�ý����FIFO���������������С��
 *
 *********************************************************************************/
 uint32_t pui32ADC0Value[8],sum;

//��ʼ��uart���ڴ�ӡ��ʾ����
void ConfigureUART(void)
{
    // ����UARTʹ�õ�GPIO��Χ�豸
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // ʹ�� UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    // ΪUARTģʽ����GPIO���š�
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // ��ʼ������̨I/O��UART
    UARTStdioConfig(0, 115200, SysCtlClockGet());//0-115200-16M
}

void ADC0Sequence0Handler(void)
{

    uint16_t i;
    // ���ADC�жϱ�־��
    ADCIntClear(ADC0_BASE, 0);
    // ��ȡADCֵ
    ADCSequenceDataGet(ADC0_BASE, 0, pui32ADC0Value);

   for(i = 0;i < 8;i ++)
    {
     sum= sum+(pui32ADC0Value[i]*3300/4096);
    }
    flag=1;
}

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    //��ʼ��LED
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_4);
    GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,0<<4);
    //���ô��ں���
    ConfigureUART();
    //ʹ��ADC0����
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    //ʹ������ADC0��IO��AIN3  PE0.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
   //ѡ��ģ��ADC0���Ź��ܡ�
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0);
    //�������з������Ĳ������� FIFO ���
    //���з����� | ������ | FIFO���
    //   SS0     |   8    |   8
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
   //���ò������еĽ��ġ�
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH3 );
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH3 );
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH3 );
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH3 );
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH3 );
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH3 );
    ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_CH3 );
    //��̵����һ�����ֽ�ʱ ȷ��END bit��λ(ADC_CTL_END�� ���ߵ��²���Ԥ�����
    ADCSequenceStepConfigure(ADC0_BASE, 0, 7, ADC_CTL_CH3 | ADC_CTL_IE | ADC_CTL_END);
    // ����ADC0����0�ж�
    ADCIntRegister(ADC0_BASE, 0,ADC0Sequence0Handler);
    ADCIntEnable(ADC0_BASE, 0);
    IntEnable(INT_ADC0SS0);
    IntMasterEnable();
    // ʹ�ܲ�������
    ADCSequenceEnable(ADC0_BASE, 0);
    //����ж�״̬��־����������Ϊ��ȷ�����жϱ�־�����ǲ���֮ǰ�������
    ADCIntClear(ADC0_BASE, 0);
    while(1)
    {
      //����ж�״̬��־����������Ϊ��ȷ�����жϱ�־�����ǲ���֮ǰ�������
      ADCIntClear(ADC0_BASE, 0);
      // ����ADCת��.
      //Ϊ�������в���һ��������������
      ADCProcessorTrigger(ADC0_BASE, 0);
      if(flag==1)
    {
            value=sum/8;
            UARTprintf("%d\n",value);
            if(value>2600)
    {
                GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,0);  //����
    }
            else
    {
                GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,GPIO_PIN_4);  //����
    }
            flag=0;
            sum=0;
    }
        //��ʱ
        SysCtlDelay(SysCtlClockGet() / 3);
   }

}
