#include "Std_Types.h"
#include "SETTINGS_interface.h"
#include "DIO_interface.h"
#include "UART_interface.h"
#include "TIMER0_interface.h"
#include "INTERRUPTS_interface.h"
#include "HBRIDGE_interface.h"


void vidHandleCommands(void);
void vidDriveForTime(void);
volatile u32 u32OVFCount = 0;
volatile u8 u8Command;
volatile u8 u8CommandFlag = 0;
void main()
{
	HBRIDGE_vidInit();
	/*Initializing UART*/
	UART_vidInit();
	TIMER0_vidInit(TIMER0_WGM_NORMAL,TIMER0_COM_NORMAL,TIMER0_CLK_1);
	INTERRUPTS_vidEnableInterrupt(INTERRUPTS_USART_RXC);
	INTERRUPTS_vidEnableInterrupt(INTERRUPTS_TIMER0_OVF);
	INTERRUPTS_vidPutISRFunction(INTERRUPTS_TIMER0_OVF,vidDriveForTime);
	INTERRUPTS_vidPutISRFunction(INTERRUPTS_USART_RXC,vidHandleCommands);
	INTERRUPTS_vidSetGlobalInterruptFlag();
	while(1);
}


void vidDriveForTime(void)
{	
	if (u8CommandFlag == 1)
	{
		switch(u8Command)
		{
			case 'a':
				HBRIDGE_vidControl(HBRIDGE_BOTH_FORWARD);
				break;
			case 'b':
				HBRIDGE_vidControl(HBRIDGE_BOTH_BACKWARD);
				break;
			case 'c':
				HBRIDGE_vidControl(HBRIDGE_BOTH_STOP);
				break;
			case 'd':
				HBRIDGE_vidControl(HBRIDGE_RIGHT_FORWARD);
				break;
			case 'e':
				HBRIDGE_vidControl(HBRIDGE_LEFT_FORWARD);
				break;
		}	
		u8CommandFlag = 0;
		u32OVFCount = 0;
	}
	u32OVFCount++;
	if (u32OVFCount == 31500)
	{
		u32OVFCount = 0;
		HBRIDGE_vidControl(HBRIDGE_BOTH_STOP);
	}
}

void vidHandleCommands(void)
{
	u8Command = UART_u8GetReceivedByte();
	u8CommandFlag = 1;
}
