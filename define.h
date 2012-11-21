
//sss111
#include  <msp430.h>
#include  "msp430_comm.h"
#include  "short_types.h"
#include  "sd.h"
#include  "limits.h"
#include  "math.h"
#include  "stdio.h"
#include  "stdlib.h"
#include  "stdarg.h"
#include  "icclbutl.h"

// macroses
#define POW_LCD_OFF             P2OUT &= ~BIT6
#define POW_LCD_ON              P2OUT |=  BIT6
#define CS_LCD_LOW              P2OUT &= ~BIT2
#define CS_LCD_HIGH             P2OUT |=  BIT2
#define LCD_CLK                 { P1OUT |= BIT4; P1OUT |= BIT4; P1OUT &= ~BIT4; P1OUT &= ~BIT4; }
#define LCD_ONE                 P1OUT |=  BIT3
#define LCD_ZERO                P1OUT &= ~BIT3
#define LCD_off                 { LCD_send_com(0x00); CS_LCD_LOW; POW_LCD_OFF; }

// lcd modes
#define OFF_MODE              0
#define START_MODE            1
#define TH_SHOW               2
#define PH_SHOW               3
#define SD_SHOW               4

#define LIGHT_BAT3              HT1621_Buffer[14] |= 0x01
#define LIGHT_BAT2              HT1621_Buffer[13] |= 0x01
#define LIGHT_BAT1              HT1621_Buffer[13] |= 0x10
#define LIGHT_BAT0              HT1621_Buffer[11] |= 0x10

#define LIGHT_oC_UP             HT1621_Buffer[10] |= 0x01
#define LIGHT_oC_LO             HT1621_Buffer[0]  |= 0x80
#define LIGHT_SD                HT1621_Buffer[13] |= 0x02
#define LIGHT_USB               HT1621_Buffer[12] |= 0x10
#define LIGHT_RF                HT1621_Buffer[12] |= 0x01
#define LIGHT_k_UP              HT1621_Buffer[11] |= 0x20
#define LIGHT_Kdm2              HT1621_Buffer[11] |= 0x01
#define LIGHT_mmptst            HT1621_Buffer[10] |= 0x10
#define LIGHT_gPa               HT1621_Buffer[9]  |= 0x10
#define LIGHT_ms                HT1621_Buffer[8]  |= 0x10
#define LIGHT_k_LO              HT1621_Buffer[8]  |= 0x20
#define LIGHT_Lk                HT1621_Buffer[10] |= 0x02
#define LIGHT_pr                HT1621_Buffer[0]  |= 0x10
#define LIGHT_ppm               HT1621_Buffer[0]  |= 0x01
#define LIGHT_m                 HT1621_Buffer[0]  |= 0x04
#define LIGHT_gm3               HT1621_Buffer[0]  |= 0x02
#define LIGHT_tr                HT1621_Buffer[0]  |= 0x08

#define LIGHT_BP_UP             HT1621_Buffer[14] |= 0x10
#define LIGHT_HP_UP             HT1621_Buffer[15] |= 0x08
#define LIGHT_BP_LO             HT1621_Buffer[15] |= 0x02
#define LIGHT_HP_LO             HT1621_Buffer[15] |= 0x10




// config's bits
#define   SPEED_MASK        BIT0
#define   S115200           0
#define   S9600             BIT0
#define   S19200            BIT1
#define   S38400            (BIT0+BIT1)

#define RXI_ENABLE              UCA0IE|=   UCRXIE
#define RXI_DISABLE             UCA0IE&=  ~UCRXIE
#define TXI_DISABLE             UCA0IE&=  ~UCTXIE
#define TX_START                { UCA0STAT=  UCLISTEN; UCA0IFG|= UCTXIFG; UCA0IE|= UCTXIE; }
#define WDRCLR                  WDTCTL=  WDTPW + WDTSSEL__VLO + WDTCNTCL + WDTIS__8192
#define SOUND_OFF               TA0CCTL3=  OUTMOD_0 + OUT
#define SOUND_ON                {P3SEL= 0; P3OUT= 0x27;}

#define SHIFT_BYTE_LCD(A,B)     { UCB0TXBUF= (A); while(UCB0STAT & UCBUSY); (B)= UCB0RXBUF; }

#define MAX_DATA_LEN          128
#define UART_BUF_LEN          272 
#define RAMSIZE               1536
#define RamBeginAddr          (Int16U)&Seconds

#define SERVICE_ADR           0xFFFF

#define PMM_STATUS_OK         0
#define PMM_STATUS_ERROR      1


#define  PACKET_LEN         (0x05)			// PACKET_LEN <= 61
#define  RSSI_IDX           (PACKET_LEN)    // Index of appended RSSI 
#define  CRC_LQI_IDX        (PACKET_LEN+1)  // Index of appended LQI, checksum
#define  CRC_OK             (BIT7)          // CRC_OK bit 
#define  PATABLE_VAL        (0x51)          // 0 dBm output 


typedef struct 
{ 
   Int16U    systick:       1;   
   Int16U    lcd_update:    1;
   Int16U    uart_recieve:  1;
   Int16U    uart_end:      1;
   Int16U    radio:         1;
   
} Flags_t;

typedef struct S_RF_SETTINGS {
    unsigned char fsctrl1;   // Frequency synthesizer control.
    unsigned char fsctrl0;   // Frequency synthesizer control.
    unsigned char freq2;     // Frequency control word, high byte.
    unsigned char freq1;     // Frequency control word, middle byte.
    unsigned char freq0;     // Frequency control word, low byte.
    unsigned char mdmcfg4;   // Modem configuration.
    unsigned char mdmcfg3;   // Modem configuration.
    unsigned char mdmcfg2;   // Modem configuration.
    unsigned char mdmcfg1;   // Modem configuration.
    unsigned char mdmcfg0;   // Modem configuration.
    unsigned char channr;    // Channel number.
    unsigned char deviatn;   // Modem deviation setting (when FSK modulation is enabled).
    unsigned char frend1;    // Front end RX configuration.
    unsigned char frend0;    // Front end RX configuration.
    unsigned char mcsm0;     // Main Radio Control State Machine configuration.
    unsigned char foccfg;    // Frequency Offset Compensation Configuration.
    unsigned char bscfg;     // Bit synchronization Configuration.
    unsigned char agcctrl2;  // AGC control.
    unsigned char agcctrl1;  // AGC control.
    unsigned char agcctrl0;  // AGC control.
    unsigned char fscal3;    // Frequency synthesizer calibration.
    unsigned char fscal2;    // Frequency synthesizer calibration.
    unsigned char fscal1;    // Frequency synthesizer calibration.
    unsigned char fscal0;    // Frequency synthesizer calibration.
    unsigned char fstest;    // Frequency synthesizer calibration control
    unsigned char test2;     // Various test settings.
    unsigned char test1;     // Various test settings.
    unsigned char test0;     // Various test settings.
    unsigned char fifothr;   // RXFIFO and TXFIFO thresholds.
 //   unsigned char iocfg2;    // GDO2 output pin configuration
 //   unsigned char iocfg0;    // GDO0 output pin configuration
    unsigned char pktctrl1;  // Packet automation control.
    unsigned char pktctrl0;  // Packet automation control.
    unsigned char addr;      // Device address.
    unsigned char pktlen;    // Packet length.
} RF_SETTINGS;


#include "global_declaration.h"