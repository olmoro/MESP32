/* Распределение энергонезависимой памяти
* Проект FSM_BS4
* 15.02.2020 2021.05.09
*
* Необходимо выдерживать длину имен и ключей не более 15 знаков
*/

#ifndef _NVS_H_
#define _NVS_H_

/*
* nXXX - имя раздела
* kYYY - ключ параметра
*/

namespace MNvs
{
  static constexpr const char* nQulon          = "qulon";    // Общие 
    static constexpr const char* kQulonLocal     = "local";    //  
    static constexpr const char* kQulonMode      = "mode";     //  
    static constexpr const char* kQulonPostpone  = "postp";    // 

    static constexpr const char* kOffsetAdc     = "offsetAdc";  // Смещение ЦАП

    static constexpr const char* kFactorV       = "factorV";  // Множитель преобразования
    static constexpr const char* kSmoothV       = "smoothV";  // Коэффициент фильтрации
    static constexpr const char* kOffsetV       = "offsetV";  // Смещение в милливольтах

    static constexpr const char* kFactorA       = "factorA";  // Множитель преобразования
    static constexpr const char* kSmoothA       = "smoothA";  // Коэффициент фильтрации
    static constexpr const char* kOffsetA       = "offsetA";  // Смещение в миллиамперах

    static constexpr const char* kQulonAkbU      = "akbU";     //  
    static constexpr const char* kQulonAkbAh     = "akbAh";    //  
    static constexpr const char* kQulonAkbInd    = "akbInd";   //  
    static constexpr const char* kQulonPowInd    = "powInd";   //  

  static constexpr const char* nTemplate       = "template"; // Шаблон 

  static constexpr const char* nDcPower        = "s-power";  // Простой источник 
    static constexpr const char* kDcVmax         = "voltMax";  //
    static constexpr const char* kDcImax         = "currMax";  //

  static constexpr const char* nCcCv           = "cccv";     // Простой заряд
    static constexpr const char* kCcCvVmax       = "voltMax";
    static constexpr const char* kCcCvVmin       = "voltMin";
    static constexpr const char* kCcCvImax       = "currMax";
    static constexpr const char* kCcCvImin       = "currMin";

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
