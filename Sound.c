// Sound.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// This routine calls the 4-bit DAC

#include "Sound.h"
#include "DAC.h"
#include "..//tm4c123gh6pm.h"
// sine wave data structure
const unsigned char SineWave[16] = {8, 10, 12, 14, 15, 16, 16, 15, 14, 13, 11, 9, 6, 4, 2, 1}; //4-bit DAC
unsigned char Index=0;           // Index varies from 0 to 15
unsigned long Sound_length=0;
// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
void Sound_Init(void){
  DAC_Init();          // Port B is DAC
  Index = 0;
   unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TIMER2_CTL_R = 0x00000000;   // 1) disable timer2A
  TIMER2_CFG_R = 0x00000000;   // 2) 32-bit mode
  TIMER2_TAMR_R = 0x00000002;  // 3) periodic mode
  TIMER2_TAILR_R = 600000;   // 4) reload value
  TIMER2_TAPR_R = 0;           // 5) clock resolution
  TIMER2_ICR_R = 0x00000001;   // 6) clear timeout flag
  TIMER2_IMR_R = 0x00000001;   // 7) arm timeout
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4

  NVIC_EN0_R = 1<<23;          // 9) enable IRQ 23 in
  TIMER2_CTL_R = 0x00000001;   // 10) enable timer2A
}

// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(unsigned long period){
// this routine sets the RELOAD and starts SysTick
	TIMER2_CTL_R = 0x00000000;   // 1) disable timer2A
	TIMER2_TAILR_R = period-1; // reload value
	TIMER2_CTL_R = 0x00000001;   // 10) enable timer2A
}

// Interrupt service routine
// Executed every 12.5ns*(period)
void Timer2A_Handler(void){
  TIMER2_ICR_R = 0x00000001;  // acknowledge
	Index = (Index+1)&0xF;      // 4,5,6,7,7,7,6,5,4,3,... 
  DAC_Out(SineWave[Index]);    // output one value each interrupt
	Sound_length++;
}
// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Timer2A_Stop(void){
	DAC_Out(0);
	Sound_length=0;
  TIMER2_CTL_R &= ~0x00000001; // disable	
}
void Timer2A_Start(void){
  TIMER2_CTL_R |= 0x00000001;   // enable
}