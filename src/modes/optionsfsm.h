#ifndef _OPTIONSFSM_H_
#define _OPTIONSFSM_H_

/*
 * Доступные пользователю регулировки измерителей,
 *
 *
 */

#include "state/mstate.h"

namespace OptionFsm
{
  // Константы, ограничивающие пользовательские регулировки
  struct MOptConsts
  {
    // задержка включения (Отложенный старт)
    static constexpr int ppone_l =  0;
    static constexpr int ppone_h = 24;

    // смещение показаний по напряжению
    static constexpr short offset_v_h = 0 + 100;
    static constexpr short offset_v_l = 0 - 100;

    // Коэффициент преобразования в милливольты
    static constexpr short factor_v_h = 0x2DA0 + 0x0100;
    static constexpr short factor_v_l = 0x2DA0 - 0x0100;    

    // Показатель сглаживания по напряжению
    static constexpr short smooth_v_h = 3 + 2;
    static constexpr short smooth_v_l = 3 - 2;    

    // смещение показаний по току
    static constexpr short offset_a_h = 0 + 100;
    static constexpr short offset_a_l = 0 - 100;


    //...
  };

  enum MODES
  {
    QULON = 0,         // 
    TEMPLATE,               // шаблон режима 
    DCSUPPLY,               // режим источника постоянного тока
    PULSEGEN,               // режим источника импульсного тока
    CCCVCHARGE,             // режим заряда "постоянный ток / постоянное напряжение"
    PULSECHARGE,            // режим импульсного заряда
    RECOVERY,               // режим восстановления
    STORAGE,                // режим хранения
    DEVICE,                 // режим заводских регулировок
    SERVICE                // режим Сервис АКБ
  };


  class MStart : public MState
  {       
    public:
      MStart(MTools * Tools);
      MState * fsm() override;
  };

  class MSetPostpone : public MState
  {       
    public:
      MSetPostpone(MTools * Tools);
      MState * fsm() override;
  };

  // ============ Пользовательские настройки измерителя напряжения ============
  class MSetVoltageOffset : public MState
  {
    public:
      MSetVoltageOffset(MTools * Tools);
      MState * fsm() override;
  };

  class MSetVoltageFactor : public MState
  {
    public:
      MSetVoltageFactor(MTools * Tools);
      MState * fsm() override;
  };

  class MSetVoltageSmooth : public MState
  {
    public:
      MSetVoltageSmooth(MTools * Tools);
      MState * fsm() override;
  };

  // ============ Пользовательские настройки измерителя тока ============
  class MSetCurrentOffset : public MState
  {
    public:
      MSetCurrentOffset(MTools * Tools);
      MState * fsm() override;
  };

  class MSetCurrentFactor : public MState
  {
    public:
      MSetCurrentFactor(MTools * Tools);
      MState * fsm() override;
  };

  class MSetCurrentSmooth : public MState
  {
    public:
      MSetCurrentSmooth(MTools * Tools);
      MState * fsm() override;
  };

  // ====================================================================




  // class MSetVoltageOffset : public MState
  // {
  //       public:
  //         MSetVoltageOffset(MTools * Tools);
  //         MState * fsm() override;
  // };

  class MNameSelection : public MState
  {
        public:
          MNameSelection(MTools * Tools);
          MState * fsm() override;
  };

  class MSetFactory : public MState
  {
      public:
          MSetFactory(MTools * Tools);
          MState * fsm() override;
  };

//***********
  class MSetDCSupplyFactory : public MState
  {
      public:
          MSetDCSupplyFactory(MTools * Tools);
          MState * fsm() override;
  };

  class MSetCcCvChargeFactory : public MState
  {
      public:
          MSetCcCvChargeFactory(MTools * Tools);
          MState * fsm() override;
  };

  class MSetExChargeFactory : public MState
  {
      public:
          MSetExChargeFactory(MTools * Tools);
          MState * fsm() override;
  };

  class MSetRecoveryFactory : public MState
  {
      public:
          MSetRecoveryFactory(MTools * Tools);
          MState * fsm() override;
  };

  class MServiceFactory : public MState
  {
      public:
          MServiceFactory(MTools * Tools);
          MState * fsm() override;
  };

  class MSetQulonFactory : public MState
  {
      public:
          MSetQulonFactory(MTools * Tools);
          MState * fsm() override;
  };
//***************

  class MStop : public MState
  {
    public:  
      MStop(MTools * Tools);   
      MState * fsm() override;
  };

  class MExit : public MState
  {
    public:  
      MExit(MTools * Tools);   
      MState * fsm() override;
  };
};

#endif // !_OPTIONSFSM_H_