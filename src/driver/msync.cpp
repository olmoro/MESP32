/*
  Синхронизация с драйвером силовой платы
  2022 июль
 */

#include "driver/msync.h"
#include "nvs.h"
#include "driver/mcommands.h"
#include "board/mboard.h"
#include "mtools.h"
#include "mcmd.h"
#include "mwake.h"
#include "stdint.h"
#include <Arduino.h>


MSync::MSync(MTools * tools) : Tools(tools), Board(tools->Board) 
{ 
  Wake = new MWake(); 
  //Wake->wakeInit( 0x00, 50 );  // Адрес в сети и время ожидания ответа
}

MSync::~MSync()
{
  delete Wake;
}
// Имя устройства
//static constexpr char Info[] = {" QMoro Rev0.0\n\0"};   // Убрать для активного 


//uint8_t cmd = MCmd::cmd_nop;

//uint8_t state1 = 0b00000000;
//uint8_t state2 = 0b00000000;


//void MSync::writeCmd(uint8_t _cmd) { cmd = _cmd; }


void MSync::doSync()
{
  static short cnt = 0;
  cnt++;
//   if(cnt >= 9) cnt = 0;

  switch (cnt)
  {
    case 1:
      Tools->postpone  = Tools->readNvsInt(MNvs::nQulon, MNvs::kQulonPostpone, 3);

    break;
    case 2:
      Tools->offsetAdc = Tools->readNvsInt(MNvs::nQulon, MNvs::kOffsetAdc, 0x0000);  // Смещение ЦАП
      Tools->setAdcOffset();                    // 0x21  Команда драйверу
    break;
    case 3:
      Tools->factorV   = Tools->readNvsInt(MNvs::nQulon, MNvs::kFactorV, 0x2DA0);  // Множитель преобразования
      Tools->setFactorU();                      // 0x32  Команда драйверу
    break;
    case 4:  
      Tools->smoothV   = Tools->readNvsInt(MNvs::nQulon, MNvs::kSmoothV, 0x0003);  // Коэффициент фильтрации
      Tools->setSmoothU();                      // 0x34  Команда драйверу
    break;
    case 5:
      Tools->offsetV   = Tools->readNvsInt(MNvs::nQulon, MNvs::kOffsetV, 0x0000);  // Смещение в милливольтах
      Tools->setOffsetU();                      // 0x36  Команда драйверу
    break;
    case 6:
      Tools->factorA   = Tools->readNvsInt(MNvs::nQulon, MNvs::kFactorA, 0x030C);  // Множитель преобразования
      Tools->setFactorI();                      // 0x39  Команда драйверу
    break;
    case 7:
      Tools->smoothA   = Tools->readNvsInt(MNvs::nQulon, MNvs::kSmoothA, 0x0003);  // Коэффициент фильтрации
      Tools->setSmoothI();                      // 0x3C  Команда драйверу
    break;
    case 8:
      Tools->offsetA   = Tools->readNvsInt(MNvs::nQulon, MNvs::kOffsetA, 0x0000);  // Смещение в миллиамперах
      Tools->setOffsetI();                      // 0x3E  Команда драйверу
    break;


    case 10:
      Tools->voltageMax  = Tools->readNvsFloat(MNvs::nQulon, MNvs::kCcCvVmax, 14.5f); // Заданное максимальное напряжение заряда, В
      // Tools->setVoltMax();                      // 0x..  Команда драйверу  
    break;
    case 11:
      Tools->voltageMin  = Tools->readNvsFloat(MNvs::nQulon, MNvs::kCcCvVmin, 13.2f); // Заданное минимальное напряжение заряда, В
      // Tools->setVoltMin();                      // 0x..  Команда драйверу  
    break;
    case 12:
      Tools->currentMax  = Tools->readNvsFloat(MNvs::nQulon, MNvs::kCcCvImax,  5.0f); // Заданный максимальный ток заряда, А
      // Tools->setCurrMax();                      // 0x..  Команда драйверу
    break;
    case 13:
      Tools->currentMin  = Tools->readNvsFloat(MNvs::nQulon, MNvs::kCcCvImin,  0.5f); // Заданный минимальный ток заряда, А
      // Tools->setCurrMax();                      // 0x..  Команда драйверу
    break;

      // Tools->setVoltMax();                      // 0x..  Команда драйверу  
      // Tools->setVoltMin();                      // 0x..  Команда драйверу  
      // Tools->setCurrMax();                      // 0x..  Команда драйверу
      // Tools->setCurrMax();                      // 0x..  Команда драйверу


    default:
    break;
  }

//   if( cmd != MCmd::cmd_nop)
//   {
//     #ifdef DEBUG_COMMANDS
//       Serial.print(" command -> 0x"); Serial.println(cmd, HEX);
//     #endif

//     switch( cmd )
//     {
//       //Команды управления процессами
//       case MCmd::cmd_read_u_i:                readUI();                   break;  // 0x10;   +
//       case MCmd::cmd_get_u:                   doGetU();                   break;  // 0x11 Чтение напряжения (мВ)
//       case MCmd::cmd_get_i:                   doGetI();                   break;  // 0x12 Чтение тока (мА)
//       case MCmd::cmd_get_ui:                  doGetUI();                  break;  // 0x13 Чтение напряжения (мВ) и тока (мА)
//       case MCmd::cmd_get_state:               doGetState();               break;  // 0x14 Чтение состояния
//       case MCmd::cmd_get_celsius:             doCelsius();                break;  // 0x15 Чтение температуры радиатора

//         // Команды управления
//       case MCmd::cmd_power_go:                doPowerGo();                break;  // 0x20   +
//       case MCmd::cmd_power_stop:              doPowerStop();              break;  // 0x21   +

//       //case MCmd::cmd_write_pid:             doSetPid();                 break;  // 0x22

//         // Команды работы с измерителем напряжения 
//       case MCmd::cmd_read_factor_u:           doGetFactorU();             break;  // 0x30   + 00->03
//       case MCmd::cmd_write_factor_u:          doSetFactorU();             break;  // 0x31   + 02->01
//       case MCmd::cmd_write_factor_default_u:  doSetFactorDefaultU();      break;  // 0x32   + 00->01
//       case MCmd::cmd_read_smooth_u:           doGetSmoothU();             break;  // 0x33   + 00->02
//       case MCmd::cmd_write_smooth_u:          doSetSmoothU();             break;  // 0x34   + 01->01
//       case MCmd::cmd_read_offset_u:           doGetOffsetU();             break;  // 0x35   + 00->03
//       case MCmd::cmd_write_offset_u:          doSetOffsetU();             break;  // 0x36   + 02->01
      
//         // Команды работы с измерителем тока
//       case MCmd::cmd_read_factor_i:             doGetFactorI();             break;  // 0x38   + 00->03
//       case MCmd::cmd_write_factor_i:            doSetFactorI();             break;  // 0x39   + 02->01
//       case MCmd::cmd_write_factor_default_i:    doSetFactorDefaultI();      break;  // 0x3A   + 00->01
//       case MCmd::cmd_read_smooth_i:             doGetSmoothI();             break;  // 0x3B   + 00->02
//       case MCmd::cmd_write_smooth_i:            doSetSmoothI();             break;  // 0x3C   + 01->01
//       case MCmd::cmd_read_offset_i:             doGetOffsetI();             break;  // 0x3D   + 00->03
//       case MCmd::cmd_write_offset_i:            doSetOffsetI();             break;  // 0x3E   + 02->01

//         // Команды работы с ПИД-регулятором
//       case MCmd::cmd_pid_configure:             doPidConfigure();           break;  // 0x40   + 0B->01
//       case MCmd::cmd_pid_write_coefficients:    doPidSetCoefficients();     break;  // 0x41   + 07->01
//       case MCmd::cmd_pid_output_range:          doPidOutputRange();         break;  // 0x42   + 05->01
//       case MCmd::cmd_pid_reconfigure:           doPidReconfigure();         break;  // 0x43   + 0B->01
//       case MCmd::cmd_pid_clear:                 doPidClear();               break;  // 0x44   + 01->01
//       case MCmd::cmd_pid_test:                  doPidTest();                break;  // 0x46   + 03->01
//       case MCmd::cmd_pwm_configure:             doPwmConfigure();           break;  // 0x47   + 03->01
//       case MCmd::cmd_pid_read_configure:        doPidGetConfigure();        break;  // 0x48   + 00->0C
//   //  case MCmd::cmd_pid_write_max_sum:         doPidSetMaxSum();           break;  // 0x49   + 0?->0?

//         // Команды работы с АЦП
//       case MCmd::cmd_adc_read_probes:           doReadProbes();             break;  // 0x50   + 00->07
//       case MCmd::cmd_adc_read_offset:           doAdcGetOffset();           break;  // 0x51   + 00->03
//       case MCmd::cmd_adc_write_offset:          doAdcSetOffset();           break;  // 0x52   + 02->01
// //case MCmd::cmd_adc_up_offset:           doAdcUpOffset();           break;  // 0x51
// //case MCmd::cmd_adc_dn_offset:           doAdcDnOffset();           break;  // 0x52
// //case MCmd::cmd_adc_fb_offset:           doAdcFbOffset();           break;  // 0x52


//         // Команды управления портами управления (в основном тестовые)
//       case MCmd::cmd_write_switch_pin:          doSwPin();                  break;  // 0x54     01->01
//         // Команды тестовые
//       case MCmd::cmd_write_power:               doSetPower();               break;  // 0x56     04->01
//       case MCmd::cmd_write_discharge:           doSetDischg();              break;  // 0x57     01->01
//       case MCmd::cmd_write_voltage:             doSetVoltage();             break;  // 0x58     03->03
//       case MCmd::cmd_write_current:             doSetCurrent();             break;  // 0x59     05->03
//       case MCmd::cmd_write_discurrent:          doSetDiscurrent();          break;  // 0x5A     03->01
//   //   case MCmd::cmd_write_surge_compensation:  doSurgeCompensation();      break;  // 0x5B   nu
//       case MCmd::cmd_write_idle_load:           doIdleLoad();               break;  // 0x5C   

//         // Команды задания порогов отключения
//       case MCmd::cmd_read_win_less_u:           doGetWinLtU();              break;  // 0x60;     00->03
//       case MCmd::cmd_write_win_less_u:          doSetWinLtU();              break;  // 0x61;     02->01
//       case MCmd::cmd_write_win_less_default_u:  doSetWinLtDefaultU();       break;  // 0x62;     02->01
//       case MCmd::cmd_read_win_up_u:             doGetWinUpU();              break;  // 0x63;     00->03
//       case MCmd::cmd_write_win_up_u:            doSetWinUpU();              break;  // 0x64;     02->01
//       case MCmd::cmd_write_win_up_default_u:    doSetWinUpDefaultU();       break;  // 0x65;     02->01

//       case MCmd::cmd_read_win_less_i:           doGetWinLtI();              break;  // 0x68;     00->03
//       case MCmd::cmd_write_win_less_i:          doSetWinLtI();              break;  // 0x69;     02->01
//       case MCmd::cmd_write_win_less_default_i:  doSetWinLtDefaultI();       break;  // 0x6A;     02->01
//       case MCmd::cmd_read_win_up_i:             doGetWinUpI();              break;  // 0x6B;     00->03
//       case MCmd::cmd_write_win_up_i:            doSetWinUpI();              break;  // 0x6C;     02->01
//       case MCmd::cmd_write_win_up_default_i:    doSetWinUpDefaultI();       break;  // 0x6D;     02->01

//         // Команды универсальные
//       case MCmd::cmd_nop:                       doNop();                    break;  // 0x00
//       case MCmd::cmd_info:                      doInfo();                   break;  // 0x03

//       default: 
//       break ;
  //   }
  //   cmd = MCmd::cmd_nop;       // не надо
  // }

}
