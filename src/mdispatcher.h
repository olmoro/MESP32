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
      OPTIONS = 0,            // режим ввода настроек (не отключаемый)
      TEMPLATE,               // шаблон режима 
      DCSUPPLY,               // режим источника постоянного тока
      CCCVCHARGE,             // режим заряда "постоянный ток / постоянное напряжение"
      DEVICE,                 // режим заводских регулировок
    };

  public:
    MDispatcher(MTools * tools);

    void run();
    void delegateWork();
    void textMode(int mode);

  private:
    MTools    * Tools;
    MBoard    * Board;
    MDisplay  * Display;
    MState    * State = nullptr;
      MSupervisor * Supervisor;

    bool latrus;
    int mode;
};

#endif //_DISPATCHER_H_
