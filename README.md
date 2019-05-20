# AMazeBall

## Overview

AMazeBall was a final project for CS 3420 created by Evan Patrick and Natalie Neamtu. It is an arcade-style maze balancing game with the goal of capturing as many "food" items as possible

## Technical Description

This project runs on the FRDM k64f board and it interfaces with an external IMU to find the orientation of the system and a 32x32 RGB LED matrix panel for displaying the game state.

Links to both of the parts:

- IMU: https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor/overview
- LED Matrix: https://www.adafruit.com/product/1484

The IMU uses I2C and the LED Matrix has a custom communication protocol that we needed to implement from scratch using GPIO.

## Youtube Link

Here is a link to a video of the project working: https://youtu.be/qpapZ6arSPE

