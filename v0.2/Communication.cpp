#include "Communication.h"

ComputerDriver::ComputerDriver(){
	Serial.begin(115200);
	buffer = String("");
	delay(50);
	
}

void ComputerDriver::outputString(String data){
	Serial.println(data);
}

void ComputerDriver::outputLight(const uint16_t g,const uint16_t r){
	Serial.println(String("RLIGHT:")+r);
	Serial.println(String("GLIGHT:")+g);
}

bool ComputerDriver::input(uint8_t &id, uint8_t &btn){
	if(Serial.available()) {
		char inChar = (char)Serial.read();
		if (inChar == STOP_CHAR) {
			id =       (uint8_t) this->buffer[0] - 'a' + 1;
			btn =      (uint8_t) this->buffer[2] - 'a' + 1;
			clearBuffer();
			return true;
		}else{
			buffer += inChar;
			return false;
		}
	}
	return false;
}

void ComputerDriver::clearBuffer(){
	while(Serial.available()) Serial.read();
	this->buffer = String("");
}