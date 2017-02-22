#ifndef Communication_h
	#define Communication_h
	#include <Wire.h>
	#include <Arduino.h>

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

			virtual void setScreen(uint8_t num) = 0;

			virtual void outputCentralLight(bool g,bool r) = 0;
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
			void setScreen(uint8_t num);
			void outputCentralLight(bool g, bool r);
		private:
			uint8_t current_time;
	};
	class HardwareDriver:public Driver{
		public:
			HardwareDriver(uint8_t playerNumber);
			void outputString(String);
			void outputLight(const uint16_t g,const uint16_t r);
			bool input(uint8_t &id, uint8_t &btn);
			void clearBuffer();
			void playSound(uint16_t id);
			void setScreen(uint8_t num);
			void outputCentralLight(bool g, bool r);
		private:
			uint16_t g,r,playerNumber;
			uint8_t currentSlaveId;
	};

#endif
