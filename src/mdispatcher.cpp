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
  #include "modes/pidfsm.h"
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
  //modeSelection = Tools->readNvsInt ( MNvs::nQulon, MNvs::kQulonMode, 0 );   // Индекс массива
modeSelection = BOOT;
sync = true;

//   // Флаг блокировки обмена с драйвером на время его рестарта
// bool MTools::getBlocking() {return blocking;}
// void MTools::setBlocking(bool bl) {blocking = bl;}

  textMode( modeSelection );
  // Tools->powInd = Tools->readNvsInt  ( MNvs::nQulon, MNvs::kQulonPowInd, 3); // 3 - дефолтный индекс массива
  // Tools->akbInd = Tools->readNvsInt  ( MNvs::nQulon, MNvs::kQulonAkbInd, 3); // Индекс массива с набором батарей
  // Tools->setVoltageNom( Tools->readNvsFloat( MNvs::nQulon, MNvs::kQulonAkbU, Tools->akb[3][0]) );   // Начальный выбор 12 вольт
  // Tools->setCapacity( Tools->readNvsFloat( MNvs::nQulon, MNvs::kQulonAkbAh, Tools->akb[3][1]) );   //             55 Ач

  Tools->postpone = Tools->readNvsInt( MNvs::nQulon, MNvs::kQulonPostpone,  3 );

}

  // Выдерживается период запуска 100мс для вычисления амперчасов
void MDispatcher::run()
{
    // Индикация при инициализации процедуры выбора режима работы
  Display->showVolt(Tools->getRealVoltage(), 3);
  Display->showAmp (Tools->getRealCurrent(), 3);

  if (State)
  {
    // Работаем с FSM
    MState * newState = State->fsm();      
    if (newState != State)                  // State изменён!
    {
      delete State;
      State = newState;
    } 
    // Если будет 0, на следующем цикле увидим
  }
  else // State не определён (0) - выбираем или показываем режим
  {

//    Serial.print("Status ne opredelen. State="); Serial.print((int)State);
//    Serial.print("  sync="); Serial.println((int)sync);

    if (sync)
    {
      Board->buzzerOn();

//    Serial.print("sync"); Serial.println((int)sync);

      State = new MBoot::MStart(Tools);
      sync = false;
    }
      //if(!sync)
    else  
    {
      if (Tools->Keyboard->getKey(MKeyboard::B_CLICK))
      {
        Tools->writeNvsInt( MNvs::nQulon, "mode", modeSelection );  // Запомнить крайний выбор режима
 
      //sync = false;
        // Serial.print("Available heap: "); Serial.println(ESP.getFreeHeap());
        // Serial.print("Core ID: ");        Serial.println(xPortGetCoreID());

        switch (modeSelection)
        {
          case BOOT:        State = new MBoot::MStart(Tools);       break;
          case OPTIONS:     State = new MOption::MStart(Tools);      break;
          case PIDTEST:     State = new MPid::MStart(Tools);        break;
          case TEMPLATE:    State = new Template::MStart(Tools); break;
          case DCSUPPLY:    State = new DcSupply::MStart(Tools); break; 
          case CCCVCHARGE:  State = new MCccv::MStart(Tools);        break;
          case DEVICE:      State = new MDevice::MStart(Tools);     break;
          default:                                                  break;
        }
      } // !B_CLICK

      if (Tools->Keyboard->getKey(MKeyboard::UP_CLICK))
      { 
        Board->buzzerOn();
        if (modeSelection == (int)DEVICE) modeSelection = OPTIONS;  // Исключена возможность выбора BOOT'а
        else modeSelection++;
        textMode( modeSelection );
      }

      if (Tools->Keyboard->getKey(MKeyboard::DN_CLICK))
      {
        Board->buzzerOn();

            Serial.print("DN_CLICK"); Serial.println();

        if (modeSelection == (int)OPTIONS) modeSelection = DEVICE;  // Исключена возможность выбора BOOT'а
        else modeSelection--;
        textMode( modeSelection );
      }
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
      sprintf(sMode, "       BOOT:     ");   // Только 17 знакомест для этого дисплея,
      sprintf(sHelp, "    ...WAIT...   ");   // в 18-й вводить пробел
    break;

    case OPTIONS:
      sprintf(sMode, "     OPTIONS:    ");
      sprintf(sHelp, " USER PARAM...S  ");
    break;

    case PIDTEST:
      sprintf(sMode, "     PIDTEST:    ");
      sprintf(sHelp, " ADJ  KP, PD, KI ");
    break;

    case TEMPLATE:
      sprintf(sMode, "    TEMPLATE:    " );
      sprintf(sHelp, "     EXAMPLE     " );
    break;

    case DCSUPPLY:
      sprintf(sMode, "  DC DCSUPPLY:   " );
      sprintf(sHelp, "  POWER SUPPLY   " );
    break;

    case CCCVCHARGE:
      sprintf(sMode, "   CC/CV CHARGE:  ");
      sprintf(sHelp, "     B-SELECT     ");
    break;

    case DEVICE:
      sprintf(sMode, "      DEVICE:     ");   // Настройки с доступом (?) разработчика (заводской доступ)
      sprintf(sHelp, "  CALIBRATION ETC ");
    break;

    default:
      sprintf(sMode, "      ERROR:      " );
      sprintf(sHelp, "   UNIDENTIFIED   " );
    break;
  }

  Display->showMode( sMode );
  Display->showHelp( sHelp );
  //  Serial.println( sMode);
}
