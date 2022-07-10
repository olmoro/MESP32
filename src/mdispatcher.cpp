/*
*
*
* июль 2022
*/
#include "mdispatcher.h"
#include "nvs.h"
#include "mtools.h"
#include "board/mboard.h"
#include "board/msupervisor.h"
#include "board/mkeyboard.h"
#include "display/mdisplay.h"
#include <string.h>
#include "modes/bootfsm.h"
#include "modes/optionsfsm.h"
#include "modes/templatefsm.h"
#include "modes/dcsupplyfsm.h"
#include "modes/cccvfsm.h"
#include "modes/devicefsm.h"
#include "Arduino.h"

MDispatcher::MDispatcher(MTools * tools) :
Tools(tools), Board(tools->Board), Display(tools->Display)
{
  char sLabel[ MDisplay::MaxString ] = { 0 };
  strcpy(sLabel, " MESP32 v0.1");
  Display->showLabel( sLabel );

  latrus = Tools->readNvsBool( MNvs::nQulon, MNvs::kQulonLocal, true );
  //modeSelection   = Tools->readNvsInt ( MNvs::nQulon, MNvs::kQulonMode, 0 );   // Индекс массива
  modeSelection   = BOOT;                      // Начинать всегда с синхронизации данных контроллеров

  textMode( modeSelection );
  Tools->powInd = Tools->readNvsInt  ( MNvs::nQulon, MNvs::kQulonPowInd, 3); // 3 - дефолтный индекс массива
  Tools->akbInd = Tools->readNvsInt  ( MNvs::nQulon, MNvs::kQulonAkbInd, 3); // Индекс массива с набором батарей
  Tools->setVoltageNom( Tools->readNvsFloat( MNvs::nQulon, MNvs::kQulonAkbU, Tools->akb[3][0]) );   // Начальный выбор 12 вольт
  Tools->setCapacity( Tools->readNvsFloat( MNvs::nQulon, MNvs::kQulonAkbAh, Tools->akb[3][1]) );   //             55 Ач

  Tools->postpone = Tools->readNvsInt( MNvs::nQulon, MNvs::kQulonPostpone,  3 );

  // Настройки АЦП и ниже - вынесены в syncingfsm 20220710 
// Tools->offsetAdc = Tools->readNvsInt( MNvs::nQulon, MNvs::kOffsetAdc, 0x0000 );  // Смещение ЦАП

  // Настройки измерителей (для ввода драйверу)
//  Tools->factorV  = Tools->readNvsInt( MNvs::nQulon, MNvs::kFactorV, 0x2DA0 );  // Множитель преобразования
//  Tools->smoothV  = Tools->readNvsInt( MNvs::nQulon, MNvs::kSmoothV, 0x0003 );  // Коэффициент фильтрации
//  Tools->offsetV  = Tools->readNvsInt( MNvs::nQulon, MNvs::kOffsetV, 0x0000 );  // Смещение в милливольтах

//  Tools->factorA  = Tools->readNvsInt( MNvs::nQulon, MNvs::kFactorA, 0x030C );  // Множитель преобразования
//  Tools->smoothA  = Tools->readNvsInt( MNvs::nQulon, MNvs::kSmoothA, 0x0003 );  // Коэффициент фильтрации
//  Tools->offsetA  = Tools->readNvsInt( MNvs::nQulon, MNvs::kOffsetA, 0x0000 );  // Смещение в миллиамперах
}

void MDispatcher::run()
{
    // Индикация при инициализации процедуры выбора режима работы
  Display->showVolt(Tools->getRealVoltage(), 3);  // 2
  Display->showAmp (Tools->getRealCurrent(), 3);   // 1

    // Выдерживается период запуска для вычисления амперчасов
  if (State)
  {
    // rabotaem so state mashinoj
    MState * newState = State->fsm();      
    if (newState != State)                  //state changed!
    {
      delete State;
      State = newState;
    } 
    //esli budet 0, na sledujushem cikle uvidim
  }
  else //state ne opredelen (0) - vybiraem ili pokazyvaem rezgim
  {
    if (Tools->Keyboard->getKey(MKeyboard::UP_CLICK))
    { 
      Board->buzzerOn();
      if (modeSelection == (int)DEVICE) modeSelection = OPTIONS;
      else modeSelection++;
      textMode( modeSelection );
    }

    if (Tools->Keyboard->getKey(MKeyboard::DN_CLICK))
    {
      Board->buzzerOn();
      if (modeSelection == (int)OPTIONS) modeSelection = DEVICE;
      else modeSelection--;
      textMode( modeSelection );
    }

    if(modeSelection != BOOT)
    {
      if (Tools->Keyboard->getKey(MKeyboard::B_CLICK))
      {
        Board->buzzerOn();

          // Запомнить крайний выбор режима
        Tools->writeNvsInt( MNvs::nQulon, "mode", modeSelection );
          //Tools->writeNvsInt( MNvs::nQulon, MNvs::kQulonMode );

          // Serial.print("Available heap: "); Serial.println(ESP.getFreeHeap());
          // Serial.print("Core ID: ");        Serial.println(xPortGetCoreID());

        switch (modeSelection)
        {
        case OPTIONS:
            State = new OptionFsm::MStart(Tools);
          break;

        case TEMPLATE:
            State = new TemplateFsm::MStart(Tools);
          break;

        case DCSUPPLY:
            State = new DcSupplyFsm::MStart(Tools);
          break; 

        case CCCVCHARGE:
            State = new CcCvFsm::MStart(Tools);
          break;

        case DEVICE:
            State = new DeviceFsm::MStart(Tools);
          break;

        default:
          break;
        }
      } // !B_CLICK
    }
    else
    {
      State = new Bootfsm::MStart(Tools);
    }
  }
}

void MDispatcher::textMode(short modeSelection)
{
  char sMode[ MDisplay::MaxString ] = { 0 };
  char sHelp[ MDisplay::MaxString ] = { 0 };

  switch(modeSelection)
  {
    case BOOT:
      sprintf( sMode, "      BOOT:       " );
      sprintf( sHelp, " -----START------ " );
    break;

    case OPTIONS:
      sprintf( sMode, "     OPTIONS:     " );
      sprintf( sHelp, "  CALIBRATION ETC " );
    break;

    case TEMPLATE:
      sprintf( sMode, "     TEMPLATE:    " );
      sprintf( sHelp, "      EXAMPLE     " );
    break;

    case DCSUPPLY:
      sprintf( sMode, "   DC DCSUPPLY:   " );
      sprintf( sHelp, "   POWER SUPPLY   " );
    break;

    case CCCVCHARGE:
      sprintf( sMode, "   CC/CV CHARGE:  " );
      sprintf( sHelp, "     B-SELECT     " );
    break;

    case DEVICE:
      sprintf( sMode, "      DEVICE:     " );   // Настройки с доступом (?) разработчика (заводской доступ)
      sprintf( sHelp, "       TOOLS      " );
    break;

    default:
      sprintf( sMode, "      ERROR:      " );
      sprintf( sHelp, "   UNIDENTIFIED   " );
    break;
  }

  Display->showMode( sMode );
  Display->showHelp( sHelp );
  //  Serial.println( sMode);
}
