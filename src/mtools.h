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
    uint8_t commBuff = 0x00;


    // Локализация
    bool getLocalization() const;
    void setLocalization(bool);

    // перенесенные из privat

    float voltageMax  = 14.5f;   // Заданное напряжение заряда или источника питания, В
    float voltageMin  = 13.2f;   // Заданное минимальное снижение напряжения, В
    float currentMax  =  5.0f;   // Заданный максимальный ток заряда, А
    float currentMin  =  0.5f;   // Заданный минимальный ток заряда, А

    // Настройки АЦП
    short offsetAdc = 0x0000;
    
    // Переменные настроек измерителей
    short factorV   = 0x2DA0;   // Коэффициент преобразования в милливольты
    short smoothV   = 0x0003;   // Коэффициент фильтрации
    short offsetV   = 0x0000;   // Начальное смещение в вольтах

    short factorA   = 0x030C;   // Коэффициент преобразования в миллиамперы
    short smoothA   = 0x0003;   // Коэффициент фильтрации
    short offsetA   = 0x0000;   // Начальное смещение в миллиамперах

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

  void  setProtErr(uint8_t val);  // protocol error - или подтверждения исполнения команды 

    // Номинальное напряжение батареи, В
    float getVoltageNom() const;
    void  setVoltageNom(float);

    //float capacityNom = 55.0f;   // Номинальная емкость батареи, А
    //float getCapacityNom();
    //void  setCapacityNom(float);

    float getRealVoltage(); //+m
    float getRealCurrent(); //+m

    // Индекс массива выбора батареи    
    int  getAkbInd();
    void setAkbInd(int);

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


    void incBattery();
    void decBattery();

    void incCapacity( float delta, bool way );
    void decCapacity( float delta, bool way );

    void incCurrentMax( float delta, bool way );
    void decCurrentMax( float delta, bool way );

    void incCurrentMin( float delta, bool way );
    void decCurrentMin( float delta, bool way );

    // void incCurrentDis( float delta, bool way );
    // void decCurrentDis( float delta, bool way );    

    void incVoltageMax( float delta, bool way );
    void decVoltageMax( float delta, bool way );

    void incVoltageMin( float delta, bool way );
//    void decVoltageMin( float delta, bool way );

    void incVoltagePow( float delta, bool way );
    void decVoltagePow( float delta, bool way );

    void incVoltagePre( float delta, bool way );
    void decVoltagePre( float delta, bool way );

    void incCurrentPre( float delta, bool way );
    void decCurrentPre( float delta, bool way );

    void incDurationOn( bool way );
    void decDurationOn( bool way );
    void incDurationOff( bool way );
    void decDurationOff( bool way );


    void incCycles();
    void decCycles();

    void incCurrentDis( float delta, bool way );
    void decCurrentDis( float delta, bool way );

    void incVoltageDis( float delta, bool way );
    void decVoltageDis( float delta, bool way );

    void incPause();
    void decPause();

    int   incNum( int v, int h, int d );
    int   decNum( int v, int l, int d );


// inc, dec 201905
    float incFloatValue( float value,  float value_l, float value_h, float delta ); 
    float decFloatValue( float value,  float value_l, float value_h, float delta );
    int   incIntValue( int value, int value_l, int value_h, int delta );
    int   decIntValue( int value, int value_l, int value_h, int delta );
//2020
    int   upiVal( int val, int min, int max, int delta );
    int   dniVal( int val, int min, int max, int delta );
    float upfVal( float val, float min, float max, float delta ); 
    float dnfVal( float val, float min, float max, float delta ); 


    void saveInt(const char * name, const char * key, const int ivalue );
    void saveFloat(const char * name, const char * key, const float fvalue );

    void activateExit(const char * s);

    void clrTimeCounter();
    void clrChargeTimeCounter();
    void clrAhCharge();
    int  getTimeCounter();
    void setTimeCounter( int ivalue );


// Общие для импульсного разряда

    void initCurrentAvr();
    void addCollectAvr( float amp );
    int  cnt = 0;
    
    float calcCurrentAvr();


    void shutdownCharge();
    void shutdownDC();


    void orderCmd(uint8_t cmd);


//    void orderReadOffsetVoltage();
//    void orderWriteOffsetVoltage();

    
//============================ Power ===============================================

    const float pows[6][2] = { { 3.3f, 12.0f},
                              { 5.0f,  8.0f},
                              { 6.3f,  6.0f},
                              { 9.0f,  4.0f}, //**
                              {12.0f,  3.0f},
                              {16.0f,  2.0f} };

        int powInd =  3;
    //static int   selectedDelay   =  0;
    //float timeCounter = 0;

    void savePowInd( const char * name ); 
    void savePowO( const char * name ); 

    void upPow();
    void dnPow();
//    void liveU();

    void powShutdown(); 

//============================ Charger ===============================================
    // Значения только для теста (читать дефолтные при чистой памяти)
    int   akbInd = 3;           // в private?

    const float akb[7][2] = { {12.0f, 100.f},    // авто 
                            {12.0f,  90.0f},      //  "
                            {12.0f,  75.0f},      //  "
                            {12.0f,  55.0f},      //  "
                            {12.0f,   9.0f},      // ибп
                            { 6.0f,  12.0f},      // дет.авто
                            { 6.0f,   4.5f} };    // фонарь

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


    void      setToQueue(uint8_t command);
    uint8_t getFromQueue();


//==== PRIVATE ==== PRIVATE ==== PRIVATE ==== PRIVATE ==== PRIVATE ==== PRIVATE ==== PRIVATE ==== PRIVATE ==== PRIVATE ====
private:
  float voltage     = 0.0f;    // Напряжение на клеммах аккумулятора, В
  float current     = 0.0f;    // Текущий измеренный ток, А
  uint8_t state1    = 0x00;
  uint8_t state2    = 0x00;

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


    const float curr_max_l =  0.2f;
    const float curr_max_h = 12.2f;

  

    const float volt_end_l = 10.0f;
    const float volt_end_h = 16.0f;

    const int   num_cycl_l = 0;
    const int   num_cycl_h = 3;

    const float curr_dis_l =  0.0f;
    const float curr_dis_h =  2.5f;

    const float duration_on_l  =  5.0f;         // для импульсных режимов
    const float duration_on_h  = 10.0f;

    const float duration_off_l  = 2.5f;
    const float duration_off_h  = 5.0f;

    const float volt_min_l = 10.0f;
    const float volt_min_h = 16.0f;

    const int   pause_l = 0;
    const int   pause_h = 3;

    const float volt_pre_l  =  1.5f;
    const float volt_pre_h  = 14.0f;

    const float curr_pre_l  =  0.5f;
    const float curr_pre_h  =  6.0f;



    float incfValue( float value, float min, float max, float delta, bool way );
    float decfValue( float value, float min, float max, float delta, bool way );
    int   inciValue( int value,  int min, int max, int delta );
    int   deciValue( int value,  int min, int max, int delta );


    // Power
    const int number_of_powers = 6;

    // Charge
    float integral = 0.0f;
    float temp = 0.0f;
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
      Константы должны быть синхронизированы с ведомым контроллером.
    */  
    static constexpr uint8_t param_shift =  0;    //0;    //4; //8;
    static constexpr uint8_t param_bits  = 16;    //12;    //16;
    static constexpr uint16_t param_max  = (((0x1 << param_bits)-1) >> param_shift); 
    static constexpr uint16_t param_mult = (((0x1 << param_bits)) >> (param_bits - param_shift));

    static constexpr uint16_t hz = 250;

  // Configuration
  uint32_t _p, _i, _d;
//  int64_t _outmax, _outmin; 
  bool _cfg_err; 

  uint32_t floatToParam(float); 
  void setCfgErr(); 




};

#endif //_MTOOLS_H_