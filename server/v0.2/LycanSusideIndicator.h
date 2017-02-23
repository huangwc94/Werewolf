#ifndef LycanSusideIndicator_h
#define LycanSusideIndicator_h
#include "GameLogic.h"

class LycanSusideIndicator{
  public:
    LycanSusideIndicator(GameLogic *gl);
    ~LycanSusideIndicator();
    bool detect(uint8_t id,uint8_t btn);

  private:
    GameLogic *gl;
    void suside(Pid lycan);
    Pid lycan_id[LYCAN_NUMBER];
    uint8_t lycan_last_action[LYCAN_NUMBER];
    unsigned long lycan_last_time[LYCAN_NUMBER];
};
#endif
