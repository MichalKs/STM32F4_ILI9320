/**
 * @file: 	main.c
 * @brief:	LED test
 * @date: 	9 kwi 2014
 * @author: Michal Ksiezopolski
 *
 *
 * @verbatim
 * Copyright (c) 2014 Michal Ksiezopolski.
 * All rights reserved. This program and the
 * accompanying materials are made available
 * under the terms of the GNU Public License
 * v3.0 which accompanies this distribution,
 * and is available at
 * http://www.gnu.org/licenses/gpl.html
 * @endverbatim
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <timers.h>
#include <led.h>
#include <comm.h>
#include <keys.h>
#include <graphics.h>
#include <font_14x27.h>
#include <font_21x39.h>
#include <font_10x20.h>
#include <font_8x16.h>
#include <tsc2046.h>
#include <gui.h>
#include <fat.h>
#include <sdcard.h>
#include <utils.h>

#define SYSTICK_FREQ 1000 ///< Frequency of the SysTick set at 1kHz.
#define COMM_BAUD_RATE 115200UL ///< Baud rate for communication with PC

void softTimerCallback(void);
void tscEvent1(uint16_t x, uint16_t y);
void tscEvent2(uint16_t x, uint16_t y);

#define DEBUG

#ifdef DEBUG
#define print(str, args...) printf(""str"%s",##args,"")
#define println(str, args...) printf("MAIN--> "str"%s",##args,"\r\n")
#else
#define print(str, args...) (void)0
#define println(str, args...) (void)0
#endif

/**
 * @brief Main function
 * @return Whatever
 */
int main(void) {
	
  COMM_Init(COMM_BAUD_RATE); // initialize communication with PC
  println("Starting program"); // Print a string to terminal

	TIMER_Init(SYSTICK_FREQ); // Initialize timer

	// Add a soft timer with callback running every 1000ms
	int8_t timerID = TIMER_AddSoftTimer(1000, softTimerCallback);
	TIMER_StartSoftTimer(timerID); // start the timer

	LED_Init(LED0); // Add an LED
	LED_Init(LED1); // Add an LED
	LED_Init(LED2); // Add an LED
	LED_Init(LED3); // Add an LED

  uint8_t buf[255]; // buffer for receiving commands from PC
  uint8_t len;      // length of command

  // test another way of measuring time delays
  uint32_t softTimer = TIMER_GetTime(); // get start time for delay

//  GRAPH_Init();

//  GRAPH_SetColor(0x00, 0x00, 0xff);
//  GRAPH_SetBgColor(0xff, 0x00, 0x00);
//  GRAPH_DrawBox(100, 100, 100, 100, 5);
//  GRAPH_DrawFilledCircle(50, 50, 50);
//  GRAPH_SetColor(0xff, 0xff, 0xff);
//  GRAPH_SetFont(font21x39Info);
//  GRAPH_DrawChar('A', 120, 50);
//  GRAPH_DrawString("Hello World", 200, 0);
//  GRAPH_SetFont(font14x27Info);
//  GRAPH_DrawString("A mouse is", 240, 0);
//  GRAPH_SetFont(font10x20Info);
//  GRAPH_DrawString("not a lion.", 280, 0);
//  GRAPH_SetFont(font8x16Info);
//  GRAPH_DrawString("To be or not to be", 170, 0);

  // draw image test
//  TIMER_Delay(3000);
//  GRAPH_ClrScreen(0, 0, 0);
//  GRAPH_DrawImage(30, 30);

  // data for example graph - sinusoidal signal
//  uint8_t graphData[320];
//  double x = 0.0;
//
//  for (int i = 0; i < 320; i++, x+=0.01*M_PI) {
//    graphData[i] = (uint8_t)(sin(x)*100 + 100);
//  }
//
//  TIMER_Delay(3000);
//  GRAPH_ClrScreen(0, 0, 0);
//  GRAPH_DrawGraph(graphData, 290, 0, 0);

  // draw example bar chart
//  TIMER_Delay(3000);
//  GRAPH_ClrScreen(0, 0, 0);
//  GRAPH_DrawBarChart(graphData+30, 32, 0, 0, 5);


  // register an event for a given region
//  TSC2046_RegisterEvent(0, 0, 1500, 4000, tscEvent1);
//  TSC2046_RegisterEvent(0, 0, 4000, 1500, tscEvent2);

  FAT_Init(SD_Init, SD_ReadSectors, SD_WriteSectors);
  int id = FAT_OpenFile("HELLO   TXT");
  uint8_t data[50];

  int i = FAT_ReadFile(id, data, 30);
  hexdumpC(data, i);

  GUI_Init();

  GUI_AddButton(50, 50, 50, 100, tscEvent1, "LED 0");
  GUI_AddButton(200, 50, 50, 100, tscEvent2, "LED 1");

	while (1) {

	  // test delay method
	  if (TIMER_DelayTimer(1000, softTimer)) {
	    LED_Toggle(LED3);
	    softTimer = TIMER_GetTime(); // get start time for delay
	  }

	  // check for new frames from PC
	  if (!COMM_GetFrame(buf, &len)) {
	    println("Got frame of length %d: %s", (int)len, (char*)buf);

	    // control LED0 from terminal
	    if (!strcmp((char*)buf, ":LED0 ON")) {
	      LED_ChangeState(LED0, LED_ON);
	    }
	    if (!strcmp((char*)buf, ":LED0 OFF")) {
	      LED_ChangeState(LED0, LED_OFF);
	    }
	  }
	  TSC2046_Update(); // run touchscreen functions
		TIMER_SoftTimersUpdate(); // run timers
	}
}
/**
 * @brief Callback function called on every soft timer overflow
 */
void softTimerCallback(void) {
//  LED_Toggle(LED0); // Toggle LED
}
/**
 * @brief Example touchscreen event handler.
 * @param x X coordinate of touch.
 * @param y Y coordinate of touch.
 */
void tscEvent1(uint16_t x, uint16_t y) {
  LED_Toggle(LED0);
}
/**
 * @brief Example touchscreen event handler.
 * @param x X coordinate of touch.
 * @param y Y coordinate of touch.
 */
void tscEvent2(uint16_t x, uint16_t y) {
  LED_Toggle(LED1);
}
