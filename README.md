# AirThings Wave Plus XIAO ESP32-C5 Interface

This repository, forked from davmoz/airthings-wave, contains Arduino source code for interfacing with an AirThings Wave Plus device using an SeeedStudio XIAO ESP32-C5 microcontroller over Bluetooth Low Energy (BLE).

## Overview

The AirThings Wave Plus is a smart indoor air quality monitor equipped with sensors for Radon, CO2, VOCs, Humidity, Temperature, and Air Pressure. This repository provides a way to read the Humidity, Temperature, both Short Term and Long Term Radon levels, Pressure, CO2, and VoC wirelessly over BLE using an SeeedStudio XIAO ESP32-C5 microcontroller.

## Features

- Read Humidity, Temperature, and Radon levels (Short Term and Long Term), Pressure, CO2, and VoC from AirThings Wave Plus over BLE
- Compatible with SeeedStudio XIAO ESP32-C5 microcontroller
- Provides real-time air quality data

## How it works

The SeeedStudio XIAO ESP32-C5 microcontroller connects to the AirThings Wave Plus over BLE and reads the sensor data. The AirThings Wave Plus samples sensor data once every minute so changes in the environment may not be immediately reflected in the sensor readings.

## Requirements

- AirThings Wave Plus device
- SeeedStudio XIAO ESP32-C5 microcontroller
- PlatformIO IDE

## Setup

1. Clone this repository
2. Open the project in PlatformIO IDE
3. Update platformio.ini for your ESP32-C5 board. See below for more details
4. Update the SERIALNUMBER define with the serial number found on the AirThing Wave Plus
4. Compile/upload the code to your SeeedStudio XIAO ESP32-C5 microcontroller using the "Upload" button in PlatformIO IDE
5. Ensure your AirThings Wave Plus device is in range of the SeeedStudio XIAO ESP32-C5

## Usage

Once the SeeedStudio XIAO ESP32-C5 is programmed and the AirThings Wave Plus device is in range, the SeeedStudio XIAO ESP32-C5 will start reading the sensor data and output it to the serial monitor. Here is a sample output:
```
## Board Support Packages

Platformio Board Support database is out of date, For XIAO boardss, use their BSP database or that from pioarduiono (https://github.com/pioarduino/platform-espressif32.git). The following are the BSP definitions in platformio.ini that you need.

[env:seeed-xiao-esp32-c5]
platform = https://github.com/Seeed-Studio/platform-seeedboards.git
board = seeed-xiao-esp32-c5
framework = arduino
upload_speed = 921600
upload_port = COM3
