# TinyPICO-Arduino-LEDMatrix-Fire

A classic "Fire Effect" demonstration on a 64 x 64 LED Matrix
Ardunio IDE TinyPICO ESP32 code

---
This is focused on performance of drawing pixels to the screen. If your using peripherals such as wifi, it will cause the display to flicker. You will need to use interupt timers to avoid the flickering. However that comes with over 50% drop in performance.
---



video: https://www.youtube.com/embed/Vy6Xjx7Kiyc

Required hardware/software: -

Arduino IDE with ESP32 support installed https://github.com/espressif/arduino-esp32#installation-instructions

LED Matrix - I got one from AliExpress

@unexpectedmaker #TinyPICO https://www.crowdsupply.com/unexpected-maker/tinypico

Brian Lough @witnessmenow Pico Matrix Shiled https://www.tindie.com/products/brianlough/tinypico-matrix-shield/

Made possible by @2dom PxMatrix https://github.com/2dom/PxMatrix & the new double buffer functionality

---

The code is not efficient, but does make use of some lookup tables to speed some things up.
This could be sped up with pure integer arithmetics and supporting lookup tables, but that would make the code a bit harder to play with and follow.

The pallet is an array of RGB565 values that I created using another script. Just replace those values for custom pallets.
360 is a magic number of values here.

I've included my attempt at an accurate FPS counter (± a couple FPS :)
It shuold report a true frame count per 1000 milliseconds. It works quite well for testing code execution cost.
Just comment out #define SHOW_FPS to turn it off

Additionally if you uncomment //#define BASIC_FPS_TEST this will just fill the Matrix with a fixed color instead of running the Plasma Effect. The idea of this is to guesstimates max realistic fps( if shown) in the current enviroment and implementation.

I was getting 88 fps in test mode and the Plasma averaged around 46 fps

If you use this code then make sure you configure PxMatrix correctly for your LED Matrix


