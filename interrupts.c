#include  "define.h"

static U16   rtc, rtc_tune;

#pragma vector=WDT_VECTOR
__interrupt void wdt_isr(void)

{
  SFRIFG1 &= ~WDTIFG;
  __no_operation(); 
}


#pragma vector=TIMER0_A0_VECTOR
__interrupt void ta00_isr(void)
{
  Flags.rssi=      1;
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void ta0_isr(void)
{          
    switch(TA0IV)
    
  case 14:
    {
      __no_operation();      
    }
    
}

#pragma vector=TIMER1_A1_VECTOR
__interrupt void ta1_isr(void)
{          
 
 
  
  TA1CTL &= ~TAIFG;

  // RTC 
  rtc += (TA1CCR0 + 1);
  
  
  if(rtc >= 1024)
  {
    rtc -= 1024;
   // if(GenClock1)     GenClock1--; ????????

    Flags.systick=    1;
 //   Flags.radio=      1;
    __low_power_mode_off_on_exit();                  // для сброса WDT и подсчета времени
  }
  
  
  
}
  
#pragma vector=CC1101_VECTOR
__interrupt void CC1101_ISR(void)
{
  
  switch(__even_in_range(RF1AIV,32))        // Prioritizing Radio Core Interrupt 
  {
    case  0: break;                         // No RF core interrupt pending                                            
    case  2: break;                         // RFIFG0 
    case  4:                                // RFIFG1
        RF1AIE &= ~(BIT1+BIT9+BITB);
        Strobe(RF_SWOR);                    // Go back to sleep
      break;

    case  6: break;                         // RFIFG2
    case  8: break;                         // RFIFG3
    case 10: break;                         // RFIFG4
    case 12: break;                         // RFIFG5
    case 14: break;                         // RFIFG6          
    case 16: break;                         // RFIFG7
    case 18: break;                         // RFIFG8
    case 20:                                // RFIFG9
      if(receiving)			    // RX end of packet
      {

        RF1AIE &= ~(BIT1+BIT9+BITB);
        // Read the length byte from the FIFO       
        RxBufferLength = ReadSingleReg( RXBYTES );
        ReadBurstReg(RF_RXFIFORD, RxBuffer, RxBufferLength); 

        if(RxBuffer[5]==TYPE_TUNE)
        {
          Flags.radio_found = 1;
          Strobe(RF_SWOR); 
//          Strobe ( RF_SWORRST );
          work_on();          // WOR interrupts on
          *((pInt8U)&rtc_ref)=RxBuffer[0];  
        }
        
        *((pInt8U)&rtc_tune)=RxBuffer[0];
         Tuner=rtc_ref-rtc_tune+75;
         Flags.radio_tune = 1;
         Flags.radio_online = 1;
         // Stop here to see contents of RxBuffer
        __no_operation(); 		   
        
        // Check the CRC results
        if(RxBuffer[CRC_LQI_IDX] & CRC_OK)  
         k=0;
         c=0;
         receiving = 0;
      /*
        *((pInt8U)&T_poll_new)=RxBuffer[0];
        Seconds = RxBuffer[2];
        Minutes = RxBuffer[3];
        Houres = RxBuffer[4];
        */       
        
       }
  
      else if(transmitting)		    // TX end of packet
      {
        RF1AIE &= ~(BITB + BIT9 + BIT1);                 // Disable TX end-of-packet interrupt
        transmitting = 0;
      }
      else while(1); 			    // trap 
      //__bic_SR_register_on_exit(LPM3_bits);
      break;
    case 22: break;                         // RFIFG10
    case 24:   TA0CTL |= MC__UP + TACLR; break;     // RFIFG11
    case 26: break;                         // RFIFG12
    case 28: break;                         // RFIFG13
    case 30:                                // RFIFG14
      RF1AIE |= BIT9 + BIT1 + BITB;                
      RF1AIFG &= ~(BIT9 + BIT1 + BITB);   	    
      RF1AIES |= BIT9;                      // Falling edge of RFIFG9
      RF1AIFG &= ~BIT9;                     // Clear a pending interrupt
      RF1AIE  |= BIT9;                      // Enable the interrupt      
      Strobe( RF_SRX );      
      break;     
    case 32:                         // RFIFG15
    {
   //   ReceiveOn();
      receiving = 1; 

    }
             break;   
  }  
//  __bic_SR_register_on_exit(LPM3_bits);     
}


#pragma vector = USCI_A0_VECTOR
__interrupt void uart_a0_isr(void)  

{  
  volatile Int8U     inbyte;
   
  switch(UCA0IV)
  {     
    case 2: inbyte=     UCA0RXBUF;         
    
  if(Flags.uart_end)
            {   
             if(inbyte == 0x0d)
              {
              Flags.uart_end= RxCounter=  TxPointer= UCA0STAT= 0;
              }
              break;
            }
    
            UCA0STAT=   0;
            if(!RxCounter && (inbyte != '$'))  break;
            if(inbyte == '$') RxCounter= 0;     
            if(RxCounter == UART_BUF_LEN) 
            { 
              RxCounter= 0; 
              break; 
            }  
            if(inbyte != 0x0d) 
            { 
              UartBuffer[RxCounter++]= inbyte; 
              break;
            }
            
            Flags.uart_recieve=  1;
            RXI_DISABLE;
            __low_power_mode_off_on_exit();
            break;
             
    case 4: if(UartBuffer[TxPointer] == 0x0d)
            {
              UCA0STAT=             UCLISTEN;   // clear error bits
              inbyte=               UCA0RXBUF;  // clear RXI_F
              RXI_ENABLE;
              TXI_DISABLE;
              Flags.uart_end= 1;
            }
            UCA0TXBUF= UartBuffer[TxPointer++];
            break;
  }  
}


  void  date_time(void)
{  
  if(Seconds++ == 59)
  {
    Seconds= 0;
    if(Minutes++ == 59)
    {
       Minutes= 0;
       if(Houres++ == 23)
       {
          Houres= 0;
          if(Days++ == MonthSize[Monthes-1])
          {
             Days= 1;
             if(Monthes++ == 12)
             {
                Monthes= 1;
                if(Years++ == 99) Years= 0;
             }
          }
       }
    }
  }
}
