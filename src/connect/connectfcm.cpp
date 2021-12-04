/*
  Сonnections state machine 
*/

#include "connectfsm.h"
#include "fsbrowser.h"
#include "mtools.h"
#include "board/mboard.h"
#include "board/msupervisor.h"
#include <Arduino.h>

namespace ConnectFsm
{
    MState * MInit::fsm()
    {
        initFSBrowser();
        return new MExe(Tools);
    };

// Что будем отдавать на отображение браузеру
    MState * MExe::fsm()
    {
        runFSBrowser( 
              Tools->getRealVoltage(),  //Board->getRealVoltage(), - уточнить
              //Board->getRealVoltage(),
              Tools->getRealCurrent(),  //Board->getRealCurrent(), 
              //Board->getRealCurrent(), 
              Board->getCelsius(),
              Tools->getAhCharge(), 
              Board->Supervisor->getFanPwm() );
        return this;
    };

};
