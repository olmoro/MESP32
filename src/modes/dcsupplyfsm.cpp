/*
 * dcsupplyfsm.cpp
 * Конечный автомат источника постоянного тока
 * В прототипе - Простой источник питания
 * 2019.05.10 2019.12.23
 */

#include "modes/dcsupplyfsm.h"
#include "mtools.h"
#include "board/mboard.h"
#include "board/mkeyboard.h"
#include "display/mdisplay.h"
#include <Arduino.h>

namespace DcSupplyFsm
{
    // Состояние "Старт", инициализация выбранного режима работы (простой источник).
    MStart::MStart(MTools * Tools) : MState(Tools) 
    {
        Tools->setVoltageMax( Tools->readNvsFloat("s-power", "voltMax", Tools->pows[4][0]) );   // Начальный выбор 12.0 вольт
        Tools->setCurrentMax( Tools->readNvsFloat("s-power", "currMax", Tools->pows[4][1]) );   //                  3.0 А
    //    Tools->powO   = Tools->readNvsFloat("s-power", "powO", 0.0f);                // Планируется для быстрого входа в режим, nu
        
        #ifdef DEBUG_SUPPLY
            Serial.print("Voltage     : "); Serial.println(Tools->getVoltageMax(), 2);         // формат XX.XX
            Serial.print("Max Current : "); Serial.println(Tools->getCurrentMax(), 2);
        #endif

        // Индикация
//        Display->getTextMode( (char*) " DC SUPPLY SELECTED  " );
        Display->showHelp( (char*) "  P-DEFINE  C-START  " );
        Display->barOff();
    }
    MState * MStart::fsm()
    {
        switch ( Keyboard->getKey() )    //Здесь так можно
        {
            case MKeyboard::C_CLICK :
                // Старт без уточнения параметров (быстрый старт), 
                // максимальный ток и напряжение - в соответствии с выбором (см таблицу в/А)
                // Восстановленные на предыдущем шаге данные заменяются (кроме индекса)
                Tools->setVoltageMax( Tools->pows[Tools->powInd][0] );
                Tools->setCurrentMax( Tools->pows[Tools->powInd][1] );
                return new MExecution(Tools);
            case MKeyboard::P_CLICK :
                return new MSetVoltage(Tools);
            default:;
        }
        Display->showVolt( Tools->getRealVoltage(), 2 );
        Display->showAmp( Tools->getRealCurrent(), 1 );

        return this;
    };

    MSetVoltage::MSetVoltage(MTools * Tools) : MState(Tools)
    {
        // Индикация помощи
//        Display->getTextMode( (char*) "  U/D-SET VOLTAGE  " );
        Display->showHelp( (char*) "  B-SAVE  C-START  " );
    }     
    MState * MSetVoltage::fsm()
    {
        switch ( Keyboard->getKey() )
        {
            case MKeyboard::C_CLICK :
                return new MExecution(Tools);
            case MKeyboard::B_CLICK :
                Tools->saveFloat( "s-power", "voltMax", Tools->getVoltageMax() ); 
                return new MExecution(Tools);
            case MKeyboard::UP_CLICK :
                Tools->voltageMax = Tools->upfVal( Tools->voltageMax, MDcConst::v_l, MDcConst::v_h, 0.1f );
                break;
            case MKeyboard::DN_CLICK:
                Tools->voltageMax = Tools->dnfVal( Tools->voltageMax, MDcConst::v_l, MDcConst::v_h, 0.1f );
                break;
            case MKeyboard::UP_LONG_CLICK:
                Tools->voltageMax = Tools->upfVal( Tools->voltageMax, MDcConst::v_l, MDcConst::v_h, 1.0f );
                break;
            case MKeyboard::DN_LONG_CLICK:
                //Tools->decVoltageMax( 1.0f, false );
                Tools->voltageMax = Tools->dnfVal( Tools->voltageMax, MDcConst::v_l, MDcConst::v_h, 1.0f );
                break;
            default:;
        }
        // Индикация ввода
        Display->showVolt( Tools->voltageMax, 1 );
        Display->showAmp( Tools->getRealCurrent(), 1 );
        return this;
    };

    // Выбор максимального тока
    MSetCurrentMax::MSetCurrentMax(MTools * Tools) : MState(Tools)
    {
        // Индикация помощи
//        Display->getTextMode( (char*) "  U/D-SET CURRENT  " );
        Display->showHelp( (char*) "  B-SAVE  C-START  " );
        } 
    MState * MSetCurrentMax::fsm()
    {
    switch ( Keyboard->getKey() )
        {
            case MKeyboard::C_CLICK :
                return new MExecution(Tools);
            case MKeyboard::B_CLICK :
                Tools->saveFloat( "s-power", "currMax", Tools->getCurrentMax() ); 
                return new MSetVoltage(Tools);
            case MKeyboard::UP_CLICK :
                //Tools->incCurrentMax( 0.1f, false );
                Tools->currentMax = Tools->upfVal( Tools->currentMax, MDcConst::c_l, MDcConst::c_h, 0.1f );

                break;
            case MKeyboard::DN_CLICK:
                //Tools->decCurrentMax( 0.1f, false );
                Tools->currentMax = Tools->dnfVal( Tools->currentMax, MDcConst::c_l, MDcConst::c_h, 0.1f );

                break;
            case MKeyboard::UP_LONG_CLICK:
                //Tools->incCurrentMax( 0.5f, false );
                Tools->currentMax = Tools->upfVal( Tools->currentMax, MDcConst::c_l, MDcConst::c_h, 0.5f );

                break;
            case MKeyboard::DN_LONG_CLICK:
                //Tools->decCurrentMax( 0.5f, false );
                Tools->currentMax = Tools->dnfVal( Tools->currentMax, MDcConst::c_l, MDcConst::c_h, 0.5f );

                break;
            default:;
        }
        Display->showVolt( Tools->getRealVoltage(), 2 );
        Display->showAmp( Tools->currentMax, 1 );
        return this;
    };


    MExecution::MExecution(MTools * Tools) : MState(Tools)
    {
        // Индикация помощи
//        Display->getTextMode( (char*) "  DC SUPPLY IS ON  " );
        Display->showHelp( (char*) "       C-STOP      " );

        Tools->clrAhCharge();                       // Обнуляются счетчик отданного заряда
        Tools->clrTimeCounter();                    // и времени

        // Задаются начальные напряжение и ток
        //Board->setVoltageVolt( Tools->getVoltageMax() );            // Voltage limit
        //Board->setCurrentAmp( Tools->getCurrentMax() * MDcConst::outputMaxFactor );     // 1.05f );     // 
        //Board->powOn();   
        Board->swOn();                            // Включение преобразователя и коммутатора.

//        Tools->setSetPoint( Tools->getVoltageMax() );

        // Настройка ПИД-регулятора
//        Tools->initPid( MDcConst::outputMin,
//                        MDcConst::outputMaxFactor * Tools->getCurrentMax(),
//                        MDcConst::k_p,
//                        MDcConst::k_i,
//                        MDcConst::k_d,
//                        MDcConst::bangMin,
//                        MDcConst::bangMax,
//                        MDcConst::timeStep );
//        Tools->setIntegral( MDcConst::integralVoltFactor * Tools->getVoltageMax() );  // Напряжение при пуске будет
                                                                // выставлено с погрешностью задающей напряжение цепи
    }     
    MState * MExecution::fsm()
    {
        Tools->chargeCalculations();
        // //Регулировка напряжения "на лету"
        // if(Keyboard->getKey(MKeyboard::UP_CLICK)) { Tools->incVoltagePow( 0.1f, false ); Tools->liveU(); }  // Добавить 100 мB
        // if(Keyboard->getKey(MKeyboard::DN_CLICK)) { Tools->decVoltagePow( 0.1f, false ); Tools->liveU(); }  // Убавить  100 мB
        // if(Keyboard->getKey(MKeyboard::UP_AUTO_CLICK)) { Tools->incVoltagePow( 0.1f, false ); Tools->liveU(); }  // Добавить по 100 мB
        // if(Keyboard->getKey(MKeyboard::DN_AUTO_CLICK)) { Tools->decVoltagePow( 0.1f, false ); Tools->liveU(); }  // Убавить по 100 мB

        // if(Keyboard->getKey(MKeyboard::B_CLICK))
        // { 
        //     Tools->saveFloat( "s-power", "voltMax", Tools->getVoltageMax() ); 
        //     Oled->showLine3RealCurrent();
        // }   

        // if(Keyboard->getKey(MKeyboard::C_CLICK)) { return new MExit(Tools); } 

        switch ( Keyboard->getKey() )
        {
            case MKeyboard::C_CLICK :
                return new MExit(Tools);

            case MKeyboard::B_CLICK :
                Tools->saveFloat( "s-power", "voltMax", Tools->getVoltageMax() );
//                Oled->showLine3RealCurrent();
                break;

            case MKeyboard::UP_CLICK :
                //Tools->incVoltageMax( 0.1f, false );
//                Oled->showLine3MaxU( Tools->getVoltageMax() );
//                Tools->liveU();
                break;  //return new MExecution(Tools);

            case MKeyboard::DN_CLICK:
                //Tools->decVoltageMax( 0.1f, false );
//                Tools->liveU();   
//                Oled->showLine3MaxU( Tools->getVoltageMax() );
                break;  //                return new MExecution(Tools);

            case MKeyboard::UP_AUTO_CLICK:
                //Tools->incVoltageMax( 0.1f, false );
//                Tools->liveU();   
//                Oled->showLine3MaxU( Tools->getVoltageMax() );
                break;  //                return new MExecution(Tools);

            case MKeyboard::DN_AUTO_CLICK:
                //Tools->decVoltageMax( 0.1f, false );
//                Tools->liveU();   
//                Oled->showLine3MaxU( Tools->getVoltageMax() );
                break;  //                return new MExecution(Tools);
            default:;
        }

//        Tools->runPidVoltage();
        Display->showVolt( Tools->getRealVoltage(), 2 );
        Display->showAmp( Tools->getRealCurrent(), 1 );
        return this;
    };

    // Процесс выхода из режима - до нажатия кнопки "С" удерживается индикация
    // о продолжительности и отданном заряде.
    MExit::MExit(MTools * Tools) : MState(Tools)
    {
        //Tools->powShutdown();
        // Tools->shutdownCharge();
//        Display->getTextMode( (char*) "   DC DCSUPPLY OFF   " );
        Display->showHelp( (char*) "              C-EXIT " );
        Display->barOff();
    }      
    MState * MExit::fsm()
    {
        if(Keyboard->getKey(MKeyboard::C_CLICK)) 
        {
            //Tools->activateExit("  DC источник   ");

//            Display->getTextMode( (char*) "     DC DCSUPPLY:    " );
            Display->showHelp( (char*) " SIMPLE POWER SUPPLY " );

            return 0;   // Возврат к выбору режима
        }
        return this;
    };
};
