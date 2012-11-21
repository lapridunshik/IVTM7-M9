

// predeclaration
extern volatile Flags_t           Flags;

extern Int8U                  UartBuffer[];
extern U8                     Seconds, Minutes, Houres, Days, Monthes, Years;
extern Int16U                 RxCounter, TxPointer, Lcd_Buffer[];
extern unsigned char          Strobe(unsigned char strobe);
extern unsigned char          ReadSingleReg(unsigned char addr);
extern U8                     HT1621_Buffer[], LcdPointer, Selector, PowerShow;


extern const Int16U           ConfigWord, NetAdr;
extern const U8               Version[], SerialNumberStr[], MonthSize[];

extern unsigned int           SetVCore         (unsigned char level);
extern unsigned int           SetVCoreUp       (unsigned char level);
extern unsigned int           SetVCoreDown     (unsigned char level);

void                          uart_process(void);
void                          date_time(void);
void                          ini(void);
void                          radio(void);
//test
void LCD_load_data            (void);
void LCD_on                   (void);
void LCD_send_com             (U8 command);
void lcd_process              (void); 
void Prog_LCD_load_data       (void);

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

void                          InitButtonLeds(void);
void                          InitRadio(void);


extern unsigned char          RxBuffer[PACKET_LEN+2];
extern unsigned char          RxBufferLength;
extern unsigned char          TxBuffer[PACKET_LEN];
extern Int16U                 Rclock;

extern unsigned char          transmitting; 
extern unsigned char          receiving; 
