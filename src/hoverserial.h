/*
 *	An Arduino library for UART control of a re-flashed hoverboard
 *
 *	https://github.com/ncmreynolds/hoverserial
 *
 *	Released under LGPL-2.1 see https://github.com/ncmreynolds/hoverserial/LICENSE for full license
 *
 */
#ifndef hoverserial_h
#define hoverserial_h
#include <Arduino.h>

class hoverserial{

	public:
		hoverserial();														//Constructor function
		~hoverserial();														//Destructor function
		void begin(Stream &);												//Start the hoverserial, passing a Stream for the hoverboard connection
		void debug(Stream &);												//Start debugging on a stream
		void housekeeping();												//Manage sending commands to the hoverboard
		bool isConnected();													//Check hoverboard is connected
		void setSpeed(int16_t);												//Set speed in speed & steer mode
		void setSteer(int16_t);												//Set speed in speed & steer mode
		void setSpeed(int16_t, int16_t);									//Set separate speeds in tank steer mode
		int16_t getSpeedRight();											//Get the speed of the right hub motor
		int16_t getSpeedLeft();												//Get the speed of the left hub motor
		int16_t getBatteryVoltage();										//Get the battery voltage x100
		int16_t getBoardTemperature();										//Get the board temperature x10 in C
	protected:
	private:
		Stream *debug_uart_ = nullptr;										//The stream used for the debugging
		Stream *hoverboard_stream_ = nullptr;								//The stream used for the hoverboard connection
		uint32_t last_command_sent_ = 0;									//Track time of last command sent to hoverboard
		uint16_t command_send_interval_ = 250;								//Interval between commands when no change is needed
		uint16_t minimum_command_send_interval_ = 50;						//Minimal interval between commands
		uint32_t last_response_received_ = 0;								//Track time of last response from hoverboard
		uint16_t response_timeout_interval_ = 500;							//Detect if the hoverboard is online
		bool hoverboard_online_ = false;
		bool tank_steer_ = false;
		
		int16_t requested_speed_ = 0;										//Current requested speed
		int16_t requested_steer_ = 0;										//Current requested steer

		typedef struct{
			uint16_t start;
			int16_t  steer;
			int16_t  speed;
			uint16_t checksum;
		} hoverserialCommand;
		hoverserialCommand command;
		
		typedef struct{
			uint16_t start;
			int16_t  cmd1;
			int16_t  cmd2;
			int16_t  speedR_meas;
			int16_t  speedL_meas;
			int16_t  batVoltage;
			int16_t  boardTemp;
			uint16_t cmdLed;
			uint16_t checksum;
		} hoverserialResponse;
		hoverserialResponse response_;
		hoverserialResponse new_response_;

		uint8_t idx = 0;                        // Index for new data pointer
		uint16_t bufStartFrame;                 // Buffer Start Frame
		byte *p;                                // Pointer declaration for the new received data
		byte incomingByte;
		byte incomingBytePrev;
		
		void send_command(uint16_t, int16_t, int16_t);						//Send commands to the hoverboard
};
#endif
