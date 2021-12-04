/*
  Конечный автомат обработки данных АЦП:
  - температуры (MAdcT),
  - напряжения питания (MAdcPG).
  Бесконечно повторяющийся цикл.
*/
#include "board/mmeasure.h"
#include "mtools.h"
#include "board/mboard.h"
#include "state/mstate.h"
#include <Arduino.h>
#include <stdint.h>

MMeasure::MMeasure(MTools * tools) : Tools(tools), Board(tools->Board)
{
  State = new MMeasureStates::MAdcT(Tools);
}

void MMeasure::run()
{
  MState * newState = State->fsm();      
  if (newState != State)                      //state changed!
  {
    delete State;
    State = newState;
  } 
}

namespace MMeasureStates
{
  int holdTime = 0;

  MState * MAdcT::fsm()
  {
    // Вычисление реальной температуры, результат в celsius




      //Serial.println("adc");





    Board->calculateCelsius();              //Tools->setCelsius( Board->getAdcT() );
//  Board->lcdBlkOn();
//  Board->ledsYellow();
   
    return new MAdcPG(Tools);
  };

    // Измерение напряжения вторичного питания
  MState * MAdcPG::fsm()
  {
    //Tools->setPowerGood( Board->getAdcPG() );     // В MTools пока не реализовано - в MBoard
  
    //return new MAdcT(Tools);
//Board->ledsOff();
//Board->lcdBlkOff();
//Board->ledsOff();

    //return new MAdcPoff(Tools);                     //MAdcT(Tools);
    return new MAdcT(Tools);
  };





  // MState * MAdcPoff::fsm()
  // {

  //   // Проверка "нажатия" кнопки "P"
  //   if( Board->TouchP() ) 
  //   { 
  //     holdTime = 0;
  //     return new MAdcPon(Tools); 
  //   } 

  //   return new MAdcDNoff(Tools);
  // };

  // MState * MAdcPon::fsm()
  // {
  //   holdTime++;                       // Считать время удержания кнопки "P" в циклах активации задачи
  //   // Проверка "отпускания" кнопки "P"
  //   if( !Board->TouchP() ) 
  //   { 
  //     Board->setStatusP(holdTime);    // Установление стауса нажатия: P_CLICK, P_AUTO_CLICK, P_LONG_CLICK
  //     return new MAdcPoff(Tools); 
  //   } 

  //   return this;
  // };


  // MState * MAdcDNoff::fsm()
  // {
  //   // Проверка "нажатия" кнопки "DN"
  //   if( Board->TouchDN() )
  //   { 
  //     holdTime = 0;
  //     return new MAdcDNon(Tools);   // Обнаружено, задать подсчет времени удержания
  //   }

  //   return new MAdcBoff(Tools);     // Не обнаружено, задать проверку другой кнопки
  // };

  // MState * MAdcDNon::fsm()
  // {
  //   holdTime++;                       // Считать время удержания кнопки "DN" в циклах активации задачи
  //   // Проверка "отпускания" кнопки "DN"
  //   if( !Board->TouchDN() ) 
  //   { 
  //     Board->setStatusDN(holdTime);   // Установление стауса нажатия: DN_CLICK, DN_AUTO_CLICK, DN_LONG_CLICK
  //     return new MAdcDNoff(Tools); 
  //   } 

  //   return this;
  // };


  // MState * MAdcBoff::fsm()
  // {
  //   // Проверка "нажатия" кнопки "B"
  //   if( Board->TouchB() )
  //   { 
  //     holdTime = 0;
  //     return new MAdcBon(Tools);
  //   }

  //   return new MAdcUPoff(Tools);
  // };

  // MState * MAdcBon::fsm()
  // {
  //   holdTime++;                       // Считать время удержания кнопки "B"
  //   // Проверка "отпускания" кнопки "B"
  //   if( !Board->TouchB() ) 
  //   { 
  //     Board->setStatusB(holdTime);    // Столько циклов Кнопка "B" удерживалась
  //     return new MAdcBoff(Tools); 
  //   } 

  //   return this;
  // };


  // MState * MAdcUPoff::fsm()
  // {
  //   // Проверка "нажатия" кнопки "UP"
  //   if( Board->TouchUP() )
  //   { 
  //     holdTime = 0;
  //     return new MAdcUPon(Tools); 
  //   }
  //   return new MAdcCoff(Tools);
  // };

  // MState * MAdcUPon::fsm()
  // {
  //   holdTime++;                       // Считать время удержания кнопки "UP"
  //   // Проверка "отпускания" кнопки "UP"
  //   if( !Board->TouchUP() ) 
  //   { 
  //     Board->setStatusUP(holdTime);   // Столько циклов Кнопка "UP" удерживалась
  //     return new MAdcUPoff(Tools); 
  //   } 

  //   return this;
  // };


  // MState * MAdcCoff::fsm()
  // {
  //   // Проверка "нажатия" кнопки "C"
  //   if( Board->TouchC() )
  //   { 
  //     holdTime = 0;
  //     return new MAdcCon(Tools);
  //   }

  //   return new MAdcT(Tools);   //MAdcPoff(Tools);     // В начало цикла опроса
  // };

  // MState * MAdcCon::fsm()
  // {
  //   holdTime++;                       // Считать время удержания кнопки "C"
  //   // Проверка "отпускания" кнопки "C"
  //   if( !Board->TouchC() ) 
  //   { 
  //     Board->setStatusC(holdTime);    // Столько циклов Кнопка "C" удерживалась
  //     return new MAdcCoff(Tools); 
  //   } 

  //   return this;
  // };

};
