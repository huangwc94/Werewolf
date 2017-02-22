#include "Communication.h"

ComputerDriver::ComputerDriver(){
	Serial.begin(115200);
	buffer = String("");
	delay(50);
	mp3_init();
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

void ComputerDriver::playSound(uint16_t id){
	Serial.println(String("PLAY:")+ id);
	play_sound(id);
}

void ComputerDriver::setScreen(uint8_t num){
	if(num>99){
		Serial.println("SCREEN:--");
	}else{
		Serial.println("SCREEN:"+num);
	}
}

void ComputerDriver::outputCentralLight(bool g, bool r){
	Serial.println(String("CLIGHT:")+g+","+r);
}


HardwareDriver::HardwareDriver(uint8_t playerNumber){
	Serial.begin(115200);
	Wire.begin();
	delay(50);
	this->g = 0;
	this->r = 0;
	this->playerNumber = playerNumber;
	this->currentSlaveId = 1;
	mp3_init();
}

void HardwareDriver::outputString(String data){
	Serial.println(data);
}

void HardwareDriver::outputLight(const uint16_t g,const uint16_t r){
	uint16_t mod_g = this->g ^ g;
	uint16_t mod_r = this->r ^ r;
	uint8_t lightStatus = 0;

	for(uint8_t i = 0;i < this->playerNumber;i++){
		lightStatus = 0;
		if(((mod_g & (1 << i)) > 0) || ((mod_r & (1 << i)) > 0)){
			lightStatus  = (g & (1 << i))> 0 ? 1 : 0;
			lightStatus |= (r & (1 << i))> 0 ? 2 : 0;

			Wire.beginTransmission(i+1);
			Wire.write(lightStatus);
			Wire.endTransmission();
		}
	}
	this->g = g;
	this->r = r;

	#ifdef DEBUG
	Serial.println(String("RLIGHT:")+r);
	Serial.println(String("GLIGHT:")+g);
	#endif
}

bool HardwareDriver::input(uint8_t &id, uint8_t &btn){
	Wire.requestFrom((int)this->currentSlaveId, 1);
	if(Wire.available()){
		btn = Wire.read();
		if(btn > 0){
			id = this->currentSlaveId;
			delay(500);
			return true;
		}else{
			this->currentSlaveId ++;
			if(this->currentSlaveId > this->playerNumber){
				this->currentSlaveId = 1;
			}
		}
	}
	return false;
}

void HardwareDriver::clearBuffer(){

}

void HardwareDriver::playSound(uint16_t id){
	#ifdef DEBUG
	Serial.println(String("PLAY:")+ id);
	#endif
	play_sound(id);
}

void HardwareDriver::setScreen(uint8_t num){
	
}
void HardwareDriver::outputCentralLight(bool g, bool r){
	
}
