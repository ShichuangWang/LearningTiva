
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

int32_t state=1;

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

    while(1)
    {
        //读取GPIO状态，检测按键sw1是否按下
        if(!(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)==GPIO_PIN_4))
        {
            SysCtlDelay(SysCtlClockGet()*0.05/3);//延时消抖
            if(!(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)==GPIO_PIN_4))
                state=state*2;
            if(state>8)
                state=2;
        }
        switch(state)
        {
            //切换点亮的灯
            case 2: GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 2);break;
            case 4: GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_2);break;
            case 8: GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 8);break;
            default:break;
        }

    }

}
