#include "define.h"
//MAIN!
void main( void )
{
  ini();
  //SOUND_OFF;
  LCD_off;
  WDRCLR;
  __enable_interrupt();
  Selector  = START_MODE; 
  LCD_on();
  change_t_poll(T_poll); 
  
  for(;;)
 {
       WDRCLR;
       if(Flags.systick)            
    {
      Flags.lcd_update = 1;
      Flags.systick    = 0;
      Rclock++;
      date_time();
    }

//    __bis_SR_register( LPM3_bits + GIE );   
//    __no_operation(); 
    
    if (Flags.rssi)                     
    {
     RSSI_Buffer[k]=RSSI_read();
     k++;
     TA0CTL |=TACLR;
     Flags.rssi = 0;
     if(k>70)
     {
       k=0;
       TA0CTL= TASSEL__SMCLK + ID__8 + MC__STOP;
     }
    }
    
        if (Flags.radio)       
       {
          P3OUT &= ~(BIT0+BIT1+BIT2); 
          P3OUT |= BIT1;                        // Pulse LED during Transmit   
          ReceiveOff();
          receiving = 0; 
          *((pInt8U)&TxBuffer[0])= T_poll;
          *((pInt8U)&TxBuffer[2])= Seconds;
          *((pInt8U)&TxBuffer[3])= Minutes;
          *((pInt8U)&TxBuffer[4])= Houres;
          *((pInt8U)&TxBuffer[5])= TYPE_TUNE;
          Transmit( (unsigned char*)TxBuffer, sizeof TxBuffer);         
          transmitting = 1;
          Flags.radio = 0; 
          c++;                      
        }
      else if(!transmitting)
        {
          ReceiveOn();
          receiving = 1; 
        }
          
          if(c==10)
          {
            Selector= FAIL;
            c=0;
            ReceiveOff();
            Strobe( RF_SXOFF );
            Flags.radio = 0;
          }

    
    if(Flags.radio_tune)
    {
    t_poll_tuner(Tuner);
    Flags.radio_tune=0;
    }
     
   if(Flags.uart_recieve)    uart_process(); 
   if(Flags.lcd_update)      
   {
     lcd_process();
   }
 }
}
