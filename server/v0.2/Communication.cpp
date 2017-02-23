#include "Communication.h"

ComputerDriver::ComputerDriver(){
	Serial.begin(115200);
	buffer = String("");
	delay(50);
	mp3_init();
	current_time = 10000;
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

void ComputerDriver::setScreen(uint16_t num){
	if(current_time == num){
		return;
	}
	current_time = num;
	if(num>9999){
		Serial.println("SCREEN:--");
	}else{
		if(num < 10)
			Serial.println(String("SCREEN:0")+num);
		else
			Serial.println(String("SCREEN:")+(num % 100));
	}
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
	this->display = new TM1637Display(7,6);
	this->display->setBrightness(0x0a);
	xbtn = 0;
	xid = 0;
	last_same = 0;
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

			if(id == xid && btn == xbtn){
				if(millis() - last_same > 300){
					last_same = millis();
					xid = id;
					xbtn = btn;
					return true;
				}else{
					return false;
				}
			}else{
				last_same = millis();
				xid = id;
				xbtn = btn;
				return true;
			}


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
const uint8_t SEG_NONE[] = {
	SEG_G,
	SEG_G,
	SEG_G,
	SEG_G
};
void HardwareDriver::setScreen(uint16_t num){


	if(num > 999){
		this->display->setSegments((uint8_t *)SEG_NONE);
	}else{
		uint8_t data[] = { 0xff, 0xff, 0xff, SEG_A | SEG_C | SEG_D | SEG_F | SEG_G};
		data[0] = this->display->encodeDigit(num / 100);
		data[1] = this->display->encodeDigit((num - ((num / 100) * 100))/ 10);
		data[2] = this->display->encodeDigit(num % 10);
		display->setSegments(data);
	}
}
