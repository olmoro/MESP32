#include "mdispatcher.h"
#include "nvs.h"
#include "mtools.h"
#include "board/mboard.h"
#include "board/msupervisor.h"
#include "board/mkeyboard.h"
#include "display/mdisplay.h"
#include <string.h>
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
  strcpy( sLabel, "FSM_BS4 v4.0" );
  Display->showLabel( sLabel );

  latrus = Tools->readNvsBool( MNvs::nQulon, MNvs::kQulonLocal, true );
  mode   = Tools->readNvsInt ( MNvs::nQulon, MNvs::kQulonMode, 0 );   // Индекс массива

  textMode( mode );
  Tools->powInd = Tools->readNvsInt  ( MNvs::nQulon, MNvs::kQulonPowInd, 3); // 3 - дефолтный индекс массива
  Tools->akbInd = Tools->readNvsInt  ( MNvs::nQulon, MNvs::kQulonAkbInd, 3); // Индекс массива с набором батарей
  Tools->setVoltageNom( Tools->readNvsFloat( MNvs::nQulon, MNvs::kQulonAkbU, Tools->akb[3][0]) );   // Начальный выбор 12 вольт
  Tools->setCapacity( Tools->readNvsFloat( MNvs::nQulon, MNvs::kQulonAkbAh, Tools->akb[3][1]) );   //             55 Ач

  Tools->postpone = Tools->readNvsInt( MNvs::nQulon, MNvs::kQulonPostpone,  3 );

  // Настройки АЦП
  Tools->offsetAdc = Tools->readNvsInt( MNvs::nQulon, MNvs::kOffsetAdc, 0x0000 );  // Смещение ЦАП

  // Настройки измерителей (для ввода драйверу)
  Tools->factorV  = Tools->readNvsInt( MNvs::nQulon, MNvs::kFactorV, 0x2DA0 );  // Множитель преобразования
  Tools->smoothV  = Tools->readNvsInt( MNvs::nQulon, MNvs::kSmoothV, 0x0003 );  // Коэффициент фильтрации
  Tools->offsetV  = Tools->readNvsInt( MNvs::nQulon, MNvs::kOffsetV, 0x0000 );  // Смещение в милливольтах

  Tools->factorA  = Tools->readNvsInt( MNvs::nQulon, MNvs::kFactorA, 0x030C );  // Множитель преобразования
  Tools->smoothA  = Tools->readNvsInt( MNvs::nQulon, MNvs::kSmoothA, 0x0003 );  // Коэффициент фильтрации
  Tools->offsetA  = Tools->readNvsInt( MNvs::nQulon, MNvs::kOffsetA, 0x0000 );  // Смещение в миллиамперах
}

void MDispatcher::run()
{
    // Индикация при инициализации процедуры выбора режима работы
  Display->showVolt( Tools->getRealVoltage(), 2 );
  Display->showAmp( Tools->getRealCurrent(), 1 );

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
        //if (mode == (int)SERVICE) mode = OPTIONS;
      if (mode == (int)DEVICE) mode = OPTIONS;
      else mode++;
      textMode( mode );
    }

    if (Tools->Keyboard->getKey(MKeyboard::DN_CLICK))
    {
      Board->buzzerOn();
        //if (mode == (int)OPTIONS) mode = SERVICE;
      if (mode == (int)OPTIONS) mode = DEVICE;
      else mode--;
      textMode( mode );
    }

    if (Tools->Keyboard->getKey(MKeyboard::B_CLICK))
    {
      Board->buzzerOn();

        // Запомнить крайний выбор режима
      Tools->writeNvsInt( MNvs::nQulon, "mode", mode );
        //Tools->writeNvsInt( MNvs::nQulon, MNvs::kQulonMode );

        // Serial.print("Available heap: "); Serial.println(ESP.getFreeHeap());
        // Serial.print("Core ID: ");        Serial.println(xPortGetCoreID());
      switch (mode)
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
}

void MDispatcher::textMode(int mode)
{
  char sMode[ MDisplay::MaxString ] = { 0 };
  char sHelp[ MDisplay::MaxString ] = { 0 };

  switch(mode)
  {
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
