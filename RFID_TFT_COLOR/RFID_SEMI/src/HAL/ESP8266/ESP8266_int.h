/*
 * ESP8266_int.h
 *
 *  Created on: Sep 8, 2026
 *      Author: lenovo
 */

#ifndef HAL_ESP8266_ESP8266_INT_H_
#define HAL_ESP8266_ESP8266_INT_H_

void HESP8266_vInit();

//connect to wifi
void HESP8266_vConnectAccessPoint(char* A_s8SSD, char* A_s8Password);

//tcp
void HESP8266_vOpenServerTCPConnection(char* IP, char* SocketNo);

void HESP8266_vSendHttpRequest(char* URL);





#endif /* HAL_ESP8266_ESP8266_INT_H_ */
