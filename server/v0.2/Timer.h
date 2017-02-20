#ifndef Timer_h
#define Timer_h
#include "Communication.h"
#include "Arduino.h"
class Timer{
  public:
    Timer(unsigned long timeout,Driver *conn);
    ~Timer();
    bool run();
  private:
    Driver *conn;
    unsigned long timeout;
    unsigned long start;
    unsigned long current;
    bool one,two,three,five,ten,thy;

};

#endif
