
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
//按键sw1中断服务函数
void key_handler();
volatile uint16_t led=1;
int main(void)
{
    //配置系统时钟，使用外部晶振驱动PLL，分频系数2.5，系统时钟80Mhz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);       //使能外设

    //配置PF1、PF2、PF3引脚为输出模式
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);

    //配置PF4引脚为输入，推挽模式，内部上拉
    GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_4, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    //配置PF4引脚为外部中断引脚，下降沿触发
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);
    //配置中断服务函数入口
    GPIOIntRegister(GPIO_PORTF_BASE, key_handler);
    //使能GPIOF中断
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4);
    //使能中断控制器的GPIOF中断
    IntEnable(INT_GPIOF);
    //使能处理器中断
    IntMasterEnable();
    //清除中断标志
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
    //配置中断优先级
    IntPrioritySet(INT_GPIOF, 0xE0);

    while(1)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, led);
    }
}
void key_handler()
{
    uint32_t status;
    //获取中断标志位
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
