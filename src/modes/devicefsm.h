#ifndef _DEVICEFSM_H_
#define _DEVICEFSM_H_

#include "state/mstate.h"

namespace DeviceFsm
{
    struct MDcConsts
    {
        // Параметры условий регулировки DC источника заряда и разрядной цепи
        static constexpr int vPwm_l     =    0;     // Предел регулирования снизу
        static constexpr int vPwm_h     = 1024;     // Предел регулирования сверху
        
        static constexpr int iPwm_l     =    0;     // 
        static constexpr int iPwm_h     = 1024;     //

        static constexpr int rPwm_l     =    0;     // r - реверсный ток (разряд) 
        static constexpr int rPwm_h     = 1024;     // 
    };

    class MStart : public MState
    {       
        public:
            MStart(MTools * Tools);
            MState * fsm() override;
    };

    class MSetVoltagePwmDelta : public MState
    {
        public:
            MSetVoltagePwmDelta(MTools * Tools);
            MState * fsm() override;
    };

    class MSetVoltagePwmMin : public MState
    {
        public:
            MSetVoltagePwmMin(MTools * Tools);
            MState * fsm() override;
    };

    class MSetVoltagePwmMax : public MState
    {
        public:
            MSetVoltagePwmMax(MTools * Tools);
            MState * fsm() override;
    };

    class MVoltagePwmExe : public MState
    {
        public:
            MVoltagePwmExe(MTools * Tools);
            MState * fsm() override;
    };

    class MSetCurrentPwmDelta : public MState
    {
        public:
            MSetCurrentPwmDelta(MTools * Tools);
            MState * fsm() override;
    };

    class MSetCurrentPwmMin : public MState
    {
        public:
            MSetCurrentPwmMin(MTools * Tools);
            MState * fsm() override;
    };

    class MSetCurrentPwmMax : public MState
    {
        public:
            MSetCurrentPwmMax(MTools * Tools);
            MState * fsm() override;
    };

    class MCurrentPwmExe : public MState
    {
        public:
            MCurrentPwmExe(MTools * Tools);
            MState * fsm() override;
    };

    class MSetDischargePwmDelta : public MState
    {
        public:
            MSetDischargePwmDelta(MTools * Tools);
            MState * fsm() override;
    };

    class MSetDischargePwmMin : public MState
    {
        public:
            MSetDischargePwmMin(MTools * Tools);
            MState * fsm() override;
    };

    class MSetDischargePwmMax : public MState
    {
        public:
            MSetDischargePwmMax(MTools * Tools);
            MState * fsm() override;
    };

    class MDischargePwmExe : public MState
    {
        public:
            MDischargePwmExe(MTools * Tools);
            MState * fsm() override;
    };
 
    class MStop : public MState
    {
        public:  
            MStop(MTools * Tools);
            MState * fsm() override;
    };
};

#endif  // !_DEVICEFSM_H_