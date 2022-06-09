/*
  morder.cpp
  Конечный автомат вызова команд
  Бесконечно повторяющийся цикл.
  09.06.2022
*/

#include "driver/morder.h"
//#include "nvs.h"
#include "mtools.h"
#include "mcmd.h"
//#include "board/mboard.h"
//#include "board/msupervisor.h"
//#include "board/mkeyboard.h"

#include "display/mdisplay.h"
#include <Arduino.h>
#include <stdint.h>
//#include <string>

MOrder::MOrder(MTools * tools) : Tools(tools), Board(tools->Board)
{
  State = new MOrderStates::MGetU(Tools);
}

void MOrder::run()
{
  MState * newState = State->fsm();      
  if (newState != State)                      //state changed!
  {
    delete State;
    State = newState;
  } 
}

namespace MOrderStates
{
  // Состояние "Фоновая команда читать напряжение"
  MState * MGetU::fsm()
  {
    //Serial.println(); Serial.print("#");
    Tools->setToQueue( MCmd::cmd_get_u );          // 0x11 Чтение напряжения (мВ);
    return new MGetSU(Tools);
  };

  // Состояние "Фоновая команда читать состояние"
  MState * MGetSU::fsm()
  {
    //Serial.print("#");
    // MCmd::cmd_get_state       // 0x14 Чтение состояния;
    return new MGetI(Tools);
  };

  // Состояние "Фоновая команда читать ток"
  MState * MGetI::fsm()
  {
    //Serial.print("#");
    Tools->setToQueue( MCmd::cmd_get_i );          // 0x12 Чтение тока (мА)
    return new MGetSI(Tools);
  };

  // Состояние "Фоновая команда читать состояние"
  MState * MGetSI::fsm()
  {
    //Serial.print("#");
    // MCmd::cmd_get_state       // 0x14 Чтение состояния;
    return new MGetT(Tools);
  };

  // Состояние "Фоновая команда читать температуру"
  MState * MGetT::fsm()
  {
    //Serial.print("#");
    //  MCmd::cmd_get_celsius     // 0x15 Чтение температуры радиатора
    return new MGetST(Tools);
  };

  // Состояние "Фоновая команда читать состояние"
  MState * MGetST::fsm()
  {
    //Serial.print("#");
    // MCmd::cmd_get_state       // 0x14 Чтение состояния;
    return new MExt(Tools);
  };

  // Состояние "Нерегулярная команда"
  MState * MExt::fsm()
  {
    //Serial.print("#");
    //  
    return new MGetSE(Tools);
  };

  // Состояние "Фоновая команда читать состояние"
  MState * MGetSE::fsm()
  {
    //Serial.print("#");
    // MCmd::cmd_get_state       // 0x14 Чтение состояния;
    return new MGetU(Tools);
  };
 
};
