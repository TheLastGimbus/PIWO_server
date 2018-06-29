#include <ESP8266WiFi.h>

const char* wifiName = "Broadcom";
const char* wifiPass = "";

#define serverPort 5000
IPAddress localIP = IPAddress(192, 168, 1, 100);
WiFiServer server(serverPort);

#define MAX_NET_CLIENTS 4
#define MAX_OUT_CLIENTS 4

IPAddress networkAddresses[MAX_NET_CLIENTS];

WiFiClient netClients[MAX_NET_CLIENTS];
WiFiClient outClients[MAX_OUT_CLIENTS];

ADC_MODE(ADC_VCC);

int rTune = 65;
int gTune = 255;
int bTune = 245;

void connectToWifi(const char* name, const char* password){
	WiFi.mode(WIFI_STA);
	WiFi.begin(name, password);
	int tryTime = millis();
	while (!WiFi.isConnected()) {
		if((millis() - tryTime) > 120000){
			Serial.print("Can't connect, reseting...");
			ESP.restart();
		}
		digitalWrite(2, 0);
		delay(500);
		digitalWrite(2, 1);
		delay(500);
		Serial.println("Connecting to WiFi...");
	}
	Serial.println("Connected to wifi!");
	WiFi.config(localIP, WiFi.gatewayIP(), WiFi.subnetMask());
	server.begin();
}

int attatchNewClient(WiFiClient newClient){
	//loop that cheks if client is form network
	for(int x = 0; x < MAX_NET_CLIENTS; x++){
		if(newClient.remoteIP() == networkAddresses[x]){
			netClients[x] = newClient;
			Serial.println("Connected " + netClients[x].remoteIP().toString() + " in network");
			return 1;
		}
	}
	//loop that cheks if there's place in outside network
	for(int x = 0; x < MAX_OUT_CLIENTS; x++){
		if(!outClients[x].connected()){
			outClients[x] = newClient;
			Serial.println("Connected " + outClients[x].remoteIP().toString() + " in outside");
			return 2;
		}
	}
	Serial.println("Not connected!");
	return 3;
}

void okRequestHandle(){
	for(int x = 0; x < MAX_NET_CLIENTS; x++){
		if(netClients[x].available()){
			String in = netClients[x].readStringUntil('~');
			if(in.indexOf("OK?") == 0){
				netClients[x].print("OK~");
				Serial.println("OK request handled with " + netClients[x].remoteIP().toString());
			}
		}
	}
}

String decToHex(byte decValue, byte desiredStringLength = 2) {
  String hexString = String(decValue, HEX);
  while (hexString.length() < desiredStringLength) hexString = "0" + hexString;

  return hexString;
}

String tuneColor(byte r, byte g, byte b){
	r = map(r, 0, 255, 0, rTune);
	g = map(g, 0, 255, 0, gTune);
	b = map(b, 0, 255, 0, bTune);
	String hexStr = "#" + decToHex(r) + decToHex(g) + decToHex(b);
	return hexStr;
}

String tuneColor(String hex){
	long long hexCol = strtoll(&hex[1], NULL, 16);
	int r = hexCol >> 16;
	int g = hexCol >> 8 & 0xFF;
	int b = hexCol & 0xFF;
	return tuneColor(r, g, b);
}

String command(String com){
	String response = "";
	if(com.indexOf("status") == 0){
		response += ("=====================\n");
		response += ("DeviceID: server\n");
		response += ("Vcc: " + String(((float)ESP.getVcc()) / 1024) + "\n");
		response += ("WiFi name: " + String(wifiName) + "\n");
		response += ("WiFi pass: " + String(wifiPass) + "\n");
		response += ("Local IP: " + WiFi.localIP().toString() + "\n");
		response += ("Server port: " + String(serverPort) + "\n");
		response += ("Network clients (max " + String(MAX_NET_CLIENTS) + "):\n");
		for(int x = 0; x < MAX_NET_CLIENTS; x++){
			response += (netClients[x].remoteIP().toString() + "\n");
		}
		response += ("Outside clients (max " + String(MAX_OUT_CLIENTS) + "):\n");
		for(int x = 0; x < MAX_OUT_CLIENTS; x++){
			response += (outClients[x].remoteIP().toString() + "\n");
		}
		response += ("=====================\n");
	}
	if(com.indexOf("setTuneMax") == 0){
		com.remove(0, 10);
		long long hexCol = strtoll(&com[1], NULL, 16);
		rTune = hexCol >> 16;
		gTune = hexCol >> 8 & 0xFF;
		bTune = hexCol & 0xFF;
		com.remove(0, 7);
		response += ("New max tune values: \n");
		response += ("Red: " + String(rTune) + " \n");
		response += ("Green: " + String(gTune) + " \n");
		response += ("Blue: " + String(bTune) + " \n");
	}
	if(com.indexOf("getTuneMax") == 0){
		response += ("Max tune values: \n");
		response += ("Red: " + String(rTune) + " \n");
		response += ("Green: " + String(gTune) + " \n");
		response += ("Blue: " + String(bTune) + " \n");
	}
	if(com.indexOf("#") == 0){
		int pixels = com.length() / 7;
		response += ("Pixels get: " + String(pixels) + "\n");

		for(int x = 0; x < pixels; x++){
			int from = x * 7;
			int to = from + 7;
			String pixel = com.substring(from, to); // getting a substring
			pixel = tuneColor(pixel); // and tuning it for untuned LEDs
			response += (String(x) + ": " + pixel + " goes to " + netClients[x].remoteIP().toString() + "\n");
			netClients[x].print(pixel + "~");
		}
	}
	return response;
}

void setup() {
	Serial.begin(115200);
	pinMode(2, OUTPUT);
	digitalWrite(2, 1);

	for(int x = 0; x < MAX_NET_CLIENTS; x++)
		networkAddresses[x] = IPAddress(192, 168, 1, (101 + x));

	connectToWifi(wifiName, wifiPass);
}

void loop() {
	if(!WiFi.isConnected())
		connectToWifi(wifiName, wifiPass);

	while (server.hasClient()){
		Serial.println("Connecting with new client...");
		attatchNewClient(server.available());
	}

	while(Serial.available()){
		Serial.println(command(Serial.readStringUntil('\n')));
	}
	for(int x = 0; x < MAX_OUT_CLIENTS; x++){
		while(outClients[x].available()){
			String res = command(outClients[x].readStringUntil('~'));
			outClients[x].print(res + "~");
		}
	}

	okRequestHandle();
}
