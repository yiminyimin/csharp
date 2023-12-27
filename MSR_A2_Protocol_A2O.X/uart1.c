/* 
 * File:   uart1.c
 * Author: YIMIN.LIANG
 *
 * Created on 2020?11?26?, ?? 1:59
 */

#include "xc.h"
#include "libpic30.h"
#include "uart1.h"

void UART1_Initialize();
void UART_Process_NoCase();

//unsigned char ENC_TYPE;
unsigned char SF = 0x02;
unsigned char ENC_WARN = 0x00;
unsigned char ENC_ERR = 0x00;
// Motor and Enocoder ID
unsigned char EEPROM_RAM[40] = 
  { 'A','2','0','L','S','B','7','1','W','1','6','1','7','0','0','4','6',' ', ' ', ' ',
    //'M','S','R','-','F','S','-','L','E','1','1','C','1','0','A','A','0','0', ' ', ' '};  //Motor
    'C', '1', '0', '6', '0', '4','E','S','T','0','2','2','0','7','4','4',' ',' ', ' ', ' '};  //Motor
unsigned char index_ee;
unsigned int data_int16;
unsigned int idx_int16;
unsigned char Set_value = 0;
unsigned char Set_function = 0;
unsigned int Enablekey1;
unsigned int COMM_Write_key;
unsigned int NVM_KEY_0;
unsigned int NVM_KEY_1;


signed long Output_value_A2OPEN;
signed long Tc = 25;


int flash_to_RAM[256] = {
0x4E41,0x3338,0x3549,0x3838,0x5730,0x3030,0x3030,0x3030,0x3030,0xFFFF,			// Encoder bar-code  index  0 ~  9, default value : NA385I88W00000000
0x3030,0x3030,0x3030,0x3030,0x3030,0x3030,0x3030,0x3030,0x3030,0xFFFF,              // Motor bar-code    index 10 ~ 19

0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, 
//ENC_INT_TYPE,
        0x0000,         // index 239(0xEF), 
                        //   0=AR3 Encoder, 1=AG3 Encoder, 2=AM3 Encoder, 3=?? Encoder
                        // 256=Linear Encoder
// BAT_COR  , BAT_LOW_CHK,   ENC_EA   , Z_CNT_SFT  ,            ,  Z_OFF_HI  ,  Z_OFF_LO  , T1_WAIT_CNT(190)
    0x0275,     0x0000,      0x0000,      0x0000,     0x0000,       0x0000,      0x0000,      0x00BE,   // index 240 ~ 247(0xF0 ~ 0xF7)
//          ,            ,LED_ON_LEVEL,            , Z_POS_CRC1 ,  Z_POS_HI  ,  Z_POS_LO  ,  FUN_flg1
    0x0000,     0x0000,      0x0000,      0x0000,     0x0000,       0x0000,      0x0000,      0x0000    // index 248 ~ 255(0xF8 ~ 0xFF)
};
int flash_to_RAM_addr;
int flash_addrOffset;
int flash_to_RAM_addr2;


// UART_Process() Status
#define WAIT_CMD       (1)
#define CHECK_CMD      (2)
#define WAIT_RX_FINISH (3)
#define REPLY          (4)
#define WAIT_TX_FINISH (5)

unsigned char RX_Char, CRC;
unsigned char UARTRx[30];
unsigned char UARTTx[30];
unsigned char UARTRxCnt=0;
unsigned char UARTTxCnt=0;
unsigned char TotalTxCnt=0;
unsigned char TotalRxByte=0;
unsigned char DecodeSts=WAIT_CMD; // UART_Process() Status
unsigned IDcmd, IDIndex, IDCheck; // UART_Process() Status
unsigned int UART_RX_TimeOutCnt=0;

#if 1
//A2 comunication protocal Command
#define CMD0  (0x00)
#define CMD1  (0xD1)
#define CMD2  (0x32)
#define CMD3  (0xE3)
#define CMD4  (0x64)
#define CMD5  (0xB5)
#define CMD6  (0x56)
#define CMD7  (0x87)
#define CMD8  (0xC8)
#define CMD9  (0x19)
#define CMDA  (0xFA)
#define CMDB  (0x2B)
#define CMDC  (0xAC)
#define CMDD  (0x7D)
#define CMDE  (0x9E)
#define CMDF  (0x4F)
#define S_CMD1            (0x91)          // S_CMD1
#define S_CMD2            (0xB2)          // S_CMD2
#define S_CMD3            (0x23)          // S_CMD3
#define S_CMD4            (0xF4)          // S_CMD4
#define S_CMD5            (0x65)          // S_CMD5
#define S_CMD6            (0x46)          // S_CMD6 ??????Program Memory Page(1 page=256 words)
#define S_CMD7            (0xD7)          // S_CMD7 ??CH1????Ram Address
#define S_CMD8            (0x78)          // S_CMD8 ??CH2????Ram Address
#define S_CMD9            (0xE9)          // S_CMD9 ????????
#define S_CMDA            (0xCA)          // S_CMDA ??????????
#define S_CMDB            (0x5B)          // S_CMDB ??????????
#define S_CMDC            (0x8C)          // S_CMDB ??????????
#define S_CMDD            (0x1D)          // S_CMDB ??????????
#define S_CMDE            (0x3E)          // S_CMDB ??????????
#define S_CMDF            (0xAF)          // S_CMDF ??????
#else//A2O
#define CMD0  (0x00)
#define CMD1  (0x91)
#define CMD2  (0xB2)
#define CMD3  (0x23)
#define CMD4  (0xF4)
#define CMD5  (0x65)
#define CMD6  (0x46)
#define CMD7  (0xD7)
#define CMD8  (0x78)
#define CMD9  (0xE9)
#define CMDA  (0xCA)
#define CMDB  (0x5B)
#define CMDC  (0x8C)
#define CMDD  (0x1D)
#define CMDE  (0x3E)
#define CMDF  (0xAF)
#define S_CMD1      0xD1//(0x91)          // S_CMD1
#define S_CMD2      0x32//(0xB2)          // S_CMD2
#define S_CMD3      0xE3//(0x23)          // S_CMD3
#define S_CMD4      0x64//(0xF4)          // S_CMD4
#define S_CMD5      0xB5//(0x65)          // S_CMD5
#define S_CMD6      0x56//(0x46)          // S_CMD6 ??????Program Memory Page(1 page=256 words)
#define S_CMD7      0x87//(0xD7)          // S_CMD7 ??CH1????Ram Address
#define S_CMD8      0xC8//(0x78)          // S_CMD8 ??CH2????Ram Address
#define S_CMD9      0x19//(0xE9)          // S_CMD9 ????????
#define S_CMDA      0xFA//(0xCA)          // S_CMDA ??????????
#define S_CMDB      0x2B//(0x5B)          // S_CMDB ??????????
#define S_CMDC      0xAC//(0xAF)          // S_CMDF ??????
#define S_CMDD      0x7D//(0xAF)          // S_CMDF ??????
#define S_CMDE      0x9E//(0xAF)          // S_CMDF ??????
#define S_CMDF      0x4F//(0xAF)          // S_CMDF ??????
#endif

// Table for checking command ID                      
unsigned char IDtable[16]={CMD0, CMD1, CMD2, CMD3, CMD4, CMD5, CMD6, CMD7, CMD8, CMD9, CMDA, CMDB, CMDC, CMDD, CMDE, CMDF};
unsigned char Rxlen[16] = {   0,    1,    1,    1,    1,    1,    1,    1,    5,    5,    5,    5,    5,    0,    1,    0};
unsigned char TxLen[16] = {   0,    5,    5,    5,    5,    5,    5,    5,    4,    4,    4,    4,    4,    0,    5,    0}; 


static const unsigned char crc_table_old[] = { // CRC8 Table, polynomial = X^8 + 1
#if 1//A2 comunication protocal Command
0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
#else//A2O
// CRC8 Table, polynomial = X^8 + X^4 + X^3 + X^2 + 1
0x00,0x1D,0x3A,0x27,0x74,0x69,0x4E,0x53,0xE8,0xF5,0xD2,0xCF,0x9C,0x81,0xA6,0xBB,0xCD,0xD0,0xF7,0xEA,0xB9,0xA4,0x83,0x9E,0x25,0x38,0x1F,0x02,0x51,0x4C,0x6B,0x76,
0x87,0x9A,0xBD,0xA0,0xF3,0xEE,0xC9,0xD4,0x6F,0x72,0x55,0x48,0x1B,0x06,0x21,0x3C,0x4A,0x57,0x70,0x6D,0x3E,0x23,0x04,0x19,0xA2,0xBF,0x98,0x85,0xD6,0xCB,0xEC,0xF1,
0x13,0x0E,0x29,0x34,0x67,0x7A,0x5D,0x40,0xFB,0xE6,0xC1,0xDC,0x8F,0x92,0xB5,0xA8,0xDE,0xC3,0xE4,0xF9,0xAA,0xB7,0x90,0x8D,0x36,0x2B,0x0C,0x11,0x42,0x5F,0x78,0x65,
0x94,0x89,0xAE,0xB3,0xE0,0xFD,0xDA,0xC7,0x7C,0x61,0x46,0x5B,0x08,0x15,0x32,0x2F,0x59,0x44,0x63,0x7E,0x2D,0x30,0x17,0x0A,0xB1,0xAC,0x8B,0x96,0xC5,0xD8,0xFF,0xE2,
0x26,0x3B,0x1C,0x01,0x52,0x4F,0x68,0x75,0xCE,0xD3,0xF4,0xE9,0xBA,0xA7,0x80,0x9D,0xEB,0xF6,0xD1,0xCC,0x9F,0x82,0xA5,0xB8,0x03,0x1E,0x39,0x24,0x77,0x6A,0x4D,0x50,
0xA1,0xBC,0x9B,0x86,0xD5,0xC8,0xEF,0xF2,0x49,0x54,0x73,0x6E,0x3D,0x20,0x07,0x1A,0x6C,0x71,0x56,0x4B,0x18,0x05,0x22,0x3F,0x84,0x99,0xBE,0xA3,0xF0,0xED,0xCA,0xD7,
0x35,0x28,0x0F,0x12,0x41,0x5C,0x7B,0x66,0xDD,0xC0,0xE7,0xFA,0xA9,0xB4,0x93,0x8E,0xF8,0xE5,0xC2,0xDF,0x8C,0x91,0xB6,0xAB,0x10,0x0D,0x2A,0x37,0x64,0x79,0x5E,0x43,
0xB2,0xAF,0x88,0x95,0xC6,0xDB,0xFC,0xE1,0x5A,0x47,0x60,0x7D,0x2E,0x33,0x14,0x09,0x7F,0x62,0x45,0x58,0x0B,0x16,0x31,0x2C,0x97,0x8A,0xAD,0xB0,0xE3,0xFE,0xD9,0xC4
#endif    
};
static const unsigned char br_table[] = {	// bit reverse table
  0x00,0x80,0x40,0xC0,0x20,0xA0,0x60,0xE0,0x10,0x90,0x50,0xD0,0x30,0xB0,0x70,0xF0,0x08,0x88,0x48,0xC8,0x28,0xA8,0x68,0xE8,0x18,0x98,0x58,0xD8,0x38,0xB8,0x78,0xF8,
  0x04,0x84,0x44,0xC4,0x24,0xA4,0x64,0xE4,0x14,0x94,0x54,0xD4,0x34,0xB4,0x74,0xF4,0x0C,0x8C,0x4C,0xCC,0x2C,0xAC,0x6C,0xEC,0x1C,0x9C,0x5C,0xDC,0x3C,0xBC,0x7C,0xFC,
  0x02,0x82,0x42,0xC2,0x22,0xA2,0x62,0xE2,0x12,0x92,0x52,0xD2,0x32,0xB2,0x72,0xF2,0x0A,0x8A,0x4A,0xCA,0x2A,0xAA,0x6A,0xEA,0x1A,0x9A,0x5A,0xDA,0x3A,0xBA,0x7A,0xFA,
  0x06,0x86,0x46,0xC6,0x26,0xA6,0x66,0xE6,0x16,0x96,0x56,0xD6,0x36,0xB6,0x76,0xF6,0x0E,0x8E,0x4E,0xCE,0x2E,0xAE,0x6E,0xEE,0x1E,0x9E,0x5E,0xDE,0x3E,0xBE,0x7E,0xFE,
  0x01,0x81,0x41,0xC1,0x21,0xA1,0x61,0xE1,0x11,0x91,0x51,0xD1,0x31,0xB1,0x71,0xF1,0x09,0x89,0x49,0xC9,0x29,0xA9,0x69,0xE9,0x19,0x99,0x59,0xD9,0x39,0xB9,0x79,0xF9,
  0x05,0x85,0x45,0xC5,0x25,0xA5,0x65,0xE5,0x15,0x95,0x55,0xD5,0x35,0xB5,0x75,0xF5,0x0D,0x8D,0x4D,0xCD,0x2D,0xAD,0x6D,0xED,0x1D,0x9D,0x5D,0xDD,0x3D,0xBD,0x7D,0xFD,
  0x03,0x83,0x43,0xC3,0x23,0xA3,0x63,0xE3,0x13,0x93,0x53,0xD3,0x33,0xB3,0x73,0xF3,0x0B,0x8B,0x4B,0xCB,0x2B,0xAB,0x6B,0xEB,0x1B,0x9B,0x5B,0xDB,0x3B,0xBB,0x7B,0xFB,
  0x07,0x87,0x47,0xC7,0x27,0xA7,0x67,0xE7,0x17,0x97,0x57,0xD7,0x37,0xB7,0x77,0xF7,0x0F,0x8F,0x4F,0xCF,0x2F,0xAF,0x6F,0xEF,0x1F,0x9F,0x5F,0xDF,0x3F,0xBF,0x7F,0xFF
};
void Spell_encoder_crc(void){  //  700ns
    if(UARTRxCnt==0){
        CRC = br_table[crc_table_old[br_table[RX_Char]]];
    }else if((UARTRxCnt>0) && (UARTRxCnt<(TotalRxByte-1))){
        CRC = br_table[crc_table_old[br_table[CRC ^ RX_Char]]];
    }
 /*
        if(UARTRxCnt==0)
		    CRC = RX_Char;
		else
		    CRC = CRC ^ RX_Char;
  */
}
short App_encoder_crc(unsigned int enc_tx_len){  
    int n,temp;
          temp = 0;

          temp = br_table[crc_table_old[br_table[temp ^ IDcmd]]];
     for(n=0;n<(enc_tx_len-1);n++){
          temp = br_table[crc_table_old[br_table[temp ^ UARTTx[n]]]];
      }
/*
          temp = temp ^ IDcmd;
     for(n=0;n<(enc_tx_len-1);n++){
          temp = temp ^ UARTTx[n];
      }
*/
          return temp;    
}  




/* ISR ROUTINE FOR THE TIMER1 INTERRUPT */
void __attribute__((interrupt,no_auto_psv)) _T1Interrupt( void )  // 8kHz
{		
   // T1CONbits.TON = 0;
    TMR1 = 0;     
    
    if(UART_RX_TimeOutCnt > 0) 
        UART_RX_TimeOutCnt++; // A2-OPEN timeout count
    
    if (UART_RX_TimeOutCnt >= TIMER1_8KHZ_0_1SECOND) { // time out
        UART_RX_TimeOutCnt = 0;
        UARTRxCnt = 0;
		RX_Char = U1RXREG;
		IFS0bits.U1RXIF = 0;	// Clear the Recieve Interrupt Flag		  
        RS485_START_RX; 
        DecodeSts = WAIT_CMD;       // return and clear all bytes
    }
    
    if((U1STAbits.RXBKIF == 1) || (U1STAbits.FERR == 1)){    
        //LATBbits.LATB2 = 1;
        UART1_Initialize(); //1.316us, YIMIN test@20230313
        //LATBbits.LATB2 = 0;
        DecodeSts = WAIT_CMD;       // return and clear all bytes
    }
    
        
     IFS0bits.T1IF = 0;
	/* reset Timer 1 interrupt flag */         
}
/*-----------------------*/



void UART1_Initialize()
{
    UARTRxCnt = 0;
    RS485_START_RX; 
    
    IEC0bits.U1TXIE = 0;
    IEC0bits.U1RXIE = 0;

    // URXEN disabled; RXBIMD RXBKIF flag when Break makes low-to-high transition after being low for at least 23/11 bit periods; UARTEN enabled; MOD Asynchronous 8-bit UART; UTXBRK disabled; BRKOVR TX line driven by shifter; UTXEN disabled; USIDL disabled; WAKE disabled; ABAUD disabled; BRGH enabled; 
    // Data Bits = 8; Parity = None; Stop Bits = 1 Stop bit sent, 1 checked at RX;
    U1MODE = (0x8080 & ~(1<<15));  // disabling UART ON bit
    // STSEL 1 Stop bit sent, 1 checked at RX; BCLKMOD disabled; SLPEN disabled; FLO Off; BCLKSEL FOSC/2; C0EN disabled; RUNOVF disabled; UTXINV disabled; URXINV disabled; HALFDPLX disabled; 
    U1MODEH = 0x00;
    // OERIE disabled; RXBKIF disabled; RXBKIE disabled; ABDOVF disabled; OERR disabled; TXCIE disabled; TXCIF disabled; FERIE disabled; TXMTIE disabled; ABDOVE disabled; CERIE disabled; CERIF disabled; PERIE disabled; 
    U1STA = 0x00;
    // URXISEL RX_ONE_WORD; UTXBE enabled; UTXISEL TX_BUF_EMPTY; URXBE enabled; STPMD disabled; TXWRE disabled; 
    U1STAH = 0x22;
    // BaudRate = 9600; Frequency = 4000000 Hz; BRG 103; 
    U1BRG = 0x67;    
    
    // BaudRate = SYSTEM_MIPS / 4*(U1BRG+1) 
    U1BRG = 11;	      //2M bps as A2 encoder at 96MIPS
    U1MODEHbits.STSEL = 0;//1 Stop bits sent, 1 checked at receive

 
    //Make sure to set LAT bit corresponding to TxPin as high before UART initialization
    U1MODEbits.UARTEN = 1;   // enabling UART ON bit
    U1MODEbits.UTXEN = 1;
    U1MODEbits.URXEN = 1;
    
    IFS0bits.U1TXIF = 0;
    IEC0bits.U1TXIE = 1;
    IFS0bits.U1RXIF = 0;
    IEC0bits.U1RXIE = 1;    
    
}

 
void __attribute__ ((interrupt, no_auto_psv)) _U1RXInterrupt(void) {
    //RB14_TOGGLE;
  
///////   Encoder Protocal          /////////////////////////////////////////
        RX_Char = U1RXREG;
#if 0 // old crc calculation
		if(UARTRxCnt==0)
		    CRC = RX_Char;
		else if((UARTRxCnt>0) && (UARTRxCnt<(TotalRxByte-1)))
		    CRC = CRC ^ RX_Char;
#else // using function
        Spell_encoder_crc();
#endif
        UARTRx[UARTRxCnt++] = RX_Char;
		IFS0bits.U1RXIF = 0;
           
    UART_Process_NoCase();    
	//IFS0bits.U1RXIF = 0;
///////  End of Encoder Protocal          /////////////////////////////////////////    
 
}



void __attribute__ ((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
//while(U1STAbits.TRMT==0); 
        UARTTxCnt++; 
        if(UARTTxCnt < TotalTxCnt){      
            U1TXREG = UARTTx[UARTTxCnt] ;  
            // LATBbits.LATB14 ^= 1;  //debug  
        }
        else{       
            while(U1STAbits.TRMT == 0); //1 = Transmit Shift Register (TSR) is empty    
            RS485_START_RX; // relocate for dspic33CK64MP102, YIMIN

            IFS0bits.U1RXIF = 0;	// Clear the Recieve Interrupt Flag
            //IEC0bits.U1RXIE = 1;	// Enable Recieve Interrupts
            DecodeSts = WAIT_CMD;			
        }
        IFS0bits.U1TXIF = 0; 
}




void UART_Process_NoCase() { //A2/A2-OPEN Protocol
    unsigned char addr;

    if(DecodeSts == WAIT_CMD){            
            if (UARTRxCnt <= 0)
                return;//break;
            else {
                DecodeSts = CHECK_CMD;
                //break;
            }
    }
    if(DecodeSts == CHECK_CMD){    
            IDcmd = UARTRx[0];
            IDIndex = IDcmd & 0x0F;
            IDCheck = IDtable[IDIndex];

            TotalRxByte = Rxlen[IDIndex];
            if ((IDcmd != IDCheck) || (TotalRxByte == 0)) {//unknown ID command
                UARTRxCnt = 0;
				RX_Char = U1RXREG;
				IFS0bits.U1RXIF = 0;	// Clear the Recieve Interrupt Flag				
                RS485_START_RX; 
                DecodeSts = WAIT_CMD;
                //break;
            }
            else {
                TotalTxCnt = TxLen[IDIndex];
            }
            if (TotalRxByte == 1) {
                DecodeSts = REPLY;
                //break;
            }
            else {
                UART_RX_TimeOutCnt++;// = 100*8; //start counting down 100ms,  8Khz Timer :0.125ms
                DecodeSts = WAIT_RX_FINISH;                
            }
    }
    if(DecodeSts == WAIT_RX_FINISH){                     
            if (UARTRxCnt == TotalRxByte) { // receive all bytes
                if ((CRC == UARTRx[4]) && (TotalRxByte == 5)){ // ID7~F
                    DecodeSts = REPLY;     // CRC check OK, then go to case REPLY
                }
                else { // unknown ID
                    UARTRxCnt = 0;
					RX_Char = U1RXREG;
					IFS0bits.U1RXIF = 0;	// Clear the Recieve Interrupt Flag					
                    RS485_START_RX; 
                    DecodeSts = WAIT_CMD;  // CRC error, return and clear all bytes
                    return;//break;   // yimin, 20210604
                }
            }
            else
                return;//break;   // still not receive all bytes
    }
    if(DecodeSts == REPLY){      
        UART_RX_TimeOutCnt = 0;

            switch (IDcmd) {
                case CMD1: //ID1,?????????????????
                    //SF = 0x02;
                    UARTTx[0] = ENC_TYPE;//0;
                    UARTTx[1] = 0;
                    UARTTx[2] = FW_VERSION;
                    UARTTx[3] = PROTOCAL;
                    //UARTTx[4] = IDcmd^UARTTx[0]^UARTTx[1]^ UARTTx[2]^ UARTTx[3];
                    UARTTx[TotalTxCnt-1] = App_encoder_crc(TotalTxCnt);
                    break;
                    
                case CMD2: //ID2,????????LATCH??                                    
                    Output_value_A2OPEN = 222222;//123;
                    SF = (SF&0xF0) | 0x02;//last 4 bit is ID_CMD
                    UARTTx[0] = SF;   
                    UARTTx[1] = Output_value_A2OPEN & 0xff;
                    UARTTx[2] = (Output_value_A2OPEN >> 8) & 0xff;
                    UARTTx[3] = (Output_value_A2OPEN >> 16) & 0xff;                      
                    //UARTTx[4] = IDcmd ^ UARTTx[0] ^ UARTTx[1] ^ UARTTx[2] ^ UARTTx[3];                     
                    UARTTx[TotalTxCnt-1] = App_encoder_crc(TotalTxCnt); // 3~5us                         
                    break;         
                    
                case CMD3: //ID3,??LATCH???                                                     
                    Output_value_A2OPEN = 333333;            
                    SF = (SF&0xF0) | 0x03;//last 4 bit is ID_CMD
                    UARTTx[0] = SF;
                    UARTTx[1] = Output_value_A2OPEN & 0xff;
                    UARTTx[2] = (Output_value_A2OPEN >> 8) & 0xff;
                    UARTTx[3] = (Output_value_A2OPEN >> 16) & 0xff;
                    //UARTTx[4] = IDcmd ^ UARTTx[0] ^ UARTTx[1] ^ UARTTx[2] ^ UARTTx[3];
                    UARTTx[4] = App_encoder_crc(TotalTxCnt);
                    break;
                    
                case CMD4: //ID4,???????   
                    SF = (SF&0xF0) | 0x04;//last 4 bit is ID_CMD
                    UARTTx[0] = SF;
                    UARTTx[1] = ENC_ERR & 0xff;
                    UARTTx[2] = ENC_WARN & 0xff;
                    UARTTx[3] = 0;
                    //UARTTx[4] = IDcmd ^ UARTTx[0] ^ UARTTx[1] ^ UARTTx[2] ^ UARTTx[3];
                    UARTTx[TotalTxCnt-1] = App_encoder_crc(TotalTxCnt);
                    break; 
                    
                case CMD5: //ID5,?? Z ???(?? 0~9999 )                                                    
                    Output_value_A2OPEN = 555555;               
                    SF = (SF&0xF0) | 0x05;//last 4 bit is ID_CMD
                    UARTTx[0] = SF;
                    UARTTx[1] = Output_value_A2OPEN & 0xff;
                    UARTTx[2] = (Output_value_A2OPEN >> 8) & 0xff;
                    UARTTx[3] = (Output_value_A2OPEN >> 16) & 0xff;
                    //UARTTx[4] = IDcmd ^ UARTTx[0] ^ UARTTx[1] ^ UARTTx[2] ^ UARTTx[3];
                    UARTTx[TotalTxCnt-1] = App_encoder_crc(TotalTxCnt);
                    break;
                    
                case CMD6: //ID6,??Zero_offset??    
                    Output_value_A2OPEN = 666666;
                    SF = (SF&0xF0) | 0x06;//last 4 bit is ID_CMD
                    UARTTx[0] = SF;
                    UARTTx[1] = Output_value_A2OPEN & 0xff;
                    UARTTx[2] = (Output_value_A2OPEN >> 8) & 0xff;
                    UARTTx[3] = (Output_value_A2OPEN >> 16) & 0xff;
                    //UARTTx[4] = IDcmd ^ UARTTx[0] ^ UARTTx[1] ^ UARTTx[2] ^ UARTTx[3];
                    UARTTx[TotalTxCnt-1] = App_encoder_crc(TotalTxCnt);
                    break;  
                    
                case CMD7: //ID7,?????Data??         					
                    //Output_value_A2OPEN = flash_to_RAM[flash_to_RAM_addr2];
                    //Output_value_A2OPEN = (Output_value_A2OPEN<<16) | flash_to_RAM[flash_to_RAM_addr];
                    UARTTx[0] = (flash_to_RAM[flash_to_RAM_addr]  >> 0) & 0xff; //0x7;
                    UARTTx[1] = (flash_to_RAM[flash_to_RAM_addr]  >> 8) & 0xff;
                    UARTTx[2] = (flash_to_RAM[flash_to_RAM_addr2] >> 0) & 0xff;
                    UARTTx[3] = (flash_to_RAM[flash_to_RAM_addr2] >> 8) & 0xff;
                    //UARTTx[4] = IDcmd ^ UARTTx[0] ^ UARTTx[1] ^ UARTTx[2] ^ UARTTx[3];
                    UARTTx[TotalTxCnt-1] = App_encoder_crc(TotalTxCnt);
                    break;                    

                case CMD8: //ID8,                        
                    //H2.18 = Rx[1]
                    //H2.19 = (Rx[3]<<8) | Rx[2]
                    //H2.20 = 0x08
                    UARTTx[0] = UARTRx[1]; //special cmd
                    UARTTx[1] = UARTRx[2]; //data0
                    UARTTx[2] = UARTRx[3]; //data1 
                    Set_function = UARTRx[2];
                    Set_value = UARTRx[3];
                    if        (UARTRx[1] == S_CMD1) {                       
                    } else if (UARTRx[1] == S_CMD2) {      
                    } else if (UARTRx[1] == S_CMD3) { // clear all error/warn
                        SF = 0x02;
                        ENC_ERR = 0; 
                        ENC_WARN = 0;
                    } else if (UARTRx[1] == S_CMD4) {
                    } else if (UARTRx[1] == S_CMD5) { // enable CMD9 and CMDB to write
                    } else if (UARTRx[1] == S_CMD6) { // for IDB?IDC write/read index(1Byte) 
                        flash_addrOffset = UARTRx[3];
                        flash_addrOffset = (flash_addrOffset<<8) | UARTRx[2];
                    } else if (UARTRx[1] == S_CMD7) { // read DATA0 RAM address
                        flash_to_RAM_addr = UARTRx[3];
                        flash_to_RAM_addr = (flash_to_RAM_addr<<8) | UARTRx[2];                        
                    } else if (UARTRx[1] == S_CMD8) { // read DATA1 RAM address                        
                        flash_to_RAM_addr2 = UARTRx[3];
                        flash_to_RAM_addr2 = (flash_to_RAM_addr2<<8) | UARTRx[2];                        
                    } else if (UARTRx[1] == S_CMD9) { // read Int_part, Yimin 20201217 //0x19//(0xE9)
                        if(Set_function == 0x00){ //TEMP_C
                            UARTTx[1] = Tc;//Read_Value
                            UARTTx[2] = 00;//test
                        }
                    } else if (UARTRx[1] == S_CMDA) { //read HW_CFG
                        UARTTx[1] = HW_CFG0;
                        UARTTx[2] = HW_CFG1;
                    } else if (UARTRx[1] == S_CMDB) {   
                    } else if ((UARTRx[1] == S_CMDC) && (COMM_Write_key == 0x9595)) {
                    } else if (UARTRx[1] == S_CMDF) { //set Int_func, Int_val 
                        if (Set_function == 0x34) { 
                            Nop();
                        //common command    
                        }else if((Set_function == 0xAA) && (Set_value == 0x55)){ Enablekey1 = 0x5555;    NVM_KEY_0 = 0x55;   NVM_KEY_1 = 0xAA;
                        }else if((Set_function == 0x95) && (Set_value == 0x95)){ COMM_Write_key = 0x9595;
                        }
                    }
                    //UARTTx[3] = IDcmd ^ UARTTx[0] ^ UARTTx[1] ^ UARTTx[2];
                    UARTTx[TotalTxCnt-1] = App_encoder_crc(TotalTxCnt);
                    break;
                    
                case CMDA: //ID10,?? Bar code or Motor Parameters
                    //test_x = 0xAAAA;
                    UARTTx[0] = UARTRx[1];
                    index_ee = UARTRx[1];

                    UARTTx[1] = flash_to_RAM[UARTRx[1]] & 0xFF;
                    UARTTx[2] = flash_to_RAM[UARTRx[1]]>>8;
                    
                    if (index_ee == 0x80){
                        UARTTx[1] = FW_VERSION_16BIT & 0xFF;
                        UARTTx[2] = FW_VERSION_16BIT>>8;
                    }else if (index_ee == 0x81){
                        UARTTx[1] = FW_VERSION_SUB;
                        UARTTx[2] = FW_VERSION_SUB>>8;
                    }
                    //UARTTx[3] = IDcmd^UARTTx[0]^UARTTx[1]^UARTTx[2];
                    UARTTx[TotalTxCnt-1] = App_encoder_crc(TotalTxCnt);
                    break;
                case CMDB: //ID11,???????? 
                    //RB14_HIGH;
                    //test_x = 0xBBBB;
                    UARTTx[0] = UARTRx[1];//idx
                    UARTTx[1] = UARTRx[2];//data low
                    UARTTx[2] = UARTRx[3];//data high
                    //UARTTx[3] = IDcmd^UARTTx[0]^UARTTx[1]^ UARTTx[2];
                    UARTTx[TotalTxCnt-1] = App_encoder_crc(TotalTxCnt);
                    if(COMM_Write_key == 0x9595){
                        idx_int16 = UARTRx[1];
                        data_int16 = UARTRx[3];
                        data_int16 = (data_int16<<8) | UARTRx[2];                    
                        flash_to_RAM[(flash_addrOffset*256) + idx_int16] = data_int16;   
                        //IDB_Flag = 1;//PARAMETER_read();//  mapping variables from RAM   // 50us 
                    }
                    //RB14_LOW;
                    break;                    
                default:
                    UARTTx[0] = SF;
                    UARTTx[1] = 0;
                    UARTTx[2] = 0;
                    UARTTx[3] = 0;
                    //UARTTx[4] = IDcmd ^ UARTTx[0] ^ UARTTx[1] ^ UARTTx[2] ^ UARTTx[3];
                    UARTTx[TotalTxCnt-1] = App_encoder_crc(TotalTxCnt);
                    break;
            }

            RS485_START_TX;            
            UARTTxCnt = 0;
            UARTRxCnt = 0;
            U1TXREG = UARTTx[UARTTxCnt];
            DecodeSts = WAIT_CMD;//WAIT_TX_FINISH;

    }
}
