// AMG8833_ThermalCamera - https://github.com/JaapDanielse/AMG8833-Thermal-Camera
// Documentation https://github.com/JaapDanielse/AMG8833-Thermal-Camera/wiki
// GNU General Public License v3.0 - Copyright (c) 2019 Jaap Danielse
// 
// Uses Melopero_AMG8833 library - https://github.com/melopero/Melopero_AMG8833 (install via IDE)
// Uses VirtualPanel - https://github.com/JaapDanielse/VirtualPanel (install via instructions)


#include <Melopero_AMG8833.h>
#include "VirtualPanel.h"

Melopero_AMG8833 sensor;

bool  PanelInit = false; // panel init flag
bool  Power = false; // Camera on / off
bool  Raw = true; // raw / interpolate status
int   statusCode = 0; // AGM8833 status 
float InterpolatePixel[5][5]; // interpoation buffer
float hightemp = 0.0; // highest temp in frame
float lowtemp  = 0.0; // ;lowest temp in frame
float ScaleTop = 25.0; // color scale top
float ScaleBottom = 15.0; // color scale botom
float ScaleStep = 0; // color scale step size


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
