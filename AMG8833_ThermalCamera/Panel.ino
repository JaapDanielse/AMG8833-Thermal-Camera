// VirtualPanel control

//----------------------------------------------------------------------------
void PanelCallback(vp_channel event) 
{ 
  // handle panel events
  switch (event) 
  {
    case PanelConnected: InitPanel(); break; // on connection init panel
    case Button_1:  SetScale(false,  1); break; // handle scale bottom up 
    case Button_2:  SetScale(false, -1); break; // handle scale bottom down
    case Button_9:  SetScale(true,   1); break; // handle scale top up
    case Button_10: SetScale(true,  -1); break; // handle scale top down
    case Button_14: ToggleMode(true); break; // toggle raw / interpolate
    case Button_17: TogglePower(); break; // toggle power on / off
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
       Panel.sendf(MonitorLog,"frame speed status %s", sensor.getErrorDescription(statusCode)); //if error send to Log
     ToggleMode(false); // use toggle to set button text and FPS mode
   }
   else   
   {
     Panel.send(Led_9, F("$OFF")); // set power led to off  
     SetScale(true, 0); // write scale to Graph panel
     Panel.send(Display_1, F("")); // clear Display_1
     statusCode = sensor.setMode(DEVICE_MODE::SLEEP); // set sensor to sleep mode
     if(statusCode != 0) 
       Panel.sendf(MonitorLog,"frame speed status %s", sensor.getErrorDescription(statusCode)); //if error send to Log
  }
}

//----------------------------------------------------------------------------
void ToggleMode(bool toggle)
{
  // toggle mode (Raw / Interpolate)
  if(toggle) Raw = !Raw; 
  
  if(Raw)
  {
    Panel.send(Button_14, F("raw")); // set button text to indicate raw mode
    Panel.send(Led_8, F("$GREEN")); // set mode led to green to indicate raw
    statusCode = sensor.setFPSMode(FPS_MODE::FPS_10); // set FPS 10 (no average) raw is fast :)
    if(statusCode != 0) 
      Panel.sendf(MonitorLog,"frame speed status %s", sensor.getErrorDescription(statusCode)); //if error send to Log
  }
  else
  {
    Panel.send(Button_14, F("inter\npolate")); // set button text to indicate interpolation
    Panel.send(Led_8, F("$BLUE")); // set mode led to blue indicating interpolation
    statusCode = sensor.setFPSMode(FPS_MODE::FPS_1); // set FPS 1 (roling average) interpolation takes long :(
    if(statusCode != 0)  
      Panel.sendf(MonitorLog,"frame speed status %s", sensor.getErrorDescription(statusCode)); //if error send to Log
  } 
 
  TempScale(); 
}

//----------------------------------------------------------------------------
void SetScale(bool top, int value)
{
  // handle temperature - color scale changes  
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
}

//----------------------------------------------------------------------------
void InitPanel()
{
  // configure panel
  Panel.send(ApplicationName,F("$10PT"));
  Panel.send(ApplicationName,F("AMG8833 IR array sensor")); 
  Panel.send(Button_1,  F("△\nscale"));
  Panel.send(Button_2,  F("▽\nscale"));
  Panel.send(Button_9,  F("▲\nscale"));
  Panel.send(Button_10, F("▼\nscale"));
  Panel.send(Button_14, F("raw"));
  Panel.send(Led_8, F("$GREEN"));
  Panel.send(Button_17, F("$6PT"));
  Panel.send(Button_17, F("⚫⚪"));
  Panel.send(Led_9, F("$OFF"));
  Panel.send(Display_1, F("$SMALL"));
  Panel.send(Display_2, F("$SMALL"));
  Panel.send(Display_4, F("$SMALL"));
  Panel.send(Graph, true);
  SetScale(true, 0);
  PanelInit = true;
}

//----------------------------------------------------------------------------
void TempScale()
{
  // output temperature - color scale
  ScaleStep = (ScaleTop - ScaleBottom) / 6; // calculate scale step (8 colors = 6 steps)
  
  Panel.send(Graph, F("$CLEAR"));
  Panel.send(GraphDrawPixel, F("$16PX")); // 16 x 16 pixel size
  
  Panel.send(GraphDrawPixel, F("$DBLUE")); // set pixel color
  Panel.send(GraphDrawPixel, _Point(30,54)); // write pixel
 
  Panel.send(GraphText, _Point(40,70)); // set text point
  Panel.sendf(GraphText, "%s°C", _FString(ScaleBottom + (ScaleStep * 0),3,1));  // write text
  Panel.send(GraphDrawPixel, F("$DPURPLE")); // set pixel color
  Panel.send(GraphDrawPixel, _Point(30,70)); // write pixel
  
  Panel.send(GraphText, _Point(40,86));
  Panel.sendf(GraphText, "%s°C", _FString(ScaleBottom + (ScaleStep * 1),3,1));
  Panel.send(GraphDrawPixel, F("$BLUE"));
  Panel.send(GraphDrawPixel, _Point(30,86));
    
  Panel.send(GraphText, _Point(40,102));
  Panel.sendf(GraphText, "%s°C", _FString(ScaleBottom + (ScaleStep * 2),3,1));
  Panel.send(GraphDrawPixel, F("$LBLUE"));
  Panel.send(GraphDrawPixel, _Point(30,102));
  
  Panel.send(GraphText, _Point(40,118));
  Panel.sendf(GraphText, "%s°C", _FString(ScaleBottom + (ScaleStep * 3),3,1));
  Panel.send(GraphDrawPixel, F("$GREEN"));
  Panel.send(GraphDrawPixel, _Point(30,118));
    
  Panel.send(GraphText, _Point(40,134));
  Panel.sendf(GraphText, "%s°C", _FString(ScaleBottom + (ScaleStep * 4),3,1));
  Panel.send(GraphDrawPixel, F("$YELLOW"));
  Panel.send(GraphDrawPixel, _Point(30,134));
    
  Panel.send(GraphText, _Point(40,150));
  Panel.sendf(GraphText, "%s°C", _FString(ScaleBottom + (ScaleStep * 5),3,1));
  Panel.send(GraphDrawPixel, F("$ORANGE"));
  Panel.send(GraphDrawPixel, _Point(30,150));
    
  Panel.send(GraphText, _Point(40,166));
  Panel.sendf(GraphText, "%s°C", _FString(ScaleBottom + (ScaleStep * 6),3,1));
  Panel.send(GraphDrawPixel, F("$RED"));  
  Panel.send(GraphDrawPixel, _Point(30,166));
}

// end module
