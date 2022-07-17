/*
  Файл: devicefsm.cpp 17.07.2022
  Конечный автомат заводских регулировок - арсенал разработчика (ручной режим):
  - коррекция смещения АЦП в ручном и автоматическом режимах;
  - коррекция приборного смещения и коэффициента преобразования по напряжению;
  - коррекция приборного смещения и коэффициента преобразования по току;
  - коррекция коэффициента фильтрации измерений;
  - 
*/

#include "modes/devicefsm.h"
#include "mtools.h"
  //#include "mcmd.h"
#include "board/mboard.h"
#include "board/mkeyboard.h"
#include "display/mdisplay.h"
  //#include "board/msupervisor.h"
#include <Arduino.h>
  //#include <string>

namespace Device
{

  short offset, factor, smooth, par;   // Можно использовать один - par

    // Состояние "Старт", инициализация выбранного режима работы (DEVICE).
  MStart::MStart(MTools * Tools) : MState(Tools)
  {
      //Отключить на всякий пожарный
    Tools->powerStop();                                 // 0x21  Команда драйверу

    // При первом включении, как правило заводском, номиналы батареи задаются в mdispather.h. 
    par = Tools->readNvsFloat("qulon", "offsetAdc", 0);
    Serial.println(); Serial.print("offsetAdc"); Serial.println(par);

    // Индикация
    Display->showMode((char*)"   DEVICE START   ");  // В каком режиме
    Display->showHelp((char*)"    P-ADJ   C-GO  ");  // Активные кнопки
    Display->barOff();
    Board->ledsOn();              // Подтверждение входа в настройки заряда белым свечением светодиода
  }
  MState * MStart::fsm()
  {
    switch ( Keyboard->getKey() )    //Здесь так можно
    {
        // Отказ от продолжения ввода параметров - стоп
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
        // Старт автоматической коррекции
      case MKeyboard::C_CLICK: Board->buzzerOn();                       return new MAuto(Tools);
        // Ручная коррекция смещения АЦП
      case MKeyboard::P_CLICK: Board->buzzerOn();                       return new MManual(Tools);
      default:;
    }
    // Индикация текущих значений, указывается число знаков после запятой
    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp (Tools->getRealCurrent(), 2);                      return this;
  };

    // Выбран режим автоматической коррекции смещения АЦП
  MAuto::MAuto(MTools * Tools) : MState(Tools)
  {

      // Индикация
    Display->showMode((char*)"       AUTO       ");  // В каком режиме
    Display->showHelp((char*)"  IN DEVELOPMENT  ");  // Активные кнопки
    Display->initBar(TFT_GREEN);
    Board->ledsGreen();              // Подтверждение
  }
  MState * MAuto::fsm()
  {
    switch (Keyboard->getKey())    //Здесь так можно
    {
      // Отказ от продолжения ввода параметров - стоп
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
    case MKeyboard::C_CLICK: Board->buzzerOn();                       return new MStop(Tools);
    case MKeyboard::B_CLICK: Board->buzzerOn();                       return new MStop(Tools);
    case MKeyboard::UP_CLICK: Board->buzzerOn();                      return new MStop(Tools);
    case MKeyboard::DN_CLICK: Board->buzzerOn();                      return new MStop(Tools);

 

    default:;
    }
    return this;
  };

    // Выбран режим ручной коррекции смещения АЦП
  MManual::MManual(MTools * Tools) : MState(Tools)
  {

      // Индикация
    Display->showMode((char*)"      MANUAL      ");  // В каком режиме
    Display->showHelp((char*)"    P-ADJ   C-GO  ");  // Активные кнопки
    Board->ledsGreen();              // Подтверждение
  }
  MState * MManual::fsm()
  {
    switch (Keyboard->getKey())
    {
      // Отказ от продолжения ввода параметров - стоп
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
     // Сохранить и перейти к следующему состоянию
    case MKeyboard::B_CLICK: Board->buzzerOn();
      Tools->saveFloat("qulon", "offsetAdc", par );                   return new MOffsetU(Tools);
  case MKeyboard::UP_CLICK: Board->buzzerOn();
    par = Tools->upiVal(par, MConst::adc_l, MConst::adc_h, 1);   break;
  case MKeyboard::DN_CLICK: Board->buzzerOn();
    par = Tools->dniVal(par, MConst::adc_l, MConst::adc_h, 1);     break;
     


    default:;
    }
    return this;
  };  

  MOffsetU::MOffsetU(MTools * Tools) : MState(Tools)
  {

  }
  MState * MOffsetU::fsm()
  {
    
    return this;
  };




  // Завершение режима DEVICE - до нажатия кнопки "С" удерживается индикация 
  MStop::MStop(MTools * Tools) : MState(Tools)
  {
    Display->showMode((char*) "       READY      ");
    Display->showHelp((char*) "      C-EXIT      ");
    Display->barStop();
    Board->ledsRed();
  }    
  MState * MStop::fsm()
  {
    switch ( Keyboard->getKey() )
    {
    case MKeyboard::C_CLICK:  Board->buzzerOn();        return new MExit(Tools);
    default:;
    }
    return this;
  };  //MStop

  // Процесс выхода из режима - до нажатия кнопки "С" удерживается индикация
  // о завершении.
  // Состояние: "Индикация итогов и выход из режима в меню диспетчера" 
  MExit::MExit(MTools * Tools) : MState(Tools)
  {
    //Tools->shutdownCharge();
    Display->showMode((char*)" DEVICE MODE OFF  ");
    Display->showHelp((char*)"  P-AGAIN  C-EXIT ");  // To select the mode
    Board->ledsOff();
    Display->barOff();
  }    
  MState * MExit::fsm()
  {
    switch ( Keyboard->getKey() )
    {
      // Вернуться в начало
    case MKeyboard::P_CLICK:  Board->buzzerOn();        return new MStart(Tools);

    case MKeyboard::C_CLICK:  Board->buzzerOn(); 
        //Tools->activateExit(" ");    // Можно сделать лучше, гасит светодиоды
      // Надо бы восстанавливать средствами диспетчера...
      Display->showMode( (char*) "   ????????????   " );
      Display->showHelp( (char*) "     B-SELECT     " );    return nullptr;
    default:;
    }
    return this;
  };

};
