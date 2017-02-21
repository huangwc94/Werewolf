#include "mp3.h"

SoftwareSerial *mp3Serial;


void sendCommand(int8_t command, int16_t dat){
  delay(20);
  int8_t Send_buf[8] = {0};
	Send_buf[0] = 0x7e; //starting byte
	Send_buf[1] = 0xff; //version
	Send_buf[2] = 0x06; //the number of bytes of the command without starting byte and ending byte
	Send_buf[3] = command; //
	Send_buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
	Send_buf[5] = (int8_t)(dat >> 8);//datah
	Send_buf[6] = (int8_t)(dat); //datal
	Send_buf[7] = 0xef; //ending byte
	for(uint8_t i=0; i<8; i++)//
	{
		mp3Serial->write(Send_buf[i]);
	}
}
void mp3_init(){
  mp3Serial = new SoftwareSerial(9,10);
  mp3Serial->begin(9600);
  delay(500);//Wait chip initialization is complete
  sendCommand(CMD_SEL_DEV, DEV_TF);//select the TF card
  delay(200);//wait for 200ms
}

void play_sound(int id){
  sendCommand(STOP_PLAY,0);
  sendCommand(CMD_PLAY_W_INDEX, id);
}
