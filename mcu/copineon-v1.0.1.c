#include <iso646.h>
#include <xc.h>

#define true 1
#define false  0
#define HIGH 1
#define LOW  0
#define FOREVER_LOOP while(1)
#define STOP_CODE while(1)
#define pinRA0 PORTAbits.RA0
#define pinRA1 PORTAbits.RA1
#define pinRA2 PORTAbits.RA2
#define pinRA3 PORTAbits.RA3
#define pinRA4 PORTAbits.RA4
#define pinRA5 PORTAbits.RA5

//This declaration is used/needed by the delay() routines.
#define _XTAL_FREQ 8000000        // 8MHz

// PIC12F1501 Configuration Bit Settings
// 'C' source line config statements

//CONFIG 1
#pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
//CONFIG 2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

void setupAnalog_RA4(void);
int analogRead_RA4(void);
int analogRead(void);

void setupAnalog_RA4(void) {
    ADCON2                = 0x00;
    WPUA                  = 0x00;    // disable all pull ups...
    OPTION_REGbits.nWPUEN = 1;       // 1 = there is no pullup function, 0 = pullup enable...
    ADCON1bits.ADPREF     = 0b00;    // Vdd is the reference positive voltage
    ADCON1bits.ADCS       = 0b010;   // Fosc/32
    ADCON1bits.ADFM       = 0b1;     // Result is right justified
    ADCON0bits.CHS        = 0b0011;  // RA4 is the analog pin
    ADCON0bits.ADON       = 0b1;     // ADC engine is enable
    ADCON0bits.GO_nDONE   = 0b0;     // Clear GoDone Flag, not-working / non-busy
    __delay_ms(1);
}

int analogRead_RA4(void) {

   setupAnalog_RA4();
   return analogRead();	// On pin RA4
}

int analogRead(void) {
    ADRESH = 0;
    ADRESL = 0;
    ADCON0bits.GO_nDONE = 1;                // Start ADC engine
    while (ADCON0bits.GO_nDONE) {};         // Just wait until the engine complete the task
    return (( ADRESH << 8 ) bitor ADRESL );    // Fuse the 8bits ADRESH:ADRESL into 16bits value
}

int int_SysLEDValue;

// Describe this function...
void ReadSystemLED() {
  // Set to Analog Input
  TRISA  = 0b10100;
  ANSELA = 0b10000;
  __delay_ms(1);
  int_SysLEDValue = analogRead_RA4();
  // Set Back to Digital
  TRISA  = 0b100;
  ANSELA = 0b0;
  __delay_ms(1);
  if (int_SysLEDValue > 175) {
    pinRA4 = HIGH;
  } else {
    pinRA4 = LOW;
  }
}

//----------------------------------------------------------------------
void initialization()
{
  OSCCONbits.IRCF = 0b1110;   // 8MHz Frequency of the System (Fosc)
  OSCCONbits.SCS  = 0b00;     // Frequency by FOSC in Config Word

  LATA   = 0b00000000;     // Clear PORT A
  TRISA  = 0b100;
  ANSELA = 0b0;
}


//----------------------------------------------------------------------
int main()
{
  initialization();
  // Reset Pin to HIGH
  pinRA5 = HIGH;
  // Reset Reading to HIGH
  pinRA4 = HIGH;

  while (true) {
    // Wait for boot
    __delay_ms(10000);
    __delay_ms(10000);
    __delay_ms(10000);
    ReadSystemLED();
    if (int_SysLEDValue > 175) {
      // If HIGH, then it is NOT booting...
      // Reset Pin to LOW=reset
      pinRA5 = LOW;
      __delay_ms(500);
      pinRA5 = HIGH;
    } else {
      // If RA4=LOW, boot is OK
      while (pinRA2 == LOW) {
        // Just rest, do nothing...
      }
      // If RA2=HIGH, scape RestMode, go back to work...
      pinRA4 = HIGH;
    }
  }

}
