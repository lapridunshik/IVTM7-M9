#include "define.h"

void main( void )
{
  ini();
  SOUND_OFF;
  LCD_off;
  WDRCLR;
  __enable_interrupt();
    
  Selector  = START_MODE; 
  LCD_on(); 
  
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
    
    if (Flags.radio)                      // Process a button press->transmit 
    {
      P3OUT &= ~(BIT0+BIT1+BIT2); 
      P3OUT |= BIT1;                        // Pulse LED during Transmit   
      ReceiveOff();
      receiving = 0; 
      *((pInt16U)&TxBuffer[0])=Rclock;
      Transmit( (unsigned char*)TxBuffer, sizeof TxBuffer);         
      transmitting = 1;
      Flags.radio = 0;                        // Re-enable 
    }
  else if(!transmitting)
    {
   //   if(RF1ASTATB==0x7F) Strobe( RF_SFTX ); 
      ReceiveOn();
      receiving = 1; 
    }
   if(Flags.uart_recieve)    uart_process(); 
   if(Flags.lcd_update)      
   {
     lcd_process();
   }
 }
}
