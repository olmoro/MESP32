#ifndef _DCSUPPLYFSM_H_
#define _DCSUPPLYFSM_H_

#include "state/mstate.h"

namespace DcSupplyFsm
{
    //Any constants of DC supply
    struct MDcConst
    {
        // Пределы регулировок напряжения и тока
        static constexpr float v_l =  2.0f;
        static constexpr float v_h = 16.0f;
        static constexpr float c_l =  0.2f;
        static constexpr float c_h = 12.2f;


        // Параметры ПИД-регулятора
        static constexpr float outputMin          = 0.0f;
        static constexpr float outputMaxFactor    = 1.05f;     // factor for current limit
        static constexpr float integralVoltFactor = 9.95f;
        static constexpr float bangMin            = 20.0f;     // За пределами, - отключить
        static constexpr float bangMax            = 20.0f;
        static constexpr unsigned long timeStep   = 100;
        // Подобранные значения для ПИД, фаза подъёма тока и далее, если в последующих фазах эти настройки будут устраивать
        static constexpr float k_p                = 0.13f;
        static constexpr float k_i                = 0.10f;
        static constexpr float k_d                = 0.04f;
    };

    class MStart : public MState
    {       
        public:
            MStart(MTools * Tools);
            MState * fsm() override;
    };

    class MSetVoltage : public MState
    {
        public:   
            MSetVoltage(MTools * Tools);
            MState * fsm() override;
    };
    

      
    class MSetCurrentMax : public MState
    {
        public:   
            MSetCurrentMax(MTools * Tools); 
            MState * fsm() override;
    };
    
    
    class MExecution : public MState
    {
        public:   
            MExecution(MTools * Tools);
            MState * fsm() override;
    };
   
    class MExit : public MState
    {
        public:  
            MExit(MTools * Tools);
            MState * fsm() override;
    };

};

#endif // !_DCSUPPLYFSM_H_