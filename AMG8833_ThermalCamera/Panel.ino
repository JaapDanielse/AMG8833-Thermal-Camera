// VirtualPanel control
// Note: where possible the F() macro is used to limit ram usage.

//----------------------------------------------------------------------------
void PanelCallback(vp_channel event) 
{ 
  // handle panel events
  switch (event) 
  {
    case PanelConnected: InitPanel(); break; // on connection init panel
    case Button_1:  SetScale(false,  1); break; // handle scale bottom up 
    case Button_2:  SetScale(false, -1); break; // handle scale bottom down
    case Button_3:  ToggleMirror(); break; // toggle mirror mode
    case Button_5:  SetRawMode(); break; // set raw mode
    case Button_6:  SetInterpolate29Mode(); break; // set interpolate 29 x 29 mode
    case Button_7:  SetInterpolate64Mode(); break; // set interpolate 64 x 64 mode
    case Button_9:  SetScale(true,   1); break; // handle scale top up
    case Button_10: SetScale(true,  -1); break; // handle scale top down
    case Button_11: SetStill(); break; 
    case Button_15: ToggleInfo(); break;
    case Button_17: TogglePower(); break; // toggle power on / off
    case GraphClick: GraphClicked(); break;
    
    default: break;
  }
}

//----------------------------------------------------------------------------
void TogglePower()
{
   // toggle power on/off
   Power = !Power;
   if(Power)
   {
     Panel.send(Led_9, F("$RED")); // set power led to red   
     statusCode = sensor.setMode(DEVICE_MODE::NORMAL); // set sensor in normal mode
     if(statusCode != 0) 
       Panel.sendf(MonitorLog,F("Mode change status %s"), sensor.getErrorDescription(statusCode)); //if error send to Log
     if(ImageModeSelect == RAW) SetRawMode(); 
     else if (ImageModeSelect == IP29) SetInterpolate29Mode(); 
     else if (ImageModeSelect == IP64) SetInterpolate64Mode(); 
     Still = OFF;
     Panel.send(Led_7, F("$OFF"));
   }
   else   
   {
     Panel.send(Led_9, F("$OFF")); // set power led to off  
     SetScale(true, 0); // write scale to Graph panel
     Panel.send(Display_1, F("")); // clear Display_1
     statusCode = sensor.setMode(DEVICE_MODE::SLEEP); // set sensor to sleep mode
     if(statusCode != 0) 
       Panel.sendf(MonitorLog,F("Mode change status %s"), sensor.getErrorDescription(statusCode)); //if error send to Log
  }
}

//----------------------------------------------------------------------------
void GraphClicked()
{ // click in graph panel
  if( Still != SHOW ) return;  // if not in still: return

  Panel.send(GraphDrawCircle, F("$DEL")); // delete the previous circle
  Panel.send(GraphDrawCircle, _Circle(clickx, clicky, 5));

  clickx = highByte(Panel.vpr_uint); // get coördinate
  clicky = lowByte(Panel.vpr_uint);

  byte dispe = byte (7 * ip * px); // calc display width and height
  
  if(clickx >= hoffset && clicky >= voffset && clickx <=  hoffset + dispe && clicky <= voffset + dispe)
  { // within the picture
    clickx = (clickx - hoffset + ((ip * px) / 2)) / (ip * px); // calculate matrix x position
    clicky = (clicky - voffset + ((ip * px) / 2)) / (ip * px); // calculate matric y position

    if(!mirror) // in mirror inverse (matrix) x
      centertemp = sensor.pixelMatrix[clicky][clickx]; 
    else
      centertemp = sensor.pixelMatrix[clicky][7 - clickx]; 

    clickx = byte((clickx * ip * px) + hoffset); // calc circle pos.
    clicky = byte((clicky * ip * px) + voffset);
    
    Panel.send(GraphDrawCircle, F("$WHITE")); // draw circle
    Panel.send(GraphDrawCircle, _Circle(clickx, clicky, 5));
    
    Panel.send(GraphText, _Point(ScaleOffset,40)); // write temp
    Panel.send(GraphText,  F("$YELLOW"));  
    Panel.sendf(GraphText, F("◯%s°C"), _FString(centertemp,4,1));
    Panel.send(GraphText,  F("$WHITE"));
  }
}

//----------------------------------------------------------------------------
void ToggleInfo()
{
  InfoPanel = !InfoPanel; // toggle
  Panel.send(Info, InfoPanel); // show or hide infopanel
}

//----------------------------------------------------------------------------
void ToggleMirror()
{
  mirror = !mirror;
  if(mirror) Panel.send(Led_1, F("$GREEN")); else Panel.send(Led_1, F("$OFF"));
  if(Still!=OFF) Still=WRITE; // if in Still just rewrite
}

//----------------------------------------------------------------------------
void SetStill()
{
  if(Power)
  { // only in power mode
    if(Still == OFF) 
    { // go get an image
      Still = GET;
    }
    if(Still == SHOW)
    { // toggle to off
      Still = OFF;
      Panel.send(Led_7, F("$OFF"));
    }
  }
}

//----------------------------------------------------------------------------
void SetRawMode()
{
  ImageModeSelect = RAW;
  Panel.send(Led_3, F("$YELLOW")); // set mode led to green to indicate raw
  Panel.send(Led_4, F("$OFF"));
  Panel.send(Led_5, F("$OFF")); // set mode led to blue indicating interpolation
  if(Power) sensor.setFPSMode(FPS_MODE::FPS_10);
  hoffset = ((255 - (16 * 8)) / 2) + 8 + 35; // set hor. image position
  voffset = ((220 - (16 * 8)) / 2) + 8;  // set vert. image position
  ScaleOffset = 30;
  TempScale(); 
  ip = 1;
  px = 16;
  if(Still!=OFF) Still=WRITE; // if in still mode just rewrite
}

//----------------------------------------------------------------------------
void SetInterpolate29Mode()
{
  ImageModeSelect = IP29; // indicate selected mode
  Panel.send(Led_3, F("$OFF")); // set mode leds
  Panel.send(Led_4, F("$YELLOW"));
  Panel.send(Led_5, F("$OFF"));
  if(Power)sensor.setFPSMode(FPS_MODE::FPS_1);
  hoffset = ((255 - (16 * 8)) / 2) + 8 + 35; // set hor. image position
  voffset = ((220 - (16 * 8)) / 2) + 12; // set vert. image position
  ScaleOffset = 30;
  TempScale(); 
  ip = 4;
  px = 4;
  if(Still!=OFF) Still=WRITE; // if in still mode just rewrite
}

//----------------------------------------------------------------------------
void SetInterpolate64Mode()
{
  ImageModeSelect = IP64;
  Panel.send(Led_3, F("$OFF"));
  Panel.send(Led_4, F("$OFF"));
  Panel.send(Led_5, F("$YELLOW"));
  if(Power) sensor.setFPSMode(FPS_MODE::FPS_1);
  hoffset = 57; // set hor. image position
  voffset = 17; // set vert. image position
  ScaleOffset = 0;
  TempScale(); 
  ip = 9;
  px = 3;
  if(Still!=OFF) Still=WRITE; // if in still mode just rewrite
}

//----------------------------------------------------------------------------
void SetScale(bool top, int value)
{ // handle temperature - color scale changes  
  if(top)
  {
    ScaleTop += (float)value;
    if (ScaleTop < ScaleBottom + 1.0) ScaleTop = ScaleBottom + 1.0;
  }
  else
  {
    ScaleBottom += (float)value;
    if (ScaleBottom > ScaleTop - 1.0) ScaleBottom = ScaleTop - 1.0;
  }
  Panel.sendf(Display_2, F("scale▲ %s°C"), _FString(ScaleTop,1,0));
  Panel.sendf(Display_4, F("scale▽ %s°C"), _FString(ScaleBottom,1,0));
  TempScale();
  if(Still!=OFF) Still=WRITE;
}

//----------------------------------------------------------------------------
void InitPanel()
{
  // configure panel
  Panel.send(ApplicationName,F("$10PT"));
  Panel.send(ApplicationName,F("AMG8833 thermal camera")); 
  Panel.send(Button_1,  F("△\nscale"));
  Panel.send(Button_2,  F("▽\nscale"));
  Panel.send(Button_9,  F("▲\nscale"));
  Panel.send(Button_10, F("▼\nscale"));
    
  Panel.send(Button_3, F("mirror"));
  Panel.send(Led_1, F("$OFF"));
  Panel.send(Button_5, F("raw\n8x8"));
  Panel.send(Led_3, F("$OFF"));
  Panel.send(Button_6, F("interp\n26x26"));
  Panel.send(Led_4, F("$OFF"));
  Panel.send(Button_7, F("interp\n64x64"));
  Panel.send(Led_5, F("$OFF"));
  Panel.send(Button_11, F("still"));
  Panel.send(Led_7, F("$OFF"));

  Panel.send(Button_15, F("info"));
  Panel.send(Button_17, F("$6PT"));
  Panel.send(Button_17, F("⚫⚪"));
  Panel.send(Led_9, F("$OFF"));
  Panel.send(Display_1, F("$SMALL"));
  Panel.send(Display_2, F("$SMALL"));
  Panel.send(Display_4, F("$SMALL"));
  Panel.send(Graph, true);
  SetScale(true, 0);
  PanelInit = true;
  SetRawMode();

  // info panel text
  Panel.send( InfoTitle, F("AMG8833 thermal camera"));
  Panel.send( InfoText,  F("Used with the AMG8833 Grid-EYE sensor by Panasonic"));
  Panel.send( InfoText,  F("Connects to GND, 5V, SDA, SCL\n"));
  Panel.send( InfoText,  F("scale△▽ sets temp./color scale bottom (scale▽ dispay)"));
  Panel.send( InfoText,  F("scale▲▼ sets temp./color scale top (scale▲ display)\n"));
  Panel.send( InfoText,  F("raw mode: 8x8 pixels 10 fps (no averaging)"));
  Panel.send( InfoText,  F("interpolation mode: 29x29 / 64x64 pixels 1 fps (roling average: 10 frames)\n"));
  Panel.send( InfoText,  F("Documentation:\nhttps://github.com/JaapDanielse/AMG8833-Thermal-Camera/wiki"));
}

//----------------------------------------------------------------------------
void TempScale()
{
  // output temperature - color scale
  ScaleStep = (ScaleTop - ScaleBottom) / 6; // calculate scale step (8 colors = 6 steps)
  
  Panel.send(Graph, F("$CLEAR")); // clear display
  Panel.send(GraphDrawPixel, F("$16PX")); // set 16 x 16 pixel size
  
  Panel.send(GraphDrawPixel, F("$DBLUE")); // set pixel color for all below bottom of scale
  Panel.send(GraphDrawPixel, _Point(ScaleOffset,54)); // write pixel
 
  Panel.send(GraphText, _Point(ScaleOffset+10,70)); // set text point
  Panel.sendf(GraphText, "%s°C", _FString(ScaleBottom + (ScaleStep * 0),3,1));  // write text

  Panel.send(GraphDrawPixel, F("$DPURPLE")); // set pixel color
  Panel.send(GraphDrawPixel, _Point(ScaleOffset,70)); // write pixel
  
  Panel.send(GraphText, _Point(ScaleOffset+10,86));
  Panel.sendf(GraphText, "%s°C", _FString(ScaleBottom + (ScaleStep * 1),3,1));
  
  Panel.send(GraphDrawPixel, F("$BLUE"));
  Panel.send(GraphDrawPixel, _Point(ScaleOffset,86));
    
  Panel.send(GraphText, _Point(ScaleOffset+10,102));
  Panel.sendf(GraphText, "%s°C", _FString(ScaleBottom + (ScaleStep * 2),3,1));
  
  Panel.send(GraphDrawPixel, F("$LBLUE"));
  Panel.send(GraphDrawPixel, _Point(ScaleOffset,102));
  
  Panel.send(GraphText, _Point(ScaleOffset+10,118));
  Panel.sendf(GraphText, "%s°C", _FString(ScaleBottom + (ScaleStep * 3),3,1));
  
  Panel.send(GraphDrawPixel, F("$GREEN"));
  Panel.send(GraphDrawPixel, _Point(ScaleOffset,118));
    
  Panel.send(GraphText, _Point(ScaleOffset+10,134));
  Panel.sendf(GraphText, "%s°C", _FString(ScaleBottom + (ScaleStep * 4),3,1));
  
  Panel.send(GraphDrawPixel, F("$YELLOW"));
  Panel.send(GraphDrawPixel, _Point(ScaleOffset,134));
    
  Panel.send(GraphText, _Point(ScaleOffset+10,150));
  Panel.sendf(GraphText, "%s°C", _FString(ScaleBottom + (ScaleStep * 5),3,1));
  
  Panel.send(GraphDrawPixel, F("$ORANGE"));
  Panel.send(GraphDrawPixel, _Point(ScaleOffset,150));
    
  Panel.send(GraphText, _Point(ScaleOffset+10,166));
  Panel.sendf(GraphText, "%s°C", _FString(ScaleBottom + (ScaleStep * 6),3,1));
  
  Panel.send(GraphDrawPixel, F("$RED"));  // for all above top of scale
  Panel.send(GraphDrawPixel, _Point(ScaleOffset,166));
}

// end module
