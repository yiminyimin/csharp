/*
 * File:   main.c
 * Author: YIMIN.LIANG
 *
 * Created on 2020?3?4?, ?? 10:28
 */


#include "xc.h"
#include "libpic30.h"
#include "uart1.h"

// Select Internal FRC at POR
//_FOSCSEL(FNOSC_FRC & IESO_OFF);
// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Source Selection (Internal Fast RC (FRC))
#pragma config IESO = OFF               // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)

// Enable Clock Switching and Configure POSC in XT mode
//_FOSC(FCKSM_CSECMD & POSCMD_XT);
//_FOSC(FCKSM_CSECMD & POSCMD_EC & OSCIOFNC_ON);
// FOSC
#pragma config POSCMD = EC              // Primary Oscillator Mode Select bits (EC (External Clock) Mode)
#pragma config OSCIOFNC = ON            // OSC2 Pin Function bit (OSC2 is general purpose digital I/O pin)
#pragma config FCKSM = CSECMD           // Clock Switching Mode bits (Clock switching is enabled,Fail-safe Clock Monitor is disabled)

#pragma config ICS = PGD2               // ICD Communication Channel Select bits (Communicate on PGC2 and PGD2)

//General Segment Code Flash Protection Level bits
//Configuration Segment Code Flash Protection Level bits
//_FSEC(GSS_HIGH & CSS_HIGH); 
//#pragma config GSS = HIGH               // General Segment Code-Protect Level bits (High Security)
//#pragma config CSS = HIGH               // Configuration Segment Code-Protect Level bits (High Security) 


unsigned int index_i;
void SYSTEM_Initialize();


int main(void)
{    
#if 1
//***************     10ms delay for 3.3V stable      ***************//
    index_i = 0;
    while(index_i < WAIT_WHILE_1LINE_5MS)index_i++; // 40000 = 4500us, 20211214 YIMIN test
    index_i = 0;
    while(index_i < WAIT_WHILE_1LINE_5MS)index_i++; // 40000 = 4500us, 20211214 YIMIN test          
//***************       ----------End----------       ***************//    
#endif
//***************     initialize the device           ***************//    
    SYSTEM_Initialize(); //Clock(96MIPS), Pin, ISR, Timer1(8k), UART1(2M,8N1), SPI1(9.6M)                            
//***************       ----------End----------       ***************//          

    while (1)
    {        
        Nop();
    }    
    return 1; 
}




