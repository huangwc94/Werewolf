#ifndef Communication_h
	#define Communication_h
	#include <Wire.h>
	#include <Arduino.h>
	#include <SoftwareSerial.h>
	#define STOP_CHAR '$'
	#include "mp3.h"

	#define DEBUG
	class Driver{
		public:

			virtual ~Driver(){};

			virtual void outputString(String data) = 0;

			virtual void outputLight(const uint16_t g,const uint16_t r) = 0;

			virtual bool input(uint8_t &id, uint8_t &btn) = 0;

			virtual void clearBuffer() = 0;

			virtual void playSound(uint16_t id) = 0;
		protected:
			String buffer;
	};

	class ComputerDriver: public Driver{
		public:
			ComputerDriver();
			void outputString(String);
			void outputLight(const uint16_t g,const uint16_t r);
			bool input(uint8_t &id, uint8_t &btn);
			void playSound(uint16_t id);
			void clearBuffer();
		private:
			int8_t Send_buf[8] = {0};
			SoftwareSerial *mySerial;
			void sendCommand(int8_t command, int16_t dat);
	};
	class HardwareDriver:public Driver{
		public:
			HardwareDriver(uint8_t playerNumber);
			void outputString(String);
			void outputLight(const uint16_t g,const uint16_t r);
			bool input(uint8_t &id, uint8_t &btn);
			void clearBuffer();
			void playSound(uint16_t id);
		private:
			uint16_t g,r,playerNumber;
			uint8_t currentSlaveId;
			int8_t Send_buf[8] = {0};
			SoftwareSerial *mySerial;
			void sendCommand(int8_t command, int16_t dat);
	};

#endif