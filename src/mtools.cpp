/*
    Набор методов, доступных разработчику для программирования собственных
 режимов работы прибора.

              !!!!!!!!!!!!!!!!!!!!!!!!!!!! 2022 июль
    Продолжается очистка от мусора проекта-прототипа, который был без драйвера.          
*/

        //#include "SPIFFS.h"
        //#include <Update.h>


#include "mtools.h"
#include "mcmd.h"
#include "driver/mcommands.h"
#include "board/mboard.h"
#include "board/mkeyboard.h"
#include "display/mdisplay.h"
#include "driver/mcommands.h"
#include <Preferences.h>
#include "Arduino.h"

    
MTools::MTools(MBoard * board, MDisplay * display) : 
  Board(board),
  Display(display),
  Keyboard(new MKeyboard),
  qPreferences(new Preferences) {}

MTools::~MTools()
{
  delete qPreferences;
  delete Keyboard;
}
  //analog:
  //MTools::MTools(MBoard * board, MDisplay * display, MKeyboard * keyboard) 
  //{
  //      Board = board;
  //      Display = display; 
  //      Keyboard = keyboard;
  //}

// Доступ к переменным MTools private
    // Локализация (зарезервировано)
bool MTools::getLocalization() const { return localization; }
void MTools::setLocalization( bool _localization ) { localization = _localization; }

    // Флаг блокировки обмена с драйвером на время его рестарта
bool MTools::getBlocking() {return blocking;}
void MTools::setBlocking(bool bl) {blocking = bl;}

    // Флаг выбора режима коррекции смещения АЦП
bool MTools::getTuningAdc() {return tuningAdc;}
void MTools::setTuningAdc(bool tu) {tuningAdc = tu;}

void  MTools::setPostpone(short hour) {postpone = hour;}
short MTools::getPostpone() {return postpone;}



  // ========================== FastPID ==========================
bool MTools::setPidCoefficients(float kp, float ki, float kd)
{
  _cfg_err = 0;
  _p = floatToParam(kp);
  _i = floatToParam(ki / hz);
  _d = floatToParam(kd * hz);

  #ifdef DEBUG_PID
    Serial.print("param_max=0x"); Serial.println(param_max, HEX);
    Serial.print("param_mult=0x"); Serial.println(param_mult, HEX);
    Serial.print("_p=0x"); Serial.println(_p, HEX);
    Serial.print("_i=0x"); Serial.println(_i, HEX);
    Serial.print("_d=0x"); Serial.println(_d, HEX);
    Serial.print("err=0x"); Serial.println(_cfg_err, HEX);
  #endif

  return ! _cfg_err;
}

uint32_t MTools::floatToParam(float in)
{
    if (in > param_max || in < 0) 
    {
        _cfg_err = true;
        return 0;
    }

    uint32_t param = in * param_mult;

    if (in != 0 && param == 0) 
    {
        _cfg_err = true;
        return 0;
    }
  
    return param;
}


void MTools::setCfgErr() 
{
    _cfg_err = true;
    _p = _i = _d = 0;
}

//   // Предвычисления коэффициентов param_mult = 0x0100

// void   MTools::setPid( float kpf, float kif, float kdf )    // KP, KI, KD 
// {
//     kp = (uint16_t) (kpf * param_mult);                     // 0.1 -> 
// //    ki = (uint16_t) (kpf / MCommands::hz * 0x0100);
// //    kd = (uint16_t) (kdf * hz * 0x0100);
// }


  //Текущие целочисленные в мВ и мА напряжение и ток преобразуются в вольты и амперы 
void    MTools::setVoltageVolt(short val)   { voltage = (float)val / 1000; }
float   MTools::getVoltageVolf()            { return voltage; }
void    MTools::setCurrentAmper(short val)  { current = (float)val / 1000; }
float   MTools::getCurrentAmper()           { return current; }
void    MTools::setState1(uint8_t val)      { state1 = val; }
uint8_t MTools::getState1()                 { return state1; }
void    MTools::setState2(uint8_t val)      { state2 = val; }
uint8_t MTools::getState2()                 { return state2; }
void    MTools::setCelsius(short val)       { celsius = val; }

// void  MTools::setProtErr(uint8_t val)  // protocol error - или подтверждения исполнения команды
// {
//   // Обработка зарезервирована
// }

void  MTools::setErr(short err) { error = err; }
short MTools::getErr()          { return error; }

bool  MTools::getAP() { return false; }



float MTools::getVoltageNom() const { return voltageNom; }
void  MTools::setVoltageNom(float _voltageNom) { voltageNom = _voltageNom; }


float MTools::getRealVoltage() { return voltage; }  //-21
float MTools::getRealCurrent() { return current; }  //-21

//float MTools::getCapacityNom() { return capacityNom; }
//void  MTools::setCapacityNom(float _capacityNom) { capacityNom = _capacityNom; }

//     // Индекс массива выбора батареи    
// int  MTools::getAkbInd() { return akbInd; }
// void MTools::setAkbInd(int _akbInd) { akbInd = _akbInd; }

float MTools::getCapacity() { return capacity; }
void  MTools::setCapacity(float _capacity) { capacity = _capacity; }

float MTools::getVoltageMax() { return voltageMax; }
void  MTools::setVoltageMax(float _voltageMax) { voltageMax = _voltageMax; }

float MTools::getVoltageMin() { return voltageMin; }
void  MTools::setVoltageMin(float _voltageMin) { voltageMin = _voltageMin; }

float MTools::getCurrentMax() { return currentMax; }
void  MTools::setCurrentMax(float _currentMax) { currentMax = _currentMax; }

float MTools::getCurrentMin() { return currentMin; }
void  MTools::setCurrentMin(float _currentMin) { currentMin = _currentMin; }

float MTools::getVoltagePre() { return voltagePre; }
void  MTools::setVoltagePre(float _voltagePre) { voltagePre = _voltagePre; } 

float MTools::getCurrentPre() { return currentPre; }
void  MTools::setCurrentPre(float _currentPre) { currentPre = _currentPre; }

float MTools::getVoltageDis() { return voltageDis; }
void  MTools::setVoltageDis(float _voltageDis) { voltageDis = _voltageDis; } 

float MTools::getCurrentDis() { return currentDis; }
void  MTools::setCurrentDis(float _currentDis) { currentDis = _currentDis; }

float MTools::getCollectAvr() { return collectAvr; }
void  MTools::setCollectAvr(float _collectAvr) { collectAvr = _collectAvr; }

float MTools::getCurrentAvr() { return currentAvr; }
void  MTools::setCurrentAvr(float _currentAvr) { currentAvr = _currentAvr; }

bool  MTools::getKeepCurr() { return keepCurr; }
void  MTools::setKeepCurr(bool _keepCurr) { keepCurr = _keepCurr; }    

bool  MTools::getToKeepVolt() { return toKeepVolt; }
void  MTools::setToKeepVolt(bool _toKeepVolt) { toKeepVolt = _toKeepVolt; }    

float MTools::getDurationOn() { return durationOn; }
void  MTools::setDurationOn(float _durationOn) { durationOn = _durationOn; }

float MTools::getDurationOff() { return durationOff; }
void  MTools::setDurationOff(float _durationOff) { durationOff = _durationOff; }

int   MTools::getNumCycles() { return numCycles; }
void  MTools::setNumCycles(int _numCycles) { numCycles = _numCycles; }

int   MTools::getPause() { return pause; }
void  MTools::setPause(int _pause) { pause = _pause; }

int   MTools::getCycle() { return cycle; }                              // pulse...
void  MTools::setCycle(int _cycle) { cycle = _cycle; }
void  MTools::decCycle() { cycle--; } 

int   MTools::getCycles() { return cycles; }                            // pulse...
void  MTools::setCycles(int _cycles) { cycles = _cycles; }


void MTools::zeroAhCounter() { timeCounter = 0; ahCharge = 0.0; }      // Обнуление счетчика ампер-часов заряда

int   MTools::getChargeTimeCounter() { return chargeTimeCounter; }
float MTools::getAhCharge()    { return ahCharge; }

int   MTools::getFulfill() { return fulfill; }

void MTools::chargeCalculations()
{
    timeCounter++;
    chargeTimeCounter = ((int)timeCounter / 10);
    //ahCharge += Tools->getRealCurrent() / 36000.0;     //    ((float)( 1000 / simpleChargerPeriod ) * 3600.0);
    ahCharge += current / 36000.0;     //    ((float)( 1000 / simpleChargerPeriod ) * 3600.0);
}

// ==================================== Nvs read/write ====================================

bool MTools::readNvsBool(const char * name, const char * key, const bool defaultValue)
{
  qPreferences->begin(name, false);                    // RW-mode (second parameter has to be false).
  bool val = qPreferences->getBool(key, defaultValue);
  qPreferences->end();                                // Close the Preferences
  return val;  
}

short MTools::readNvsShort(const char * name, const char * key, const short defaultValue)
{
  qPreferences->begin(name, false);                    // RW-mode (second parameter has to be false).
  short val = qPreferences->getShort(key, defaultValue);
  qPreferences->end();                                // Close the Preferences
  return val;  
}

int MTools::readNvsInt(const char * name, const char * key, const int defaultValue)
{
  qPreferences->begin(name, false);                    // RW-mode (second parameter has to be false).
  int val = qPreferences->getInt(key, defaultValue);
  qPreferences->end();                                // Close the Preferences
  return val;  
}

float MTools::readNvsFloat(const char * name, const char * key, const float defaultValue)
{
  qPreferences->begin(name, false);
  float val = qPreferences->getFloat(key, defaultValue);
  qPreferences->end();
  return val;
}

void MTools::writeNvsBool(const char * name, const char * key, const bool bValue)
{
  qPreferences->begin(name, false);
  qPreferences->putBool(key, bValue);
  qPreferences->end();
}

void MTools::writeNvsShort(const char * name, const char * key, const short sValue)
{
  qPreferences->begin(name, false);
  qPreferences->putInt(key, sValue);
  qPreferences->end();
}

void MTools::writeNvsInt(const char * name, const char * key, const int iValue )
{
  qPreferences->begin(name, false);
  qPreferences->putInt(key, iValue);
  qPreferences->end();
}

void MTools::writeNvsFloat(const char * name, const char * key, const float fValue )
{
  qPreferences->begin(name, false);
  qPreferences->putFloat(key, fValue);
  qPreferences->end();
}

// Clear all keys in opened preferences
bool MTools::clearAllKeys(const char * name)
{
  qPreferences->begin(name, false);
  bool err = qPreferences->clear();
  qPreferences->end();
  return err;
}

// Remove a key
void MTools::removeKey(const char * name, const char * key)
{
  qPreferences->begin(name, false);
  qPreferences->remove(key);
  qPreferences->end();
}


// // Read/write nvs 202207
// void  MTools::saveInt(const char * name, const char * key, const short value ) {writeNvsInt(name, key, value );}

// short MTools::readInt(const char * name, const char * key, const short defaultValue)
// {
//   qPreferences->begin(name, true);                    // RW-mode (second parameter has to be false).
//   short value = qPreferences->getInt(key, defaultValue);
//   qPreferences->end();                                // Close the Preferences
//   return value;  
// }

// void  MTools::saveFloat(const char * name, const char * key, const float value) {writeNvsFloat( name, key, value);}

// short MTools::readFloat(const char * name, const char * key, const short defaultValue)
// {
//   qPreferences->begin(name, true);
//   float value = qPreferences->getFloat(key, defaultValue);
//   qPreferences->end();
//   return value;
// }
// //!Read/write nvs 202207




void MTools::showUpDn() 
{ 
}


void MTools::activateExit(const char * s)
{
    Board->ledsOff(); 
}

//==================== common ==========================

// void MTools::incBattery()
// {
//     if(akbInd == 0 ) { akbInd = number_of_batteries - 1; }
//     else { akbInd--; } 
//     voltageNom  = akb[akbInd][0];
//     capacity = akb[akbInd][1];
// }

// void MTools::decBattery()
// {
//     if(akbInd == number_of_batteries - 1 ) { akbInd = 0; }
//     else { akbInd++; } 
//     voltageNom  = akb[akbInd][0];
//     capacity = akb[akbInd][1];
// }



void MTools::clrTimeCounter() { timeCounter = 0; }
void MTools::clrChargeTimeCounter() { chargeTimeCounter = 0; }
void MTools::clrAhCharge() { ahCharge = 0; }
int  MTools::getTimeCounter() { return timeCounter; }
void MTools::setTimeCounter( int ivalue ) { timeCounter = ivalue; }


uint8_t buffCmd = MCmd::cmd_nop;             // 0x00 - нет операции

uint8_t MTools::getBuffCmd()            {return buffCmd;}
void    MTools::setBuffCmd(uint8_t cmd) {buffCmd = cmd;}

short MTools::getParamMult() {return paramMult;}
void  MTools::setParamMult(short _pm) {paramMult = _pm;}

short MTools::getCooler() {return cool;}
void  MTools::setCooler(short val) {cool = val;}

void  MTools::setAdcV( short val ) { adcV = val; }
void  MTools::setAdcI( short val ) { adcI = val; }

short MTools::getAdcOffset() {return offsetAdc;}
void  MTools::setAdcOffset(short val) {offsetAdc = val;}

short MTools::getAdcV() {return adcV;}
short MTools::getAdcI() {return adcI;}

short MTools::getLtV() {return ltV;}
short MTools::getUpV() {return upV;}
short MTools::getLtI() {return ltI;}
short MTools::getUpI() {return upI;}

// ======================== ЦЕЛЕВЫЕ КОМАНДЫ УПРАВЛЕНИЯ ДРАЙВЕРОМ SAMD21 MO MINI ========================
  //Команды чтения результатов измерений:
void MTools::txReadUIS()                                {buffCmd = MCmd::cmd_get_uis;}                  // 0x10;
void MTools::txGetU()                                   {buffCmd = MCmd::cmd_get_u;}                    // 0x11 Чтение напряжения (мВ)
void MTools::txGetI()                                   {buffCmd = MCmd::cmd_get_i;}                    // 0x12 Чтение тока (мА)
void MTools::txGetUI()                                  {buffCmd = MCmd::cmd_get_ui;}                   // 0x13 Чтение напряжения (мВ) и тока (мА)
void MTools::txGetState()                               {buffCmd = MCmd::cmd_get_state;}                // 0x14 Чтение состояния
void MTools::txCelsius()                                {buffCmd = MCmd::cmd_get_celsius;}              // 0x15 Чтение температуры радиатора

  // Команды управления силовым преобразователем
void MTools::txPowerGo(float spU, float spI, uint8_t mode)
{
  setpointU = (short)(spU * 1000);
  setpointI = (short)(spI * 1000);
  pidMode   = mode;
  buffCmd = MCmd::cmd_power_go;
}                                                                                                       // 0x20

void MTools::txPowerStop()                              {buffCmd = MCmd::cmd_power_stop;}               // 0x21

  // Команды управления измерителями:
    // Множитель преобразования в милливольты
void MTools::txGetFactorU()                             {buffCmd = MCmd::cmd_read_factor_u;}            // 0x30 Чтение
void MTools::txSetFactorU(short val) {factorV = val;     buffCmd = MCmd::cmd_write_factor_u;}           // 0x31 Запись
void MTools::txSetFactorDefaultU()                      {buffCmd = MCmd::cmd_write_factor_default_u;}   // 0x32 Возврат к заводскому
    // Параметр сглаживания по напряжению
void MTools::txGetSmoothU()                             {buffCmd = MCmd::cmd_read_smooth_u;}            // 0x33 Чтение
void MTools::txSetSmoothU(short val) {smoothV = val;     buffCmd = MCmd::cmd_write_smooth_u;}           // 0x34 Запись
    // Приборное смещение по напряжению
void MTools::txGetShiftU()                              {buffCmd = MCmd::cmd_read_offset_u;}            // 0x35 Чтение
void MTools::txSetShiftU(short val)  {shiftV  = val;     buffCmd = MCmd::cmd_write_offset_u;}           // 0x36 Запись
    // Множитель преобразования в миллиамперы
void MTools::txGetFactorI()                             {buffCmd = MCmd::cmd_read_factor_i;}            // 0x38 Чтение
void MTools::txSetFactorI(short val) {factorI = val;     buffCmd = MCmd::cmd_write_factor_i;}           // 0x39 Запись
void MTools::txSetFactorDefaultI()                      {buffCmd = MCmd::cmd_write_factor_default_i;}   // 0x3A Возврат к заводскому
    // Параметр сглаживания по току
void MTools::txGetSmoothI()                             {buffCmd = MCmd::cmd_read_smooth_i;}            // 0x3B Чтение
void MTools::txSetSmoothI(short val) {smoothI = val;     buffCmd = MCmd::cmd_write_smooth_i;}           // 0x3C Запись
    // Приборное смещение по току
void MTools::txGetShiftI()                              {buffCmd = MCmd::cmd_read_offset_i;}            // 0x3D Чтение
void MTools::txSetShiftI(short val)  {shiftI  = val;     buffCmd = MCmd::cmd_write_offset_i;}           // 0x3E Запись

  // Команды работы с ПИД-регулятором:
void MTools::txSetPidConfig(uint8_t _m, float _kp, float _ki, float _kd, uint16_t _minOut, uint16_t _maxOut)
{
  pidMode = _m;
  kp      = (short) (_kp * param_mult);
  ki      = (short)((_ki * param_mult) * hz);
  kd      = (short)((_kd * param_mult) / hz);
  minOut  = _minOut;
  maxOut  = _maxOut;
  buffCmd = MCmd::cmd_pid_configure;                                                                    // 0x40 Запись
}

void MTools::txSetPidCoeff(short m, float _kp, float _ki, float _kd)    // 0x41 Запись
{
    pidMode = m;
    kp      = (short) (_kp * param_mult);
    ki      = (short)((_ki * param_mult) * hz);
    kd      = (short)((_kd * param_mult) / hz);
    buffCmd = MCmd::cmd_pid_write_coefficients;                                                         // 0x41 Запись
}

void MTools::txSetPidCoeffV(float _kp, float _ki, float _kd)
{
    pidMode = 1;
    kp      = (short)(_kp * param_mult);
    ki      = (short)((_ki * param_mult) * hz);
    kd      = (short)((_kd * param_mult) / hz);
    buffCmd = MCmd::cmd_pid_write_coefficients;                                                         // 0x41 Запись
}

void MTools::txSetPidCoeffI(float _kp, float _ki, float _kd)
{
    pidMode = 2;
    kp      = (short)(_kp * param_mult);
    ki      = (short)((_ki * param_mult) * hz);
    kd      = (short)((_kd * param_mult) / hz);
    buffCmd = MCmd::cmd_pid_write_coefficients;                                                         // 0x41 Запись
}

void MTools::txSetPidCoeffD(float _kp, float _ki, float _kd)
{
    pidMode = 3;
    kp      = (short)(_kp * param_mult);
    ki      = (short)((_ki * param_mult) * hz);
    kd      = (short)((_kd * param_mult) / hz);
    buffCmd = MCmd::cmd_pid_write_coefficients;                                                         // 0x41 Запись
}

void MTools::txSetPidOutputRange(uint8_t _m, uint16_t _minOut, uint16_t _maxOut)
{
    pidMode = _m;
    minOut  = _minOut;
    maxOut  = _maxOut;
    buffCmd = MCmd::cmd_pid_output_range;                                                               // 0x42 Запись
}

void MTools::txSetPidReconfig(uint8_t _m, float _kp, float _ki, float _kd, uint16_t _minOut, uint16_t _maxOut)
{
    pidMode = _m;
    kp      = (short)(_kp * param_mult);
    ki      = (short)((_ki * param_mult) * hz);
    kd      = (short)((_kd * param_mult) / hz);
    minOut  = _minOut;
    maxOut  = _maxOut;
    buffCmd = MCmd::cmd_pid_reconfigure;                                                                // 0x43 Запись
}

void MTools::txPidClear()                               {buffCmd = MCmd::cmd_pid_clear;}                // 0x44
void MTools::txGetPidParamMult()                        {buffCmd = MCmd::cmd_pid_read_param_mult;}      // 0x47 Get param_mult
void MTools::txGetPidConfig()                           {buffCmd = MCmd::cmd_pid_read_configure;}       // 0x48 get mode, kP, kI, kD, min, max - возвращает параметры текущего режима регулирования

  // Команда управления вентилятором
void MTools::txSetCooler(short val)                     {cool = val; buffCmd = MCmd::cmd_set_cooler;}   // 0x4F Задать скорость вентилятора

  // Команды работы с АЦП
void MTools::txGetProbes()                              {buffCmd = MCmd::cmd_adc_read_probes;}          // 0x50
void MTools::txGetAdcOffset()                           {buffCmd = MCmd::cmd_adc_read_offset;}          // 0x51  
void MTools::txSetAdcOffset(short val) {offsetAdc = val; buffCmd = MCmd::cmd_adc_write_offset;}         // 0x52
void MTools::txAdcAutoOffset()                          {buffCmd = MCmd::cmd_adc_auto_offset;}          // 0x53 nu 

  // Команды управления тестовые

  // Команды задания порогов отключения
void MTools::txGetLtV()                            {buffCmd = MCmd::cmd_get_lt_v;}                      // 0x60
void MTools::txSetLtV(short val)        {ltV = val; buffCmd = MCmd::cmd_set_lt_v;}                      // 0x61
void MTools::txSetLtDefaultV(short val) {ltV = val; buffCmd = MCmd::cmd_set_lt_default_v;}              // 0x62
void MTools::txGetUpV()                            {buffCmd = MCmd::cmd_get_up_v;}                      // 0x63
void MTools::txSetUpV(short val)        {upV = val; buffCmd = MCmd::cmd_set_up_v;}                      // 0x64
void MTools::txSetUpDefaultV(short val) {upV = val; buffCmd = MCmd::cmd_set_up_default_v;}              // 0x65

void MTools::txGetLtI()                            {buffCmd = MCmd::cmd_get_lt_i;}                      // 0x68
void MTools::txSetLtI(short val)        {ltI = val; buffCmd = MCmd::cmd_set_lt_i;}                      // 0x69
void MTools::txSetLtDefaultI(short val) {ltI = val; buffCmd = MCmd::cmd_set_lt_default_i;}              // 0x6A
void MTools::txGetUpI()                            {buffCmd = MCmd::cmd_get_up_i;}                      // 0x6B
void MTools::txSetUpI(short val)        {upI = val; buffCmd = MCmd::cmd_set_up_i;}                      // 0x6C
void MTools::txSetUpDefaultI(short val) {upI = val; buffCmd = MCmd::cmd_set_up_default_i;}              // 0x6D


//================= Power =========================================

//void MTools::savePowInd(const char * name) { writeNvsInt( name, "powInd", powInd ); }
//void MTools::savePowU(const char * name) { writeNvsInt( name, "voltMax", voltageMax ); }
//void MTools::savePowI(const char * name) { writeNvsInt( name, "currMax", currentMax ); }
//void MTools::savePowO(const char * name) { writeNvsInt( name, "powO", powO ); }

void MTools::upPow()
{
    if(powInd == 0 ) { powInd = number_of_powers - 1; }
    else { powInd--; } 
    voltageMax = pows[powInd][0];
    currentMax = pows[powInd][1];
}

void MTools::dnPow() 
{
    if(powInd == number_of_powers - 1 ) { powInd = 0; }
    else { powInd++; } 
    voltageMax = pows[powInd][0];
    currentMax = pows[powInd][1];
}

//================= Charger =========================================


bool MTools::postponeCalculation()
{
    timeCounter--;
    chargeTimeCounter = timeCounter / 10;   //2;
    if( chargeTimeCounter == 0 ) return true;
    return false;
}

// 202207 
short MTools::updnInt(short value, short below, short above, short additives)
{
    value += additives;
    if(value > above) return above;
    if(value < below) return below;
    return value;
}

float MTools::updnFloat(float value, float below, float above, float additives)
{
    value += additives;
    if(value > above) return above;
    if(value < below) return below;
    return value;
}
