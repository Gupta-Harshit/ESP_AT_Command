/*Program to send device id, voltage and time to the server using ans esp8266 WiFi module
 *Communicating with the esp8266 through AT commands
 *Please read the At instructon set link - https://www.espressif.com/sites/default/files/documentation/4a-esp8266_at_instruction_set_en.pdf*/

#include <SoftwareSerial.h>
#define RX 11
#define TX 10
#define TIMEOUT 10000 //waiting time for response from esp8266
SoftwareSerial esp8266(RX, TX);

/*records the response from the esp8266 (same logic can be used wheneve you want the recieve data from the esp - not necessarily a response to a command)*/
bool getResponse(String correctResponse)
{
	unsigned long deadline = millis() + TIMEOUT;
	String res = "";
	while (millis()<=deadline)
	{
		if (esp8266.available())   //checking if esp8266 wants to transmit something
		{
			char c = esp8266.read();
			res += c;
			if (res.indexOf(correctResponse) > 0) //if response is equal to the desired response return true
			{
				Serial.println(res);
				return true;
			}
		}
	}
	Serial.println(res);
	return false;	
}

//sends a command to the esp8266, take the command to be sent and the desired response as arguments
bool sendCommand(String command, String ack)
{
	esp8266.println(command);   //command is sent to esp8266 (println adds \r\n to the end of every command indecating the end of every command)
	if (!getResponse(ack))
	{
		Serial.println("Fail");
		return false;
	}
	return true;
}
void setup() {
	Serial.begin(9600);
	esp8266.begin(9600); //default baud rate of esp8266 is 115200, the baud rate is changed so that the esp and arduino work on the same rate (it can be changed with the command - AT+UART_DEF=9600 (read at instruction set pdf))
	delay(1000);
	
	sendCommand("AT", "OK");
	delay(500);
	sendCommand("AT+CWMODE_CUR=1", "OK");   //command to set the current WiFi mode 1 - STA, 2 - AP, 3 - STA + AP
	if (sendCommand("AT+CWJAP?", "No AP")) //command to check if the module is already connected the a wifi
	{
		sendCommand("AT+CWJAP_CUR=\"Customice\",\"Anand@21\"", "OK");  //connecting the module to wifi
		delay(5000);
	}
	sendCommand("AT+CIPMUX=0", "OK");    //enable disable multiple connection 0 - single connection mode
	sendCommand("AT+CIPSTART=\"TCP\",\"http://us-central1-voltagebase-edce1.cloudfunctions.net\",80", "OK");  //establish connecton
	//sendCommand(getrqst, "SEND OK", 0);
}
void loop()
{	
	String getrqst,sendcom;
	/*the get request is needed to be sent along with the host header, other headers may be added
	 *the headers should be seperated eith a "\r\n" and an extra "\r\n" is added to the string by the "println" function */
	getrqst = "GET /add?id=123&voltage=3.3&time=" + String(millis()) + " HTTP/1.1\r\nHOST: us-central1-voltagebase-edce1.cloudfunctions.net\r\n";
	sendcom = "AT+CIPSEND=" + String(getrqst.length() + 2);   //the size of the data is the size of the string "getrqst" + the extra "\r\n" added by the "println" function
	
	if(!sendCommand(sendcom, "OK"))
		sendCommand("AT+CIPSTART=\"TCP\",\"http://us-central1-voltagebase-edce1.cloudfunctions.net\",80", "OK");
	if(!sendCommand(getrqst, "200 OK"))
		sendCommand("AT+CIPSTART=\"TCP\",\"http://us-central1-voltagebase-edce1.cloudfunctions.net\",80", "OK");
	delay(10000);
}