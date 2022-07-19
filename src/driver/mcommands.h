#ifndef _MCOMMANDS_H_
#define _MCOMMANDS_H_
/*
  19.07.2022
*/

#include "stdint.h"

class MTools;
class MBoard;
class MWake;

class MCommands
{
  public:
    MCommands(MTools * tools);
    ~MCommands();

    void doCommand();
    short dataProcessing();

  private:
    MTools * Tools;   // = nullptr;
    MBoard * Board;
    MWake  * Wake;    // = nullptr;

  public:
      enum ROLES
    {
      RS = 0,    // режим прямого регулирования
      RU,        // режим управления напряжением
      RI,        // режим управления током
      RD         // режим управления током разряда
    };

    void writeCmd(uint8_t cmd);
    
  
  private:
    // static constexpr uint8_t param_shift = 12;
    // static constexpr uint8_t param_bits  = 16;
    // static constexpr uint16_t param_max  = (((0x1 << param_bits)-1) >> param_shift); 
    // static constexpr uint16_t param_mult = (((0x1 << param_bits)) >> (param_bits - param_shift));

    // static constexpr uint16_t hz = 10;

      //Команды управления процессами
    void readUI();                    // 0x10 Чтение напряжения (мВ), тока (мА), состояния

    void doGetU();                    // 0x11 Чтение напряжения (мВ)
    void doGetI();                    // 0x12 Чтение тока (мА)
    void doGetUI();                   // 0x13 Чтение напряжения (мВ) и тока (мА)
    void doGetState();                // 0x14 Чтение состояния
    void doCelsius();                 // 0x15 Чтение температуры радиатора


      // Команды управления
    void doPowerGo();                // 0x20   
    void doPowerStop();              // 0x21   
    //void doSetPid();                // 0x22 nu

      // Команды работы с измерителем напряжения 
    void doGetFactorU();             // 0x30   
    void doSetFactorU();             // 0x31   
    void doSetFactorDefaultU();      // 0x32   
    void doGetSmoothU();             // 0x33   
    void doSetSmoothU();             // 0x34   
    void doGetOffsetU();             // 0x35   
    void doSetOffsetU();             // 0x36   
      
      // Команды работы с измерителем тока
    void doGetFactorI();             // 0x38   
    void doSetFactorI();             // 0x39   
    void doSetFactorDefaultI();      // 0x3A   
    void doGetSmoothI();             // 0x3B   
    void doSetSmoothI();             // 0x3C   
    void doGetOffsetI();             // 0x3D   
    void doSetOffsetI();             // 0x3E   

      // Команды работы с регуляторами
    void doPidConfigure();           // 0x40   
    void doPidSetCoefficients();     // 0x41   
    void doPidOutputRange();         // 0x42   
    void doPidReconfigure();         // 0x43   
    void doPidClear();               // 0x44   
    void doPidTest();                // 0x46   
    void doPidGetParamMult();        // 0x47   
    void doPidGetConfigure();        // 0x48   
//    void doPidSetMaxSum();           // 0x49    nu

      // Команды работы с АЦП
    void doReadProbes();             // 0x50   
    void doAdcGetOffset();           // 0x51   
    void doAdcSetOffset();           // 0x52   

      // Команды управления портами управления (в основном тестовые)
    void doSwPin();                  // 0x54   
      // Команды тестовые
    void doSetPower();               // 0x56   
    void doSetDischg();              // 0x57
    void doSetVoltage();             // 0x58   
    void doSetCurrent();             // 0x59   
    void doSetDiscurrent();          // 0x5A 
    void doSurgeCompensation();      // 0x5B   
    void doIdleLoad();               // 0x5C   

      // Команды задания порогов отключения
    void doGetWinLtU();              // 0x60
    void doSetWinLtU();              // 0x61
    void doSetWinLtDefaultU();       // 0x62
    void doGetWinUpU();              // 0x63 
    void doSetWinUpU();              // 0x64 
    void doSetWinUpDefaultU();       // 0x65 

    void doGetWinLtI();              // 0x68 
    void doSetWinLtI();              // 0x69  
    void doSetWinLtDefaultI();       // 0x6A 
    void doGetWinUpI();              // 0x6B 
    void doSetWinUpI();              // 0x6C
    void doSetWinUpDefaultI();       // 0x6D 

      // Команды универсальные
    void doNop();                    // 0x00
//    void doErr();                    // 0x01
//    void doEcho();                   // 0x02
    void doInfo();                   // 0x03

    // uint32_t floatToParam(float in);
    void txU08(uint8_t id,  uint8_t value);
    void txU16(uint8_t id, uint16_t value);
    void txU32(uint8_t id, uint32_t value);

};

#endif  //!_MCOMMANDS_H_
