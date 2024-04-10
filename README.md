# Space Invaders Game
Coded and debugged in Keil_v5 in C language

Runs on LM4F120/TM4C123 microcontroller

Author: Fady Abd Elshahid

This is a project for the edX Lab 15 of "Embedded Systems - Shape The World: Multi-Threaded Interfacing" MOOC UTAustinX UT.6.20x by Jon Valvano & Ramesh Yerraballi 

### Game Mechanics
The game is based on the old Space Invaders Game and uses the Nokia 5110 LCD as display
- Movement
  - The player's SpaceShip moves by moving a slider potentiometer which outputs an analog signal that is then converted into digital signal through a DAC and fed into the game engine


  - The enemy sprites move to both sides of the screeen periodically and that is controlled by a timer

- Shooting
  - The Player can shoot lasers by pressing the two swithces, which are configured as edge triggered interrupts
    - The Player's ship normal attack shoots a laser when the player presses switch_1
    - The Player's ship special attack shoots multi lasers when the player presses switch_2
  - The enemies can shoot lasers periodically and that is controlled by a timer, every time the timers reaches zero a random living enemy will shoot a laser
  - The Player has 3 lives and the enemies have 1 live each, when the laser hits its target, that target's life is reduced by one
  - killing all the enemies will result in A win event, losing all your lives will result in a Game over event
 
### Required Hardware I/O connections
  - Slider potentiometer
    - pin 1 connected to ground
    - pin 2 connected to PE2/AIN1
    - pin 3 connected to +3.3V  
  - Two switches
    - fire button connected to PE0
    - special weapon fire button connected to PE1
  - resistors for DAC (4-bit precision)
    - 8*R resistor DAC bit 0 on PB0 (least significant bit)
    - 4*R resistor DAC bit 1 on PB1
    - 2*R resistor DAC bit 2 on PB2
    - 1*R resistor DAC bit 3 on PB3 (most significant bit)
  - Nokia 5110 LCD
    - Blue Nokia 5110
      - Signal        (Nokia 5110) LaunchPad pin
      - Reset         (RST, pin 1) connected to PA7
      - SSI0Fss       (CE,  pin 2) connected to PA3
      - Data/Command  (DC,  pin 3) connected to PA6
      - SSI0Tx        (Din, pin 4) connected to PA5
      - SSI0Clk       (Clk, pin 5) connected to PA2
      - 3.3V          (Vcc, pin 6) power
      - back light    (BL,  pin 7) not connected, consists of 4 white LEDs which draw ~80mA total
      - Ground        (Gnd, pin 8) ground

  - Red SparkFun Nokia 5110 (LCD-10168)
      - Signal        (Nokia 5110) LaunchPad pin
      - 3.3V          (VCC, pin 1) power
      - Ground        (GND, pin 2) ground
      - SSI0Fss       (SCE, pin 3) connected to PA3
      - Reset         (RST, pin 4) connected to PA7
      - Data/Command  (D/C, pin 5) connected to PA6
      - SSI0Tx        (DN,  pin 6) connected to PA5
      - SSI0Clk       (SCLK, pin 7) connected to PA2
      - back light    (LED, pin 8) not connected, consists of 4 white LEDs which draw ~80mA total

