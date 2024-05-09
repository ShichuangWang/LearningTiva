
/**
 * main.c
 */

/*
 * 程序中使用的UART0和板子上的上传程序的USB线是连接在一起的，只需接到电脑上就可以使用这个串口，上传程序后，板载led会闪烁，同时若用串口调试助手向板子发送字符，板子会将发送的字符发送回来
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
//串口中断服务函数
void UART0_Handler();

int main(void)
{
    //配置系统时钟，使用外部晶振驱动PLL，分频系数2.5，系统时钟80Mhz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    //使能外设
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    //配置PF2引脚，这个引脚连接到了板载LED
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_2);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
    //将PA0、PA1引脚配置成串口引脚
    GPIOPinConfigure(GPIO_PA0_U0RX);//RX=GPIO_PIN_0
    GPIOPinConfigure(GPIO_PA1_U0TX);//TX=GPIO_PIN_1
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    //配置串口波特时钟源及参数
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_SYSTEM);
    UARTConfigSetExpClk( UART0_BASE, SysCtlClockGet(),115200,  UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE);
    //使能FIFO并设置FIFO深度
    UARTFIFOEnable(UART0_BASE);
    UARTFIFOLevelSet(UART0_BASE, UART_FIFO_RX1_8,UART_FIFO_TX1_8);
    //配置串口中断
    UARTIntRegister(UART0_BASE, UART0_Handler);
    UARTIntEnable(UART0_BASE, UART_INT_RX|UART_INT_RT);//开启串口接收中断和接收超时中断
    UARTIntClear(UART0_BASE, UART0_BASE);
    //启用串口0的中断并开启处理器中断控制器
    IntEnable(INT_UART0);
    IntMasterEnable();
    //使能串口
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
    //读取中断状态
    uint32_t status=UARTIntStatus(UART0_BASE,true);
    //清除中断标志位
    UARTIntClear(UART0_BASE, status);
    //检查FIFO中是否有字符，若有，读取出来并发送
    while(UARTCharsAvail(UART0_BASE))
    {
        rxbuf=UARTCharGetNonBlocking(UART0_BASE);
        UARTCharPutNonBlocking(UART0_BASE, rxbuf);
    }
}

