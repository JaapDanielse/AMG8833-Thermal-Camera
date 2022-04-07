// AMG8833_ThermalCamera - https://github.com/JaapDanielse/AMG8833-Thermal-Camera
// Documentation https://github.com/JaapDanielse/AMG8833-Thermal-Camera/wiki
// GNU General Public License v3.0 - Copyright (c) 2022 Jaap Danielse
// 
// Uses Melopero_AMG8833 library - https://github.com/melopero/Melopero_AMG8833 (install via IDE)
// Uses VirtualPanel - https://github.com/JaapDanielse/VirtualPanel (install via instructions)
//
// V1.0 07-04-2022 - Jaap Daniëlse 
//      Initial release

#include <Melopero_AMG8833.h>
#include "VirtualPanel.h"

Melopero_AMG8833 sensor;

bool  PanelInit = false; // panel init flag
bool  Power = false; // Camera on / off
bool  InfoPanel = false;
int   statusCode = 0; // AGM8833 status 
float InterpolatePixel[10][10]; // pixel interpolation buffer
float hightemp = 0.0; // highest temp in frame
float lowtemp  = 0.0; // ;lowest temp in frame
float centertemp = 0.0; // center of frame temp.
float ScaleTop = 25.0; // color scale top
float ScaleBottom = 15.0; // color scale botom
float ScaleStep = 0; // color scale step size
byte   ScaleOffset = 30;
byte   hoffset = ((255 - (16 * 8)) / 2) + 8 + 35; // calculate image position
byte   voffset = ((220 - (16 * 8)) / 2) + 8; 
enum  ImageMode { RAW, IP29, IP64 };
ImageMode ImageModeSelect = RAW;
enum  StillMode { OFF, GET, WRITE, SHOW};
StillMode Still = OFF;
byte  ip = 1;
byte  px = 16;
byte  clickx = 0;
byte  clicky = 0;
bool  mirror = false;

//----------------------------------------------------------------------------
void setup() 
{
  Panel.begin(); // init port and protocol
  while(!PanelInit) Panel.receive(); // Wait for panel init
  Wire.begin(); // Start I2C  
  InitAMG8833();  // init AGM8833
}

//----------------------------------------------------------------------------
void loop() 
{
  Panel.receive(); // read panel input

  if(Power) // if camera on
  {
    SensorData(); // read sensor data
  } 
}
