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
    void goodMansuside(Pid gid);
    uint8_t lycan_last_action[PLAYER_NUMBER];
    unsigned long lycan_last_time[PLAYER_NUMBER];
};
#endif
