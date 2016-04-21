#include <user_config.h>
#include <SmingCore/SmingCore.h>

#include <led.h>

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
	#define WIFI_SSID  "ALICE-WLAN56" // Put you SSID and Password here
	#define WIFI_PWD "4174363082906394"
#endif

HttpServer server;
// static int counter;

Led blueLED;
#define LED_PIN 16 //GPIO2

//mDNS using ESP8266 SDK functions
void startmDNS() {
    struct mdns_info *info = (struct mdns_info *)os_zalloc(sizeof(struct mdns_info));
    info->host_name = (char *) "daniel"; // You can replace test with your own host name
    info->ipAddr = WifiStation.getIP();
	//actually the record is service_name no server
    info->server_name = (char *) "http";
    info->server_port = 80;
    // info->txt_data[0] = (char *) "version = now";
    espconn_mdns_init(info);
	// espconn_mdns_enable();
	// espconn_mdns_server_register();
}

// void onIndex(HttpRequest &request, HttpResponse &response)
// {
// 	// TemplateFileStream *tmpl = new TemplateFileStream("index.html");
// 	// auto &vars = tmpl->variables();
// 	// vars["counter"] = String(counter++);
// 	// response.sendTemplate(tmpl); // this template object will be deleted automatically
// 	response.sendString("hellooo");
// }

void onIndex(HttpRequest &request, HttpResponse &response)
{
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();

	json["foo"] = "bar";

	response.sendJsonObject(stream);

	blueLED.toggle();
}

void onTemp(HttpRequest &request, HttpResponse &response)
{
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();

	json["temp"] = "23.6";

	response.sendJsonObject(stream);
}



void onFile(HttpRequest &request, HttpResponse &response)
{
	String file = request.getPath();
	if (file[0] == '/')
		file = file.substring(1);

	if (file[0] == '.')
		response.forbidden();
	else
	{
		response.setCache(86400, true); // It's important to use cache for better performance.
		response.sendFile(file);
	}
}


void startWebServer()
{
	server.listen(80);
	server.addPath("/", onIndex);
	server.addPath("/temp", onTemp);

	Serial.println("\r\n=== WEB SERVER STARTED ===");
	Serial.println(WifiStation.getIP());
	Serial.println("==============================\r\n");
}


// Will be called when WiFi station was connected to AP
void connectOk()
{
	Serial.println("I'm CONNECTED");
	// if (!fileExist("index.html"))
	// 		fileSetContent("index.html", "<h3>Congrats !! You are Connected to your ESP module with mDNS address test.local</h3>");

	startWebServer();
	startmDNS();  // Start mDNS "Advertise" of your hostname "test.local" for this example

	blueLED.on();
}
void connectFail()
{
	debugf("I'm NOT CONNECTED!");
	WifiStation.waitConnection(connectOk, 10, connectFail);

	blueLED.off();
}

void init()
{
	spiffs_mount();

	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial

	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiStation.enable(true);
	WifiAccessPoint.enable(false);

	wifi_set_broadcast_if(STATIONAP_MODE);

	WifiStation.waitConnection(connectOk, 30, connectFail);

	blueLED.init(LED_PIN);

}
