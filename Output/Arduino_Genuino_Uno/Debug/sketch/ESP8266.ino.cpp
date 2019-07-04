#include <Arduino.h>
#line 1 "C:\\Users\\Harshit\\source\\repos\\ESP8266\\sketches\\ESP8266.ino"
#line 1 "C:\\Users\\Harshit\\source\\repos\\ESP8266\\sketches\\ESP8266.ino"
#include <SoftwareSerial.h>
#define RX 11
#define TX 10
#define TIMEOUT 10000
SoftwareSerial esp8266(RX, TX);
#line 6 "C:\\Users\\Harshit\\source\\repos\\ESP8266\\sketches\\ESP8266.ino"
bool getResponse(String correctResponse);
#line 26 "C:\\Users\\Harshit\\source\\repos\\ESP8266\\sketches\\ESP8266.ino"
bool sendCommand(String command, String ack);
#line 38 "C:\\Users\\Harshit\\source\\repos\\ESP8266\\sketches\\ESP8266.ino"
void setup();
#line 54 "C:\\Users\\Harshit\\source\\repos\\ESP8266\\sketches\\ESP8266.ino"
void loop();
#line 6 "C:\\Users\\Harshit\\source\\repos\\ESP8266\\sketches\\ESP8266.ino"
bool getResponse(String correctResponse)
{
	unsigned long deadline = millis() + TIMEOUT;
	String res = "";
	while (millis()<=deadline)
	{
		if (esp8266.available())
		{
			char c = esp8266.read();
			res += c;
			if (res.indexOf(correctResponse) > 0)
			{
				Serial.println(res);
				return true;
			}
		}
	}
	Serial.println(res);
	return false;	
}
bool sendCommand(String command, String ack)
{
	//Serial.println(command);
	//delay(1000);
	esp8266.println(command);
	if (!getResponse(ack))
	{
		Serial.println("Fail");
		return false;
	}
	return true;
}
void setup() {
	Serial.begin(9600);
	esp8266.begin(9600);
	delay(1000);
	sendCommand("AT", "OK");
	delay(500);
	sendCommand("AT+CWMODE_CUR=1", "OK");
	if (sendCommand("AT+CWJAP?", "No AP"))
	{
		sendCommand("AT+CWJAP_CUR=\"Customice\",\"Anand@21\"", "OK");
		delay(5000);
	}
	sendCommand("AT+CIPMUX=0", "OK");
	sendCommand("AT+CIPSTART=\"TCP\",\"http://us-central1-voltagebase-edce1.cloudfunctions.net\",80", "OK");
	//sendCommand(getrqst, "SEND OK", 0);
}
void loop()
{	
	String getrqst,sendcom;
	getrqst = "GET /add?id=123&voltage=3.3&time=" + String(millis()) + " HTTP/1.1\r\nHOST: us-central1-voltagebase-edce1.cloudfunctions.net\r\n";
	sendcom = "AT+CIPSEND=" + String(getrqst.length() + 2);
	if(!sendCommand(sendcom, "OK"))
		sendCommand("AT+CIPSTART=\"TCP\",\"http://us-central1-voltagebase-edce1.cloudfunctions.net\",80", "OK");
	if(!sendCommand(getrqst, "200 OK"))
		sendCommand("AT+CIPSTART=\"TCP\",\"http://us-central1-voltagebase-edce1.cloudfunctions.net\",80", "OK");
	delay(10000);
}
