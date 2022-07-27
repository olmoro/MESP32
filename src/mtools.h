#ifndef _MTOOLS_H_
#define _MTOOLS_H_

#include "stdint.h"

class MBoard;
class MDisplay;
class MKeyboard;
class Preferences;

class MTools
{
  public:
    MTools(MBoard * board, MDisplay * display);
    ~MTools();
   
    //pointers are public - easier to access
    MBoard      * Board        = nullptr; // external
    MDisplay    * Display      = nullptr; // external
    MKeyboard   * Keyboard     = nullptr; // local
    Preferences * qPreferences = nullptr; // local

    // Буфер команд(ы) на исполнение, при необходимости будет очередь (queue) 
    uint8_t commBuff  = 0x00;   

    short error;              // Код ошибки обработки команды драйвером
    void  setErr(short err);
    short getErr();

    // Локализация
    bool getLocalization() const;
    void setLocalization(bool);
    
    // Флаг блокировки обмена с драйвером на время его рестарта
    bool getBlocking();
    void setBlocking(bool);

    // Флаг выбора режима коррекции смещения АЦП
    bool getTuningAdc();
    void setTuningAdc(bool);

    // перенесенные из privat

    float voltageMax  = 14.5f;   // Заданное напряжение заряда или источника питания, В
    float voltageMin  = 13.2f;   // Заданное минимальное снижение напряжения, В
    float currentMax  =  5.0f;   // Заданный максимальный ток заряда, А
    float currentMin  =  0.5f;   // Заданный минимальный ток заряда, А

    // Настройки АЦП
    short offsetAdc = 0x0000;
    short adcV      = 0x0000;
    short adcI      = 0x0000;

    short getAdcV();
    short getAdcI();
    
    // Переменные настроек измерителей
    short factorV   = 0x2DA0;   // Коэффициент преобразования в милливольты
    short smoothV   = 0x0003;   // Коэффициент фильтрации
    short shiftV    = 0x0000;   // Начальное смещение в вольтах

    short factorI   = 0x030C;   // Коэффициент преобразования в миллиамперы
    short smoothI   = 0x0003;   // Коэффициент фильтрации
    short shiftI    = 0x0000;   // Начальное смещение в миллиамперах

    // ========== Управление ПИД-регулятором, частота фиксирована ==========

bool setPidCoefficients(float kp, float ki, float kd);


      // Pid parameters 
    uint16_t setpoint   = 0x0800;     // 
    uint16_t setpointU  = 0x3390;     // 13200
    uint16_t setpointI  = 0x0BB8;     //  3000
    uint16_t pidMode    = 0x01;       // 1 - начать с регулирования по напряжению
    uint16_t kp         = 0x0019;
    uint16_t ki         = 0x000C;
    uint16_t kd         = 0x0019;
    uint16_t minOut     = 0x0220;
    uint16_t maxOut     = 0x1000;

    // PWM
    uint8_t  pwmInvert = (uint8_t)false;   // Выбор полярности PWM (v55: для отключения при сбросе - 0x00)
    uint16_t pwmPeriod = 0x1012;           // Выбор частоты (через период)




    // Test
    uint8_t  swOnOff   = (uint8_t)false;




  //   // Предвычисления коэффициентов
  // void  setPid( float kp, float ki, float kd );    // KP, KI, KD 





    // Текущие целочисленные в мВ и мА напряжение и ток преобразуются в вольты и амперы 
  void    setVoltageVolt(short);
  float   getVoltageVolf();
  void    setCurrentAmper(short);
  float   getCurrentAmper();
  void    setState1(uint8_t val);
  uint8_t getState1();
  void    setState2(uint8_t val);
  uint8_t getState2();
  void    setCelsius(short val);

//  void  setProtErr(uint8_t val);  // protocol error - или подтверждения исполнения команды 

    // Номинальное напряжение батареи, В
    float getVoltageNom() const;
    void  setVoltageNom(float);

    //float capacityNom = 55.0f;   // Номинальная емкость батареи, А
    //float getCapacityNom();
    //void  setCapacityNom(float);

    float getRealVoltage(); //+m
    float getRealCurrent(); //+m

    // // Индекс массива выбора батареи    
    // int  getAkbInd();
    // void setAkbInd(int);

    // Заданная емкость батареи, А-ч
    float getCapacity();
    void  setCapacity(float);

    // Заданное напряжение заряда, В
    float getVoltageMax();
    void  setVoltageMax(float);

    // Заданное минимальное снижение напряжения, В
    float getVoltageMin();
    void  setVoltageMin(float);

    // Заданный максимальный ток заряда, А
    float getCurrentMax();
    void  setCurrentMax(float);

    // Заданный минимальный ток заряда, А
    float getCurrentMin();
    void  setCurrentMin(float);    

    // Заданное напряжение предзаряда, В
    float getVoltagePre();
    void  setVoltagePre(float); 

    // Заданный ток предзаряда, А
    float getCurrentPre();
    void  setCurrentPre(float);    

    // Заданное минимальное напряжение разряда, В
    float getVoltageDis();
    void  setVoltageDis(float); 

    // Заданный ток разряда, А
    float getCurrentDis();
    void  setCurrentDis(float);    

//    float currentOld  =  0.0f;   // Для формирования импульсов в фазе удержания напряжения nu

    // Накопитель для вычисления среднего (тока)
    float getCollectAvr();
    void  setCollectAvr(float);    

    // Средний ток в импульсе
    float getCurrentAvr();
    void  setCurrentAvr(float);    

    //bool  keepCurr    = false;  // Признак окончания роста тока 
    bool  getKeepCurr();
    void  setKeepCurr(bool);    

    // Признак перехода к фазе удержания напряжения nu?
    bool  getToKeepVolt();
    void  setToKeepVolt(bool);    

    // Заданная длительность импульса заряда, с 
    float getDurationOn();
    void  setDurationOn(float);    

    // Заданная пауза или длительность разряда, с
    float getDurationOff();
    void  setDurationOff(float);    

    int   getNumCycles();
    void  setNumCycles(int);

    int   getPause();
    void  setPause(int);
   

    int   getCycle();
    void  setCycle(int);
    void  decCycle(); 


    int   getCycles();
    void  setCycles(int);

    short getParamMult();
    void  setParamMult(short pm);

    // будут в private
//    float powO        =  0.0f;   // резерв

//    int   cycle;
//    int   cycles;               // Всего задано циклов в имульсе заряда, разряда или паузы
    int   postpone    =  0;      // Заданная задержка включения (отложенный старт), ч


    bool getAP();

    void  zeroAhCounter();      // Обнуление счетчика ампер-часов заряда


  int   readNvsBool  (const char * name, const char * key, const bool  defaultValue );
  int   readNvsInt   (const char * name, const char * key, const int   defaultValue );
  float readNvsFloat (const char * name, const char * key, const float defaultValue );

  void  writeNvsBool (const char * name, const char * key, const bool bValue );
  void  writeNvsInt  (const char * name, const char * key, const int iValue );
  void  writeNvsFloat(const char * name, const char * key, const float fValue );


    void  clearAllKeys (const char * name);


    // void incBattery();
    // void decBattery();

// 202207
    short updnInt(short value, short below, short above, short additives);
    float updnFloat(float value, float below, float above, float additives);




      // 202207 read/write nvs
    void  saveInt(const char * name, const char * key, const short value);
    short readInt(const char * name, const char * key, const short defaultValue);
    void  saveFloat(const char * name, const char * key, const float fvalue);
    short readFloat(const char * name, const char * key, const short defaultValue);


    void activateExit(const char * s);

    void clrTimeCounter();
    void clrChargeTimeCounter();
    void clrAhCharge();
    int  getTimeCounter();
    void setTimeCounter( int ivalue );

    uint8_t getBuffCmd();
    void    setBuffCmd(uint8_t cmd);

    short getCooler();
    void  setCooler(short val);

      // АЦП - настройки
    void  setAdcV(short val);
    void  setAdcI(short val);
    short getAdcOffset();
    void  setAdcOffset(short val);

    short getLtV();
    short getUpV();
    short getLtI();
    short getUpI();

// ======================== ЦЕЛЕВЫЕ КОМАНДЫ УПРАВЛЕНИЯ ДРАЙВЕРОМ SAMD21 MO MINI ========================

      // Команды чтения результатов измерений
    void txReadUIS();                                     // 0x10;
    void txGetU();                                        // 0x11 Чтение напряжения (мВ)
    void txGetI();                                        // 0x12 Чтение тока (мА)
    void txGetUI();                                       // 0x13 Чтение напряжения (мВ) и тока (мА)
    void txGetState();                                    // 0x14 Чтение состояния
    void txCelsius();                                     // 0x15 Чтение температуры радиатора
      
      // 
    void txPowerGo(float spU, float spI, uint8_t mode);   // 0x20
    void txPowerStop();                                   // 0x21

      // Команды работы с измерителем напряжения
        // Множитель преобразования в милливольты
    void txGetFactorU();                                  // 0x30 Чтение
    void txSetFactorU(short val);                         // 0x31 Запись
    void txSetFactorDefaultU();                           // 0x32 Возврат к заводскому
        // Параметр сглаживания
    void txGetSmoothU();                                  // 0x33 Чтение
    void txSetSmoothU(short val);                         // 0x34 Запись
        // Приборный сдвиг
    void txGetShiftU();                                   // 0x35 Чтение
    void txSetShiftU(short val);                          // 0x36 Запись

      // Команды работы с измерителем тока
        // Множитель преобразования в миллиамперы
    void txGetFactorI();                                  // 0x38 Чтение
    void txSetFactorI(short val);                         // 0x39 Запись
    void txSetFactorDefaultI();                           // 0x3A Возврат к заводскому
        // Параметр сглаживания
    void txGetSmoothI();                                  // 0x3B Чтение
    void txSetSmoothI(short val);                         // 0x3C Запись
        // Приборный сдвиг
    void txGetShiftI();                                   // 0x3D Чтение
    void txSetShiftI(short val);                          // 0x3E Запись

      // Команды работы с ПИД-регулятором
    void txSetPidConfig(uint8_t m, float kp, float ki, float kd, uint16_t minOut, uint16_t maxOut);   // 0x40 Запись

    void txSetPidCoeff(short m, float kp, float ki, float kd);    // 0x41 Запись
    void txSetPidCoeffV(float kp, float ki, float kd);    // 0x41 Запись
    void txSetPidCoeffI(float kp, float ki, float kd);    // 0x41 Запись
    void txSetPidCoeffD(float kp, float ki, float kd);    // 0x41 Запись

    void txSetPidOutputRange(uint8_t m, uint16_t minOut, uint16_t maxOut);                               // 0x42
    void txSetPidReconfig(uint8_t m, float kp, float ki, float kd, uint16_t minOut, uint16_t maxOut);    // 0x43, w/o clear
    void txPidClear();                                    // 0x44

    void txGetPidParamMult();                             // 0x47 Get param_mult
    void txGetPidConfig();                                // 0x48 get mode, kP, kI, kD, min, max - возвращает параметры текущего режима регулирования

    void txSetCooler(short val);                          // 0x4F Задать скорость вентилятора

    void txGetProbes();                                   // 0x50
    void txGetAdcOffset();                                // 0x51
    void txSetAdcOffset(short val);                       // 0x52
    void txAdcAutoOffset();                               // 0x53 (пока в резерве)


//   // Команды тестовые
// const uint8_t cmd_set_switch_pin            = 0x54; // sw_pin D4 PA14

// const uint8_t cmd_set_power                 = 0x56; // пользоваться с осторожностью - выяснение пределов регулирования
// const uint8_t cmd_set_discharge             = 0x57; // не проверена
// const uint8_t cmd_set_voltage               = 0x58; // старая, не проверена
// const uint8_t cmd_set_current               = 0x59; // старая, не проверена 
// const uint8_t cmd_set_discurrent            = 0x5A; // старая, не проверена
// const uint8_t cmd_set_surge_compensation    = 0x5B; // параметры подавления всплеска напряжения na
// const uint8_t cmd_set_idle_load             = 0x5C; // параметры доп.нагрузки ХХ

  // Команды задания порогов отключения
    void txGetLtV();                                      // 0x60
    void txSetLtV(short val);                             // 0x61
    void txSetLtDefaultV(short val);                      // 0x62
    void txGetUpV();                                      // 0x63
    void txSetUpV(short val);                             // 0x64
    void txSetUpDefaultV(short val);                      // 0x65

    void txGetLtI();                                      // 0x68
    void txSetLtI(short val);                             // 0x69
    void txSetLtDefaultI(short val);                      // 0x6A
    void txGetUpI();                                      // 0x6B
    void txSetUpI(short val);                             // 0x6C
    void txSetUpDefaultI(short val);                      // 0x6D

// =====================================================================================================





    
//============================ Power ===============================================

    const float pows[6][2] = { { 3.3f, 12.0f},
                              { 5.0f,  8.0f},
                              { 6.3f,  6.0f},
                              { 9.0f,  4.0f}, //**
                              {12.0f,  3.0f},
                              {16.0f,  2.0f} };

        int powInd =  3;

  //  void savePowInd( const char * name ); 
  //  void savePowO( const char * name ); 

    void upPow();
    void dnPow();

//============================ Charger ===============================================
    // Значения только для теста (читать дефолтные при чистой памяти)
    // int   akbInd = 3;           // в private?

    // const float akb[7][2] = { {12.0f, 100.f},    // авто 
    //                         {12.0f,  90.0f},      //  "
    //                         {12.0f,  75.0f},      //  "
    //                         {12.0f,  55.0f},      //  "
    //                         {12.0f,   9.0f},      // ибп
    //                         { 6.0f,  12.0f},      // дет.авто
    //                         { 6.0f,   4.5f} };    // фонарь

    int   getChargeTimeCounter();       // common?
    float getAhCharge();                // common?
    int   getFulfill();

    bool postponeCalculation();

    void chargeCalculations();
    void showUpDn();                // " UP/DN, В-выбор "


    // Any constants - приборные ограничения
    const float capacity_l          =  10.0f;   // минимальная емкость АКБ, Ач
    const float capacity_h          = 200.0f;   // максимальная емкость АКБ, Ач

    const float min_akb_u           =   3.5f;   // min_AkbU минимальное напряжение АКБ, в
    const float max_akb_u           =  16.5f;   // max_ZU максимально разрешенное напряжение при установке, в
    const float min_akb_i           =   0.2f;
    const float max_akb_i           =  12.0f;   // приборное ограничение тока, А

    const float volt_l              =   1.0f;
    const float volt_h              =  16.0f;

    const int min_akb_delay         =   0;      // Ограничения отсрочки заряда, ч
    const int max_akb_delay         =  24;      //
    const int charge_time_out_limit =  1; //48;      // Ограничение продолжительности заряда, ч

    const float current_increment_in_one_step = 0.50f; // / 0.20; // приращение тока за один шаг, А


//==== PRIVATE ==== PRIVATE ==== PRIVATE ==== PRIVATE ==== PRIVATE ==== PRIVATE ==== PRIVATE ==== PRIVATE ==== PRIVATE ====
private:
// Пороги отключения по умолчанию: мВ, мА  (имитация аппаратной поддержки)
static constexpr short lt_default_v  =  -200;   // при переплюсовке
static constexpr short up_default_v  = 18000;   // исполняется конструктивно
static constexpr short lt_default_i  = -1500;   // максимальный ток разряда
static constexpr short up_default_i  =  6000;   // максимальный ток заряда
  // Пороги отключения: мВ, мА  (имитация аппаратной поддержки)
short ltV = lt_default_v;
short upV = up_default_v;
short ltI = lt_default_i;
short upI = up_default_i;





  float voltage     = 0.0f;    // Напряжение на клеммах аккумулятора, В
  float current     = 0.0f;    // Текущий измеренный ток, А
  uint8_t state1    = 0x00;
  uint8_t state2    = 0x00;
  short celsius     = 0x0000;   // Температура радиатора в "попугаях" АЦП

// Переменные будут здесь // это параноя //
    bool  localization = true ;     // LAT
    float voltageNom  = 12.0f;   // Номинальное напряжение батареи, В
  //float capacityNom = 55.0f;   // Номинальная емкость батареи, А    nu
    float capacity    = 50.f;    // Заданная емкость батареи, А-ч

//    float voltageMax  = 14.5f;   // Заданное напряжение заряда, В
//    float voltageMin  = 13.2f;   // Заданное минимальное снижение напряжения, В
//    float currentMax  =  5.0f;   // Заданный максимальный ток заряда, А
//    float currentMin  =  0.5f;   // Заданный минимальный ток заряда, А

    float voltagePre  =  6.0f;   // Заданное напряжение предзаряда, В
    float currentPre  =  3.0f;   // Заданный ток предзаряда, А
    float voltageDis  = 10.7f;   // Заданное минимальное напряжение разряда, В
    float currentDis  =  2.8f;   // Заданный ток разряда, А
    float collectAvr  =  0.0f;   // Накопитель для вычисления среднего (тока)
    float currentAvr  =  0.0f;   // Средний ток в импульсе
    bool  keepCurr    = false;   // Признак окончания роста тока   
    bool  toKeepVolt  = false;   // Признак перехода к фазе удержания напряжения nu?
    float durationOn  = 10.0f;   // Заданная длительность импульса заряда, с 
    float durationOff =  5.0f;   // Заданная пауза или длительность разряда, с

    int   numCycles   =  3;      // Заданное число циклов, например заряд/разряд
    int   pause       =  1;      // Заданная пауза между зарядом и разрядом, ч
    // float powO        =  0.0f;   // резерв

    int   cycle;
    int   cycles;               // Всего задано циклов в имульсе заряда, разряда или паузы
    // int   postpone    =  0;      // Заданная задержка включения (отложенный старт), ч
    int   count;                // для подсчета числа измерений тока в импульсе

// Пределы регулирования уточнить
// Некоторые пределы зависят от выбора батареи ( const только для приборных ограничений !!! ) 
// Здесь указать приборные ограничения !!!

    const bool onlimit = false;
    const bool onloop  = true;

    // Power
    const int number_of_powers = 6;

    // Charge
    // float integral = 0.0f;
    // float temp = 0.0f;
    const int number_of_batteries = 7;
    float ahCharge = 0.0f;
    int   fulfill  = 75;            // TEST 
    int   timeCounter = 0;
    int   chargeTimeCounter = 0;

//    void printAll( const char * s );

//================= Measures =====================


    int   keyCode = 0;    




  // ========================== FastPID ==========================
      /* Прототип: A fixed point PID controller with a 32-bit internal calculation pipeline.
      https://github.com/mike-matera/FastPID/tree/master/examples/VoltageRegulator
      Константы синхронизированы с ведомым контроллером (20220715)
    */  
  static constexpr uint8_t  param_shift = 12;
  static constexpr uint8_t  param_bits  = 16;
  static constexpr uint16_t param_max   = (((0x1ULL << param_bits)-1) >> param_shift);              // 0x000F
  static constexpr uint16_t param_mult  = (((0x1ULL << param_bits)) >> (param_bits - param_shift)); // 0x1000
  static constexpr uint16_t hz = 10;

  short paramMult;    // Полученный от драйвера


  // Configuration
  uint32_t _p, _i, _d;
//  int64_t _outmax, _outmin; 
  bool _cfg_err; 

  uint32_t floatToParam(float); 
  void setCfgErr(); 

  short cool = 0;           // Скорость вентилятора - уточнить

  bool blocking;    // 
  bool tuningAdc  = false;  // Флаг подстройки смещения АЦП
};

#endif //_MTOOLS_H_