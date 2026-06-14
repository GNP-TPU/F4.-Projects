#include "ModBus.h"

bool 		 	INPUT_DISCRETE[MODBUS_MAX_REGISTER_SIZE];
bool 		 	COILS[MODBUS_MAX_REGISTER_SIZE];
uint16_t 	INPUT_REGISTERS[MODBUS_MAX_REGISTER_SIZE];
uint16_t 	HOLDING_REGISTERS[MODBUS_MAX_REGISTER_SIZE];
//====================================================================================================
uint16_t ModbusRTU_CRC_Calculate(uint8_t* buffer, uint16_t length){
	if(length < 2){
		return 0x0000;
	}
	
	uint16_t crc = 0xFFFF;  																											// Начальное значение CRC

  for(uint16_t buffer_pointer = 0; buffer_pointer < length; buffer_pointer++) {
		crc ^= (uint16_t)buffer[buffer_pointer];  																						// Применяем XOR к текущему байту
		for(uint8_t bit = 0; bit < 8; bit++) {  																		// Обрабатываем каждый бит
			if(crc & 0x0001) {  																											// Если младший бит равен 1
				crc >>= 1;  																														// Сдвигаем вправо
				crc ^= 0xA001;  																												// Применяем полином
      }
			else{
				crc >>= 1;  																														// Просто сдвигаем вправо
      }
		}
	}
	
	return crc;
}
//====================================================================================================
void ModbusRTU_Rx_Byte_Handler(ModbusRTU* Modbus, uint8_t byte){
	if(Modbus->Rx.Buffer_Index < MODBUS_MAX_FRAME_SIZE){
		Modbus->Rx.Buffer[Modbus->Rx.Buffer_Index++] = byte;
		Modbus->Rx.In_Progress = true;
	}
	else{
		Modbus->Rx.In_Progress = false;
	}
}

uint8_t ModbusRTU_Tx_Byte_Handler(ModbusRTU* Modbus){
	if(Modbus->Tx.Buffer_Index < Modbus->Tx.Buffer_Length){
		Modbus->Tx.In_Progress = true;
		return Modbus->Tx.Buffer[Modbus->Tx.Buffer_Index++];
	}
	else{
		Modbus->Tx.In_Progress = false;
		return 0;
	}
}
//====================================================================================================
void ModbusRTU_Init(ModbusRTU* Modbus){
	
	for(uint8_t i = 0; i < MODBUS_MAX_REGISTER_SIZE; i += 2){
		if(i % 2 == 0)
			COILS[i] = true;
		else
			COILS[i] = false;
		HOLDING_REGISTERS[i] = 0x0001 + i;
		HOLDING_REGISTERS[i + 1] = 0x0002 + i;
	}
	
	if(Modbus->Input_Discrete == NULL)
		Modbus->Input_Discrete = INPUT_DISCRETE;
	
	if(Modbus->Coils == NULL)
		Modbus->Coils = COILS;
	
	if(Modbus->Input_Registers == NULL)
		Modbus->Input_Registers = INPUT_REGISTERS;
	
	if(Modbus->Holding_Registers == NULL)
		Modbus->Holding_Registers = HOLDING_REGISTERS;
	
	
}

void ModbusRTU_Request_Initialize(ModbusRTU* Modbus){
	Modbus->Response_Available = false;
	
	Modbus->FunctionCode = Modbus->Tx.Buffer[1];
	
	Modbus->Tx.Buffer_Length = Modbus->Tx.Buffer_Index;
	
	uint16_t Transmit_CRC = ModbusRTU_CRC_Calculate(Modbus->Tx.Buffer, Modbus->Tx.Buffer_Length);
	
	Modbus->Tx.Buffer[Modbus->Tx.Buffer_Index++] = (Transmit_CRC) & 0xFF;
	Modbus->Tx.Buffer[Modbus->Tx.Buffer_Index++] = (Transmit_CRC >> 8) & 0xFF;
	
	Modbus->Tx.Buffer_Length = Modbus->Tx.Buffer_Index;
	Modbus->Tx.Buffer_Index = 0;
	Modbus->Tx.In_Progress = true;
	Modbus->Rx.In_Progress = false;
	
	if(Modbus->StartDataTransmission != NULL){
		Modbus->StartDataTransmission();
	}
}

void ModbusRTU_Request_Handler(ModbusRTU* Modbus){
	if(Modbus->Rx.In_Progress){
		Modbus->Rx.In_Progress = false;
			
		Modbus->Rx.Buffer_Length = Modbus->Rx.Buffer_Index;
		Modbus->Rx.Buffer_Index = 0;
	}
	
	if(Modbus->Rx.Buffer[0] != Modbus->SlaveAddress){
		return;
	}
	
	uint16_t Received_CRC = (Modbus->Rx.Buffer[Modbus->Rx.Buffer_Length - 2]) |
													(Modbus->Rx.Buffer[Modbus->Rx.Buffer_Length - 1] << 8); 
	uint16_t Expected_CRC = ModbusRTU_CRC_Calculate(Modbus->Rx.Buffer, Modbus->Rx.Buffer_Length - 2);
	
	if(Received_CRC != Expected_CRC){
		Modbus->Rx.Buffer_Index = 0;
		return;
	}
	
	Modbus->FunctionCode = Modbus->Rx.Buffer[1];
	
	Modbus->Tx.Buffer_Index = 0;
	
	Modbus->Tx.Buffer[Modbus->Tx.Buffer_Index++] = Modbus->SlaveAddress;
	Modbus->Tx.Buffer[Modbus->Tx.Buffer_Index++] = Modbus->FunctionCode;
	
	uint8_t Initial_Tx_Buffer_Index = Modbus->Tx.Buffer_Index;
	
	switch(Modbus->FunctionCode){
		case MODBUS_READ_HOLDING_REGISTERS:{
			uint16_t Starting_Address = (Modbus->Rx.Buffer[2] << 8) | Modbus->Rx.Buffer[3];
			uint16_t Quantity = (Modbus->Rx.Buffer[4] << 8) | Modbus->Rx.Buffer[5];
			uint8_t Byte_Count = Quantity * 2;
				
			Modbus->Tx.Buffer[Modbus->Tx.Buffer_Index++] = Byte_Count;
				
			for(uint8_t i = 0; i < Quantity; i++){
				Modbus->Tx.Buffer[Modbus->Tx.Buffer_Index++] = (Modbus->Holding_Registers[Starting_Address + i] >> 8) & 0xFF;
				Modbus->Tx.Buffer[Modbus->Tx.Buffer_Index++] = (Modbus->Holding_Registers[Starting_Address + i] >> 0) & 0xFF;
			}
			break;
		}
			
		default:{
			if(Modbus->FunctionCodes_Handler != NULL){
				Modbus->FunctionCodes_Handler();
			}
			
			if(Modbus->Tx.Buffer_Index == Initial_Tx_Buffer_Index){		
				Modbus->Tx.Buffer_Index = 0;
				Modbus->Tx.Buffer[Modbus->Tx.Buffer_Index++] = Modbus->SlaveAddress;
				Modbus->Tx.Buffer[Modbus->Tx.Buffer_Index++] = Modbus->FunctionCode | 0x80;
				Modbus->Tx.Buffer[Modbus->Tx.Buffer_Index++] = MODBUS_EXCEPTION_ILLEGAL_FUNCTION;
			}
			break;
		}
	}
	
	ModbusRTU_Response_Initialize(Modbus);
}

void ModbusRTU_Response_Initialize(ModbusRTU* Modbus){
	Modbus->Tx.Buffer_Length = Modbus->Tx.Buffer_Index;
	
	uint16_t Transmit_CRC = ModbusRTU_CRC_Calculate(Modbus->Tx.Buffer, Modbus->Tx.Buffer_Length);
	
	Modbus->Tx.Buffer[Modbus->Tx.Buffer_Index++] = (Transmit_CRC) & 0xFF;
	Modbus->Tx.Buffer[Modbus->Tx.Buffer_Index++] = (Transmit_CRC >> 8) & 0xFF;
	
	Modbus->Tx.Buffer_Length = Modbus->Tx.Buffer_Index;
	Modbus->Tx.Buffer_Index = 0;
	Modbus->Tx.In_Progress = true;
	Modbus->Rx.In_Progress = false;
	
	if(Modbus->StartDataTransmission != NULL){
		Modbus->StartDataTransmission();
	}
}

void ModbusRTU_Response_Handler(ModbusRTU* Modbus){
	if(Modbus->Rx.In_Progress){
		Modbus->Rx.In_Progress = false;
			
		Modbus->Rx.Buffer_Length = Modbus->Rx.Buffer_Index;
		Modbus->Rx.Buffer_Index = 0;
	}
	
	if(Modbus->Rx.Buffer[1] != Modbus->FunctionCode){
		return;
	}
	
	uint16_t Received_CRC = (Modbus->Rx.Buffer[Modbus->Rx.Buffer_Length - 2]) |
													(Modbus->Rx.Buffer[Modbus->Rx.Buffer_Length - 1] << 8); 
	uint16_t Expected_CRC = ModbusRTU_CRC_Calculate(Modbus->Rx.Buffer, Modbus->Rx.Buffer_Length - 2);
	
	if(Received_CRC != Expected_CRC){
		return;
	}
	
	Modbus->Tx.Buffer_Index = 0;
	
	if(Modbus->FunctionCodes_Handler != NULL){
		Modbus->FunctionCodes_Handler();
	}
	else{
			
	}
	
	Modbus->Response_Available = true;
}
//====================================================================================================
bool ModbusRTU_CheckDeviceAvailability(ModbusRTU* Modbus, uint8_t DeviceAddress){
	Modbus->Tx.Buffer_Index = 0;
					
	Modbus->Tx.Buffer[Modbus->Tx.Buffer_Index++] = DeviceAddress;									// Device address
	Modbus->Tx.Buffer[Modbus->Tx.Buffer_Index++] = MODBUS_READ_HOLDING_REGISTERS; // Function code
	Modbus->Tx.Buffer[Modbus->Tx.Buffer_Index++] = 0x00;													// Starting address (Hi)
	Modbus->Tx.Buffer[Modbus->Tx.Buffer_Index++] = 0x00;													// Starting address (Lo)
	Modbus->Tx.Buffer[Modbus->Tx.Buffer_Index++] = 0x00;													// Quantity of registers (Hi)
	Modbus->Tx.Buffer[Modbus->Tx.Buffer_Index++] = 0x01;													// Quantity of registers (Lo)
					
	ModbusRTU_Request_Initialize(Modbus);
	
	uint32_t Request_Initialized_Time = Modbus->GetSystemTime_ms();
	
	while((Modbus->GetSystemTime_ms() - Request_Initialized_Time < MODBUS_MAX_TIMEOUT) &&
				(!Modbus->Response_Available));
	
	if(Modbus->Response_Available){
		return true;
	}
	else{
		return false;
	}
}

void ModbusRTU_AutoSearchDevices(ModbusRTU* Modbus){
	bool SearchingDeviceAvailabilityFlag = false;
	Modbus->AddressesArrayPosition = 0;
	Modbus->AddressesAmount = 0;
	
	for(uint8_t SelectedAddress = 1; SelectedAddress <= 32; SelectedAddress++){
		SearchingDeviceAvailabilityFlag = ModbusRTU_CheckDeviceAvailability(Modbus, SelectedAddress);
		
		if(SearchingDeviceAvailabilityFlag){
			Modbus->AddressesArray[Modbus->AddressesArrayPosition] = SelectedAddress;
			
			Modbus->AddressesArrayPosition++;
			Modbus->AddressesAmount++;
		}
	}
	
	Modbus->AddressesArrayPosition = 0;
}
//====================================================================================================
