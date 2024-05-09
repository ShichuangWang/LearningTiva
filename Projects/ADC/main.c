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
 *这个数组用于存储从ADC FIFO中读取的数据。它必须与正在使用的音序器的FIFO一样大。
 *这个例子使用序列3，其FIFO深度为1。如果另一个序列使用较深的FIFO，则必须更改数组大小。
 *
 *********************************************************************************/
 uint32_t pui32ADC0Value[8],sum;

//初始化uart串口打印显示函数
void ConfigureUART(void)
{
    // 启用UART使用的GPIO外围设备
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // 使能 UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    // 为UART模式配置GPIO引脚。
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // 初始化控制台I/O的UART
    UARTStdioConfig(0, 115200, SysCtlClockGet());//0-115200-16M
}

void ADC0Sequence0Handler(void)
{

    uint16_t i;
    // 清除ADC中断标志。
    ADCIntClear(ADC0_BASE, 0);
    // 读取ADC值
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
    //初始化LED
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_4);
    GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,0<<4);
    //配置串口函数
    ConfigureUART();
    //使能ADC0外设
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    //使能配置ADC0的IO角AIN3  PE0.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
   //选择模拟ADC0引脚功能。
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0);
    //采样序列发生器的采样数和 FIFO 深度
    //序列发生器 | 采样数 | FIFO深度
    //   SS0     |   8    |   8
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
   //配置采样序列的节拍。
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH3 );
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH3 );
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH3 );
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH3 );
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH3 );
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH3 );
    ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_CH3 );
    //编程到最后一个半字节时 确定END bit置位(ADC_CTL_END） 否者导致不可预测错误
    ADCSequenceStepConfigure(ADC0_BASE, 0, 7, ADC_CTL_CH3 | ADC_CTL_IE | ADC_CTL_END);
    // 启用ADC0序列0中断
    ADCIntRegister(ADC0_BASE, 0,ADC0Sequence0Handler);
    ADCIntEnable(ADC0_BASE, 0);
    IntEnable(INT_ADC0SS0);
    IntMasterEnable();
    // 使能采样序列
    ADCSequenceEnable(ADC0_BASE, 0);
    //清除中断状态标志。这样做是为了确保，中断标志在我们采样之前被清除。
    ADCIntClear(ADC0_BASE, 0);
    while(1)
    {
      //清除中断状态标志。这样做是为了确保，中断标志在我们采样之前被清除。
      ADCIntClear(ADC0_BASE, 0);
      // 触发ADC转换.
      //为采样序列产生一个处理器触发。
      ADCProcessorTrigger(ADC0_BASE, 0);
      if(flag==1)
    {
            value=sum/8;
            UARTprintf("%d\n",value);
            if(value>2600)
    {
                GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,0);  //灯灭
    }
            else
    {
                GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,GPIO_PIN_4);  //灯亮
    }
            flag=0;
            sum=0;
    }
        //延时
        SysCtlDelay(SysCtlClockGet() / 3);
   }

}
