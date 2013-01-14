#include  "define.h"

void  choose_uart_speed(void)
{
  switch(ConfigWord & SPEED_MASK)
  {
    case    S9600:    UCA0BRW=  6;  UCA0MCTL= (13 << 4) + UCOS16; break;
    case    S19200:   UCA0BRW=  3;  UCA0MCTL= (1 << 1) + (6 << 4) + UCOS16; break;
     
    default:         UCA0BRW=  6;  UCA0MCTL= (13 << 4) + UCOS16;          // 9600 
  } 
}

void  ini(void)
{
  __disable_interrupt();
  
  SFRIE1= WDTIE;
  SFRIE1= SFRIFG1= SYSCTL= 0;         // ����� ��������� ����
  SFRRPCR=  BIT2 + BIT3;              // �������� �������� ���
  
    // WDT
  WDTCTL=   WDTPW + WDTSSEL__VLO + WDTCNTCL + WDTIS__32K;    // ~ 2.4c
  
    // PMM
  PMMCTL0=  PMMPW;                    // Vcore = 1,4 �
  SVSMHCTL= SVSHE + SVSHRVL0;         // ��� ������ ���������� �� ����. ����� 2 �
  SVSMLCTL= SVSLE;                    // ��� ������ ���������� �� ����. 1,4 �
  PMMRIE=   SVSHPE + SVSLPE;          // ��� ����� �� ������������ H � L
      // Ports init  
  
  PMAPKEYID=  PMAPKEY;

  PMAPCTL=    0;
  P1MAP2=     PM_UCB0SOMI;
  P1MAP3=     PM_UCB0SIMO;
  P1MAP4=     PM_UCB0CLK;
  P1MAP5=     PM_UCA0RXD;
  P1MAP6=     PM_UCA0TXD;
    
  P2MAP0=     31;
  P2MAP1=     31;
  
  P3MAP4=     PM_TA0CCR3A;
  PMAPKEYID=  0;
  
  P1SEL= 0x7c;
  P1OUT= 0;
  P1IE= P1DS= P1REN= 0;
  P1DIR= ~BIT7;
 
  P2DIR= 0xfc;  
  P2SEL= 0x03;
  P2OUT= 0x04;
  P2IE= P2DS= P2REN= 0;
 
  
 // P3SEL= BIT4;
  P3REN= 0x07;
  P3DIR= 0x37; 
  P3OUT= 0x30;
  P3DS=  0; 

  
  P5SEL=  BIT0 + BIT1;    
  P5OUT=  P5DS= P5REN= 0; 
  P5DIR=  0;
  
  PJOUT= BIT1;
  PJDS= PJREN= 0;
  PJDIR=  255;  
//XT1BYPASS 
  // set UCS
  UCSCTL6=  XCAP0 + XCAP1; //Lowest current consumption. XT2 oscillator operating range is 4 MHz to 8 MHz.

  // ���� ��������� XT1
  while(UCSCTL7 & XT1LFOFFG)  UCSCTL7 &= ~XT1LFOFFG; 
  while(UCSCTL7 & XT2OFFG)  UCSCTL7 &= ~XT2OFFG;
  
  

  
  /*
  if(RFERRIFG)
  {
    char lvl=1;
    while(RF1AIFERR & LVERR)
    {
      SetVCoreUp(lvl);
      lvl++;
      RF1AIFERR &= ~BIT0;
    }
  }
  */
  
  UCSCTL1=  DCORSEL_1;                                        // 0.5...3.5 MHz
  UCSCTL2=  63;                                               // 32,768*(31+1) ~ 2MHz �� ������ FLL
  UCSCTL3=  FLLREFDIV_0 + SELREF__XT1CLK;                     // 32kHz �� ���� FLL

  UCSCTL5=  DIVA__32;                                         // ACLK = XT1/32 = 1024 Hz
  UCSCTL8=  0;                                                // ��������� ������������ �� ����������

   // ���� ��������� DCO
  while(UCSCTL7 & DCOFFG) UCSCTL7 &= ~DCOFFG; 
  SFRIFG1 &= ~OFIFG; 
  
  UCSCTL4=    SELM__DCOCLKDIV + SELS__DCOCLKDIV + SELA__XT1CLK;    // MCLK - FLL(2MHz), SMCLK - FLL(2MHz), ACLK - XT1/32(1024Hz)
  
  // TA0
  TA0CTL=     TASSEL__SMCLK + ID__8 + MC__STOP + TACLR; //SMCLK
  TA0CCTL0=   CCIE;  
  TA0CCR0 = 0x0064;
  
  // TA1 inclk = ACLK (1024Hz)
  TA1CTL=     TASSEL_1 + MC__UP + TACLR + TAIE;  // clk= ACLK ���������� ������������� �� �������
  TA1CCTL0=   TA1CCTL1= TA1CCTL2= 0;
  TA1CCR0=    51; // ������ ����� 0,05�
  
  //UART
/*
  UCA0CTL1=    UCSWRST + UCSSEL1; 
  UCA0CTL0= 0;  
  UCA0STAT=    0x00;                            // enable error symbol set int flag ?
  choose_uart_speed();
  UCA0CTL1=    UCSSEL1;
  UCA0IE =     UCRXIE;
*/
  
  //UART-SPI
  
  UCB0CTL1=   UCSSEL__SMCLK + UCSWRST;  // SMCLK (1MHz)
  UCB0CTL0=   UCSYNC + UCMST + UCCKPH;
  UCB0STAT=   0;
  UCB0BRW=    14;                       // bitrate ~ 150 kbit
 // UCB0MCTL=   0;
  UCB0CTL1=   UCSSEL__SMCLK; 
  
  //Radio
  
 // Increase PMMCOREV level to 2 for proper radio operation
  //__delay_cycles(10000);

          SetVCore(2);                            
  RF1AIFERR &= ~BIT0;
  ResetRadioCore();     
  InitRadio();
  ReceiveOff();
  receiving = 1; 
  transmitting = 0;
 // ������������� ���������� �� �������� RAM  

  if((Seconds > 59) || (Minutes > 59) || (Houres > 23)  || (Days > 31) || (Monthes > 12) || (Years < 12) || (Years > 99) )
  {
    Rclock= Rclock_ext= diff= Seconds= Minutes= Houres= 0;
    Days= Monthes= 4;
    Years = 12;
  }

//SOUND_OFF;
}