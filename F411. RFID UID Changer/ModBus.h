//====================================================================================================
#ifndef MODBUS_H
#define MODBUS_H
//====================================================================================================
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
//====================================================================================================
#define MODBUS_MAX_TIMEOUT												10

#define MODBUS_MAX_COILS_SIZE											2000
#define MODBUS_MAX_REGISTER_SIZE									100
#define MODBUS_MAX_FRAME_SIZE											256

// Modbus standart function codes
#define MODBUS_READ_COILS													0x01
#define MODBUS_READ_DISCRETE_INPUTS								0x02
#define MODBUS_READ_HOLDING_REGISTERS							0x03
#define MODBUS_READ_INPUT_REGISTERS								0x04

#define MODBUS_WRITE_SINGLE_COIL									0x05
#define MODBUS_WRITE_SINGLE_REGISTER							0x06

#define MODBUS_WRITE_MULTIPLE_COILS								0x0F
#define MODBUS_WRITE_MULTIPLE_REGISTERS						0x10

#define MODBUS_READ_WRITE_MULTIPLE_REGISTERS			0x17

#define MODBUS_READ_DEVICE_IDENTIFICATION_0				0x2B
#define MODBUS_READ_DEVICE_IDENTIFICATION_1				0x0E

// Modbus user function codes ([0x41; 0x48] and [0x64; 0x6E])
#define MODBUS_USER_FC_GET_READERS_DATA						0x41

// Modbus exceptions
#define MODBUS_EXCEPTION_ILLEGAL_FUNCTION					0x01
#define MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS			0x02
#define MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE				0x03
#define MODBUS_EXCEPTION_SERVER_DEVICE_VALUE			0x04
//====================================================================================================
typedef uint32_t(*GetSystemTimeFunction)(void);

typedef void(*FunctionCodesHandler)(void);
typedef void(*StartPeripheralDataTransmission)(void);

typedef enum{
	MODBUS_MODE_SLAVE,
	MODBUS_MODE_MASTER
}Modbus_Mode;

typedef struct{
	uint8_t 		Buffer[MODBUS_MAX_FRAME_SIZE];
	uint8_t 		In_Progress;
	uint8_t			Frame_Ready;
	
	uint16_t 		Buffer_Index;
	uint16_t 		Buffer_Length;
}Modbus_Buffer_Struct;

typedef struct{	
	Modbus_Mode												Mode;
	
	uint8_t														SlaveAddress;
	uint8_t 													FunctionCode;
	
	Modbus_Buffer_Struct 							Rx;
	Modbus_Buffer_Struct 							Tx;
	
	StartPeripheralDataTransmission		StartDataTransmission;
	FunctionCodesHandler 							FunctionCodes_Handler;
		
	GetSystemTimeFunction							GetSystemTime_ms;
	uint32_t 													Request_Initialized_Time;
	
	bool		 													Request_Initialized;	
	
	bool		 													Response_Available;
	bool		 													Response_Waiting;
	bool		 													Response_Waiting_Timeout;	
	
	uint8_t														CurrentAddress;
	uint8_t														AddressesArray[32];
	uint8_t														AddressesArrayPosition;
	uint8_t														AddressesAmount;
	
	bool*															Input_Discrete;
	bool*															Coils;
	uint16_t*													Input_Registers;
	uint16_t*													Holding_Registers;
	
}ModbusRTU;
//====================================================================================================
uint16_t ModbusRTU_CRC_Calculate(uint8_t* buffer, uint16_t length);

void ModbusRTU_Init(ModbusRTU*);

void ModbusRTU_Data_Handler(ModbusRTU*);
void ModbusRTU_Rx_Byte_Handler(ModbusRTU* Modbus, uint8_t);
uint8_t ModbusRTU_Tx_Byte_Handler(ModbusRTU* Modbus);

void ModbusRTU_Request_Initialize(ModbusRTU*);
void ModbusRTU_Request_Handler(ModbusRTU*);

void ModbusRTU_Response_Initialize(ModbusRTU*);
void ModbusRTU_Response_Handler(ModbusRTU*);

bool ModbusRTU_CheckDeviceAvailability(ModbusRTU*, uint8_t);
void ModbusRTU_AutoSearchDevices(ModbusRTU*);
//====================================================================================================
#endif
//====================================================================================================