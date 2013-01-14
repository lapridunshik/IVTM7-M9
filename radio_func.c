#include "define.h"
/*
void Transmit1(U8 data, S8 n)
{
      P3OUT &= ~(BIT0+BIT1+BIT2); 
      P3OUT |= BIT1;                        // Pulse LED during Transmit   
      
      ReceiveOff();
      receiving = 0; 
      *((pInt8U)&TxBuffer[n])=data;
      Transmit( (unsigned char*)TxBuffer, sizeof TxBuffer);         
      transmitting = 1;
      //Flags.radio = 0;     
}

void Receive1(void)
{
      ReceiveOn();
      receiving = 1; 
}

U8 radio_search(t_poll)
{
 
}
*/



extern RF_SETTINGS rfSettings;

unsigned int i = 0; 

S16 RSSI_read(void)
{
    U8 rssi_dec;
    S16 rssi_dBm;
    U8 rssi_offset = 74;
    rssi_dec = ReadSingleReg( RSSI );
    if (rssi_dec >= 128)
      rssi_dBm = (S16)((S16)( rssi_dec - 256) / 2) - rssi_offset;
    else
      rssi_dBm = (rssi_dec / 2) - rssi_offset;
    return rssi_dBm;
}

 void change_t_poll(U16 new_t_poll) //function changing polling time
{
  if(new_t_poll<64)  
  {
    new_t_poll*=1024;
    WriteSingleReg(WOREVT0, *((pInt8U)&new_t_poll));
    WriteSingleReg(WOREVT1, *((pInt8U)&new_t_poll+1));
    WriteSingleReg(WORCTRL, 0x70);
  }
     else  if(new_t_poll<2040)
       { 
          new_t_poll*=32;
          WriteSingleReg(WOREVT0, *((pInt8U)&new_t_poll));
          WriteSingleReg(WOREVT1, *((pInt8U)&new_t_poll+1));
          WriteSingleReg(WORCTRL, 0x71);
       }
          else  if(new_t_poll<65520)
          {
             // new_t_poll*=32;
              WriteSingleReg(WOREVT0, *((pInt8U)&new_t_poll));
              WriteSingleReg(WOREVT1, *((pInt8U)&new_t_poll+1));
              WriteSingleReg(WORCTRL, 0x72);
          }
  /*
              else if(new_t_poll<65520)
              {
                WriteSingleReg(WOREVT0, *((pInt8U)&new_t_poll));
                WriteSingleReg(WOREVT1, *((pInt8U)&new_t_poll+1));
                WriteSingleReg(WORCTRL, 0x73);
              }
         */
  }

void t_poll_tuner(S32 tuned_t_poll) //function tuning polling time
{
	U32 changed_t_poll;
	
	changed_t_poll = T_poll*1024 + tuned_t_poll;
	if ((T_poll < 64) && (changed_t_poll < 65536))
	{       
                WriteSingleReg(WORCTRL, 0xF0);
                WriteSingleReg(WOREVT0, *((pInt8U)&changed_t_poll));
                WriteSingleReg(WOREVT1, *((pInt8U)&changed_t_poll+1));
                WriteSingleReg(WORCTRL, 0x70);
		return;
	}
	
	changed_t_poll = T_poll*32 + tuned_t_poll/32;
	if ((T_poll < 2040) && (changed_t_poll < 65536))
	{
                WriteSingleReg(WOREVT0, *((pInt8U)&changed_t_poll));
                WriteSingleReg(WOREVT1, *((pInt8U)&changed_t_poll+1));
		WriteSingleReg(WORCTRL, 0x71);
		return;
	}
	
	changed_t_poll = T_poll + tuned_t_poll/2040;
	if ((T_poll < 65536) && (changed_t_poll < 65536))
	{
                WriteSingleReg(WOREVT0, *((pInt8U)&changed_t_poll));
                WriteSingleReg(WOREVT1, *((pInt8U)&changed_t_poll+1));
		WriteSingleReg(WORCTRL, 0x72);
		return;
	}
	/*
	changed_t_poll = T_poll + tuned_t_poll/1024;
	if ((T_poll < 65520) && (changed_t_poll < 65536))
	{
                WriteSingleReg(WOREVT0, *((pInt8U)&changed_t_poll));
                WriteSingleReg(WOREVT1, *((pInt8U)&changed_t_poll+1));
		WriteSingleReg(WORCTRL, 0x73);
		return;
	}
        */
        return;
}


void work_on(void)   //enable WOR interrupts
    {
  RF1AIFG &= 0;                         
  RF1AIE = BITE+BITF;
  change_t_poll(T_poll);
  WriteSingleReg(MCSM2 , 0x00); //max RX termination time
    }

void InitRadio(void)
{
  // Set the High-Power Mode Request Enable bit so LPM3 can be entered
  // with active radio enabled 
  PMMCTL0_H = 0xA5;
  PMMCTL0_L |= PMMHPMRE_L; 
  PMMCTL0_H = 0x00; 
  
  WriteRfSettings(&rfSettings);
  
  WriteSinglePATable(PATABLE_VAL);
  
}


void Transmit(unsigned char *buffer, unsigned char length)
{
  RF1AIES |= BIT9;                          
  RF1AIFG &= ~(BITB + BIT9);                         // Clear pending interrupts
  RF1AIE |= BIT9;                           // Enable TX end-of-packet interrupt
  
  WriteBurstReg(RF_TXFIFOWR, buffer, length);     
  
  Strobe( RF_STX );                         // Strobe STX   

}

void ReceiveOn(void)
{   

  RF1AIES |= BIT9;                          // Falling edge of RFIFG9
  RF1AIFG &= ~(BITB + BIT9);                         // Clear a pending interrupt
  RF1AIE  |= BITB + BIT9;                          // Enable the interrupt 

  // Radio is in IDLE following a TX, so strobe SRX to enter Receive Mode

  while( !(RF1AIFCTL1 & RFINSTRIFG));
  Strobe( RF_SRX );
  /*
  U8 gg;
  __delay_cycles(1850);
    gg=ReadSingleReg(MARCSTATE);
  __no_operation(); 
  */
  
}

void ReceiveOff(void)
{
  RF1AIE &= ~(BITB + BIT9);                          // Disable RX interrupts
  RF1AIFG &= ~(BITB + BIT9);                         // Clear pending IFG
 // P3OUT &= ~BIT2;
  // It is possible that ReceiveOff is called while radio is receiving a packet.
  // Therefore, it is necessary to flush the RX FIFO after issuing IDLE strobe 
  // such that the RXFIFO is empty prior to receiving a packet.

  Strobe( RF_SIDLE );
  Strobe( RF_SFRX  );   
  
  }

// *****************************************************************************
// @fn          ResetRadioCore
// @brief       Reset the radio core using RF_SRES command
// @param       none
// @return      none
// *****************************************************************************
void ResetRadioCore (void)
{
  Strobe(RF_SRES);                          // Reset the Radio Core
  Strobe(RF_SNOP);                          // Reset Radio Pointer
}
