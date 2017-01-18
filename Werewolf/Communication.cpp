#include "Communication.h"

ComputerDriver::ComputerDriver(){
	Serial.begin(115200);
	delay(50);
	
}

void ComputerDriver::outputString(String data){
	Serial.println(data);
}

void ComputerDriver::outputLight(const uint16_t g,const uint16_t r){
	Serial.println(String("RLIGHT:")+r);
	Serial.println(String("GLIGHT:")+g);
}

bool ComputerDriver::input(uint8_t &id, uint8_t & btn){
	id = 1;
	btn = 2;
	return false;
}

void ComputerDriver::clearBuffer(){
	Serial.flush();
	this->buffer = String("");
}