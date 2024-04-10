// SpaceInvadersGame.c
// Runs on LM4F120/TM4C123
// Fady Abd Elshahid
// This is a starter project for the edX Lab 15 of Embedded Systems Shape The World MOOC
// April 10, 2023

// ******* Required Hardware I/O connections *******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Blue Nokia 5110
// ---------------
// Signal        (Nokia 5110) LaunchPad pin
// Reset         (RST, pin 1) connected to PA7
// SSI0Fss       (CE,  pin 2) connected to PA3
// Data/Command  (DC,  pin 3) connected to PA6
// SSI0Tx        (Din, pin 4) connected to PA5
// SSI0Clk       (Clk, pin 5) connected to PA2
// 3.3V          (Vcc, pin 6) power
// back light    (BL,  pin 7) not connected, consists of 4 white LEDs which draw ~80mA total
// Ground        (Gnd, pin 8) ground

// Red SparkFun Nokia 5110 (LCD-10168)
// -----------------------------------
// Signal        (Nokia 5110) LaunchPad pin
// 3.3V          (VCC, pin 1) power
// Ground        (GND, pin 2) ground
// SSI0Fss       (SCE, pin 3) connected to PA3
// Reset         (RST, pin 4) connected to PA7
// Data/Command  (D/C, pin 5) connected to PA6
// SSI0Tx        (DN,  pin 6) connected to PA5
// SSI0Clk       (SCLK, pin 7) connected to PA2
// back light    (LED, pin 8) not connected, consists of 4 white LEDs which draw ~80mA total

//---------Preprocessor----------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ADC.h"
#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "TExaS.h"
//-----function_prototype-------
void EnableInterrupts(void);  // Enable interrupts
void Move(void);
void Draw(void);
//-----global_variables-------
char String[10]; // null-terminated ASCII string
unsigned long Distance;   // units 0.001 cm
unsigned long ADCdata;    // 12-bit 0 to 4095 sample
unsigned long Flag=0;       // semaphore for enemy movement in the game between systick and main loop
unsigned long TimerCount=0; 
unsigned long FrameCount=0;
unsigned long enemy_width=8;
unsigned long SpaceShip_width=12;
unsigned long speed_divider=3; // Enemy moves 1 pixel every (25ms * speed_divider)

// Initialize SysTick interrupts to trigger at 40 Hz, 25 ms
void SysTick_Init(unsigned long period){
NVIC_ST_CTRL_R=0;
NVIC_ST_RELOAD_R=period-1; //period=2000,000 for 40hz
NVIC_ST_CURRENT_R=0;
NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000;    // Priority 2 
NVIC_ST_CTRL_R=0x07;
}
// executes every 25 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void){ 

}


//-----------------sprite_images--------------------
const unsigned char Enemy1[] ={
 0x42, 0x4D, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x0F, 0xF0, 0x0F, 0xF0, 0x00, 0xF0,
 0x0F, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0xFF, 0xFF, 0xF0, 0x00, 0xFF, 0xFF, 0x00, 0xFF,

};


const unsigned char Enemy2[] ={
 0x42, 0x4D, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x0F, 0x00, 0x00, 0xF0, 0xF0, 0x00, 0x00, 0x0F, 0xFF, 0x0F,
 0xF0, 0xFF, 0x0F, 0xFF, 0xFF, 0xF0, 0xFF, 0x0F, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0xFF, 0xFF, 0xF0, 0x00, 0xFF, 0xFF, 0x00, 0xFF,

};


const unsigned char Laser0[] ={
 0x42, 0x4D, 0xA2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x2C, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x0F, 0xF0,
 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x70, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0xFF, 0xF0, 0x00, 0x00, 0x7F, 0xF0, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0xFF, 0x00,
 0x00, 0x00, 0xFF,

};

const unsigned char Laser1[] ={
 0x42, 0x4D, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x0F, 0xF0,
 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0xFF,

};

const unsigned char SpaceShip[] ={
 0x42, 0x4D, 0xEE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xF0, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x0F, 0x00, 0xFF, 0x00,
 0xF0, 0xFF, 0x0F, 0x00, 0xFF, 0x00, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0x0F, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0xF0, 0x0F,
 0xFF, 0xFF, 0xFF, 0xF0, 0x0F, 0x00, 0xF0, 0x0F, 0xFF, 0xFF, 0xFF, 0xF0, 0x0F, 0x00, 0x00, 0x0F, 0x0F, 0xFF, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0xFF, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x0F,
 0x00, 0xFF, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,

};

const unsigned char Explosion[] ={
 0x42, 0x4D, 0xD6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x0F, 0x07, 0x80, 0x08, 0x70, 0xF0, 0x00, 0x00, 0x07, 0x70,
 0x80, 0x08, 0x07, 0x70, 0x00, 0x00, 0x00, 0xF0, 0x77, 0x77, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x87, 0x00, 0x00, 0x00, 0x87, 0x07, 0x00, 0x00, 0x70, 0x78, 0x00, 0x00, 0xF8, 0x00,
 0x00, 0x00, 0x00, 0x8F, 0x00, 0x00, 0x70, 0x08, 0x00, 0x00, 0x80, 0x07, 0x00, 0x00, 0x00, 0x87, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x08, 0x70, 0x00, 0x00, 0x08, 0x00,
 0x77, 0x77, 0x00, 0x80, 0x00, 0x00, 0x07, 0x00, 0x80, 0x08, 0x00, 0x70, 0x00, 0x00, 0x00, 0x07, 0x80, 0x08, 0x70, 0x00, 0x00, 0x00, 0xFF,

};

const unsigned char image[] ={
 0x42, 0x4D, 0xB6, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x40, 0x08, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x78, 0x88, 0xC8, 0x80, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xC8, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x88, 0x88, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0xEF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x88,
 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x8E, 0xFF, 0x00, 0x00, 0x00, 0x0F, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE8, 0xFF, 0x00, 0x00, 0xF8, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF, 0xFF, 0x00, 0x00, 0xF8, 0x88,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x90, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF, 0xFF, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x8F, 0x00, 0x88, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8F, 0xF0, 0x00, 0x8E, 0xFF,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF0, 0xF0, 0x8E, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8F, 0xFF, 0xF0, 0xFE, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x88, 0x80, 0x88, 0xFF, 0xF8, 0xF0, 0xF0, 0x88, 0x80, 0x80, 0x80, 0x88, 0x8F, 0x88, 0x88, 0x8F, 0x00, 0x80, 0x00, 0x00, 0xF0, 0x80, 0x80, 0x00, 0xF8, 0x8C, 0x8F, 0x00, 0x00, 0x00, 0x8F,
 0xF8, 0xF0, 0xF8, 0xFF, 0x00, 0x00, 0x07, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x0F, 0xFF, 0x08, 0x8F, 0x00, 0x00, 0x80, 0x80, 0x88, 0x80, 0x8F, 0x00, 0x08, 0x00, 0x80, 0x00, 0x00,
 0x00, 0xF0, 0x0F, 0xF0, 0x00, 0x80, 0x00, 0x00, 0xF0, 0x00, 0x00, 0xEF, 0xFF, 0xF0, 0x08, 0xEF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0xF8, 0x0F, 0xF0, 0x0F, 0xE8, 0x00,
 0x00, 0x8F, 0x80, 0x00, 0x00, 0xFF, 0x00, 0x0F, 0x80, 0x8F, 0x80, 0x0F, 0xFF, 0xF8, 0x00, 0xF8, 0x00, 0xF0, 0x00, 0x00, 0x80, 0x00, 0x00, 0x88, 0xFF, 0x00, 0x00, 0x8F, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x80, 0x0F, 0xF0, 0x00, 0x00, 0x8F, 0x80, 0x00, 0x00, 0xF8, 0x80, 0xFF, 0x00, 0xFF, 0x00, 0x0F, 0x80, 0x0F, 0xF0, 0x08, 0xFF, 0xF8, 0x00, 0x88, 0x00, 0xFF, 0xF8, 0x00,
 0x08, 0x00, 0x00, 0x08, 0xFF, 0xF0, 0x00, 0x8F, 0x00, 0x00, 0x00, 0x08, 0x70, 0x00, 0x00, 0x0F, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x80, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x08, 0xFF, 0x00, 0x0F,
 0x80, 0x0F, 0xF0, 0x00, 0x08, 0xF8, 0x00, 0x00, 0x08, 0xFF, 0xF0, 0x00, 0x08, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0x00, 0x8F, 0x00, 0x00, 0x00, 0x0C, 0x80, 0x00, 0x00, 0xF0, 0x08, 0xFF, 0x00, 0x00,
 0x00, 0xF8, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xF0, 0x08, 0xFF, 0x00, 0x0F, 0xF0, 0x0F, 0xF0, 0x00, 0x88, 0xFF, 0x80, 0x0F, 0xF0, 0x8F, 0xF0, 0x0F, 0xFF, 0x00, 0x00, 0x08, 0x8F, 0xFF, 0xFF, 0xFF,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x0F, 0xF0, 0x0F, 0xF0, 0x08, 0xF8, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x08, 0xFF, 0x00, 0x0F, 0xF0, 0x0F, 0xF0, 0x00, 0xFF, 0xFF, 0xF0, 0x00,
 0xF0, 0x08, 0xF0, 0x08, 0xF8, 0x08, 0xF0, 0x0F, 0xEF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x0F, 0x80, 0x0F, 0xF0, 0x0F, 0xF8, 0x0F, 0xFF, 0x00, 0xFF, 0x00, 0x00,
 0x08, 0xFF, 0x00, 0x0F, 0xF0, 0x0F, 0xF0, 0x00, 0xFF, 0xFF, 0xF0, 0x00, 0xFF, 0x08, 0xF0, 0x08, 0xF8, 0x08, 0xF0, 0x0F, 0xEF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
 0xFF, 0x80, 0xFF, 0x80, 0x0F, 0xF0, 0x0F, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x08, 0xFF, 0x00, 0x0F, 0xF0, 0x0F, 0xF8, 0x00, 0x8F, 0xFF, 0xF8, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x80, 0x0F,
 0x8E, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0xF8, 0x00, 0xFF, 0x00, 0x8F, 0x00, 0xFF, 0xF0, 0x00, 0xFF, 0x00, 0x00, 0x08, 0xFF, 0x00, 0x00, 0x00, 0x0F, 0xF8, 0x00,
 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x8F, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x0E, 0x8F, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x80, 0x08, 0x88, 0x0F, 0x88, 0x00, 0xFF, 0x00, 0x8E, 0xF0,
 0x88, 0x8F, 0x88, 0x88, 0x88, 0xFF, 0x08, 0x88, 0x88, 0x8F, 0xF8, 0x88, 0x88, 0x80, 0x88, 0x88, 0x88, 0x80, 0xFF, 0xF0, 0x00, 0x00, 0x80, 0x00, 0x08, 0xEF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x0F, 0x8E, 0x8F, 0xFE, 0xEF, 0x88, 0x88, 0xF8, 0xE8, 0xF8, 0x88, 0x8F, 0xF8, 0x88, 0x8F, 0xFF, 0x88, 0x88, 0x88, 0x8F, 0xF8, 0x88, 0x88, 0x88, 0xFE, 0x88, 0x88, 0x8F, 0xFF, 0xEE,
 0x88, 0x88, 0x00, 0x00, 0x00, 0x88, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xEE, 0x8F, 0xF8, 0xFF, 0x8E, 0xEF, 0xFE, 0x8F, 0xF8, 0x8E, 0xFF, 0x88, 0x8E, 0x8F, 0x88, 0xF8, 0x88,
 0x88, 0xF8, 0xFF, 0x88, 0x88, 0x8F, 0xFF, 0x8E, 0xEE, 0xF0, 0xFF, 0xF8, 0xEE, 0xE0, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0x00, 0xFF, 0x00,
 0xFF, 0xF8, 0x88, 0x88, 0xFF, 0xF8, 0x8F, 0xFF, 0xFF, 0x8F, 0x8F, 0xF8, 0x8F, 0xFF, 0x88, 0x8F, 0xFF, 0x88, 0x88, 0x8F, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0xFF,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x0F, 0xF0, 0x0F, 0xFF, 0xF8, 0x00, 0x88, 0x80, 0x0F, 0xF0, 0x00, 0x08, 0x8F, 0x00, 0x00, 0x08,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x0F, 0xF0, 0x0F, 0x80, 0x0F, 0xFF,
 0xF8, 0x00, 0x88, 0x80, 0x0F, 0x80, 0x0F, 0x8C, 0x88, 0x00, 0x08, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x78, 0xFF,
 0xF0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF0, 0x08, 0x80, 0x0F, 0x8F, 0xF8, 0x00, 0xFF, 0x00, 0x0F, 0x80, 0x0F, 0x8C, 0xCF, 0x80, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
 0x00, 0x08, 0x8F, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8F, 0x8F, 0x8F, 0xF0, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0x00, 0x88, 0x00, 0x00, 0x08, 0x8F, 0x00, 0xFF, 0xF0, 0x88, 0x80, 0x0F, 0xF8,
 0x88, 0x80, 0x0F, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x0C, 0xF0, 0x00, 0x08, 0x8E, 0x88, 0x00, 0x00, 0x0E, 0x88, 0x00, 0x00, 0x00, 0x88, 0xF8, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
 0x8E, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0xF0, 0x8F, 0x80, 0x0F, 0xFF, 0x88, 0x80, 0x0F, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x7C, 0x00, 0xF0, 0x00, 0x00, 0x88, 0x00, 0x00, 0x0E, 0x88,
 0x00, 0x00, 0x00, 0x88, 0xF8, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x8E, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0xF0, 0x8F, 0x80, 0x0F, 0xFF, 0x88, 0x80, 0x08, 0x88, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x80, 0x7C, 0x00, 0xF0, 0x00, 0x00, 0x88, 0x00, 0x00, 0x0F, 0x8E, 0xF0, 0x00, 0x00, 0x8F, 0x8F, 0x88, 0x00, 0x00, 0x08, 0x00, 0xFF, 0xF8, 0xEF, 0x00, 0xFF, 0x00, 0x8F, 0x00, 0xFF, 0xF0,
 0x8F, 0xF0, 0x08, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x80, 0x8C, 0x08, 0x80, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x0E, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0xF0, 0x00, 0xFF, 0xE8, 0xF8, 0x00, 0xFF, 0x00, 0x8F, 0x00, 0xF8, 0x80, 0xF0, 0xF0, 0x08, 0xFF, 0xFF, 0x88, 0x00, 0xFF, 0x80, 0x00, 0x08, 0x00, 0x00, 0xC8, 0x88, 0x8C, 0xF0, 0xF8, 0x80, 0x00,
 0x00, 0x00, 0xF8, 0x88, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x0F, 0xF0, 0x08, 0xF8, 0x00, 0xFF, 0x00, 0x8F, 0x00, 0xF0, 0x00, 0x80, 0xF0, 0x00, 0xF8, 0x08, 0x88, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0x8C, 0x8F, 0xCC, 0x88, 0xCC, 0xCC, 0xC8, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x0F, 0xF0, 0x08, 0x80, 0x00, 0xFF, 0x00,
 0x8F, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x00, 0xF8, 0x00, 0x88, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x08, 0xCC, 0xCC, 0xF8, 0xC8, 0x8C, 0x80, 0x00, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x8E, 0xEF, 0xFF,
 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x08, 0x88, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xF0, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x8C, 0xCC, 0xFF, 0xFF,
 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0xEE, 0xEE, 0xEE, 0xE7, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x8E, 0xE0, 0x00, 0x00, 0x08, 0x8F, 0x80, 0x00, 0x00, 0xF0, 0xFF, 0xF0, 0x00,
 0x08, 0xF8, 0x80, 0x00, 0x00, 0x0F, 0x00, 0x08, 0xCC, 0x88, 0x78, 0x8C, 0xC8, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x0F, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xE0, 0x00, 0x00, 0x08, 0x88, 0x88, 0xEE,
 0xE8, 0x88, 0x88, 0xEE, 0x80, 0xE8, 0x88, 0x88, 0x00, 0xEF, 0xFF, 0xFF, 0xFF, 0xE8, 0x88, 0x88, 0x88, 0xFF, 0x00, 0x00, 0x8C, 0xFF, 0xFF, 0xCC, 0xCC, 0xCC, 0x80, 0xFF, 0x00, 0x00, 0x00, 0xFE,
 0xEE, 0xEE, 0xEE, 0xE8, 0x88, 0x80, 0x00, 0x00, 0x00, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xE0, 0x8E, 0xEE, 0xEE, 0x00, 0xEE, 0xEE, 0xEE, 0xEE, 0xE8, 0xEE, 0xEE, 0xEE, 0x00, 0x00, 0x00,
 0x8C, 0xCF, 0xFF, 0x8C, 0xC8, 0x8C, 0xCC, 0x8F, 0x00, 0x00, 0x08, 0xEE, 0xEE, 0xEF, 0xFF, 0xFF, 0x88, 0xE8, 0x00, 0x00, 0x00, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xE0, 0x0E, 0xEE, 0xEE,
 0x00, 0x0E, 0xEE, 0xEE, 0xEE, 0x0E, 0xEE, 0xEE, 0xE0, 0x00, 0x00, 0x08, 0xCC, 0xC8, 0xF8, 0xFF, 0xC8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8E, 0xE8, 0xEE, 0xFF, 0xFF, 0xFF, 0x88, 0x80, 0x00, 0x00,
 0x00, 0x0E, 0xEE, 0xE0, 0x0E, 0xEE, 0xEE, 0xE0, 0x00, 0x0E, 0xEE, 0xEE, 0x00, 0x00, 0xEE, 0xEE, 0xE0, 0x0E, 0xEE, 0xEE, 0xE0, 0x00, 0x00, 0x0C, 0xC8, 0x8C, 0xCC, 0xC8, 0x8C, 0x80, 0x00, 0x00,
 0x00, 0x00, 0x88, 0x8E, 0xFF, 0xFC, 0x88, 0xCC, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
 0x00, 0x00, 0x00, 0x88, 0x00, 0x8C, 0x8C, 0xFC, 0x88, 0xC8, 0x00, 0x00, 0x00, 0x00, 0x88, 0x8E, 0xF8, 0xCC, 0x88, 0xCC, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x88, 0x00, 0x8C, 0x8C, 0xFC, 0x88, 0xC8, 0x00, 0x00, 0x00, 0x00, 0x8E, 0x88, 0x8C, 0x87, 0x88, 0x8F,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC7, 0x8C, 0x08,
 0x80, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x08, 0x88, 0x88, 0x88, 0x8E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x09, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x80, 0xF8, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,

};

//-------------Delay_Function----------------
void Delay1ms(unsigned long time){
  unsigned long i;
  while(time > 0){
    i = 5714;  // this number means 1ms
    while(i > 0){
      i = i - 1;
    }
    time = time - 1; // decrements every 1 ms
  }
}

//------------------ Sprites Data Structures ---------------------
struct state{
unsigned long x; // x position
unsigned long y; // y position
const unsigned char *image[2]; //ptr->image
unsigned long life;	//0=dead, 1=alive
};

typedef struct state Stype;
Stype Enemy[5]; // Enemy sprites
Stype player;		// Player sprites


//---------------Enemy_State_Initialization------------
void Enemy_Init(void){
unsigned long i;
	for(i=0;i<5;i++){
	Enemy[i].x=(i+1)*4+i*12-2; // initail enemy x-positions
	Enemy[i].y=12;
	Enemy[i].image[0]=	Enemy1;	
	Enemy[i].image[1]=	Enemy2;	
	Enemy[i].life=1;
	}
}
//---------------player_State_Initialization------------
void SpaceShip_Init(void){
	player.x=ADC0_In(); // initail enemy x-positions is the slider position
	player.y=47; // bottom of the screen
	player.image[0]=	SpaceShip;	
	player.life=3;
}

int main(void){ 
	//Initializations
  volatile unsigned long delay;
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_Scope);  // texas scope
	ADC0_Init();             // initialize ADC0, channel 1, sequencer 3
	Nokia5110_Init();        // initialize Nokia5110 LCD (optional)
	SysTick_Init(2000000);   // initialize SysTick for 40 Hz interrupts
  EnableInterrupts();
	
// print the game splash screen image
	Nokia5110_PrintBMP(0, 47, image, 0);
	Nokia5110_DisplayBuffer();
// wait 100ms then clear display
	Delay1ms(100);//100 ms delay
	Nokia5110_ClearBuffer();
	Nokia5110_DisplayBuffer();
  while(1){ 
	}
}