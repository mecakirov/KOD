/*************************************************************
 *
 * COMP: SCIENCE ENERGY
 *
 * Date          : 05.11.2023
 * By            : Selim Öztürk
 * e-mail        : selim@iddef.org
 *
 * */
#include "hardware_configs.h"

//#define BOOT_TEST

unsigned long t_timer = 0;
unsigned long g_timer = 0;
unsigned long g_timerSn = 0;
unsigned short gps_open = 0;
//unsigned int water_count = 0; 

void init_gpio(void);
void init_clock(void);
void timer_init(void);
void init_uart(void);


void hardware_config_init(void){
  
    init_gpio();
    init_clock();
    timer_init();
    init_uart();
}
/*
void debug_print(char*data)
{
    char msg[200];
    snprintf((char*)msg,200,(char*)"%s: %s", (char*)APP_NAME, data );
    gsm_modul_uart_send((char *)msg);
}
*/
void give_boot_message(void)
{
    uint32_t  dt[1];
    dt[0] = BOOT_PSW;
    flash_write_adress(BOOT_MESSAGE_ADRESS, (uint32_t  *)dt , 1);
    __disable_interrupt();
    __no_operation();
    __delay_cycles(1000000);         
               
    //((void ( * )())BOOT_MAIN_ADRES_JUMP)();     

    //((void ( * )())0xF800)(); 
    (*(void (**)(void)) (0xfffe))();
    
    /** works well **/  
    //dt[0] = 0x2d00;
    //flash_write_adress(0xfffe, (uint32_t  *)dt, 1);
    //(*(void (**)(void)) (0xfffe))();
}

//******************************************************************************
// UART SEND         ***********************************************************
//******************************************************************************
void gsm_modul_uart_send(char *pStr)
{
    while (*pStr != 0)
    {
        while (!(IFG2 & UCA0TXIFG));

        UCA0TXBUF = *pStr++;
    }
}

//******************************************************************************
// GSM STATUS         ***********************************************************
//******************************************************************************
bool gsm_modul_get_status(void)
{
    if (P2IN & GSM_STATUS){return 1;}
    else{return 0;}
}


//unsigned char uart_watch[20]; unsigned char count = 0;
//******************************************************************************
// UART INTERRUPT         ******************************************************
//******************************************************************************
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    char data = 0;

    if (IFG2 & UCA0RXIFG)
    { 
        data = UCA0RXBUF;              
        gsm_modul_receive_data(data);
    }

}
 
//******************************************************************************
// TIMER INTERRUPT         *****************************************************
//******************************************************************************
//unsigned int tmr_count = 0;//test
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
    /** GSM TASKS TIMER UPDATE **/
     gsm_modul_timer_update();
     
     rtc_timer_sampler();
   
     //if(tmr_count++ > 1000){__bic_SR_register_on_exit(LPM3_bits);} // test
}

//******************************************************************************
// port1       *****************************************************
//******************************************************************************

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    //water_count++;    
    //__delay_cycles(10); 
    adc_water_adder();
    P1IFG &= ~0x04;                           // P1.5 IFG cleared    
 
}


//******************************************************************************
// DEVICE SOFTWARE RESET         ***********************************************
//******************************************************************************
/*void software_reset_and_record_all_datas(void)
{
// records
    (*(void (**)(void)) (0xfffe))();
}
*/

void clear_all_backup(void)
{  
  uint16_t i;
  for (i = 0; i < 53; i++){
    flash_clear_sector((APP2_ADRESS + (i * 512)));
    __delay_cycles(10);         
  }
 /* uint32_t start_sector , end_sector , flash_read = 0,i=0;
  start_sector = MSP430_FIND_SECTOR(APP2_ADRESS);
  end_sector   = MSP430_FIND_SECTOR(BOOT_MESSAGE_ADRESS);
          
        for(i = start_sector; i < end_sector; i++){
              flash_read = flash_read_adress(MSP430_SELECT_SECTOR(i));
              if(flash_read != 0xFFFFFFFF){
                flash_clear_sector( MSP430_SELECT_SECTOR(i) );
               
                //gsm_modul_uart_send((char *)"boot: sector cleaned\n");  
              } 
              __delay_cycles(10 ); 
          }
        
        flash_read = flash_read_adress( 0xFA00);
        if(flash_read != 0xFFFFFFFF){
          flash_clear_sector( 0xFA00 ); // son sektör
        }*/
}



//******************************************************************************
//******************************************************************************
//******************************************************************************
// HARDWARE CONFIGS        *****************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************



/*
 * Test edilmedi
 * */
/*void sleep_set(sleeps_stds s){
    switch(s){
    case SLEEP_MODE_1: __bis_SR_register(LPM1_bits);  break;
    case SLEEP_MODE_2: __bis_SR_register(LPM2_bits);  break;
    case SLEEP_MODE_3: __bis_SR_register(LPM3_bits);  break;
    case SLEEP_MODE_4: __bis_SR_register(LPM4_bits);  break;
    case SLEEP_CPU_OFF: __bis_SR_register(LPM0_bits);  break;
    }
}*/

/*
 * Test edilmedi, diÄŸer tÃ¼m stadeler eklenmeli , Ã¶rneÄŸin "watchdog clear time"
 * */
/*void watchdog_config(watchdog_stds s){
    switch(s){
    case WDT_CLOSE_STD: WDTCTL = WDTPW | WDTHOLD;  break;
    case WDT_32MS_STD:  WDTCTL = WDT_MRST_32;  break;
    }
}
*/
/*
 * bu fonksiyonun girdisi mcu'nun kristal MHZ'i olmalÄ± config edebilmeliyiz
 * */
void init_clock(void)
{
    DCOCTL = 0;                          // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 |= DIVA_0;                        // ACLK/1 [ACLK/(0:1,1:2,2:4,3:8)]
    BCSCTL2 = 0;                              // SMCLK [SMCLK/(0:1,1:2,2:4,3:8)]
    BCSCTL3 |= LFXT1S_0;                    // LFXT1S0 32768-Hz crystal on LFXT1
}

/*
 * bu fonksiyonun girdisi kaÃ§ ms timer interruptÄ±nÄ± ayarlayacaÄŸÄ±nÄ± belirlemeli
 * */
void timer_init(void)
{
    TACCTL0 = CCIE;                           // TACCR0 interrupt enabled
    TACCR0 = 33;//327;                        // 32768-1 1 sn ------ 32.767 = 33 1 ms
    TACTL = TASSEL_1 | MC_1;                  // ACLK, upmode
}

/**
 * bu fonksiyonun girdisi baundrate olmalÄ± ve br'i ayarlayabilmeliyiz
 * */
void init_uart(void)
{
    UCA0CTL1 |= UCSSEL_1;                     // ACLK
    UCA0BR0 = 3;                              // 32768Hz 9600
    UCA0BR1 = 0;                              // 32768Hz 9600
    UCA0MCTL = UCBRS_3;                       // Modulation UCBRSx = 3
    UCA0CTL1 &= ~UCSWRST;                   // **Initialize USCI state machine**
    IFG2 &= ~(UCA0RXIFG);
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
    __enable_interrupt();
}

void init_gpio(void)
{

    /***********/
    /* PORT P1 */
    /**********/
    // Pin 1.0, GSM DC, output (Active HIGH)
    // Pin 1.1, NC
    // Pin 1.2, PULSE, Input
    // Pin 1.3, NC
    // Pin 1.4, NC
    // Pin 1.5, NC
    // Pin 1.6, NC
    // Pin 1.7, NC
    P1DIR = ~0x0004;//PULSE;                                               // Inputs
    P1DIR |= GSM_DC_EN;     // Output
    P1OUT = 0;                                                 // All P1.x reset
    P1REN &= ~PULSE;                   // disable internal pullup resisitor
    P1IE |= 0x04;                             // P1.4 interrupt enabled
    P1IES |= 0x04;                            // P1.4 Hi/lo edge
    P1IFG &= ~0x04;
        
    /***********/
    /* PORT P2 */
    /**********/
    
    // Pin 2.0, NTC_ADC, input, ADC
    // Pin 2.1, , I/O
    // Pin 2.2, GSM_POWERKEY, Output
    // Pin 2.3, VBAT_ADC, input, ADC
    // Pin 2.4, NC
    // Pin 2.5, GSM_STATUS, Input
    // Pin 2.6, Crystal XIN, input
    // Pin 2.7, Crystal XOUT, output
    P2SEL |= BIT6 + BIT7;            // 32.768 kHz crystal input and output pins
    P2DIR = ~(0x0001 + 0x0008 + 0x0020 + 0x0040);//(VBAT_ADC + GSM_STATUS + BIT6);     // Input
    P2DIR |= GSM_POWERKEY + NTC_EN + BIT7;              // Output Note: BIT7 is for crystal XOUT on P2.7
    P2OUT = 0;                                   // All P2.x reset
    P2REN &= ~(NTC_ADC + VBAT_ADC + GSM_STATUS + BIT6); // disable internal pullup resisitor

    /***********/
    /* PORT P3 */
    /**********/
    // Pin 3.0, GSM_RESET   // Pin 3.0 Output
    // Pin 3.1, SDA         // Pin 3.1 Input
    // Pin 3.2, SCL         // Pin 3.2 Output
    // Pin 3.3, NC
    // Pin 3.4, UART TXD, Special
    // Pin 3.5, UART RX, Special
    // Pin 3.6, NC
    // Pin 4.7, NC
    // Setup I/O for UART

    P3DIR |= GSM_RESET;   
    P3OUT = 0;
    P3SEL |= RXD + TXD;             // enable UART
    
    /***********/
    /* PORT P4 */
    /**********/
    
    // Pin 4.0, NC
    // Pin 4.1, NC
    // Pin 4.2, NC
    // Pin 4.3, NC
    // Pin 4.4, NC
    // Pin 4.5, NC
    // Pin 4.6, NC
    // Pin 4.7, NC

    P4OUT = 0;   
}



void initADC_bat(void)
{
    ADC10CTL0 = 0;
    ADC10CTL1 = INCH_3 | SHS_0 | ADC10DIV_3;
    ADC10CTL0 = SREF_1 | ADC10SHT_3 | REFON | ADC10ON | REF2_5V; // P2.0 ADC option select
}


float adc_read_batery(void)
{
    initADC_bat();
    float vbattery = 0;
    unsigned adc = 0;
    adc = 0;
    int i;
    for (i = 0; i < 4; i++)
    {
        ADC10CTL0 &= ~ADC10IFG;         // Clear conversion complete flag
        ADC10CTL0 |= (ENC | ADC10SC);   // Begin ADC conversion
        while (!(ADC10CTL0 & ADC10IFG))
        ;
        adc += ADC10MEM;                // Read ADC
    }

    // Average
    adc >>= 2;
    vbattery = (float)(adc * (float)(2.5 / 1023.0));        // value * vref(2.5) / 1023 * 2

    return vbattery * 2.0;
}

void initADC_ntc(void)
{
    NTC_ON();
    __delay_cycles(1000000);
    ADC10CTL0 = 0;
    ADC10CTL1 = INCH_0 | SHS_0 | ADC10DIV_3;
    ADC10CTL0 = SREF_1 | ADC10SHT_3 | REFON | ADC10ON | REF2_5V;
}

//unsigned short adc_read_ntc(void)
float adc_read_ntc(void)
{
    initADC_ntc();
    float RT, RT0, B, VCC, VR, R, ln, TX, T0;
    unsigned adc = 0;
    RT0 = 10000.0;    // Ãƒï¿½Ã‚Â©
    B = 3380.0;       // K
    VCC = 3.0;     // Supply voltage
    R = 15000.0;       // R=8.45KÃƒï¿½Ã‚Â©
    T0 = 25.0 + 273.15;

    adc = 0;
    int i;
    for (i = 0; i < 4; i++)
    {
        ADC10CTL0 &= ~ADC10IFG;         // Clear conversion complete flag
        ADC10CTL0 |= (ENC | ADC10SC);   // Begin ADC conversion
        while (!(ADC10CTL0 & ADC10IFG))
        ;
        adc += ADC10MEM;                // Read ADC
    }

    // Average
    adc >>= 2;
    
    VR = (float)adc * (float)(2.5 / 1023.0);
    RT = (VR * R) / (VCC - VR);               // Resistance of RT 
    //Rntc = Rseries/(1023/ADC – 1); // for pull-up configuration
    //RT = R / (1023.0 / ADC10MEM - 1.0);
    ln = log(RT / RT0);
    TX = 1.0 / ((ln / B) + (1.0 / T0));  // Temperature from thermistor
    TX = TX - 273.15;
    
    NTC_OFF();
    return TX;
}
//unsigned short adc_read_ntc(void)
int adc_read_temp(void)
{
    unsigned adc = 0;
    int c;
    int i;

    // Configure ADC
    ADC10CTL0 = 0;
    ADC10CTL1 = INCH_10 | ADC10DIV_3;
    ADC10CTL0 = SREF_1 | ADC10SHT_3 | REFON | ADC10ON;

    // Take four readings of the ADC and average
    adc = 0;
    for (i = 0; i < 4; i++)
    {
        ADC10CTL0 &= ~ADC10IFG;         // Clear conversion complete flag
        ADC10CTL0 |= (ENC | ADC10SC);   // Begin ADC conversion
        while (!(ADC10CTL0 & ADC10IFG))
        ;
        adc += ADC10MEM;                // Read ADC
    }

    // Average
    adc >>= 2;
    // Convert to temperature for Vref = 1.5V
    c = ((27069L * adc) - 18169625L) >> 16;

    return c;
}

