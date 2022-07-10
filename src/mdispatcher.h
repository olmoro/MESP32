#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_

class MTools;
class MBoard;
class MDisplay;
class MState;
  class MSupervisor;  //+m

class MDispatcher
{
  public:
    enum MODES
    {
      BOOT = 0,            // режим синхронизации
      OPTIONS,                // режим ввода настроек
      TEMPLATE,               // шаблон режима 
      DCSUPPLY,               // режим источника постоянного тока
      CCCVCHARGE,             // режим заряда "постоянный ток / постоянное напряжение"
      DEVICE,                 // режим заводских регулировок
    };

    //short modeSelection = OPTIONS;     // перенесен в MTools как short modeSelection ---

  public:
    MDispatcher(MTools * tools);

    void run();
    void delegateWork();
    void textMode(short modeSelection);

  private:
    MTools    * Tools;
    MBoard    * Board;
    MDisplay  * Display;
    MState    * State = nullptr;
      MSupervisor * Supervisor;

    bool latrus;
    //short modeSelection = SYNCING;    // Начать с синхронизации параметров
    short modeSelection = CCCVCHARGE;    // 
};

#endif //_DISPATCHER_H_
