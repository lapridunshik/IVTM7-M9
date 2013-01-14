

// predeclaration
extern volatile Flags_t           Flags;

extern Int8U                  UartBuffer[];
extern U8                     Seconds, Minutes, Houres, Days, Monthes, Years, Rclock, Rclock_ext, k;
extern Int16U                 RxCounter, TxPointer, T_poll,  Lcd_Buffer[], rtc_ref;
extern Int32S                 T_poll_tuning;
extern Int16S                 RSSI_Buffer[], Tuner;
extern unsigned char          Strobe(unsigned char strobe);
extern unsigned char          ReadSingleReg(unsigned char addr);
extern U8                     HT1621_Buffer[], LcdPointer, Selector, PowerShow, c;
extern S8                     diff;

extern const Int16U           ConfigWord, NetAdr;
extern const U8               Version[], SerialNumberStr[], MonthSize[];

extern unsigned int           SetVCore         (unsigned char level);
extern unsigned int           SetVCoreUp       (unsigned char level);
extern unsigned int           SetVCoreDown     (unsigned char level);

S16                           RSSI_read(void);

void                          uart_process(void);
void                          date_time(void);
void                          ini(void);
void                          radio(void);
void                          LCD_load_data(void);
void                          LCD_on(void);
void                          LCD_send_com(U8 command);
void                          lcd_process(void); 
void                          Prog_LCD_load_data(void);
void                          change_t_poll(U16 new_t_poll);
void                          ResetRadioCore (void);
void                          WriteRfSettings(RF_SETTINGS *pRfSettings);
void                          WriteSingleReg(unsigned char addr, unsigned char value);
void                          WriteBurstReg(unsigned char addr, unsigned char *buffer, unsigned char count);
void                          ReadBurstReg(unsigned char addr, unsigned char *buffer, unsigned char count);
void                          WriteSinglePATable(unsigned char value);
void                          WriteBurstPATable(unsigned char *buffer, unsigned char count); 
void                          Transmit(unsigned char *buffer, unsigned char length);
void                          ReceiveOn(void);
void                          ReceiveOff(void);
void                          work_on(void);
void                          t_poll_tuner(S32 tuned_t_poll);

void                          InitButtonLeds(void);
void                          InitRadio(void);


extern unsigned char          RxBuffer[PACKET_LEN+2];
extern unsigned char          RxBufferLength;
extern unsigned char          TxBuffer[PACKET_LEN];

extern unsigned char          transmitting; 
extern unsigned char          receiving; 
