/*
 * File:   uart1.h
 * Author: YIMIN.LIANG
 * Check by: YIMIN.LIANG 
 * Created on 20210217,  10:28
 */



// SYSTEM
#define SYSTEM_96MIPS 96000000 //96M
#define WAIT_WHILE_1LINE_5MS 47710 // 47710 = 5ms, 20220330 YIMIN test,accroding to C while(index_i < 50000)index_i++;
#define WAIT_WHILE_1LINE_500US (WAIT_WHILE_1LINE_5MS / 10)
#define WAIT_WHILE_1LINE_50US (WAIT_WHILE_1LINE_500US / 10)

#define TIMER1_8KHZ_1SECOND (8000 * 1)
#define TIMER1_8KHZ_2SECOND (8000 * 2)
#define TIMER1_8KHZ_0_1SECOND (800 * 1)

#define FLASH_PARA_NUM 1024 // 1024 = 1 page


// define RS485 Tx/Rx control pin
#define RS485_START_TX   (LATBbits.LATB9 = 1)
#define RS485_START_RX   (LATBbits.LATB9 = 0)

 

#define ENC_TYPE     32 // abs            (Drive : -79) //YIMIN for One-FirmWare ,20211222

// MSR Firmware Version X.XX.XX.XX
//                      A.BC.CC.DD
// A : MP=1, else=0
// B : phase, D1=1, D3=3, MP=0
// C : FW_VERSION(001~255), Drive(-88)
// D : Field Test, reset after C updated
#define FW_VERSION_A       0 // A : MP=1, else=0
#define FW_VERSION_B       0 // B : phase, D1=1, D3=3, MP=0
#define FW_VERSION       101 // C : FW_VERSION(001~255), Drive(-88)
#define FW_VERSION_16BIT (FW_VERSION_A*10000) + (FW_VERSION_B*1000) + FW_VERSION
#define FW_VERSION_SUB    38 // D : Field Test, reset after C updated
//                  | A2OPEN           
// FW_VERSION       | Drive(-88)       
// FW_VERSION_16BIT | ID_A, idx=0x80   
// FW_VERSION_SUB   | ID_A, idx=0x81  

#define PROTOCAL   16 // A2 Encoder 1.6
#define HW_CFG0  0x03
#define HW_CFG1  0x00
 
// SF
#define WARN_CUR 0x20 //WARN cur
#define ERR_CUR  0x40 //ERR cur
// ERR
#define RESET_ERR  0x02 // AL027 clear at ID1
#define INTER_ERR  0x04 // AL028 Math Error
#define SOURCE_ERR 0x20 // AL033
#define COM_ERR    0x40 // AL034
#define TEMP_ERR   0x80 // AL035
// WARN
#define SOURCE_WARN 0x08
#define TEMP_WARN   0x80


         //-----------------------------------------------------------------------------------------------
         //        Program Memory (dsPIC33CK64MP102) 64KB FLASH, 8KB RAM
         //-----------------------------------------------------------------------------------------------
         //      +------------------------+ 0x000000
         //      |   GOTO  Instruction    |
         //      +------------------------+ 0x000002
         //      |     Reset Address      |
         //      +------------------------+ 0x000004
         //      | Interrupt Vector Table |
         //      |                        | 0x0001FE      
         //      +------------------------+---------------------------------------------------------------------------
         //      |                        | 0x000200, LENGTH = 0x0E00       
         //      |      User Program      | Bootloader area   (1792 word)        
         //      +--    Flash Memory    --+---------------------------------------------------------------------------
         //      |  (22144 Instructions)  | 0x001000, LENGTH = 0x0400        
         //      |                        | APP_IVT          
         //      +--                    --+---------------------------------------------------------------------------
         //      |                        | 0x001400, LENGTH = 0x7BFC        
         //      |                        | APP code Usercode (13310 word)         
         //      +--                    --+---------------------------------------------------------------------------
         //      |                        | 0x008FFC, LENGTH = 0x0004        
         //      |                        | Password          (2 word)             
         //      +--                    --+---------------------------------------------------------------------------
         //      |                        | 0x009000, LENGTH = 0x1000        
         //      |                        | Parameter area    (2048 word)              
         //      +--                    --+---------------------------------------------------------------------------
         //      |                        | 0x00A000, LENGTH = 0x0F00        
         //      |                        | Reserved              
         //      +------------------------+---------------------------------------------------------------------------
         //      |  Device Configuration  | 0x00AF00
         //      |                        |
         //      +------------------------+ 0x00AFFE
         //      |     Unimplemented      | 0x00B000
         //      |      (Read '0's)       |
         //      +------------------------+ 0x7FFFFE
         //-----------------------------------------------------------------------------------------------
        
         //-----------------------------------------------------------------------------------------------
         //        SFR & RAM Memory (unit : bytes)
         //-----------------------------------------------------------------------------------------------
         //                                                                 User define
         //      +-----------+-----------------+ 0x0000                 +-----------------+ 0x000
         //       4Kbyte     |     SFR         |          
         //       SFR Space  |                 |
         //      +-----------+-----------------+ 0x0FFE   
         //                  |     X DATA      | 0x1000 
         //       8Kbyte     |     RAM(4K)     |  
         //       SRAM Space |                 | 
         //                  |                 |          
         //                  +-----------------+ 0x1FFE    
         //                  |     Y DATA      | 0x2000    
         //                  |     RAM(4K)     |       
         //                  |                 |     
         //                  |                 |            
         //      +-----------+-----------------+ 0x2FFE     
         //                                                                        
         //
         //-------------------------------------------------------------------------------------------------