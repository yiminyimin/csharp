/* 
 * File:   system.c
 * Author: YIMIN.LIANG
 *
 * Created on 2020?11?26?, ?? 1:59
 */

#include "xc.h"
#include "libpic30.h"
#include "uart1.h"

void PIN_MANAGER_Initialize();
void CLOCK_Initialize();
void INTERRUPT_Initialize();
void Init_Timer1();
void UART1_Initialize(); // in uart1.c 

void SYSTEM_Initialize(void)
{
    PIN_MANAGER_Initialize();
    CLOCK_Initialize();
    INTERRUPT_Initialize();
    Init_Timer1();
    UART1_Initialize(); // in uart1.c  
}

void CLOCK_Initialize(void){
        
RCONbits.SWDTEN=0;            /* Disable Watch Dog Timer*/
    
//code example for 50 MIPS system clock using POSC with 10 MHz external crystal	
//for Force Sensor PCB, 40MIPS with 8MHz external crystal. by YIMIN.2020.11.20
// Configure PLL prescaler, both PLL postscalers, and PLL feedback divider
// 8M*M/(N1*N2*N3) = 8M*160/(1*4*1) = 320M -> 80MIPS, because 1 instruction = 4 cycle
// 8M*M/(N1*N2*N3) = 8M*192/(1*4*1) = 384M -> 96MIPS, because 1 instruction = 4 cycle
CLKDIVbits.PLLPRE = 1; // N1=1,                      1~8
PLLFBDbits.PLLFBDIV = 192;//160;//100; // M = 100,   16~200
PLLDIVbits.POST1DIV = 4;//5; // N2=5,                1~7
PLLDIVbits.POST2DIV = 1; // N3=1                     1~7
// Initiate Clock Switch to Primary Oscillator with PLL (NOSC=0b011)
__builtin_write_OSCCONH(0x03);
__builtin_write_OSCCONL(OSCCON | 0x01);
// Wait for Clock switch to occur
while (OSCCONbits.OSWEN!= 0);
// Wait for PLL to lock
while (OSCCONbits.LOCK!= 1);
    
}
void PIN_MANAGER_Initialize (void)
{
    /****************************************************************************
     * Setting the Output Latch SFR(s)
     ***************************************************************************/
    LATA = 0x0000;
    LATB = 0x0000;

    /****************************************************************************
     * Setting the GPIO Direction SFR(s)
     ***************************************************************************/
    TRISA = 0x001F;
    TRISB = 0xFF7D;

    /****************************************************************************
     * Setting the Weak Pull Up and Weak Pull Down SFR(s)
     ***************************************************************************/
    CNPDA = 0x0000;
    CNPDB = 0x0000;
    CNPUA = 0x0000;
    CNPUB = 0x0000;

    /****************************************************************************
     * Setting the Open Drain SFR(s)
     ***************************************************************************/
    ODCA = 0x0000;
    ODCB = 0x0000;

    /****************************************************************************
     * Setting the Analog/Digital Configuration SFR(s)
     ***************************************************************************/
    ANSELA = 0x001F;
    ANSELB = 0x0205;


    /****************************************************************************
     * Set the PPS
     ***************************************************************************/
    __builtin_write_RPCON(0x0000); // unlock PPS

    RPOR3bits.RP39R = 0x0001;    //RB7->UART1:U1TX
    RPINR18bits.U1RXR = 0x0028;  //RB8->UART1:U1RX
    TRISBbits.TRISB9 =  0;       //RB9->output pin UART_EN           

    __builtin_write_RPCON(0x0800); // lock PPS
}
void INTERRUPT_Initialize (void)
{
        /* Set Timer1 interrupt priority to 7 (level 7 is highest) */
        IPC0bits.T1IP = 7;
        
        /* Set UART1 TX interrupt priority to 7 (level 7 is highest) */
        IPC3bits.U1TXIP = 7; 
        /* Set UART1 RX interrupt priority to 7 (level 7 is highest) */
        IPC2bits.U1RXIP = 7;         
}

void Init_Timer1( void )
{
    T1CON = 0;              // Timer reset
 	IFS0bits.T1IF = 0;      // Reset Timer1 interrupt flag
	//IPC0bits.T1IP = 6;      // Timer1 Interrupt priority level=4
 	IEC0bits.T1IE = 1;      // Enable Timer1 interrupt
 	TMR1=  0x0000;  	

	//PR1 =  5000;             // Timer1 period register = 0.5ms => 40M/5000 = 8k  
    //PR1 =  10000;            // Timer1 period register => 80M/10000 = 8k  
    PR1 =  12000;            // Timer1 period register => 96M/12000 = 8k 
    
    T1CONbits.TON = 1;
}
