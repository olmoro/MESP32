/*


*/

        //#include "SPIFFS.h"
        #include <Update.h>


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

    // Индекс массива выбора батареи    
int  MTools::getAkbInd() { return akbInd; }
void MTools::setAkbInd(int _akbInd) { akbInd = _akbInd; }

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

//Preferences qPreferences;


int MTools::readNvsBool(const char * name, const char * key, const bool defaultValue )
{
  qPreferences->begin(name, true);                    // RW-mode (second parameter has to be false).
  bool val = qPreferences->getBool(key, defaultValue);
  qPreferences->end();                                // Close the Preferences
  return val;  
}

int MTools::readNvsInt(const char * name, const char * key, const int defaultValue )
{
  qPreferences->begin(name, true);                    // RW-mode (second parameter has to be false).
  int val = qPreferences->getInt(key, defaultValue);
  qPreferences->end();                                // Close the Preferences
  return val;  
}

float MTools::readNvsFloat(const char * name, const char * key, const float defaultValue )
{
  qPreferences->begin(name, true);
  float val = qPreferences->getFloat(key, defaultValue);
  qPreferences->end();
  return val;
}

void MTools::writeNvsBool(const char * name, const char * key, const bool bValue )
{
  qPreferences->begin(name, false);
  qPreferences->putBool(key, bValue);
//    Serial.println( qPreferences.getInt( key, iValue ));      // test
  qPreferences->end();
}

void MTools::writeNvsInt(const char * name, const char * key, const int iValue )
{
  qPreferences->begin(name, false);
  qPreferences->putInt(key, iValue);
//    Serial.println( qPreferences.getInt( key, iValue ));      // test
  qPreferences->end();
}

void MTools::writeNvsFloat(const char * name, const char * key, const float fValue )
{
  qPreferences->begin(name, false);
  qPreferences->putFloat(key, fValue);
//    Serial.println( qPreferences.getFloat( key, fValue ));
  qPreferences->end();
}

void MTools::clearAllKeys(const char * name) {
    qPreferences->begin(name, false);
    qPreferences->clear();
    qPreferences->end();
}


//Saves 201905
void MTools::saveInt(   const char * name, const char * key, const int   ivalue ) { writeNvsInt(   name, key, ivalue ); }
void MTools::saveFloat( const char * name, const char * key, const float fvalue ) { writeNvsFloat( name, key, fvalue ); }


void MTools::showUpDn() 
{ 
//    Oled->showLine2Text(" UP/DN, В-выбор "); 
}


void MTools::activateExit(const char * s)
{
    // Oled->showLine1Heap(ESP.getFreeHeap());
    Board->ledsOff(); 
}

//==================== common ==========================

void MTools::incBattery()
{
    if(akbInd == 0 ) { akbInd = number_of_batteries - 1; }
    else { akbInd--; } 
    voltageNom  = akb[akbInd][0];
    capacity = akb[akbInd][1];
//    Oled->showLine3Akb( voltageNom, capacity );
}

void MTools::decBattery()
{
    if(akbInd == number_of_batteries - 1 ) { akbInd = 0; }
    else { akbInd++; } 
    voltageNom  = akb[akbInd][0];
    capacity = akb[akbInd][1];
//    Oled->showLine3Akb( voltageNom, capacity );
}

void MTools::incCapacity( float delta, bool way )
{
    capacity = incfValue( capacity, capacity_l, capacity_h, delta, way );
//    Oled->showLine3Capacity( capacity ); 
}
void MTools::decCapacity( float delta, bool way )
{
    capacity = decfValue( capacity, capacity_l, capacity_h, delta, way );
//    Oled->showLine3Capacity( capacity ); 
}

void MTools::incCurrentMax( float delta, bool way )
{
    currentMax = incfValue( currentMax, curr_max_l, curr_max_h, delta, way );
//    Oled->showLine3MaxI( currentMax );
}
void MTools::decCurrentMax( float delta, bool way )
{
    currentMax = decfValue( currentMax, curr_max_l, curr_max_h, delta, way );
//    Oled->showLine3MaxI( currentMax );
}

void MTools::incCurrentMin( float delta, bool way )
{
    currentMin = incfValue( currentMin, curr_max_l, curr_max_h, delta, way );   // пределы уточнить
//    Oled->showLine3MaxI( currentMin );
}
void MTools::decCurrentMin( float delta, bool way )
{
    currentMin = decfValue( currentMin, curr_max_l, curr_max_h, delta, way );   // пределы уточнить
//    Oled->showLine3MaxI( currentMin );
}

// void MTools::incCurrentDis( float delta, bool way )
// {
//     currentDis = incfValue( currentDis, curr_dis_l, curr_dis_h, delta, way );   // пределы уточнить
//     Oled->showLine3MaxI( currentDis );
// }
// void MTools::deccurrentDis( float delta, bool way )
// {
//     currentDis = decfValue( currentDis, curr_dis_l, curr_dis_h, delta, way );   // пределы уточнить
//     Oled->showLine3MaxI( currentDis );
// }




void MTools::incVoltageMax( float delta, bool way )
{
    voltageMax = incfValue( voltageMax, volt_end_l, volt_end_h, delta, way );
//    Oled->showLine3MaxU( voltageMax );
}
void MTools::decVoltageMax( float delta, bool way )
{
    voltageMax = decfValue( voltageMax, volt_end_l, volt_end_h, delta, way );
//    Oled->showLine3MaxU(voltageMax);
}

void MTools::incVoltageMin( float delta, bool way )
{
    voltageMin = incfValue( voltageMin, volt_end_l, volt_end_h, delta, way );   // пределы уточнить
//    Oled->showLine3MaxU( voltageMin );
}
// void MTools::decVoltageMin( float delta, bool way )
// {
//     voltageMin = decfValue( voltageMin, volt_end_l, volt_end_h, delta, way );   // пределы уточнить
// //    Oled->showLine3MaxU(voltageMin);
// }

void MTools::incDurationOn( bool way )
{
    durationOn = incfValue ( durationOn, duration_on_l, duration_on_h, 0.5, way );
//    Oled->showLine3Sec(durationOn); 
}
void MTools::decDurationOn( bool way )
{
    durationOn = decfValue ( durationOn, duration_on_l, duration_on_h, 0.5, way );
//    Oled->showLine3Sec(durationOn); 
}

void MTools::incDurationOff( bool way)
{
    durationOff = incfValue ( durationOff, duration_off_l, duration_off_h, 0.5, way );
//    Oled->showLine3Sec(durationOff); 
}
void MTools::decDurationOff( bool way )
{
    durationOff = decfValue ( durationOff, duration_off_l, duration_off_h, 0.5, way );
//    Oled->showLine3Sec(durationOff); 
}

// void MTools::incPostpone( int delta )
// {
//     postpone = inciValue( postpone, postpone_l, postpone_h, delta );
// //    Oled->showLine3Delay( postpone );
// }
// void MTools::decPostpone( int delta )
// {
//     postpone = deciValue( postpone, postpone_l, postpone_h, delta );
// //    Oled->showLine3Delay( postpone );
// }

void MTools::incVoltagePre( float delta, bool way )
{
    voltagePre = incfValue( voltagePre, volt_pre_l, volt_pre_h, delta, way );
//    Oled->showLine3MaxU( voltagePre );
}
void MTools::decVoltagePre( float delta, bool way )
{
    voltagePre = decfValue( voltagePre, volt_pre_l, volt_pre_h, delta, way );
//    Oled->showLine3MaxU( voltagePre );
}

void MTools::incCurrentPre( float delta, bool way )
{
    currentPre = incfValue( currentPre, curr_pre_l, curr_pre_h, delta, way );
//    Oled->showLine3MaxI( currentPre );
}
void MTools::decCurrentPre( float delta, bool way )
{
    currentPre = decfValue( currentPre, curr_pre_l, curr_pre_h, delta, way );
//    Oled->showLine3MaxI( currentPre );
}


void MTools::incVoltagePow( float delta, bool way )
{
    voltageMax = incfValue( voltageMax, volt_l, volt_h, delta, way );
//    Oled->showLine3MaxU( voltageMax );
}
void MTools::decVoltagePow( float delta, bool way )
{
    voltageMax = decfValue( voltageMax, volt_l, volt_h, delta, way );
//    Oled->showLine3MaxU(voltageMax);
}

void MTools::incCycles()
{
    numCycles = inciValue ( numCycles, num_cycl_l, num_cycl_h, 1 );
//    Oled->showLine3Num(numCycles); 
}
void MTools::decCycles()
{
    numCycles = deciValue ( numCycles, num_cycl_l, num_cycl_h, 1 );
//    Oled->showLine3Num(numCycles); 
}

void MTools::incCurrentDis( float delta, bool way )
{
    currentDis = incfValue( currentDis, curr_dis_l, curr_dis_h, delta, way );
//    Oled->showLine3MaxI( currentDis );
}
void MTools::decCurrentDis( float delta, bool way )
{
    currentDis = decfValue( currentDis, curr_dis_l, curr_dis_h, delta, way );
//    Oled->showLine3MaxI( currentDis );
}

void MTools::incVoltageDis( float delta, bool way )
{
    voltageDis = incfValue( voltageDis, volt_min_l, volt_min_h, delta, way );
//    Oled->showLine3MaxU( voltageDis );
}
void MTools::decVoltageDis( float delta, bool way )
{
    voltageDis = decfValue( voltageDis, volt_min_l, volt_min_h, delta, way );
//    Oled->showLine3MaxU(voltageDis);
}

void MTools::incPause()
{
    pause = inciValue( pause, pause_l, pause_h, 1 ); 
//    Oled->showLine3Delay( pause ); 
}
void MTools::decPause()
{
    pause = deciValue( pause, pause_l, pause_h, 1 ); 
//    Oled->showLine3Delay( pause ); 
}

int MTools::incNum( int v, int h, int d )
{
    v += d;
    if (v >= h) return h;
    else        return v;
}
int MTools::decNum( int v, int l, int d )
{
    v -= d;
    if (v <= l) return l;
    else        return v;
}

// inc, dec 201905
float MTools::incFloatValue( float value,  float value_l, float value_h, float delta ) {
    if( value >= value_h - delta ) return value_h;
    else return value += delta;
}

float MTools::decFloatValue( float value,  float value_l, float value_h, float delta ) {
    if( value <= value_l + delta ) return value_l;
    else return value -= delta;
}

int MTools::incIntValue( int value, int value_l, int value_h, int delta ) {
  if( value >= value_h - delta ) return value_h;
  else return value += delta;
}

int MTools::decIntValue( int value, int value_l, int value_h, int delta ) {
  if( value <= value_l + delta ) return value_l;
  else return value -= delta;
}


void MTools::clrTimeCounter() { timeCounter = 0; }
void MTools::clrChargeTimeCounter() { chargeTimeCounter = 0; }
void MTools::clrAhCharge() { ahCharge = 0; }
int  MTools::getTimeCounter() { return timeCounter; }
void MTools::setTimeCounter( int ivalue ) { timeCounter = ivalue; }




void MTools::shutdownCharge() 
{
//    output = 0.0;

    shutdownDC();
    // cmd = cmd_power_stop;            // doPowerStop()    0x21
    Board->ledsRed();
}

void MTools::shutdownDC()           // Подумать об общей команде
{
    //Board->powOff();
    Board->swOff();
    Board->ledsOff();
}

    uint8_t buffCmd = MCmd::cmd_nop;             // 0x00 - нет операции

uint8_t MTools::getBuffCmd() { return buffCmd; }

// Вариант 2022 - не доделано
// Здесь будет проверка ответа и задаваться число попыток связи с драйвером
// с учетом времени на получение ответа (порядка 1,5 мс)
// Буфер очищать: 0x00 - пустая команда
bool MTools::powerGo(short spU, short spI, uint8_t mode)
{
    setpointU = spU;
    setpointI = spI;
    pidMode   = mode;
    buffCmd = MCmd::cmd_power_go;
    buffCmd = MCmd::cmd_nop;
    return true;
}      // 2022 0x20


bool MTools::powerStop()   { buffCmd = MCmd::cmd_power_stop; buffCmd = MCmd::cmd_nop; return true; }      // 2022 0x21




  bool adcUpOffset() {buffCmd = MCmd::cmd_adc_up_offset; return true;}                         // 0x51
  bool adcDnOffset() {buffCmd = MCmd::cmd_adc_dn_offset; return true;}                         // 0x52
  bool adcFbOffset() {buffCmd = MCmd::cmd_adc_fb_offset; return true;}                         // 0x53

//================= Power =========================================

void MTools::savePowInd(const char * name) { writeNvsInt( name, "powInd", powInd ); }
//void MTools::savePowU(const char * name) { writeNvsInt( name, "voltMax", voltageMax ); }
//void MTools::savePowI(const char * name) { writeNvsInt( name, "currMax", currentMax ); }
//void MTools::savePowO(const char * name) { writeNvsInt( name, "powO", powO ); }

void MTools::upPow()
{
    if(powInd == 0 ) { powInd = number_of_powers - 1; }
    else { powInd--; } 
    voltageMax = pows[powInd][0];
    currentMax = pows[powInd][1];
//    Oled->showLine3Power( voltageMax, currentMax );
}

void MTools::dnPow() 
{
    if(powInd == number_of_powers - 1 ) { powInd = 0; }
    else { powInd++; } 
    voltageMax = pows[powInd][0];
    currentMax = pows[powInd][1];
//    Oled->showLine3Power( voltageMax, currentMax );
}


// void MTools::liveU()
// {
//     setPoint = voltageMax;
// }

//================= Charger =========================================

void MTools::powShutdown() 
{
//    output = 0.0;
    Board->swOff();
    Board->ledsOff();
    Board->ledROn();
}


bool MTools::postponeCalculation()
{
    timeCounter--;
    chargeTimeCounter = timeCounter / 10;   //2;
    if( chargeTimeCounter == 0 ) return true;
      return false;
}


void MTools::initCurrentAvr() { collectAvr = 0.0f;  cnt = 0; count = 0; }       // оставить один счетчик

void MTools::addCollectAvr( float amp ) { collectAvr += amp;  cnt++; count++; }       // оставить один счетчик

float MTools::calcCurrentAvr()
{
    if( cnt == 0 ) return 0.0;
    #ifdef DEBUG_CHARGE
        Serial.print("getCurrentAver : ");  
        Serial.print( collectAvr ); Serial.print(", ");    
        Serial.print( cnt );        Serial.print(", ");  
        Serial.println( collectAvr / (float)cnt );
    #endif
    return collectAvr / (float)cnt;
}







// ========================== Service ====================================

// mb common
float MTools::incfValue( float value,  float value_l, float value_h, float delta, bool way ) 
{
    if( value >= value_h - delta )
    {
        if( way ) { return value_l; }
        else      { return value_h; }        
    } else return value += delta;
}

float MTools::upfVal( float val, float val_l, float val_h, float delta )   //2020
{
    if( (val += delta) > val_h ) return val_h; 
    return val;
}


float MTools::decfValue( float value,  float value_l, float value_h, float delta, bool way )
{
    if( value <= value_l + delta )
    {
        if( way ) { return value_h; }
        else      { return value_l; } 
    } else return value -= delta;
}

float MTools::dnfVal( float val, float val_l, float val_h, float delta )   //2020
{
    if( (val -= delta) < val_l ) return val_l; 
    return val;
}


int MTools::inciValue( int value,  int value_l, int value_h, int delta ) 
{
    if( value >= value_h ) return value_l; 
    return value += delta; 
}

int MTools::upiVal( int val, int val_l, int val_h, int delta ) //2020
{
    if( (val += delta) > val_h ) return val_h; 
    return val;
}

int MTools::deciValue( int value,  int value_l, int value_h, int delta )
{
    if( value <= value_l ) return value_h; 
    return value -= delta; 
}

int MTools::dniVal( int val, int val_l, int val_h, int delta ) //2020
{
    if( (val -= delta) < val_l ) return val_l; 
    return val;
}
