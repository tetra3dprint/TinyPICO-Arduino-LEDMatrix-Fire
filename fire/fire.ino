/*******************************************************************
    TinyPICO Arduino Fire - classic!

    Written by @tetra3dprint
    Twiter: https://twitter.com/tetra3dprint

    TinyPICO https://www.crowdsupply.com/unexpected-maker/tinypico
    by Seon
    Twitter: https://twitter.com/unexpectedmaker

    Adapted from https://github.com/witnessmenow/WiFi-Tetris-Clock
    by Brian Lough
    Twitter: https://twitter.com/witnessmenow

    PxMatrix https://github.com/2dom/PxMatrix
    by @2dom
    Twitter: https://twitter.com/2dom

    I used this awesome tool to convert images into arrays for the pallet
    https://littlevgl.com/image-to-c-array
    
 *******************************************************************/
//------------------------------------------------------------------------------------------------------------------
// Pallet of colors
#include "pallet.h"
//------------------------------------------------------------------------------------------------------------------
// enable double buffer for PxMatrix
#define double_buffer
#include <PxMatrix.h>
// Download the latest version if the library manager fails to compile
// https://github.com/2dom/PxMatrix
//
// Adafruit GFX library is a dependancy for the PxMatrix Library
// Can be installed from the library manager
// https://github.com/adafruit/Adafruit-GFX-Library
//------------------------------------------------------------------------------------------------------------------
//
#define MATRIX_WIDTH 64
#define MATRIX_HEIGHT 64
#define ROW_PATTERN 32  // 64 x 64 is 1/32
#define SHOW_TIME 5
//------------------------------------------------------------------------------------------------------------------
PxMATRIX display(MATRIX_WIDTH, MATRIX_HEIGHT, /*P_LAT*/22, /*P_OE*/26/*TinyPICO*/, /*P_A*/19, /*P_B*/23, /*P_C*/18, /*P_D*/5, /*P_E*/15);
//------------------------------------------------------------------------------------------------------------------
// Ticker is used when no other peripherals such as wifi to maximise perfomrance of outputting pixels to the screen
// If a peripheral such as wifi is used, then the interupt timer must also be used to avoid flickering.
// Comment out #define NO_PERIPHERALS to use the interupt timer
#define NO_PERIPHERALS
//------------------------------------------------------------------------------------------------------------------
#ifdef NO_PERIPHERALS
#include <Ticker.h>
Ticker display_ticker;
//------------------------------------------------------------------------------------------------------------------
void draw() {
  display.display(SHOW_TIME);
}
//------------------------------------------------------------------------------------------------------------------
#else
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
hw_timer_t * timer = NULL;
//------------------------------------------------------------------------------------------------------------------
void IRAM_ATTR draw() {
  portENTER_CRITICAL_ISR(&timerMux);
  display.display(SHOW_TIME);
  portEXIT_CRITICAL_ISR(&timerMux);
}
#endif
//------------------------------------------------------------------------------------------------------------------
// OTT FPS counter
#define SHOW_FPS
#ifdef SHOW_FPS
#include "FPS.h"
FPS fps;
#endif
//------------------------------------------------------------------------------------------------------------------
void setup() {
  //Serial.begin(115200); // I only enable this for debug

  display.begin(ROW_PATTERN, SPI_BUS_CLK, 27, SPI_BUS_MISO, SPI_BUS_SS); // TinyPICO

  //display.setFastUpdate(true); // only works with brightnewss 255
  display.setBrightness(200);
  display.clearDisplay();
  display.setTextColor(0xFFFF);
  display.setTextSize(1);

#ifdef NO_PERIPHERALS
  display_ticker.attach(0.002, draw);
#else
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &draw, true);
  timerAlarmWrite(timer, 2000, true);
  timerAlarmEnable(timer);
#endif

  delay(1000);
#ifdef SHOW_FPS
  fps.getReady();
#endif
}
//------------------------------------------------------------------------------------------------------------------
// Loop variables
int Bit = 0, NBit = 1;
float c;
float fireBuffer[MATRIX_WIDTH][MATRIX_HEIGHT][2];
//------------------------------------------------------------------------------------------------------------------
void loop() {
  // fill the bottom row with some fire!
  for (int x = 1; x < MATRIX_WIDTH - 1; x++) {
    fireBuffer[x][MATRIX_HEIGHT - 2][Bit] = random(0, 359);
    if (random(0, 100) > 80) {
      fireBuffer[x][MATRIX_HEIGHT - 2][Bit] = 0;
      fireBuffer[x][MATRIX_HEIGHT - 3][Bit] = 0;
    }
  }
  // You don't need a switching buffer to achieve this effect, but I found it easier to tame.
  for (int y = 1; y < MATRIX_HEIGHT - 1; y++) {
    for (int x = 1; x < MATRIX_WIDTH - 1; x++) {
      c = (fireBuffer[x - 1][y][Bit] +
           fireBuffer[x + 1][y][Bit] +
           fireBuffer[x][y - 1][Bit] +
           fireBuffer[x][y + 1][Bit] +
           fireBuffer[x][y][Bit]) / 5.0;
       
      // this is a load of hackery to make it look nice
      if (c > 180 && c < 230) c -= 0.2;
      else if (c > 90 && c < 180) c -= 0.4;
      else if (c <= 60) c -= 3;
      else c -= 1;
      if (c < 0) c = 0;
      if (c >= 360) c = 359;
      fireBuffer[x][y - 1][NBit] = c;

      // the pallet is an array of rgb values, so the index needs to be shited into the correct position 
      int index = (int)c * 3;
      if (c == 0) display.drawPixelRGB888(x, y, 0x00, 0x00, 0x00); // ensure we are writing black pixels irrespectiveive of whats in the pallet
      else display.drawPixelRGB888(x, y, pallet[index], pallet[index + 1], pallet[index + 2]);
    }
  }
  // flip the fire buffer
  NBit = Bit;
  Bit = 1 - Bit;
#ifdef SHOW_FPS
  fps.tick();
  display.setCursor(1, 1);
  display.println("FPS:" + fps.getStringFPS());
#endif
  //------------------------------------------------------------------------------------------------------------------
  // flip the buffer
  display.showBuffer();
}
//------------------------------------------------------------------------------------------------------------------
