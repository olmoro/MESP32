/*
 * Работа с драйвером силовой платы
 * read  - чтение через драйвер
 * write - запись через драйвер
 * 07.2022, некоторые не используемые команды не поддерживаются
 */

#include "driver/mcommands.h"
#include "nvs.h"
#include "board/mboard.h"
#include "mtools.h"
#include "mcmd.h"
#include "mwake.h"
#include "stdint.h"
#include <Arduino.h>


MCommands::MCommands(MTools * tools) : Tools(tools), Board(tools->Board) 
{ 
  Wake = new MWake(); 
  Wake->wakeInit( 0x00, 50 );  // Адрес в сети и время ожидания ответа
}

MCommands::~MCommands()
{
  delete Wake;
}
// Имя устройства
static constexpr char Info[] = {" QMoro Rev0.0\n\0"};   // Убрать для активного 


uint8_t cmd = MCmd::cmd_nop;

//uint8_t state1 = 0b00000000;
//uint8_t state2 = 0b00000000;

void MCommands::writeCmd(uint8_t _cmd) {cmd = _cmd;}

void MCommands::doCommand()
{
  cmd = Tools->getBuffCmd();

  #ifdef WO_UIS
    Tools->setBuffCmd(MCmd::cmd_nop);
  #else
    Tools->getTuningAdc() ? Tools->setBuffCmd(MCmd::cmd_adc_read_probes) : Tools->setBuffCmd(MCmd::cmd_get_uis);
  #endif

  if( cmd != MCmd::cmd_nop)
  {
    #ifdef DEBUG_COMMANDS
      Serial.print(" command -> 0x"); Serial.println(cmd, HEX);
    #endif

    switch( cmd )
    {
      //Команды чтения результатов измерений
      case MCmd::cmd_get_uis:                 readUI();                   break;  // 0x10;
      case MCmd::cmd_get_u:                   doGetU();                   break;  // 0x11 Чтение напряжения (мВ)
      case MCmd::cmd_get_i:                   doGetI();                   break;  // 0x12 Чтение тока (мА)
      case MCmd::cmd_get_ui:                  doGetUI();                  break;  // 0x13 Чтение напряжения (мВ) и тока (мА)
      case MCmd::cmd_get_state:               doGetState();               break;  // 0x14 Чтение состояния
      case MCmd::cmd_get_celsius:             doCelsius();                break;  // 0x15 Чтение температуры радиатора

        // Команды управления
      case MCmd::cmd_power_go:                doPowerGo();                break;  // 0x20
      case MCmd::cmd_power_stop:              doPowerStop();              break;  // 0x21

      //case MCmd::cmd_write_pid:             doSetPid();                 break;  // 0x22

        // Команды работы с измерителем напряжения 
      case MCmd::cmd_read_factor_u:           doGetFactorU();             break;  // 0x30   + 00->03
      case MCmd::cmd_write_factor_u:          doSetFactorU();             break;  // 0x31   + 02->01
      case MCmd::cmd_write_factor_default_u:  doSetFactorDefaultU();      break;  // 0x32   + 00->01
      case MCmd::cmd_read_smooth_u:           doGetSmoothU();             break;  // 0x33   + 00->02
      case MCmd::cmd_write_smooth_u:          doSetSmoothU();             break;  // 0x34   + 01->01
      case MCmd::cmd_read_offset_u:           doGetOffsetU();             break;  // 0x35   + 00->03
      case MCmd::cmd_write_offset_u:          doSetOffsetU();             break;  // 0x36   + 02->01
      
        // Команды работы с измерителем тока
      case MCmd::cmd_read_factor_i:             doGetFactorI();             break;  // 0x38   + 00->03
      case MCmd::cmd_write_factor_i:            doSetFactorI();             break;  // 0x39   + 02->01
      case MCmd::cmd_write_factor_default_i:    doSetFactorDefaultI();      break;  // 0x3A   + 00->01
      case MCmd::cmd_read_smooth_i:             doGetSmoothI();             break;  // 0x3B   + 00->02
      case MCmd::cmd_write_smooth_i:            doSetSmoothI();             break;  // 0x3C   + 01->01
      case MCmd::cmd_read_offset_i:             doGetOffsetI();             break;  // 0x3D   + 00->03
      case MCmd::cmd_write_offset_i:            doSetOffsetI();             break;  // 0x3E   + 02->01

        // Команды работы с ПИД-регулятором
      case MCmd::cmd_pid_configure:             doPidConfigure();           break;  // 0x40   + 0B->01
      case MCmd::cmd_pid_write_coefficients:    doPidSetCoefficients();     break;  // 0x41   + 07->01
      case MCmd::cmd_pid_output_range:          doPidOutputRange();         break;  // 0x42   + 05->01
      case MCmd::cmd_pid_reconfigure:           doPidReconfigure();         break;  // 0x43   + 0B->01
      case MCmd::cmd_pid_clear:                 doPidClear();               break;  // 0x44   + 01->01
      case MCmd::cmd_pid_test:                  doPidTest();                break;  // 0x46   + 03->01
      case MCmd::cmd_pid_read_param_mult:       doPidGetParamMult();        break;  // 0x47
      case MCmd::cmd_pid_read_configure:        doPidGetConfigure();        break;  // 0x48   + 00->0C
      //  case MCmd::cmd_pid_write_max_sum:         doPidSetMaxSum();           break;  // 0x49   + 0?->0?
      case MCmd::cmd_set_cooler:                doCooler();                 break;  // 0x4F Задать скорость вентилятора


        // Команды работы с АЦП
      case MCmd::cmd_adc_read_probes:           doReadProbes();             break;  // 0x50   + 00->07
      case MCmd::cmd_adc_read_offset:           doAdcGetOffset();           break;  // 0x51   + 00->03
      case MCmd::cmd_adc_write_offset:          doAdcSetOffset();           break;  // 0x52   + 02->01
      case MCmd::cmd_adc_auto_offset:           doAdcAutoOffset();          break;  // 0x53


        // Команды управления тестовые
      case MCmd::cmd_write_switch_pin:          doSwPin();                  break;  // 0x54     01->01
      case MCmd::cmd_write_power:               doSetPower();               break;  // 0x56     04->01
      case MCmd::cmd_write_discharge:           doSetDischg();              break;  // 0x57     01->01
      case MCmd::cmd_write_voltage:             doSetVoltage();             break;  // 0x58     03->03
      case MCmd::cmd_write_current:             doSetCurrent();             break;  // 0x59     05->03
      case MCmd::cmd_write_discurrent:          doSetDiscurrent();          break;  // 0x5A     03->01
  //   case MCmd::cmd_write_surge_compensation:  doSurgeCompensation();      break;  // 0x5B   nu
      case MCmd::cmd_write_idle_load:           doIdleLoad();               break;  // 0x5C   

        // Команды задания порогов отключения
      case MCmd::cmd_get_lt_v:           doGetLtV();              break;  // 0x60;     00->03
      case MCmd::cmd_set_lt_v:          doSetLtV();              break;  // 0x61;     02->01
      case MCmd::cmd_set_lt_default_v:  doSetLtDefaultV();       break;  // 0x62;     02->01
      case MCmd::cmd_get_up_v:             doGetUpV();              break;  // 0x63;     00->03
      case MCmd::cmd_set_up_v:            doSetUpV();              break;  // 0x64;     02->01
      case MCmd::cmd_set_up_default_v:    doSetUpDefaultV();       break;  // 0x65;     02->01

      case MCmd::cmd_get_lt_i:           doGetLtI();              break;  // 0x68;     00->03
      case MCmd::cmd_set_lt_i:          doSetLtI();              break;  // 0x69;     02->01
      case MCmd::cmd_set_lt_default_i:  doSetLtDefaultI();       break;  // 0x6A;     02->01
      case MCmd::cmd_get_up_i:             doGetUpI();              break;  // 0x6B;     00->03
      case MCmd::cmd_set_up_i:            doSetUpI();              break;  // 0x6C;     02->01
      case MCmd::cmd_set_up_default_i:    doSetUpDefaultI();       break;  // 0x6D;     02->01

        // Команды универсальные
      case MCmd::cmd_nop:                       doNop();                    break;  // 0x00
      case MCmd::cmd_info:                      doInfo();                   break;  // 0x03

      default: 
      break ;
    }
    cmd = MCmd::cmd_nop;       // не надо
  }

}

  // Обработка принятого пакета
short MCommands::dataProcessing()
{
  Wake->wakeRead();
  int cmd = Wake->getCommand();                         // Код команды в ответе
//  Serial.print("cmd=0x");   Serial.println( cmd, HEX );

  switch(cmd)
  {
    // Ответ на команду чтения результатов измерения напряжения (мВ), тока (мА)
    // и двух байт состояния драйвера (всего 7 байт, включая байт ошибки)
    case MCmd::cmd_get_uis:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 7) )
      {
        Tools->setVoltageVolt(Wake->get16(1));
        Tools->setCurrentAmper(Wake->get16(3));
        Tools->setState1(Wake->get08(5));
        Tools->setState2(Wake->get08(6));
        return 0; //Tools->setProtErr(0);
      }
      else  return 1; //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

    // Ответ на команду чтения результата измерения напряжения (мВ)
    // всего 3 байта, включая байт ошибки
    case MCmd::cmd_get_u:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 3) )
      {
        Tools->setVoltageVolt(Wake->get16(1));
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

    // Ответ на команду чтения результатов измерения тока (мА)
    // всего 3 байта, включая байт ошибки
    case MCmd::cmd_get_i:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 3) )
      {
        Tools->setCurrentAmper(Wake->get16(1));
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

    // Ответ на команду чтения результатов измерения напряжения (мВ), тока (мА)
    // всего 5 байт, включая байт ошибки
    case MCmd::cmd_get_ui:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 5) )
      {
        Tools->setVoltageVolt(Wake->get16(1));
        Tools->setCurrentAmper(Wake->get16(3));
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

    // Ответ на команду чтения двух байт состояния драйвера (всего 3 байта, включая байт ошибки)
    case MCmd::cmd_get_state:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 3) )
      {
        Tools->setState1(Wake->get08(1));
        Tools->setState2(Wake->get08(2));
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

    // Ответ на команду чтения результата преобразования данных датчика температуры
    // всего 3 байтf, включая байт ошибки
    case MCmd::cmd_get_celsius:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 3) )
      {
        Tools->setCelsius(Wake->get16(1));
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;





      // Ответ на команду старта преобразователя с заданными максимальными U и I
      // (всего 1 байт, байт ошибки)
    case MCmd::cmd_power_go:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 5) )    // ??
      {
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола - или нет подтверждения исполнения команды 
    break;

      // Ответ на команду отключения преобразователя и цепи разряда
      // (всего 1 байт, байт ошибки)
    case MCmd::cmd_power_stop:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 1) )
      {
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола - или нет подтверждения исполнения команды 
    break;


      // ========= Обработка ответов на команды работы с измерителем напряжения =========
      // Чтение множителя преобразования в милливольты           0x30
    case MCmd::cmd_read_factor_u:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 3) )
      {
        Tools->factorV = Wake->get16(1);
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

      // Запись множителя преобразования в милливольты           0x31
    case MCmd::cmd_write_factor_u:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 1) )
      {
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

      // Возврат к заводскому множителю                         0x32
    case MCmd::cmd_write_factor_default_u:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 1) )
      {
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

      // Чтение параметра сглаживания                           0x33
    case MCmd::cmd_read_smooth_u:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 2) )
      {
        Tools->smoothV = Wake->get08(1);
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

      // Запись параметра сглаживания                           0x34
    case MCmd::cmd_write_smooth_u:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 1) )
      {
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

      //  Чтение приборного смещения                            0x35
    case MCmd::cmd_read_offset_u:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 3) )
      {
        Tools->shiftV = (float)Wake->get16(1) / 1000;
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

      //  Запись приборного смещения                            0x36
    case MCmd::cmd_write_offset_u:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 1) )
      {
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

      // ========= Обработка ответов на команды работы с измерителем тока =========
      // Чтение множителя преобразования в миллиамперы           0x38
    case MCmd::cmd_read_factor_i:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 3) )
      {
        Tools->factorI = Wake->get16(1);
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

      // Запись множителя преобразования в миллиамперы           0x39
    case MCmd::cmd_write_factor_i:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 1) )
      {
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

      // Возврат к заводскому множителю                         0x3A
    case MCmd::cmd_write_factor_default_i:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 1) )
      {
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

      // Чтение параметра сглаживания                           0x3B
    case MCmd::cmd_read_smooth_i:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 2) )
      {
        Tools->smoothI = Wake->get08(1);
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

      // Запись параметра сглаживания                           0x3C
    case MCmd::cmd_write_smooth_i:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 1) )
      {
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

      //  Чтение приборного смещение                            0x3D
    case MCmd::cmd_read_offset_i:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 3) )
      {
        Tools->shiftI = Wake->get16(1);
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

      //  Запись приборного смещения                            0x3E
    case MCmd::cmd_write_offset_i:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 1) )
      {
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

      // ================ Команды работы с ПИД-регулятором =================
      // Параметры не возвращаются, только подтверждение исполнения команды
    case MCmd::cmd_pid_configure:               // 0x40   + 0B->01
    case MCmd::cmd_pid_write_coefficients:      // 0x41   + 07->01
    case MCmd::cmd_pid_output_range:            // 0x42   + 05->01
    case MCmd::cmd_pid_reconfigure:             // 0x43   + 0B->01
    case MCmd::cmd_pid_clear:                   // 0x44   + 01->01
    case MCmd::cmd_pid_test:                    // 0x46   + 03->01
      if((Wake->get08(0) == 0) && (Wake->getNbt() == 1))
      {
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

      // Чтение множителя параметров ПИД-регуляторов
    case MCmd::cmd_pid_read_param_mult:                         // 0x47
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 1) )
      {
        return 0;  //Tools->setProtErr(0);                      // Подтверждение
      }
      else  return 1;  //Tools->setProtErr(1);                  // ошибка протокола  
    break;

      // Чтение настроек ПИД-регулятора                         // 0x48
    case MCmd::cmd_pid_read_configure:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 12) )
      {
        Tools->pidMode  = Wake->get08(1);
        Tools->kp       = Wake->get16(2);
        Tools->ki       = Wake->get16(4);
        Tools->kd       = Wake->get16(6);
        Tools->minOut   = Wake->get16(8);
        Tools->maxOut   = Wake->get16(10);
        return 0;  //Tools->setProtErr(0);                      // Подтверждение
      }
      else  return 1;  //Tools->setProtErr(1);                  // ошибка
    break;

      // case cmd_pid_write_max_sum:         doPidSetMaxSum();           break;  // 0x49   + 0?->0?

      // ================ Команды работы с АЦП =================
      // Чтение АЦП                                        0x50   + 00->07
    case MCmd::cmd_adc_read_probes:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 7) )
      {
        Tools->setAdcV(Wake->get16(1));
        Tools->setAdcI(Wake->get16(3));
        // состояние
        // состояние
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

      // Чтение смещения АЦП                               0x51   + 00->03
    case MCmd::cmd_adc_read_offset:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 3) )
      {
        Tools->txSetAdcOffset(Wake->get16(1));
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

      // Запись смещения АЦП                                0x52   + 02->01
    case MCmd::cmd_adc_write_offset:
      if( (Wake->get08(0) == 0) && (Wake->getNbt() == 1) )
      {
        return 0;  //Tools->setProtErr(0);
      }
      else  return 1;  //Tools->setProtErr(1);  // ошибка протокола или нет подтверждения исполнения команды 
    break;

        // Команды задания порогов отключения
      // case cmd_get_lt_u:             doGetLtV();              break;  // 0x60;     00->03
      // case cmd_set_lt_u:            doSetLtV();              break;  // 0x61;     02->01
      // case cmd_set_lt_default_u:    doSetLtDefaultU();       break;  // 0x62;     02->01
      // case cmd_get_up_u:               doGetUpU();              break;  // 0x63;     00->03
      // case cmd_set_up_u:              doSetUpU();              break;  // 0x64;     02->01
      // case cmd_set_up_default_u:      doSetUpDefaultU();       break;  // 0x65;     02->01

      // case cmd_get_lt_i:             doGetLtI();              break;  // 0x68;     00->03
      // case cmd_set_lt_i:            doSetLtI();              break;  // 0x69;     02->01
      // case cmd_set_lt_default_i:    doSetLtDefaultI();       break;  // 0x6A;     02->01
      // case cmd_get_up_i:               doGetUpI();              break;  // 0x6B;     00->03
      // case cmd_set_up_i:              doSetUpI();              break;  // 0x6C;     02->01
      // case cmd_set_up_default_i:      doSetUpDefaultI();       break;  // 0x6D;     02->01



    default:
    return 2;   // Нет такой команды
    break;
  }
}


#ifdef OLD
// передать информацию об устройстве
void doInfo()
{
  char ch = 1;
  int i = 0;

  for( i = 0; i < frame && ch; i++ )
  {
  ch = txDat[i] = Info[i];

  #ifdef DEBUG_WAKE
    Serial.print( ch );
  #endif
  }
  
  txReplay( i, txDat[0] );        // Искусственный прием, об ошибках не сообщается
}

// передать эхо
void doEcho()
{
  for( int i = 0; i < rxNbt && i < frame; i++ )
  txDat[i] = rxDat[i];
  txReplay( rxNbt, txDat[0] );
  #ifdef DEBUG_WAKE
    Serial.print("команда эхо = "); Serial.print( rxNbt );
  #endif
}

// ошибка приема пакета
void doErr()
{
  txReplay(1, err_tx);
  #ifdef DEBUG_WAKE
    Serial.println("обработка ошибки");
  #endif
}

// // Формирование регистра состояния 1
// void doState1()
// {
//   _switchStatus         ? state1 |= 0b10000000 : state1 &= 0b01111111; 
//   _converterStatus      ? state1 |= 0b01000000 : state1 &= 0b10111111; 
//   _currentControlStatus ? state1 |= 0b00100000 : state1 &= 0b11011111; 
//   _voltageControlStatus ? state1 |= 0b00010000 : state1 &= 0b11101111; 
//   _chargeStatus         ? state1 |= 0b00001000 : state1 &= 0b11110111; 
//   _dischargeStatus      ? state1 |= 0b00000100 : state1 &= 0b11111011; 
//   _pauseStatus          ? state1 |= 0b00000010 : state1 &= 0b11111101; 
//   _reserve1Status       ? state1 |= 0b00000001 : state1 &= 0b11111110;

//   switchFoff(_switchStatus);        // Непрерывное подтверждение состояния
//   converterOff(_converterStatus);
// }

// // Формирование регистра состояния 2 
// void doState2()
// {
//   _overHeatingStatus     ? state2 |= 0b10000000 : state2 &= 0b01111111; 
//   _overloadStatus        ? state2 |= 0b01000000 : state2 &= 0b10111111; 
//   _powerLimitationStatus ? state2 |= 0b00100000 : state2 &= 0b11011111; 
//   _reversePolarityStatus ? state2 |= 0b00010000 : state2 &= 0b11101111; 
//   _shortCircuitStatus    ? state2 |= 0b00001000 : state2 &= 0b11110111; 
//   _calibrationStatus     ? state2 |= 0b00000100 : state2 &= 0b11111011; 
//   _upgradeStatus         ? state2 |= 0b00000010 : state2 &= 0b11111101; 
//   _reserve2Status        ? state2 |= 0b00000001 : state2 &= 0b11111110; 
// }
#endif

// constexpr float hz = 10.0;    // Пока без вариантов

// bool _cfg_err;    // Этого не будет - проверки на стороне ESP минимальны

// // На стороне ESP все float параметры преобразуются в целочисленные 
// //                                                    НЕ ПРОВЕРЕНО
// uint32_t MCommands::floatToParam(float in)
// {
//   if (in > param_max || in < 0)
//   {
//     _cfg_err = true;
//     return 0;
//   }

//   uint32_t param = in * param_mult;

//   if (in != 0 && param == 0) {
//     _cfg_err = true;
//     return 0;
//   }
  
//   return param;
// }

// Запись байта в буфер передатчика по индексу 
void MCommands::txU08(uint8_t id,  uint8_t value)
{
  Wake->setU8( id, value );
}

// Запись двухбайтового числа в буфер передатчика по индексу 
void MCommands::txU16(uint8_t id, uint16_t value)
{
  Wake->setU8( id,   uint8_t(( value >>  8 ) & 0xff ));
  Wake->setU8( id+1, uint8_t(  value         & 0xff ));
}

// Запись четырехбайтового числа в буфер передатчика по индексу 
void MCommands::txU32(uint8_t id, uint32_t value)
{
  Wake->setU8( id,   uint8_t(( value >> 24 ) & 0xff ));
  Wake->setU8( id+1, uint8_t(( value >> 16 ) & 0xff ));
  Wake->setU8( id+2, uint8_t(( value >>  8 ) & 0xff ));
  Wake->setU8( id+3, uint8_t(  value         & 0xff ));
}

//================= Команды управления процессами =================

// Команда запроса данных измерений драйвером напряжения и тока
// Запрос: 0xC0, 0x10, 0x00, 0x52     t = 0.35ms                  - ok
// Ожидаемый ответ: целочисленные знаковые в милливольтах и миллиамперах и два байта состояний.
// Ответ:  0xC0, 0x10, 0x07, 0x00, 0xFC, 0xEE, 0x00, 0x21, 0x00, 0x00, 0xDE - ok,  t = 0.95ms
void MCommands::readUI()
{        
  Wake->configAsk( 0, MCmd::cmd_get_uis);
}

// 0x11 Чтение напряжения (мВ)
// Ожидаемый ответ: целочисленное знаковое в милливольтах.
void MCommands::doGetU()
{
  Wake->configAsk( 0, MCmd::cmd_get_u);
}

// 0x12 Чтение тока (мА)
// Ожидаемый ответ: целочисленное знаковое в миллиамперах.
void MCommands::doGetI()
{
  Wake->configAsk( 0, MCmd::cmd_get_i);
}

// 0x13 Чтение напряжения (мВ) и тока (мА)
// Ожидаемый ответ: целочисленные знаковые в милливольтах и миллиамперах.
void MCommands::doGetUI()
{
  Wake->configAsk( 0, MCmd::cmd_get_ui);
}

// 0x14 Чтение состояния
// Ожидаемый ответ: два байта состояний.
void MCommands::doGetState()
{
  Wake->configAsk( 0, MCmd::cmd_get_state);
}

// 0x15 Чтение температуры радиатора
// Ожидаемый ответ: целочисленное знаковое ADC как есть.
void MCommands::doCelsius()
{
  Wake->configAsk( 0, MCmd::cmd_get_celsius);
}

// Команда управления PID регулятором 0x20
// Запрос: 0xC0, 0x20, 0x05, 0x12, 0x34, 0x56, 0x78, 0x01, 0x64       - ok
// Ответ:  0xC0, 0x20, 0x01, 0x00, 0x7D                               - ok
void MCommands::doPowerGo()
{
  int id = 0;
  id = Wake->replyU16( id, Tools->setpointU );
  id = Wake->replyU16( id, Tools->setpointI );
  id = Wake->replyU08( id, Tools->pidMode );
  Wake->configAsk( id, MCmd::cmd_power_go);
}

// Команда отключения регулятора  0x21 
// Запрос: 0xC0, 0x21, 0x00, 0xBB                                     - ok
// Ответ:  0xC0, 0x21, 0x01, 0x00, 0xD6                               - ok 
void MCommands::doPowerStop()  
{
  Wake->configAsk( 0, MCmd::cmd_power_stop);
}

// void MCommands::doSetPid()                 // 0x22
// {
//   // nu
//   Wake->configReply( 5, 0, cmd_write_pid);    // txNbt, err, command   - 
// }


// =============== Команды работы с измерителем напряжения ================
// Команда чтения множителя по напряжению 0x30 
// Запрос: 0xC0, 0x30, 0x00, 0x93                           - ok
// Ответ:  0xC0, 0x30, 0x03, 0x00, 0x01, 0x23, 0x41         - ok
void MCommands::doGetFactorU() 
{
  Wake->configAsk( 0, MCmd::cmd_read_factor_u);
  // ...
}

// Команда записи множителя по напряжению 0x31 (0x0123)
// Запрос: 0xC0, 0x31, 0x02, 0x01, 0x23, 0x53               - ok
// Ответ:  0xC0, 0x31, 0x01, 0x00, 0x9C                     - ok
void MCommands::doSetFactorU() 
{
  int id = 0;
  id = Wake->replyU16( id, Tools->factorV );
  Wake->configAsk( id, MCmd::cmd_write_factor_u);
}

// Команда замены множителя по напряжению на заводской 0x32
// Запрос: 0xC0, 0x32, 0x00, 0x02                           - ok
// Ответ:  0xC0, 0x32, 0x01, 0x00, 0x78                     - ok проверить исполнение
void MCommands::doSetFactorDefaultU() 
{
  Wake->configAsk( 0, MCmd::cmd_write_factor_default_u);
}


// Команда чтения коэффициента фильтрации 0x33
// Запрос: 0xC0, 0x33, 0x00, 0xC6                           - ok
// Ответ:  0xC0, 0x33, 0x02, 0x00, 0x03, 0xB3               - ok
void MCommands::doGetSmoothU() 
{
  Wake->configAsk( 0, MCmd::cmd_read_smooth_u);
  // ...
}

// Команда записи коэффициента фильтрации 0x34
// Запрос: 0xC0, 0x34, 0x01, 0x03, 0x4B                     - ok
// Ответ:  0xC0, 0x34, 0x01, 0x00, 0xA9                     - ok
void MCommands::doSetSmoothU() 
{
  int id = 0;
  id = Wake->replyU08( id, Tools->smoothV );
  Wake->configAsk( id, MCmd::cmd_write_smooth_u);
}

// Команда чтения смещения по напряжению 0x35
// Запрос: 0xC0, 0x35, 0x00, 0x6C                           - ok
// Ответ:  0xC0, 0x35, 0x03, 0x00, 0x02, 0x89, 0x17         - ok
void MCommands::doGetOffsetU() 
{
  Wake->configAsk( 0, MCmd::cmd_read_offset_u);
  // ...
}

// Команда записи смещения по напряжению 0x36 (0x0289)
// Запрос: 0xC0, 0x36, 0x02, 0x89, 0x51                     - ok
// Ответ:  0xC0, 0x36, 0x01, 0x00, 0xE6                     - ok
void MCommands::doSetOffsetU() 
{
  int id = 0;
  id = Wake->replyU16( id, Tools->shiftV );
  Wake->configAsk( id, MCmd::cmd_write_offset_u);
}



// =============== Команды работы с измерителем тока ================
// Команда чтения множителя по току 0x38 
// Запрос: 0xC0, 0x38, 0x00, 0xE5                           - ok
// Ответ:  0xC0, 0x38, 0x03, 0x00, 0xAB, 0xCD, 0x80         - ok
void MCommands::doGetFactorI() 
{
  Wake->configAsk( 0, MCmd::cmd_read_factor_i);
  // ...
}

// Команда записи множителя по току 0x39 (0xabcd)
// Запрос: 0xC0, 0x39, 0x02, 0xAB, 0xCD, 0xB0               - ok
// Ответ:  0xC0, 0x39, 0x01, 0x00, 0xB9                     - ok
void MCommands::doSetFactorI() 
{
  int id = 0;
  id = Wake->replyU16( id, Tools->factorI );
  Wake->configAsk( id, MCmd::cmd_write_factor_i);
}

// Команда замены множителя по току на заводской 0x3A
// Запрос: 0xC0, 0x3A, 0x00, 0x3A                           - ok
// Ответ:  0xC0, 0x3A, 0x01, 0x00, 0x5D                     - ok проверить исполнение
void MCommands::doSetFactorDefaultI() 
{
  Wake->configAsk( 0, MCmd::cmd_write_factor_default_i);
}


// Команда чтения коэффициента фильтрации 0x3B
// Запрос: 0xC0, 0x3B, 0x00, 0xB0                           - ok
// Ответ:  0xC0, 0x3B, 0x02, 0x00, 0x02, 0xF1               - ok
void MCommands::doGetSmoothI() 
{
  Wake->configAsk( 0, MCmd::cmd_read_smooth_i);
  // ...
}

// Команда записи коэффициента фильтрации 0x3C (0x02)
// Запрос: 0xC0, 0x3C, 0x01, 0x02, 0x30                     - ok
// Ответ:  0xC0, 0x3C, 0x01, 0x00, 0x8C                     - ok
void MCommands::doSetSmoothI() 
{
  int id = 0;
  id = Wake->replyU08( id, Tools->smoothI );
  Wake->configAsk( id, MCmd::cmd_write_smooth_i);
}


// Команда чтения смещения по току 0x3D
// Запрос: 0xC0, 0x3D, 0x00, 0x1A                           - ok
// Ответ:  0xC0, 0x3D, 0x03, 0x00, 0x00, 0x0B, 0x88         - ??  0x000B - default only??
void MCommands::doGetOffsetI() 
{
  Wake->configAsk( 0, MCmd::cmd_read_offset_i);
  // ...
}

// Команда записи смещения по току 0x3E (0x0ab9)
// Запрос: 0xC0, 0x3E, 0x02, 0x0A, 0xB9, 0x85               - ok
// Ответ:  0xC0, 0x3E, 0x01, 0x00, 0xc3                     - ok
void MCommands::doSetOffsetI() 
{
  int id = 0;
  id = Wake->replyU16( id, Tools->shiftI );
  Wake->configAsk( id, MCmd::cmd_write_offset_i);
}


//================= Команды работы с регуляторами ================= 
// Конфигурирование пид-регулятора с очисткой регистров   0x40
// set mode, kp, ki, kd, min, max
// Запрос: 0xC0, 0x40, 0x0B,  0x01, 
//                            0xAB, 0xCD, 
//                            0xBA, 0x12, 
//                            0xCD, 0xEF, 
//                            0x45, 0x67, 
//                            0x89, 0xAC, 
//                            0xF8                          - ok
// Ответ:  0xC0, 0x40, 0x01, 0x00, 0xD8                     - ok
void MCommands::doPidConfigure()
{
  int id = 0;
  id = Wake->replyU08( id, Tools->pidMode ); // 0x01;
  id = Wake->replyU16( id, Tools->kp );      // 0xabcd
  id = Wake->replyU16( id, Tools->ki );      // 0xba12
  id = Wake->replyU16( id, Tools->kd );      // 0xcdef
  id = Wake->replyU16( id, Tools->minOut );  // 0x4567
  id = Wake->replyU16( id, Tools->maxOut );  // 0x89ac
  Wake->configAsk( id, MCmd::cmd_pid_configure);
}

// ввод коэффициентов kp, ki, kd для заданного режима     0x41 
// set mode, kp, ki, kd  
// Запрос: 0xC0, 0x41, 0x07,  0x01
//                            0xAB, 0xCD, 
//                            0xBA, 0x12, 
//                            0xCD, 0xEF, 
//                            0x1F                          - ok
// Ответ:  0xC0, 0x41, 0x01, 0x00, 0x73                     - ok
void MCommands::doPidSetCoefficients() 
{
  int id = 0;
  id = Wake->replyU08( id, Tools->pidMode ); // 0x01;
  id = Wake->replyU16( id, Tools->kp );      // 0xabcd
  id = Wake->replyU16( id, Tools->ki );      // 0xba12
  id = Wake->replyU16( id, Tools->kd );      // 0xcdef
  Wake->configAsk( id, MCmd::cmd_pid_write_coefficients);
}

// ввод диапазона вывода для заданного режима     0x42 
// set mode, min, max
// Запрос: 0xC0, 0x42, 0x05,  0x01
//                            0x45, 0x67, 
//                            0x89, 0xAC, 
//                            0x59                          - ok
// Ответ:  0xC0, 0x42, 0x01, 0x00, 0x97                     - ok
void MCommands::doPidOutputRange() 
{
  int id = 0;
  id = Wake->replyU08( id, Tools->pidMode ); // 0x01;
  id = Wake->replyU16( id, Tools->minOut );  // 0x4567
  id = Wake->replyU16( id, Tools->maxOut );  // 0x89ac
  Wake->configAsk( id, MCmd::cmd_pid_output_range);
} 

// Конфигурирование пид-регулятора без очистки регистров     0x43 
// set kp, ki, kd,min, max w/o clear
// Запрос: 0xC0, 0x43, 0x0B,  0x01
//                            0xAB, 0xCD, 
//                            0xBA, 0x12, 
//                            0xCD, 0xEF, 
//                            0x45, 0x67, 
//                            0x89, 0xAC, 
//                            0x08                          - ok
// Ответ:  0xC0, 0x43, 0x01, 0x00, 0xD8                     - ok
void MCommands::doPidReconfigure() 
{
  int id = 0;
  id = Wake->replyU08( id, Tools->pidMode ); // 0x01;
  id = Wake->replyU16( id, Tools->kp );      // 0xabcd
  id = Wake->replyU16( id, Tools->ki );      // 0xba12
  id = Wake->replyU16( id, Tools->kd );      // 0xcdef
  id = Wake->replyU16( id, Tools->minOut );  // 0x4567
  id = Wake->replyU16( id, Tools->maxOut );  // 0x89ac
  Wake->configAsk( id, MCmd::cmd_pid_reconfigure);
}

// Очистка регистров регулятора     0x44 
// clear mode
// Запрос: 0xC0, 0x44, 0x01, 0x01, 0x18                     - ok
// Ответ:  0xC0, 0x44, 0x01, 0x00, 0x46                     - ok
void MCommands::doPidClear() 
{
  int id = 0;
  id = Wake->replyU08( id, Tools->pidMode ); // 0x01;
  Wake->configAsk( id, MCmd::cmd_pid_clear);
}  

// Тестовая. Тест пид-регулятора     0x46 
// Задает ПИД-регулятору режим регулирования U,I или D и задает уровень.
// В режиме OFF ПИД-регулятор отключен, но схема скоммутирована как для регулирования 
// по напряжению. Уровень предназначен для подачи непосредственно на PWM с осторожностью. 
// mode, setpoint, sw
// Запрос: 0xC0, 0x46, 0x03,  0x01
//                            0x43, 0x21, 
//                            0xAD                          - ok
// Ответ:  0xC0, 0x46, 0x01, 0x00, 0x09                     - ok
void MCommands::doPidTest() 
{
  int id = 0;
  id = Wake->replyU08( id, Tools->pidMode );   // 0x01;
  id = Wake->replyU16( id, Tools->setpoint );  // 0x4321
  Wake->configAsk( id, MCmd::cmd_pid_test);
}




// Чтение param_mult     0x47 
void MCommands::doPidGetParamMult() 
{
  int id = 0;
  id = Wake->replyU16( id, Tools->getParamMult() );
  Wake->configAsk( id, MCmd::cmd_pid_read_param_mult);
}  




// Возвращает параметры текущего режима регулирования     0x48 
// mode, kP, kI, kD, min, max 
// Запрос: 0xC0, 0x48, 0x00, 0x53                           - ok
// Ответ:  0xC0, 0x48, 0x0C,  0x00
//                            0x02, 
//                            0x00, 0x19,
//                            0x00, 0x0C,
//                            0x00, 0x19,
//                            0x02, 0x20,
//                            0x10, 0x00, 
//                                  0xF3                    - ok
void MCommands::doPidGetConfigure() 
{
  int id = 0;
  Wake->configAsk( id, MCmd::cmd_pid_read_configure);
  // ...
}  

// // Задает максимальный интеграл при вычислении шага регулирования
// // Задает максимальный интеграл при вычислении шага рег     0x49
// void MCommands::doPidSetMaxSum() 
// {
//   int id = 0;

//   Wake->configAsk( id, cmd_pid_write_max_sum);
// }    

// 0x4F Задать скорость вентилятора 
void MCommands::doCooler()
{
  int id = 0;
  id = Wake->replyU16( id, Tools->getCooler());
  Wake->configAsk( id, MCmd::cmd_set_cooler);
}



//================= Команды работы с АЦП =================

// Команда запроса результатов проеобразования АЦП 0x50
// Запрос: 0xC0, 0x50, 0x00, 0xC9                           - ok
// Ожидаемый ответ: целочисленные 
// Ответ:  0xC0, 0x50, 0x07, 0x00, 0xED, 0xC1, 0xED, 0xD0, 0x00, 0x00, 0xD0 - ??
void MCommands::doReadProbes()
{        
  Wake->configAsk( 0, MCmd::cmd_adc_read_probes);
  // ...
}

// Команда чтения смещения АЦП  0x51
void MCommands::doAdcGetOffset()
{
  Wake->configAsk( 0, MCmd::cmd_adc_read_offset);
  // ...
}

// Команда записи смещения АЦП  0x52
void MCommands::doAdcSetOffset()
{
  int id = 0;
  //  id = Wake->replyU16( id, Board->readAdcOffset() );
  id = Wake->replyU16( id, Tools->getAdcOffset());
  Wake->configAsk( id, MCmd::cmd_adc_write_offset);
}  

// Команда автоматической компенсации смещения АЦП 0x53
void MCommands::doAdcAutoOffset()
{
  Wake->configAsk( 0, MCmd::cmd_adc_auto_offset);
}  



// ================= Команды тестирования =================
// Команда управления ключами подключения нагрузки     0x54
// MINI: закомментировать powerFailure() или замкнуть D8,9 и A3,4 
// Запрос: 0xC0, 0x54, 0x01, 0x00, 0x0C                     - ok
// Ответ:  0xC0, 0x54, 0x01, 0x00, 0x0C                     - ok
void MCommands::doSwPin()
{
  int id = 0;
  id = Wake->replyU08( id, Tools->swOnOff );  // 0x00;
  Wake->configAsk( id, MCmd::cmd_write_switch_pin);
}



// Команда проверки пределов регулирования преобразователя снизу. 0x56
// Использовать с осторожностью, только для проверки низковольтной схемы.
// ПИД-регулятор отключается, коммутатор включен, преобразователь включен      
// Запрос: 0xC0, 0x56, 0x04, 0x03, 0x35, 0x02, 0x22, 0x6A   - ok
// Ответ:  0xC0, 0x56, 0x01, 0x00, 0x43                     - ok
void MCommands::doSetPower()
{
  int id = 0;
  id = Wake->replyU16( id, Board->getPwmVal() );  // 0x0335
  id = Wake->replyU16( id, Board->getDacVal() );  // 0x0222
  Wake->configAsk( id, MCmd::cmd_write_power);
}


// Команда проверка управления цепью разряда.      0x57
// Пользоваться с осторожностью, выставив порог отключения
// Запрос: 0xC0, 0x57, 0x01, 0x32, 0xEA                     - ok
// Ответ:  0xC0, 0x57, 0x01, 0x00, 0xE8                     - ok
void MCommands::doSetDischg()
{
  int id = 0;
  id = Wake->replyU08( id, Board->getPerc() );   // 50 = 0x32
  Wake->configAsk( id, MCmd::cmd_write_discharge);
}


// Команда включения и поддержание заданного напряжения в мВ   0x58
// Запрос: 0xC0, 0x58, 0x03, 0x01, 0x12, 0x34, 0x62         - 
// Ответ:  0xC0, 0x58, 0x03, 0x00, 0x12, 0x34, 0xC9         -
// ... 
void MCommands::doSetVoltage()
{
  int id = 0;
  id = Wake->replyU08( id, Tools->pidMode );  // 0x01;
  id = Wake->replyU16( id, Tools->setpointU );
  Wake->configAsk( id, MCmd::cmd_write_voltage);
}

// Команда задать ток в мА и включить 0x59
// Запрос: 0xC0, 0x59, 0x05, 0x00, 0x56, 0x78, 0xAB, 0xCD, 0x17         - ok
// Ответ:  0xC0, 0x59, 0x03, 0x01, 0x00, 0x00, 0x0D                     - ok сообщение об ошибке!
void MCommands::doSetCurrent()
{
  int id = 0;
  id = Wake->replyU08( id, Tools->swOnOff );  // 0x00;
  id = Wake->replyU16( id, Tools->setpointI );
  id = Wake->replyU16( id, Tools->factorI );
  Wake->configAsk( id, MCmd::cmd_write_current);
}


// Команда задать код DAC или ток разряда в мА и включить    0x5A
// Запрос: 0xC0, 0x5A, 0x03, 0x01, 0x43, 0x21, 0xF0         - ok
// Ответ:  0xC0, 0x5A, 0x01, 0x00, 0xF8                     - ok
void MCommands::doSetDiscurrent()
{
  int id = 0;
  id = Wake->replyU08( id, Tools->pidMode );   // 0x01;
  id = Wake->replyU16( id, Tools->setpoint );  // 0x4321
  Wake->configAsk( id, MCmd::cmd_write_discurrent);
}





// Команда задать параметры компенсации перенапряжения - отменено     0x5B

// Запрос: 0xC0, 0x5B, 0x02, 0x12, 0x34, 0xEC               - 
// Ответ:  0xC0, 0x5B, 0x01, 0x00, 0xDD                     - 
void MCommands::doSurgeCompensation()
{
  int id = 0;
//   id = Wake->replyU08( id, Board->get() );  // 0x00;
//   id = Wake->replyU16( id, Board->get() );
  Wake->configAsk( id, MCmd::cmd_write_surge_compensation);
}

// Команда задать параметры доп. нагрузки на ХХ       0x5C
// Запрос: 0xC0, 0x5C, 0x04, 0x05, 0x55, 0x01, 0x07, 0x40   - ok
// Ответ:  0xC0, 0x5C, 0x01, 0x00, 0x29                     - ok
void MCommands::doIdleLoad()
{
  int id = 0;
  id = Wake->replyU16( id, Board->getIdleI() );   // 0x0555
  id = Wake->replyU16( id, Board->getIdleDac() ); // 0x0107
  Wake->configAsk( id, MCmd::cmd_write_idle_load);
}


// ================ Команды управления порогами отключения ================

// Команда чтения нижнего порога отключения по напряжению  0x60;
// Запрос: 0xC0, 0x60, 0x00, 0xE4                           - ok
// Ответ:  0xC0, 0x60, 0x03, 0x00, 0xFF, 0x38, 0x2C         - ok
void MCommands::doGetLtV()              
{
  int id = 0;
  // ...
  Wake->configAsk( id, MCmd::cmd_get_lt_v );
}

// Команда записи нижнего порога отключения по напряжению  0x61;
// Запрос: 0xC0, 0x61, 0x02, 0xFF, 0x38, 0x73               - ok
// Ответ:  0xC0, 0x61, 0x01, 0x00, 0xE7                     - ok
void MCommands::doSetLtV()
{
  int id = 0;
  id = Wake->replyU16( id, Tools->getLtV() ); // 0xFF38
  Wake->configAsk( id, MCmd::cmd_set_lt_v );
}

// Команда восстановления заводского нижнего порога отключения по напряжению  0x62;
// Запрос: 0xC0, 0x62, 0x00, 0x75                           - ok
// Ответ:  0xC0, 0x62, 0x01, 0x00, 0x03                     - ok
void MCommands::doSetLtDefaultV() 
{
  int id = 0;
  Wake->configAsk( id, MCmd::cmd_set_lt_default_v );
}

// Команда чтения верхнего порога отключения по напряжению  0x63;
// Запрос: 0xC0, 0x63, 0x00, 0xB1                           - ok
// Ответ:  0xC0, 0x63, 0x03, 0x00, 0x46, 0x50, 0x75         - ok
void MCommands::doGetUpV()              
{
  int id = 0;
  // ...
  Wake->configAsk( id, MCmd::cmd_get_up_v );
}

// Команда записи верхнего порога отключения по напряжению  0x64;
// Запрос: 0xC0, 0x64, 0x02, 0x46, 0x50, 0xE5               - ok
// Ответ:  0xC0, 0x64, 0x01, 0x00, 0x79                     - ok
void MCommands::doSetUpV()
{
  int id = 0;
  id = Wake->replyU16( id, Tools->getUpV() ); // 0xFF38
  Wake->configAsk( id, MCmd::cmd_set_up_v );
}

// Команда восстановления заводского верхнего порога отключения по напряжению  0x65;
// Запрос: 0xC0, 0x65, 0x00, 0x1B                           - ok
// Ответ:  0xC0, 0x65, 0x01, 0x00, 0x79                     - ok
void MCommands::doSetUpDefaultV() 
{
  int id = 0;
  Wake->configAsk( id, MCmd::cmd_set_up_default_v );
}

// Команда чтения нижнего порога отключения по току  0x68;
// Запрос: 0xC0, 0x68, 0x00, 0x92                           - ok
// Ответ:  0xC0, 0x68, 0x03, 0x00, 0xFA, 0x24, 0xD3         - ok
void MCommands::doGetLtI()              
{
  int id = 0;
  // ...
  Wake->configAsk( id, MCmd::cmd_get_lt_i );
}

// Команда записи нижнего порога отключения по току  0x69;
// Запрос: 0xC0, 0x69, 0x02, 0xFA, 0x24, 0xAE               - ok
// Ответ:  0xC0, 0x69, 0x01, 0x00, 0xC2                     - ok
void MCommands::doSetLtI()
{
  int id = 0;
  id = Wake->replyU16( id, Tools->getLtI() ); // 0x
  Wake->configAsk( id, MCmd::cmd_set_lt_i );
}

// Команда восстановления заводского нижнего порога отключения по току  0x6A;
// Запрос: 0xC0, 0x6A, 0x00, 0x03                           - ok
// Ответ:  0xC0, 0x6A, 0x01, 0x00, 0x26                     - ok
void MCommands::doSetLtDefaultI() 
{
  int id = 0;
  Wake->configAsk( id, MCmd::cmd_set_lt_default_i );
}

// Команда чтения верхнего порога отключения по току  0x6B;
// Запрос: 0xC0, 0x6B, 0x00, 0xC7                           - ok
// Ответ:  0xC0, 0x6B, 0x03, 0x00, 0x13, 0x88, 0x56         - ok
void MCommands::doGetUpI()              
{
  int id = 0;
  // ...
  Wake->configAsk( id, MCmd::cmd_get_up_i );
}

// Команда записи верхнего порога отключения по току  0x6C;
// Запрос: 0xC0, 0x6C, 0x02, 0x13, 0x88, 0xE4               - ok
// Ответ:  0xC0, 0x6C, 0x01, 0x00, 0xF7                     - ok
void MCommands::doSetUpI()
{
  int id = 0;
  id = Wake->replyU16( id, Tools->getUpI() ); // 0x
  Wake->configAsk( id, MCmd::cmd_set_up_i );
}

// Команда восстановления заводского верхнего порога отключения по току  0x6D;
// Запрос: 0xC0, 0x6D, 0x00, 0x6D                           - ok
// Ответ:  0xC0, 0x6D, 0x01, 0x00, 0x5C                     - ok
void MCommands::doSetUpDefaultI() 
{
  int id = 0;
  Wake->configAsk( id, MCmd::cmd_set_up_default_i );
}

// ================ Команды универсальные ================

// нет операции   0x00
// Запрос: 0xC0, 0x00, 0x00, 0x6D                           - 
// Ответ:  0xC0, 0x00, 0x01, 0x00, 0x5C                     - 
void MCommands::doNop()
{  
  Wake->configAsk( 0x00, MCmd::cmd_nop );

}

// Считать информацию о драйвере
// Запрос: 0xC0, 0x03, 0x00, 0xEB                           - ok
// Ответ:  0xC0, 0x03, 0x13, 0x44 ... 0x0A, 0x00, 0x9C      - добавить байт ошибки
void MCommands::doInfo()
{  
  int id = 0;
  Wake->configAsk( id, MCmd::cmd_info );
}
