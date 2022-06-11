/*


*/

#include "modes/optionsfsm.h"
#include "nvs.h"
#include "mtools.h"
#include "mcmd.h"
#include "board/mboard.h"
#include "board/mkeyboard.h"
#include "display/mdisplay.h"
#include <Arduino.h>
#include <string>


namespace OptionFsm
{
  // static int mode = QULON;
  // char sName[ 15 ] = { 0 };   // Ограничение ESP32


  // Состояние "Старт", инициализация выбранного режима работы.
  MStart::MStart(MTools * Tools) : MState(Tools)
  {
    #ifdef DEBUG_OPTIONS
      Serial.println("Options: Start");
    #endif
      // Индикация
    Display->showMode( (char*) " OPTIONS SELECTED " );
    Display->showHelp( (char*) "  P-NEXT  C-STOP  " );
    Board->ledsOn();
  }
  MState * MStart::fsm()
  {
    switch ( Keyboard->getKey() )
    {
      case MKeyboard::C_CLICK: Board->buzzerOn();
      return new MStop(Tools);

      case MKeyboard::P_CLICK: Board->buzzerOn();
          // Продолжение выбора объекта настройки
      return new MSetPostpone(Tools);

      // case MKeyboard::B_CLICK: Board->buzzerOn();
      //   Tools->setToQueue(MCmd::cmd_read_offset_u);       // Взять актуальное смещение
      // return new MSetVoltageOffset(Tools);
      
      default:;
    }
    return this;
  };


MSetPostpone::MSetPostpone(MTools * Tools) : MState(Tools) 
{
    // Подсказка
    Display->showMode( (char*) "  U/D-SET POSTPONE   " );
    Display->showHelp( (char*) "  P-DEFINE   C-EXIT  " );
}
MState * MSetPostpone::fsm()
{
    switch ( Keyboard->getKey() )
    {
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();
    return new MExit(Tools);

    case MKeyboard::P_CLICK: Board->buzzerOn();
    return new MSetVoltageOffset(Tools);

    case MKeyboard::UP_CLICK:
    case MKeyboard::UP_AUTO_CLICK: Board->buzzerOn();
      Tools->postpone = Tools->upiVal( Tools->postpone, MOptConsts::ppone_l, MOptConsts::ppone_h, 1 );
    break;

    case MKeyboard::DN_CLICK:
    case MKeyboard::DN_AUTO_CLICK: Board->buzzerOn();
      Tools->postpone = Tools->dniVal( Tools->postpone, MOptConsts::ppone_l, MOptConsts::ppone_h, 1 );
    break;

    case MKeyboard::B_CLICK :
      Tools->saveInt( MNvs::nQulon, MNvs::kQulonPostpone, Tools->postpone );   // Выбор заносится в энергонезависимую память
        // #ifdef DEBUG_OPTIONS
        //     Serial.println(Tools->postpone);
        // #endif
    return new MSetVoltageOffset(Tools);
    
    default:;
    }

    Display->showDuration( Tools->postpone, MDisplay::HOUR );
    return this;
};







  MSetVoltageOffset::MSetVoltageOffset( MTools * Tools ) : MState(Tools)
  {
    // Смещение восстановлено из nvs при инициализации прибора
    // Индикация помощи
    Display->showMode( (char*) "  VOLTAGE OFFSET  " );
    Display->showHelp( (char*) " B-Y P-NEX C-STOP " );
        Board->ledsGreen();
  }
  MState * MSetVoltageOffset::fsm()
  {
    switch( Keyboard->getKey() )
    {
    case MKeyboard::C_CLICK: Board->buzzerOn();
    return new MStop(Tools);
    
    case MKeyboard::P_CLICK: Board->buzzerOn();
    return new MSetVoltageFactor(Tools);
    
    case MKeyboard::UP_CLICK: Board->buzzerOn();
      // Смещение по 10 мВ за шаг
      Tools->offsetV = Tools->upiVal( Tools->offsetV, MOptConsts::offset_v_l, MOptConsts::offset_v_h, 10 );
//      Tools->setToQueue(MCmd::cmd_write_offset_u);
    break;
    
    case MKeyboard::DN_CLICK: Board->buzzerOn();
      // Смещение по 10 мВ за шаг
      Tools->offsetV = Tools->dniVal( Tools->offsetV, MOptConsts::offset_v_l, MOptConsts::offset_v_h, 10 );
//      Tools->setToQueue(MCmd::cmd_write_offset_u);
    break;
    
    case MKeyboard::B_CLICK: Board->buzzerOn();
      Tools->saveInt( MNvs::nQulon, MNvs::kOffsetV, Tools->offsetV );
        // #ifdef DEBUG_OPTIONS
        //     Serial.println(MTools::offsetV);
        // #endif
    return this;      //return new MStop(Tools);

    default:;
    }
    // Изменение смещения отображается на текущем значении (спорно)
    Display->showVolt( Tools->getRealVoltage(), 2 );
    Display->showAmp( Tools->getRealCurrent(), 1 );
    return this;
  };

  MSetVoltageFactor::MSetVoltageFactor( MTools * Tools ) : MState(Tools)
  {
      // Смещение восстановлено из nvs при инициализации прибора
      // Индикация помощи
      Display->showMode( (char*) "  VOLTAGE FACTOR  " );
      Display->showHelp( (char*) " B-YES P-NO C-EX  " );
  }
  MState * MSetVoltageFactor::fsm()
  {
    switch( Keyboard->getKey() )
    {
    case MKeyboard::C_CLICK : Board->buzzerOn();
    return new MStop(Tools);
    
    // case MKeyboard::P_CLICK : Board->buzzerOn();
    // return new MSetVoltageSmooth(Tools);
    
    // case MKeyboard::UP_CLICK : Board->buzzerOn();
    //   //Tools->incVoltageOffset( 0.01f, false );
    //   MTools::offsetV = Tools->upfVal( MTools::offsetV, MOptConsts::offset_v_l, MOptConsts::offset_v_h, 0.01f );
    // break;
    
    // case MKeyboard::DN_CLICK : Board->buzzerOn();
    //   //Tools->decVoltageOffset( 0.01f, false );
    //   MTools::offsetV = Tools->dnfVal( MTools::offsetV, MOptConsts::offset_v_l, MOptConsts::offset_v_h, 0.01f );
    // break;
    
    // case MKeyboard::B_CLICK : Board->buzzerOn();
    //   Tools->saveInt( MNvs::nQulon, MNvs::kOffsetV, MTools::offsetV );
    //     // #ifdef DEBUG_OPTIONS
    //     //     Serial.println(MTools::offsetV);
    //     // #endif
    //  return new MStop(Tools);
    default :;
    }


    return this;
  };









  // Завершение режима - до нажатия кнопки "С" удерживается индикация 
  // о ...
  // Состояние: "Завершение "
  MStop::MStop(MTools * Tools) : MState(Tools)
  {
    Tools->shutdownCharge();

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

