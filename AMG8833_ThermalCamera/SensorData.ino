// AMG8833 Sensor data processing

//----------------------------------------------------------------------------
void InitAMG8833()
{
  sensor.initI2C();
  statusCode = sensor.resetFlagsAndSettings();
  if(statusCode != 0)  
    Panel.sendf(MonitorLog, F("reset status %s"), sensor.getErrorDescription(statusCode));
  statusCode = sensor.setMode(DEVICE_MODE::SLEEP);
  if(statusCode != 0) 
    Panel.sendf(MonitorLog, F("sleep mode status %s"), sensor.getErrorDescription(statusCode));
  statusCode = sensor.setFPSMode(FPS_MODE::FPS_10);
  if(statusCode != 0) 
    Panel.sendf(MonitorLog, F("frame speed status %s"), sensor.getErrorDescription(statusCode));
}

//----------------------------------------------------------------------------
void SensorData()
{
  statusCode = sensor.updateThermistorTemperature(); // read thermistor data
  if(statusCode != 0) 
    Panel.sendf(MonitorLog, F("thermistor update status %s"), sensor.getErrorDescription(statusCode));

  statusCode = sensor.updatePixelMatrix(); // reads sensor matrix
  if(statusCode != 0) 
    Panel.sendf(MonitorLog, F("matrix update status %s"), sensor.getErrorDescription(statusCode));
 
  hightemp = -500.0; // set hightemp low out of range
  lowtemp =   500.0; // set lowtemp high out of range
  
  if(Raw)
    OutputRawGrid(); // raw (8x8) pixel output
  else
    OutputInterpolatedGrid(); // interpolate to 
      
  Panel.sendf(GraphCaption_1, F("high %s°C"), _FString(hightemp,4,2));
  Panel.sendf(GraphCaption_2, F("low %s°C"), _FString(lowtemp,4,2));
  Panel.sendf(Display_1, F("thermistor ref. %s°C"), _FString(sensor.thermistorTemperature,4,2));
}

//----------------------------------------------------------------------------
void OutputRawGrid()
{
  // Raw output
  
  const int hoffset = ((255 - (16 * 8)) / 2) + 8 + 35; // calculate image position
  const int voffset = ((220 - (16 * 8)) / 2) + 8; 

  Panel.send(GraphDrawPixel, F("$16PX"));

  for (int y = 0; y < 8; y++)
  {
    for (int x = 0; x < 8; x++)
    {
      SetColor(sensor.pixelMatrix[x][y]); // set color to scale
      if(sensor.pixelMatrix[x][y] > hightemp) hightemp = sensor.pixelMatrix[y][x]; // find high temp
      if(sensor.pixelMatrix[x][y] < lowtemp) lowtemp = sensor.pixelMatrix[y][x]; // find low temp
       
      Panel.send(GraphDrawPixel, _Point(byte(hoffset + (y * 16)), byte(voffset + (x * 16)))); // write pixel, flip x<>y to turn 90 deg.
    }
  }
}

//----------------------------------------------------------------------------
void OutputInterpolatedGrid()
{
  // interpolated output
  
  const int hoffset = ((255 - (16 * 8)) / 2) + 8 + 35; // calculate image position
  const int voffset = ((220 - (16 * 8)) / 2) + 12; 

  Panel.send(GraphDrawPixel, F("$4PX")); // set 4x4 pixel size

  // loop over the input
  for (int y = 0; y < 7; y++)
  {
    for (int x = 0; x < 7; x++)
    {
      // fill the corners with measurements
      InterpolatePixel[0][0] = sensor.pixelMatrix[x][y];
      InterpolatePixel[4][0] = sensor.pixelMatrix[x+1][y];
      InterpolatePixel[0][4] = sensor.pixelMatrix[x][y+1];
      InterpolatePixel[4][4] = sensor.pixelMatrix[x+1][y+1];
      // fill the top an bottom row with interpolations
      InterpolatePixel[2][0] = (InterpolatePixel[0][0] + InterpolatePixel[4][0]) / 2;
      InterpolatePixel[1][0] = (InterpolatePixel[0][0] + InterpolatePixel[2][0]) / 2;
      InterpolatePixel[3][0] = (InterpolatePixel[2][0] + InterpolatePixel[4][0]) / 2;
      InterpolatePixel[2][4] = (InterpolatePixel[0][4] + InterpolatePixel[4][4]) / 2;
      InterpolatePixel[1][4] = (InterpolatePixel[0][4] + InterpolatePixel[2][4]) / 2;
      InterpolatePixel[3][4] = (InterpolatePixel[2][4] + InterpolatePixel[4][4]) / 2;
      // fill the columns with interpolations
      for(int ix = 0; ix < 5; ix++)
      {
        InterpolatePixel[ix][2] = (InterpolatePixel[ix][0] + InterpolatePixel[ix][4]) / 2;
        InterpolatePixel[ix][1] = (InterpolatePixel[ix][0] + InterpolatePixel[ix][2]) / 2;
        InterpolatePixel[ix][3] = (InterpolatePixel[ix][2] + InterpolatePixel[ix][4]) / 2;
      }

      // loop over the interpolation buffer
      for(int ix = 0; ix < 5; ix++)
      {
        for(int iy = 0; iy < 5; iy++)
        {
          SetColor(InterpolatePixel[ix][iy]);
          Panel.send(GraphDrawPixel, _Point(byte(hoffset + (y * 16) + (iy * 4)), byte(voffset + (x * 16) + (ix * 4)))); // write pixel, flip x<>y to turn 90 deg.
        }
      }
      if(sensor.pixelMatrix[x][y] > hightemp) hightemp = sensor.pixelMatrix[y][x]; // find high temp
      if(sensor.pixelMatrix[x][y] < lowtemp) lowtemp = sensor.pixelMatrix[y][x]; // find low temp
    }
  }
}

//----------------------------------------------------------------------------
void SetColor(float pixel)
{
  // calucalte temperature to color 
  if     (pixel < ScaleBottom + (ScaleStep * 0)) Panel.send(GraphDrawPixel, F("$DBLUE")); // set color
  else if(pixel < ScaleBottom + (ScaleStep * 1)) Panel.send(GraphDrawPixel, F("$DPURPLE"));
  else if(pixel < ScaleBottom + (ScaleStep * 2)) Panel.send(GraphDrawPixel, F("$BLUE"));
  else if(pixel < ScaleBottom + (ScaleStep * 3)) Panel.send(GraphDrawPixel, F("$LBLUE"));
  else if(pixel < ScaleBottom + (ScaleStep * 4)) Panel.send(GraphDrawPixel, F("$GREEN"));
  else if(pixel < ScaleBottom + (ScaleStep * 5)) Panel.send(GraphDrawPixel, F("$YELLOW"));
  else if(pixel < ScaleBottom + (ScaleStep * 6)) Panel.send(GraphDrawPixel, F("$ORANGE"));
  else if(pixel > ScaleBottom + (ScaleStep * 6)) Panel.send(GraphDrawPixel, F("$RED"));
}

// end module
