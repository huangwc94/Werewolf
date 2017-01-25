#ifndef Communication_h
	#define Communication_h
	#include <Wire.h>
	#include <Arduino.h>

	#define STOP_CHAR '$'


	class Driver{
		public:

			virtual ~Driver(){};

			virtual void outputString(String data) = 0;

			virtual void outputLight(const uint16_t g,const uint16_t r) = 0;

			virtual bool input(uint8_t &id, uint8_t &btn) = 0;

			virtual void clearBuffer() = 0;
		protected:
			String buffer;
	};

	class ComputerDriver: public Driver{
		public:
			ComputerDriver();
			void outputString(String);
			void outputLight(const uint16_t g,const uint16_t r);
			bool input(uint8_t &id, uint8_t &btn);
			void clearBuffer();
	};

	// class HardwareDriver:Driver{

	// };


#endif




