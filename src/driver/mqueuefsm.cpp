/*
  mqueuefsm.cpp
  Конечный автомат очереди команд
  08.06.2022
*/

#include "driver/mqueuefsm.h"
//#include "nvs.h"
#include "mtools.h"
#include "mcmd.h"
//#include "board/mboard.h"
//#include "board/msupervisor.h"
//#include "board/mkeyboard.h"
#include "display/mdisplay.h"
#include <Arduino.h>
#include <string>

namespace QueueFsm
{
  // Состояние "Старт", инициализация.
  MStart::MStart(MTools * Tools) : MState(Tools)
  {}
  MState * MStart::fsm()
  {
    return new MGetU(Tools);
  };

  // Состояние "Фоновая команда читать напряжение"
  MGetU::MGetU(MTools * Tools) : MState(Tools)
  {}
  MState * MGetU::fsm()
  {
    // MCmd::cmd_get_u           // 0x11 Чтение напряжения (мВ);
    return new MGetSU(Tools);
  };

  // Состояние "Фоновая команда читать состояние"
  MGetSU::MGetSU(MTools * Tools) : MState(Tools)
  {}
  MState * MGetSU::fsm()
  {
    // MCmd::cmd_get_state       // 0x14 Чтение состояния;
    return new MGetI(Tools);
  };

  // Состояние "Фоновая команда читать ток"
  MGetI::MGetI(MTools * Tools) : MState(Tools)
  {}
  MState * MGetI::fsm()
  {
    //  MCmd::cmd_get_i           // 0x12 Чтение тока (мА)
    return new MGetSI(Tools);
  };

  // Состояние "Фоновая команда читать состояние"
  MGetSI::MGetSI(MTools * Tools) : MState(Tools)
  {}
  MState * MGetSI::fsm()
  {
    // MCmd::cmd_get_state       // 0x14 Чтение состояния;
    return new MGetT(Tools);
  };

  // Состояние "Фоновая команда читать температуру"
  MGetT::MGetT(MTools * Tools) : MState(Tools)
  {}
  MState * MGetT::fsm()
  {
    //  MCmd::cmd_get_celsius     // 0x15 Чтение температуры радиатора
    return new MGetST(Tools);
  };

  // Состояние "Фоновая команда читать состояние"
  MGetST::MGetST(MTools * Tools) : MState(Tools)
  {}
  MState * MGetST::fsm()
  {
    // MCmd::cmd_get_state       // 0x14 Чтение состояния;
    return new MExt(Tools);
  };

  // Состояние "Нерегулярная команда"
  MExt::MExt(MTools * Tools) : MState(Tools)
  {}
  MState * MExt::fsm()
  {
    //  
    return new MGetSE(Tools);
  };

  // Состояние "Фоновая команда читать состояние"
  MGetSE::MGetSE(MTools * Tools) : MState(Tools)
  {}
  MState * MGetSE::fsm()
  {
    // MCmd::cmd_get_state       // 0x14 Чтение состояния;
    return new MGetU(Tools);
  };
 
};  //MExit

// !Конечный автомат очереди команд