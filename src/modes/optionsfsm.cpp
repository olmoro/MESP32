/*
    ВВод и коррекция пользовательских параметров
  Вариант 202207
*/

#include "modes/optionsfsm.h"
//#include "nvs.h"
#include "mtools.h"
#include "mcmd.h"
#include "board/mboard.h"
#include "board/mkeyboard.h"
#include "display/mdisplay.h"
#include <Arduino.h>
#include <string>


namespace MOption
{
  // Состояние "Старт", инициализация выбранного режима работы.
  MStart::MStart(MTools * Tools) : MState(Tools)
  {
    #ifdef DEBUG_OPTIONS
      Serial.println("Options: Start");
    #endif
    cnt = 7;
      // Индикация
    Display->showMode( (char*) " OPTIONS SELECTED " );
    Display->showHelp( (char*) "            C-GO  " );
    Board->ledsOn();
  }
  MState * MStart::fsm()
  {
    switch ( Keyboard->getKey() )
    {
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();            return new MStop(Tools);

      case MKeyboard::C_CLICK: Board->buzzerOn();                 return new MSetPostpone(Tools);

      case MKeyboard::B_CLICK: Board->buzzerOn();
        if(--cnt <= 0)                                                      return new MClearCccvKeys(Tools);
        break;

      default:;
    }

    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp (Tools->getRealCurrent(), 3);                return this;
  };

    //================================================================================ MClearCccvKeys

  MClearCccvKeys::MClearCccvKeys(MTools * Tools) : MState(Tools)
  {
    Display->showMode((char*)"      CLEAR?      ");   // В каком режиме
    Display->showHelp((char*)"  P-NO     C-YES  ");   // Активные кнопки
    Board->ledsBlue();
    cnt = 50;                                         // 5с 
  }
  MState * MClearCccvKeys::fsm()
  {
    switch  (Keyboard->getKey())
    {
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
    case MKeyboard::P_CLICK: Board->buzzerOn();                       return new MSetPostpone(Tools);
    case MKeyboard::C_CLICK: Board->buzzerOn();
      done = Tools->clearAllKeys("options");
      vTaskDelay(2 / portTICK_PERIOD_MS);
      #ifdef TEST_KEYS_CLEAR
        Serial.print("\nAll keys \"options\": ");
        (done) ? Serial.println("cleared") : Serial.println("err");
      #endif
      break;
    default:                                                          break;
    }
    if(--cnt <= 0)                                                    return new MStart(Tools);
    Display->showMode((char*)"     CLEARING     ");   // В каком режиме
    Display->showHelp((char*)"    ___WAIT___    ");   // Активные кнопки - нет
    return this;
  };





MSetPostpone::MSetPostpone(MTools * Tools) : MState(Tools) 
{
//    pp = Tools->readNvsInt("qulon", "postp", fixed);
      // Подсказка
    Display->showMode((char*) "  POSTPONE      +/-  ");
    Display->showHelp((char*) "  P-DEFINE   C-EXIT  ");
    Board->ledsGreen();

}
MState * MSetPostpone::fsm()
{
    switch (Keyboard->getKey())
    {
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();              return new MExit(Tools);

    case MKeyboard::P_CLICK: Board->buzzerOn();                   return new MExit(Tools); // M...(Tools);

    case MKeyboard::B_CLICK: Board->buzzerOn();       
//      Tools->writeNvsShort("qulon", "postp", pp);                       return new MExit(Tools); //M...(Tools);

    case MKeyboard::UP_CLICK: Board->buzzerOn();
    case MKeyboard::UP_AUTO_CLICK: 
      pp = Tools->updnInt(pp, below, above, +1);
      Tools->setPostpone(pp);                                     break;

    case MKeyboard::DN_CLICK: Board->buzzerOn();
    case MKeyboard::DN_AUTO_CLICK:
      pp = Tools->updnInt(pp, below, above, -1);
      Tools->setPostpone(pp);                                     break;
    
    default:;
    }
//Serial.print("\npostpone="); Serial.print(Tools->getPostpone());
    Display->showDuration(Tools->getPostpone() * 3600, MDisplay::SEC);
    return this;
};

 








  // Завершение режима - до нажатия кнопки "С" удерживается индикация 
  // о ...
  // Состояние: "Завершение "
  MStop::MStop(MTools * Tools) : MState(Tools)
  {
    //Tools->shutdownCharge();

    Display->showMode( (char*) "     OPTIONS:     " );
    Display->showHelp( (char*) "      C-STOP      " );
    Board->ledsRed();
  }    
  MState * MStop::fsm()
  {
    switch ( Keyboard->getKey() )
    {
      case MKeyboard::C_CLICK:  Board->buzzerOn();  
      return new MExit(Tools);

      case MKeyboard::P_CLICK:  Board->buzzerOn();  
      return new MStart(Tools);                       // Вернуться в начало

      default:;
    }
    return this;
  };


  // Выход из режима - до нажатия кнопки "С" удерживается индикация 
  MExit::MExit(MTools * Tools) : MState(Tools)
  {
    // Индикация помощи
    Display->showMode( (char*) "     OPTIONS:     " );
    Display->showHelp( (char*) " C-RET TO SELECT  " );
  }      
  MState * MExit::fsm()
  {
    switch ( Keyboard->getKey() )
    {
      case MKeyboard::C_CLICK: Board->buzzerOn();
          Board->ledsOff();

        // Надо бы восстанавливать средствами диспетчера...
        Display->showMode( (char*) "     OPTIONS:     " );
        Display->showHelp( (char*) "   U/D B-SELECT   " );
      return nullptr;                             // Возврат к выбору режима
      default:;
    }
    return this;
  };

};

