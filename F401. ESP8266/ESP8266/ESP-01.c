//-------------------------------------------------------------------------------------------//
#include <stdio.h>
//-------------------------------------------------------------------------------------------//
#include "UART.h"
#include "ESP-01.h"
#include "TIM.h"
//-------------------------------------------------------------------------------------------//
char basic_page[] = "<!DOCTYPE html> <html>\n<head><meta name=\"viewport\"\
		content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n\
		<title>LED CONTROL</title>\n<style>html { font-family: Helvetica; \
		display: inline-block; margin: 0px auto; text-align: center;}\n\
		body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\
		h3 {color: #444444;margin-bottom: 50px;}\n.button {display: block;\
		width: 80px;background-color: #1abc9c;border: none;color: white;\
		padding: 13px 30px;text-decoration: none;font-size: 25px;\
		margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n\
		.button-on {background-color: #1abc9c;}\n.button-on:active \
		{background-color: #16a085;}\n.button-off {background-color: #34495e;}\n\
		.button-off:active {background-color: #2c3e50;}\np {font-size: 14px;color: #888;margin-bottom: 10px;}\n\
		</style>\n</head>\n<body>\n<h1>ESP8266 LED CONTROL</h1>\n";

char LED_ON[] = "<p>LED Status: ON</p><a class=\"button button-off\" href=\"/ledoff\">OFF</a>";
char LED_OFF[] = "<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/ledon\">ON</a>";
char Terminate[] = "</body></html>";

char buffer[200];
uint8_t buffer_counter = 0, OK_flag = 0;
char* pa;
char LinkID;
int v, D;

void Clear_Buffer(void){
	uint16_t i = 0;
	while(i < sizeof(buffer)){
		buffer[i] = 0;
		i++;
	}
	buffer_counter = 0;
}

void ESP_Init(void){
	/*Reset the module*/
	SendString1("AT+RST\r\n");
	MegaDelay_us(50000);
	/*Check if it's okay*/
	SendString1("AT\r\n");
	MegaDelay_us(50000);
	/*Setting connection mode*/
	SendString1("AT+CWMODE=1\r\n");
	MegaDelay_us(50000);
	
	/*Connecting to the Wi-Fi*/
	SendString1("AT+CWJAP=\"Xiaomi_3841\",\"GP21270463\"\r\n");
	MegaDelay_us(50000);
	Clear_Buffer();
	while(strstr(buffer, "WIFI GOT IP\r\n\r\nOK\r\n") == 0);
	/*Getting IP address*/
	SendString1("AT+CIFSR\r\n");
	while(strstr(buffer, "CIFSR:STAIP") == 0);
	MegaDelay_us(50000);
	MegaDelay_us(50000);
	MegaDelay_us(50000);

	SendString1("AT+CIPMUX=1\r\n");
	MegaDelay_us(50000);
	MegaDelay_us(50000);
	MegaDelay_us(50000);
	
	SendString1("AT+CIPSERVER=1,80\r\n");
	MegaDelay_us(50000);
	MegaDelay_us(50000);
	MegaDelay_us(50000);
	
	//SendString2("Connection established successfully!");
	Clear_Buffer();
	MegaDelay_us(50000);
	MegaDelay_us(50000);
}

int ESP_Send(char* strq, int LinkID){
	Clear_Buffer();
	int len;
  len = strlen(strq);
	D = sizeof(strq);
	char data[100];
	sprintf(data, "AT+CIPSEND=%d,%d\r\n", LinkID, len);
	SendString1(data);
	while(strstr(buffer, ">") == 0);
	SendString1(strq);
	while(strstr(buffer, "SEND OK") == 0);
	SendString1("AT+CIPCLOSE=5\r\n");
	while(strstr(buffer, "OK\r\n") == 0);
	MegaDelay_us(50000);
	MegaDelay_us(50000);
	Clear_Buffer();
	MegaDelay_us(50000);
	MegaDelay_us(50000);
	MegaDelay_us(50000);
	return 1;
}

void Server_Handle(char* strq, int LinkID){
	Clear_Buffer();
	char data[1024] = {0};
	if(strstr(strq, "/ledon")){
		sprintf(data, basic_page);
		strcat(data, LED_ON);
		strcat(data, Terminate);
		ESP_Send(data, LinkID);
	}
	else if(strstr(strq, "/ledoff")){
		sprintf(data, basic_page);
		strcat(data, LED_OFF);
		strcat(data, Terminate);
		ESP_Send(data, LinkID);
	}
	else if(strstr(strq, "/a")){
		sprintf(data, "HTTP/1.1 200 OK");
		ESP_Send(data, LinkID);
	}
	else{
		sprintf(data, basic_page);
		strcat(data, LED_OFF);
		strcat(data, Terminate);
		ESP_Send(data, LinkID);
	}
	Clear_Buffer();
}

void Server_Start(void){
	Clear_Buffer();
	LinkID = 0;
	while(strstr(buffer, "CONNECT") == 0);
	while(strstr(buffer, "+IPD,") == 0);
	MegaDelay_us(50000);
	pa = strstr(buffer, "+IPD,");
	pa = pa + 5;
	LinkID = pa[0];
  LinkID = LinkID - 0x30;
	if(strstr(buffer, "GET / ")){
		Server_Handle("/ ", LinkID);
	}
	else if(strstr(buffer, "GET /ledon")){
		Server_Handle("/ledon", LinkID);
	}
	else if(strstr(buffer, "GET /ledoff")){
		Server_Handle("/ledoff", LinkID);
	}
	else if(strstr(buffer, "GET /a")){
		Server_Handle("/a", LinkID);
	}
}
//-------------------------------------------------------------------------------------------//
