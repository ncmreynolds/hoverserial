/*
 *	An Arduino library for hoverserial support
 *
 *	https://github.com/ncmreynolds/hoverserial
 *
 *	Released under LGPL-2.1 see https://github.com/ncmreynolds/hoverserial/LICENSE for full license
 *
 */
#ifndef hoverserial_cpp
#define hoverserial_cpp
#include "hoverserial.h"

hoverserial::hoverserial()	//Constructor function
{
}

hoverserial::~hoverserial()	//Destructor function
{
}

void hoverserial::begin(Stream &connectionStream){
	hoverboard_stream_ = &connectionStream;	//Set the stream used for the connection
	if(debug_uart_ != nullptr){
		debug_uart_->println(F("hoverserial started"));
	}
}

void hoverserial::debug(Stream &terminalStream)
{
	debug_uart_ = &terminalStream;		//Set the stream used for the terminal
}

void hoverserial::housekeeping()
{
	if(hoverboard_stream_ != nullptr){
		if(hoverboard_online_ == true && millis() - last_command_sent_ > command_send_interval_){
			last_command_sent_ = millis();
			send_command(0xABCD, requested_speed_, requested_steer_);
			if(hoverboard_online_) {
				if(debug_uart_ != nullptr){
					if(tank_steer_ == true) {
						debug_uart_->print(F("Right:"));
						debug_uart_->print(response_.speedR_meas);
						debug_uart_->print('/');
						debug_uart_->print(requested_speed_);
						debug_uart_->print(F(" Left:"));
						debug_uart_->print(response_.speedL_meas);
						debug_uart_->print('/');
						debug_uart_->print(requested_steer_);
					} else {
						debug_uart_->print(F("Requested speed:"));
						debug_uart_->print(requested_speed_);
						debug_uart_->print(F(" Requested steer:"));
						debug_uart_->print(requested_steer_);
						debug_uart_->print(F(" Right:"));
						debug_uart_->print(response_.speedR_meas);
						debug_uart_->print(F(" Left:"));
						debug_uart_->print(response_.speedL_meas);
					}
					debug_uart_->print(F(" Voltage:"));
					debug_uart_->print(float(response_.batVoltage)/100.0);
					debug_uart_->print(F(" Temp:"));
					debug_uart_->println(float(response_.boardTemp)/10.0);
				}
			}
		} else {
			if(hoverboard_stream_->available()) {
				incomingByte = hoverboard_stream_->read();								// Read the incoming byte
				bufStartFrame = ((uint16_t)(incomingByte) << 8) | incomingBytePrev;		// Construct the start frame
			} else {
				return;
			}
			// Copy received data
			if(bufStartFrame == 0xABCD) { // Initialize if new data is detected
				p       = (byte *)&new_response_;
				*p++    = incomingBytePrev;
				*p++    = incomingByte;
				idx     = 2;  
			} else if(idx >= 2 && idx < sizeof(hoverserialResponse)) {  // Save the new received data
				*p++    = incomingByte; 
				idx++;
			} 
			// Check if we reached the end of the package
			if(idx == sizeof(hoverserialResponse)) {
				uint16_t checksum;
				checksum = (uint16_t)(new_response_.start ^ new_response_.cmd1 ^ new_response_.cmd2 ^ new_response_.speedR_meas ^ new_response_.speedL_meas
				^ new_response_.batVoltage ^ new_response_.boardTemp ^ new_response_.cmdLed);
				if(new_response_.start == 0xABCD && checksum == new_response_.checksum) {
					// Copy the new data
					memcpy(&response_, &new_response_, sizeof(hoverserialResponse));	//Copy to the stored response
					last_response_received_ = millis();
					if(hoverboard_online_ == false) {
						hoverboard_online_ = true;
						if(debug_uart_ != nullptr){
							debug_uart_->println(F("hoverboard connected"));
						}
					}
				}
				else
				{
					if(debug_uart_ != nullptr){
						debug_uart_->println("Non-valid data skipped");
					}
				}
				idx = 0;    // Reset the index (it prevents to enter in this if condition in the next cycle)
			}
			// Update previous states
			incomingBytePrev = incomingByte;
		}
		if(hoverboard_online_ == true && millis() - last_response_received_ > response_timeout_interval_) {
			hoverboard_online_ = false;
			if(debug_uart_ != nullptr){
				debug_uart_->println(F("hoverboard disconnected"));
			}
		}
	}
}

void hoverserial::send_command(uint16_t commandCode, int16_t value1, int16_t value2){
	/*
	if(debug_uart_ != nullptr){
		debug_uart_->print(F("hoverserial command:"));
		debug_uart_->print(commandCode, HEX);
		debug_uart_->print(' ');
		debug_uart_->print(value1);
		debug_uart_->print(' ');
		debug_uart_->println(value2);
	}
	*/
	command.start    = (uint16_t)commandCode;
	command.steer    = (int16_t)value1;
	command.speed    = (int16_t)value2;
	command.checksum = (uint16_t)(command.start ^ command.steer ^ command.speed);

	//Send the command
	hoverboard_stream_->write((uint8_t *) &command, sizeof(command)); 
}
bool hoverserial::isConnected() {	//Check hoverboard is connected
	return hoverboard_online_;
}
int16_t hoverserial::getSpeedRight() {
	return response_.speedR_meas;
}
int16_t hoverserial::getSpeedLeft() {
	return response_.speedL_meas;
}
int16_t hoverserial::getBatteryVoltage() {
	return response_.batVoltage;
}
int16_t hoverserial::getBoardTemperature() {
	return response_.boardTemp;
}
void hoverserial::setSpeed(int16_t value) {	//Set speed in speed & steer mode
	if(requested_speed_ != value) {
		requested_speed_ = value;
		tank_steer_ = false;
		if(millis() - last_command_sent_ > minimum_command_send_interval_) {
			last_command_sent_ = millis() - command_send_interval_;
		}
	}
}
void hoverserial::setSteer(int16_t value) {	//Set speed in speed & steer mode
	if(requested_steer_ != value) {
		requested_steer_ = value;
		tank_steer_ = false;
		if(millis() - last_command_sent_ > minimum_command_send_interval_) {
			last_command_sent_ = millis() - command_send_interval_;
		}
	}
}
void hoverserial::setSpeed(int16_t  value1, int16_t  value2) {	//Set separate speeds in tank steer mode
	if(requested_speed_ != value1 || requested_steer_ != value2) {
		requested_speed_ = value1;
		requested_steer_ = value2;
		tank_steer_ = true;
		if(millis() - last_command_sent_ > minimum_command_send_interval_) {
			last_command_sent_ = millis() - command_send_interval_;
		}
	}
}

#endif
