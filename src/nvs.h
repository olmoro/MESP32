/* Распределение энергонезависимой памяти
* Проект FSM_BS4
* 15.02.2020 2021.05.09 2022.07
*
* Необходимо выдерживать длину имен и ключей не более 15 знаков
* nXXX - имя раздела
* kXXX - ключ параметра
* tXXX - ключ параметра
*/

#ifndef _NVS_H_
#define _NVS_H_

namespace MNvs
{
  static constexpr const char* nQulon          = "qulon";    // Общие 
    static constexpr const char* kQulonLocal     = "local";    //  
    static constexpr const char* kQulonMode      = "mode";     //  
    static constexpr const char* kQulonPostpone  = "postp";    // 

//    static constexpr const char* kOffsetAdc     = "offsetAdc";  // Смещение АЦП

    static constexpr const char* kFactorV       = "factorV";  // Множитель преобразования
    static constexpr const char* kSmoothV       = "smoothV";  // Коэффициент фильтрации
    static constexpr const char* kOffsetV       = "offsetV";  // Сдвиг adcV

    static constexpr const char* kFactorI       = "factorI";  // Множитель преобразования
    static constexpr const char* kSmoothI       = "smoothI";  // Коэффициент фильтрации
    static constexpr const char* kOffsetI       = "offsetI";  // Сдвиг adcI


// старые, будут отменены --------------------------------------
    // static constexpr const char* kKpU       = "kpU";  // PID kPU
    // static constexpr const char* kKiU       = "kiU";  // PID kiU
    // static constexpr const char* kKdU       = "kdU";  // PID kdU

    // static constexpr const char* kKpI       = "kpI";  // PID kPI
    // static constexpr const char* kKiI       = "kiI";  // PID kiI
    // static constexpr const char* kKdI       = "kdI";  // PID kdI

    // static constexpr const char* kKpD       = "kpD";  // PID kPD
    // static constexpr const char* kKiD       = "kiD";  // PID kiD
    // static constexpr const char* kKdD       = "kdD";  // PID kdD
//--------------------------------------------------------------

    static constexpr const char* kQulonAkbU      = "akbU";     //  
    static constexpr const char* kQulonAkbAh     = "akbAh";    //  
    static constexpr const char* kQulonAkbInd    = "akbInd";   //  
    static constexpr const char* kQulonPowInd    = "powInd";   //  

  static constexpr const char* nTemplate       = "template"; // Шаблон 

  static constexpr const char* nDcPower        = "s-power";  // Простой источник 
    static constexpr const char* kDcVmax         = "voltMax";  //
    static constexpr const char* kDcImax         = "currMax";  //

  static constexpr const char* nCcCv          = "cccv";     // Заряд CC/CV
    static constexpr const char* kKpV         = "kpV";  // PID kPU
    static constexpr const char* kKiV         = "kiV";  // PID kiU
    static constexpr const char* kKdV         = "kdV";  // PID kdU

    static constexpr const char* kKpI         = "kpI";  // PID kPI
    static constexpr const char* kKiI         = "kiI";  // PID kiI
    static constexpr const char* kKdI         = "kdI";  // PID kdI

    static constexpr const char* kKpD         = "kpD";  // PID kPD
    static constexpr const char* kKiD         = "kiD";  // PID kiD
    static constexpr const char* kKdD         = "kdD";  // PID kdD

  static constexpr const char* nPTest          = "pidtest";     // ПИД тест
    static constexpr const char* tMode          = "mode";     // Выбор заряд/разряд 

    static constexpr const char* tSpV           = "spV";      //
    static constexpr const char* tSpI           = "spI";      //
    static constexpr const char* tSpD           = "spD";      //

    static constexpr const char* tKpV           = "kpV";      //
    static constexpr const char* tKiV           = "kiV";      //
    static constexpr const char* tKdV           = "kdV";      //

    static constexpr const char* tKpI           = "kpI";      //
    static constexpr const char* tKiI           = "kiI";      //
    static constexpr const char* tKdI           = "kdI";      //    

// старые, будут отменены --------------------------------------
    // static constexpr const char* kMaxU        = "maxU";
    // static constexpr const char* kMinU        = "minU";
//--------------------------------------------------------------

    static constexpr const char* kMaxV        = "maxV";
    static constexpr const char* kMinV        = "minV";
    static constexpr const char* kMaxI        = "maxI";
    static constexpr const char* kMinI        = "minI";






  static constexpr const char* nExChrg         = "e-charge"; // Расширенный заряд

  static constexpr const char* nRecBat         = "recovery"; // Восстановление

  static constexpr const char* nStor           = "storage";  // Хранение
    static constexpr const char* kStorVmax       = "voltMax";
    static constexpr const char* kStorVmin       = "voltMin";
    static constexpr const char* kStorImax       = "currMax";
    static constexpr const char* kStorImin       = "currMin";

  static constexpr const char* nServBat        = "service";  // Сервис батареи



};

#endif  // !_NVS_H_
