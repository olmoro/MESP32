#ifndef _MBOARD_H_
#define _MBOARD_H_

/*
 * Вариант  202101
 * pcb: eltr_v56, v5.7
 */

#include "stdint.h"

class MSupervisor;
class PCF8574;

class MBoard
{
  public:
    MSupervisor * Supervisor = nullptr;       // local

    static constexpr uint8_t ch_fan = 1;      // PWM channel

  public:
    MBoard();

    void initEsp32();
    void initPCF8574();
    void initControl();
    void initSD();

    void ledROn();	  // Turn the RED LED on
    void ledROff();	  // Turn the RED LED off

    void ledGOn();    // Turn the GREEN LED on
    void ledGOff();	  // Turn the GREEN LED off

    void ledBOn();	  // Turn the BLUE LED on
    void ledBOff();	  // Turn the BLUE LED off

    void ledsOn();	
    void ledsOff();
    void ledsRed();
    void ledsGreen();
    void ledsBlue();
    void ledsYellow();	

    void blinkWhite(short cnt);
    void blinkWhite();
    void blinkRed(short cnt);
    void blinkGreen(short cnt);
    void blinkBlue(short cnt);
    void blinkYellow(short cnt);

    void buzzerOn();
    void buzzerOff();

    void lcdBlkOn();
    void lcdBlkOff();

//    void lcdRstOn();
//    void lcdRstOff();

    void swOn();
    void swOff();

    void outOn();
    void outOff();

//    void p6On();
//    void p6Off();

    bool getNeg();
    void  calculateCelsius();
    float getCelsius();
    int16_t getAdcPG();

    // uint16_t getSetpoint()  { return setpoint; }
    // uint16_t getSetpointU() { return setpointU; }
    // uint16_t getSetpointI() { return setpointI; }
    // uint16_t getPidMode()   { return pidMode; }
    // uint16_t getKp()        { return kp; }
    // uint16_t getKi()        { return ki; }
    // uint16_t getKd()        { return kd; }
    // uint16_t getMinOut()    { return minOut; }
    // uint16_t getMaxOut()    { return maxOut; }


    //uint16_t getOffsetAdc() { return offsetAdc; }

    //int16_t getFactorU()    { return factorU; }
    //int8_t  getSmoothU()    { return smoothU; }
    //int16_t getOffsetU()    { return offsetU; }

    //int16_t getFactorI()    { return factorI; }
    //int8_t  getSmoothI()    { return smoothI; }
    //int16_t getOffsetI()    { return offsetI; }

    // int8_t  getPwmInvert()  { return pwmInvert; } //= (uint8_t)false;   // Выбор полярности PWM (v55: для отключения при сбросе - 0x00)
    // int16_t getPwmPeriod()  { return pwmPeriod; } //= 0x1012;           // Выбор частоты (через период)

    // int8_t  getSwOnOff()    { return swOnOff; }
    int16_t getPwmVal()     { return pwmVal; }
    int16_t getDacVal()     { return dacVal; }
    int8_t  getPerc()       { return perc; }
    int16_t getIdleI()      { return idleI; }   // Минимальный ток, при котором не нужна дополнительная нагрузка 
    int16_t getIdleDac()    { return idleDac; } // Ток в коде DAC

    int16_t getWinLU()      { return winLU; }
    int16_t getWinUpU()     { return winUpU; }
    int16_t getWinLI()      { return winLI; }
    int16_t getWinUpI()     { return winUpI; }

//    float getRealVoltage(); //+21
//    float getRealCurrent(); //+21

    // Настройки АЦП

//    void  setAdcOffset( short );
//    int16_t readAdcOffset() { return adcOffset; }
    void  setAdcV( short );
    void  setAdcI( short );


          // вернуть в tools
    // Текущие целочисленные в мВ и мА напряжение и ток преобразуются в вольты и амперы 
//    void  setVoltageVolt(short);
//    float getVoltageVolf();
//    void  setCurrentAmper(short);
//    float getCurrentAmper();

//    void  setSt1(uint8_t val);
//    void  setSt2(uint8_t val);
//    void  setProtErr(uint8_t val);  // protocol error - или подтверждения исполнения команды 

    // Обработка ответов от драйвера силовой платы
    void  readFactorU(short);
    void  readSmoothU(uint8_t);
    //void  readOffsetU(short);
    void  readFactorI(short);
    void  readSmoothI(uint8_t);
    void  readOffsetI(short);

    // ПИД
    void  readPidMode(uint8_t);    // mode
    void  readKp(short); // kP
    void  readKi(short); // kI
    void  readKd(short); // kD
    void  readMinOut(short); // min
    void  readMaxOut(short); // max



  private:
    PCF8574 * PCF_38 = nullptr;

    uint8_t pcfOut;

    /* Set cycles that measurement operation takes the result from touchRead, threshold and detection
    *  accuracy depend on these values. Defaults are 0x1000 for measure and 0x1000 for sleep.
    *  With default values touchRead takes 0.5ms */
    const uint16_t time_measure = 0x1000; //0x1000;
    const uint16_t time_sleep   = 0x1000; //0x1000;

      // Время ожидания ответа по UART2
    #ifdef UART2
      const unsigned long time_out = 200;
    #endif


    
    // uint16_t touchInput = weight_ratio_no;      //
    // int holdTime = 0;                           // Время удержания кнопки ( * nu )


    // MF52AT MF52 B 3950 NTC термистор 2% 10 kOm
    const float reference_resistance = 10000.0f;    // R2 10kOm 1%
    const float nominal_resistance   = 10000.0f;    //    10kOm 2%
    const float nominal_temperature  =    25.0f;
    const float b_value              =  3950.0f;

    float celsius = 25.0;

    float readVoltage( int adc );
    float readSteinhart( const int adc );


// ================= Это всё дубли, окончательно они в tools 

    // // Pid parameters 
    // uint16_t setpoint   = 0x0800;     // 
    // uint16_t setpointU  = 0x3390;     // 13200
    // uint16_t setpointI  = 0x0BB8;     //  3000
    // uint16_t pidMode    = 0x01;       // 1 - начать с регулирования по напряжению
    // uint16_t kp         = 0x0019;
    // uint16_t ki         = 0x000C;
    // uint16_t kd         = 0x0019;
    // uint16_t minOut     = 0x0220;
    // uint16_t maxOut     = 0x1000;

    // ADCU
    //uint16_t factorU   = 0x2DA0;    //0x2D1A;
    //uint8_t  smoothU   = 0x03;
    //uint16_t offsetU   = 0x0000;
    // ADCI
    //uint16_t factorI   = 0x030C;
    //uint8_t  smoothI   = 0x03;
    //uint16_t offsetI   = 0x000B;
   
    // // PWM
    // uint8_t  pwmInvert = (uint8_t)false;   // Выбор полярности PWM (v55: для отключения при сбросе - 0x00)
    // uint16_t pwmPeriod = 0x1012;           // Выбор частоты (через период)

    // // Test
    // uint8_t  swOnOff   = (uint8_t)false;
    uint16_t pwmVal    = 0x0335;
    uint16_t dacVal    = 0x0222;
    uint8_t  perc      = 50;
    uint16_t idleI     = 0x0555;        // Минимальный ток, при котором не нужна дополнительная нагрузка 
    uint16_t idleDac   = 0x0107;        // Ток в коде DAC

    // Пороги отключения                   default
    int16_t winLU     =  -200;          // 0xFF38;
    int16_t winUpU    = 18000;          // 0x4650;
    int16_t winLI     = -1500;          // 0xFA24;
    int16_t winUpI    =  5000;          // 0x1388;



    int16_t adcV      = 0x0000;
    int16_t adcI      = 0x0000;
    int16_t adcOffset = 10;    

//    float voltage      =  0.0f;    // //+21 Напряжение на клеммах аккумулятора, В ( с поправкой на падение напряжения на проводах)
//    float current      =  0.0f;    // //+21 Текущий измеренный ток, А
// ======================================================================

};

#endif // !_MBOARD_H_
