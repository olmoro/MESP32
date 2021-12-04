/*
    Дисплей TFT 1.8" 4-SPI 128*160
    HSPI
    2020.02.09 2021.03.24
    OlMoro
*/

    #include "mdisplay.h"
    #include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
    //  #include "User_Setup.h"   // Подключение задано в .ini
    #include <SPI.h>
    #include <Arduino.h>

    //  SPIClass hspi(HSPI);   // Подключение задано в User_Setup.h

    MDisplay::MDisplay()
    {
        tft = new TFT_eSPI();         // Invoke library, pins defined in platformio.ini
        initTFT();
    }

    MDisplay::~MDisplay()   { delete tft; }


#ifdef CLOCK
uint8_t MDisplay::conv2d(const char* p) 
{
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

void MDisplay::initTFT()
{
  tft->init();
  tft->setRotation(2);                      // (0)  moro=2 поворот на 180°  (Alt+0176)
  tft->fillScreen(TFT_BLACK);                // Цвет фона вне круга
  tft->setTextColor(TFT_GREEN, TFT_BLACK);  // Adding a black background colour erases previous text automatically
  
  // Draw clock face
  tft->fillCircle(64, 64, 61, TFT_BLUE);    // Внешний диск
  tft->fillCircle(64, 64, 57, TFT_BLACK);   // Внутренний диск

  // Draw 12 lines
  for(int i = 0; i < 360; i += 30) 
  {
    sx  = cos((i-90)*0.0174532925);
    sy  = sin((i-90)*0.0174532925);
    x0  = sx*57 + 64;
    yy0 = sy*57 + 64;
    x1  = sx*50 + 64;
    yy1 = sy*50 + 64;

    tft->drawLine(x0, yy0, x1, yy1, TFT_BLUE);
  }

  // Draw 60 dots
  for(int i = 0; i < 360; i += 6) 
  {
    sx  = cos((i-90)*0.0174532925);
    sy  = sin((i-90)*0.0174532925);
    x0  = sx*53 + 64;
    yy0 = sy*53 + 64;
    
    tft->drawPixel(x0, yy0, TFT_BLUE);                            // Минутные точки
    if(i ==  0 || i == 180) tft->fillCircle(x0,   yy0, 1, TFT_CYAN);     // 
    if(i ==  0 || i == 180) tft->fillCircle(x0+1, yy0, 1, TFT_CYAN);
    if(i == 90 || i == 270) tft->fillCircle(x0,   yy0, 1, TFT_CYAN);
    if(i == 90 || i == 270) tft->fillCircle(x0+1, yy0, 1, TFT_CYAN);
  }

  tft->fillCircle(65, 65, 2, TFT_RED);    // Кружочек в центре

  targetTime = millis() + 1000; 

}

void MDisplay::runClock()
{
  if ( targetTime < millis() ) 
  {
    targetTime = millis() + 1000;
    ss++;              // Advance second
    if ( ss == 60 ) 
    {
      ss = 0;
      mm++;            // Advance minute
      if ( mm > 59 ) 
      {
        mm = 0;
        hh++;          // Advance hour
        if ( hh > 23 ) 
        {
          hh = 0;
        }
      }
    }

    // Pre-compute hand degrees, x & y coords for a fast screen update
    sdeg = ss*6;                  // 0-59 -> 0-354
    mdeg = mm*6+sdeg*0.01666667;  // 0-59 -> 0-360 - includes seconds
    hdeg = hh*30+mdeg*0.0833333;  // 0-11 -> 0-360 - includes minutes and seconds
    hx = cos((hdeg-90)*0.0174532925);    
    hy = sin((hdeg-90)*0.0174532925);
    mx = cos((mdeg-90)*0.0174532925);    
    my = sin((mdeg-90)*0.0174532925);
    sx = cos((sdeg-90)*0.0174532925);    
    sy = sin((sdeg-90)*0.0174532925);

    if (ss == 0 || initial) 
    {
      initial = 0;
      // Erase hour and minute hand positions every minute
      tft->drawLine(ohx, ohy, 65, 65, TFT_BLACK);
      ohx = hx*33 + 65;    
      ohy = hy*33 + 65;
      tft->drawLine(omx, omy, 65, 65, TFT_BLACK);
      omx = mx*44 + 65;    
      omy = my*44 + 65;
    }

    // // Draw text at position 64,125 using fonts 4
    // // Only font numbers 2,4,6,7 are valid. Font 6 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : . a p m
    // // Font 7 is a 7 segment font and only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : .
    // //tft->drawCentreString("Time flies",64,130,4);
    // tft->drawCentreString("22.2",64,40,6);

    // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
    // Перерисовывайте новые позиции стрелок, часовые и минутные стрелки здесь не стираются,
    // чтобы избежать мерцания
    tft->drawLine(osx, osy, 65, 65, TFT_BLACK);
    tft->drawLine(ohx, ohy, 65, 65, TFT_WHITE);
    tft->drawLine(omx, omy, 65, 65, TFT_WHITE);
    osx = sx*47+65;    
    osy = sy*47+65;
    tft->drawLine(osx, osy, 65, 65, TFT_RED);

//  tft->fillCircle(sx*60+65, sy*60+65, 2, TFT_GREEN);    // Кружочек ротации

    tft->fillCircle(65, 65, 2, TFT_RED);      // Точка в центре
  }
}

#endif

void MDisplay::initLCD()
{
    initTFT();                  // включая CLOCK
}

//void MDisplay::runDisplay(float u, float i, float celsius, int time, float ah, int percent, bool ap)
void MDisplay::runDisplay( float celsius, bool ap)
{
    runClock();
    displayVoltage();
    displayCurrent();
    displayMode();
    displayHelp();
    displayFulfill();
    displayDuration();
    displayAmphours();
    displayHeap();
    displayCelsius( celsius );
    displayLabel();
}

void MDisplay::showVolt( float _volt, uint8_t _places )
{
    volt    = _volt;
    placesV = _places;
}

// Пока без центровки в строке
void MDisplay::displayVoltage()
{
    uint8_t xpos = MVoltage::po_x;
    uint8_t ypos = MVoltage::po_y;
    tft->setTextColor( MVoltage::fgcolor, MVoltage::bgcolor ); // Set foreground and backgorund colour
    xpos += tft->drawFloat( volt, placesV, xpos, ypos, MVoltage::font );
    xpos += tft->drawString(        " V ", xpos, ypos, MVoltage::font );
}

void MDisplay::showAmp( float _amp, uint8_t _places )
{
    amp     = _amp;
    placesI = _places;
}

void MDisplay::displayCurrent()
{
    uint8_t xpos = MCurrent::po_x;
    uint8_t ypos = MCurrent::po_y;
    tft->setTextColor( MCurrent::fgcolor, MCurrent::bgcolor ); // Set foreground and backgorund colour
    xpos += tft->drawFloat( amp, placesI, xpos, ypos, MCurrent::font );
    xpos += tft->drawString(        " A ", xpos, ypos, MCurrent::font );
}

void MDisplay::showMode( char *s ) { strcpy( modeString, s ); }

void MDisplay::displayMode()
{
    uint8_t xpos = MMode::po_x;
    uint8_t ypos = MMode::po_y;
    tft->setTextColor( MMode::fgcolor, MMode::bgcolor ); // Set foreground and backgorund colour
    tft->drawCentreString( modeString, xpos, ypos, MMode::font ); // 16 chars only
}

void MDisplay::showHelp( char *s ) { strcpy( helpString, s ); }

void MDisplay::displayHelp()
{
    uint8_t xpos = MHelp::po_x;
    uint8_t ypos = MHelp::po_y;
    tft->setTextColor( MHelp::fgcolor, MHelp::bgcolor ); // Set foreground and backgorund colour
    tft->drawString( helpString, xpos, ypos, MHelp::font );
}


void MDisplay::fulfill( int _val, uint16_t _color )
{
    percent = _val;
    color   = _color;
}

void MDisplay::displayFulfill()
{
    //    uint32_t color = TFT_RED;           // TEST
    //    percent = 50;

    int x = percent + MBar::min_x;
    int maxX = 127; //100 - percent;

    for (int i = 0; i <= 5; i++)
    {
        int y = MBar::po_y + i;

        tft->drawFastHLine( MBar::min_x, y, x, color );
        tft->drawFastHLine( x, y, maxX, TFT_BLACK );        //TFT_DARKGREY ); 
    } 
        //Serial.println(x);

//   int32_t x = 20;
//   int32_t y = 90;
//   int32_t h = 94;
//   int32_t w = 108;
//   for (y = 90; y < h; y++)
//     tft->drawFastHLine(x, y, w, TFT_RED);
}

void MDisplay::showDuration( int duration, int _plan )
{
    upSeconds = duration;
    plan = _plan;                   // 0 - hhh:mm:ss,  1 - hhh
}

void MDisplay::displayDuration()
{
    uint8_t xpos = MDuration::po_x;
    uint8_t ypos = MDuration::po_y;
    tft->setTextColor( MDuration::fgcolor, MDuration::bgcolor ); // Set foreground and backgorund colour

        upSeconds = upSeconds % 86400;          // С учетом периода вызова задачи
        unsigned long hours = upSeconds / 3600;
        upSeconds = upSeconds % 3600;
        unsigned long minutes = upSeconds / 60;
        upSeconds = upSeconds % 60;
    
    if( plan == SEC )
    {
        // hours:minutes:seconds
        xpos += tft->drawNumber(     hours, xpos, ypos, MDuration::font );
        xpos += tft->drawChar(         ':', xpos, ypos, MDuration::font );
        if( minutes <= 9 ) 
            xpos += tft->drawChar(     '0', xpos, ypos, MDuration::font );
        xpos += tft->drawNumber(   minutes, xpos, ypos, MDuration::font );
        xpos += tft->drawChar(         ':', xpos, ypos, MDuration::font );
        if( upSeconds <= 9 ) 
            xpos += tft->drawChar(     '0', xpos, ypos, MDuration::font );
        xpos += tft->drawNumber( upSeconds, xpos, ypos, MDuration::font );
    }
    else
    {
        // hours
        xpos += 5;
        xpos += tft->drawNumber(    hours, xpos, ypos, MDuration::font );
        xpos += tft->drawString( " hours", xpos, ypos, MDuration::font );
    }       
}

void MDisplay::showAh( float val ) { ah = val; } 

void MDisplay::displayAmphours()
{
    uint8_t xpos = MAmpHours::po_x;
    uint8_t ypos = MAmpHours::po_y;
    tft->setTextColor( MAmpHours::fgcolor, MAmpHours::bgcolor ); // Set foreground and backgorund colour
    xpos += tft->drawFloat(  ah, 1, xpos, ypos, MAmpHours::font );
    xpos += tft->drawString( " Ah", xpos, ypos, MAmpHours::font );
}

void MDisplay::displayHeap()
{
    uint8_t xpos = MHeap::po_x;
    uint8_t ypos = MHeap::po_y;
    tft->setTextColor( MHeap::fgcolor, MHeap::bgcolor ); // Set foreground and backgorund colour
    tft->drawNumber( ESP.getFreeHeap(), xpos, ypos, MAmpHours::font );
}

void MDisplay::displayCelsius( float celsius )
{
    uint8_t xpos = MCelsius::po_x;
    uint8_t ypos = MCelsius::po_y;
    tft->setTextColor( MCelsius::fgcolor, MCelsius::bgcolor ); // Set foreground and backgorund colour
    xpos += tft->drawFloat( celsius, 1, xpos, ypos, MCelsius::font );
    xpos += tft->drawString(      " C", xpos, ypos, MCelsius::font );
}

void MDisplay::showLabel( char *s ) { strcpy( labelString, s ); }

void MDisplay::displayLabel()
{
    uint8_t xpos = MLabel::po_x;
    uint8_t ypos = MLabel::po_y;
    tft->setTextColor( MLabel::fgcolor, MLabel::bgcolor ); // Set foreground and backgorund colour
    tft->drawCentreString( labelString, xpos, ypos, MLabel::font ); // 16 chars only
}


//void MDisplay::getTextLabel( char *s ) { strcpy( newLabelString, s ); }
void MDisplay::barOff() { fulfill( 0, TFT_DARKGREY ); }

void MDisplay::initBar( uint16_t color )
{
    static int x = 0;
    x += 4;
    if (x >= 100 ) x = 0;
    fulfill( x, color );
}

void MDisplay::barStop() 
{ 
    fulfill( 100, TFT_RED ); 
}
