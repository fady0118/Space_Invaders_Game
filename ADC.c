// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0 SS3 to be triggered by
// software and trigger a conversion, wait for it to finish,
// and return the result. 
// Daniel Valvano
// January 15, 2016

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include "ADC.h"
#include "..//tm4c123gh6pm.h"

// This initialization function sets up the ADC 
// Max sample rate: <=125,000 samples/second
// SS3 triggering event: software trigger
// SS3 1st sample source:  channel 1
// SS3 interrupts: enabled but not promoted to controller
void ADC0_Init(void){ volatile unsigned long delay;
	//configure PF1
	SYSCTL_RCGC2_R|=0x20;     // enable clock of portE
	delay=SYSCTL_RCGC2_R;
	GPIO_PORTF_DIR_R|=0x02;  // PF1 output
	GPIO_PORTF_AFSEL_R&=~0x02; // disable alt functions on PE2
	GPIO_PORTF_DEN_R|=0x02;  // enable digital I/O function on PE2
	GPIO_PORTF_AMSEL_R&=~0x02; // disable analog on PE2
	
	// GPIO pin configuration
  SYSCTL_RCGC2_R|=0x10;     // enable clock of portE
	delay=SYSCTL_RCGC2_R;
	GPIO_PORTE_DIR_R&=~0x04;  // PE2 input
	GPIO_PORTE_AFSEL_R|=0x04; // enable alt functions on PE2
	GPIO_PORTE_DEN_R&=~0x04;  // disable digital I/O function on PE2
	GPIO_PORTE_AMSEL_R|=0x04; // enable analog on PE2
	// ADC initializtion
	SYSCTL_RCGC0_R|=0x10000;    // enable clock for ADC0 (bit16)
	SYSCTL_RCGC0_R&=~0x300;     // set sample rate to 125,000 samples/second (bits8-9 oxoo)
	ADC0_SSPRI_R=0x0123;        // Priority highest for sequencer 3 
	ADC0_ACTSS_R&=~0x08;        // disable seq 3 before configuration
	ADC0_EMUX_R&=~0xF000;       // seq3 is software(Processor) trigger
	ADC0_SSMUX3_R=0x01;         // set channel Ain1 (PE2)
	ADC0_SSCTL3_R|=0x06;        // no TS0 D0, yes IE0 END0
	ADC0_ACTSS_R|=0x08;         // enable seq 3 
}


//------------ADC0_In------------
// sample capture procedure
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
unsigned long ADC0_In(void){  
	unsigned long data;
  ADC0_PSSI_R|=0x08;             // initiate ss3
	while((ADC0_RIS_R&0x08)==0){}; // wait for conversion done
	data=ADC0_SSFIFO3_R&0xFFF;     // read result
	ADC0_ISC_R|=0x08;              // acknowledge ss3
	return data;
}
