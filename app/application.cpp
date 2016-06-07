#include <user_config.h>
#include <SmingCore/SmingCore.h>

#include <led.h>
#include <button.h>

NtpClient ntpClient("pool.ntp.org", 10*60);

rBootHttpUpdate* otaUpdater = 0;

#define ROM_0_URL "http://192.168.1.140/rom0.bin"
#define SPIFFS_URL "http://192.168.1.140/rboot.bin"

HttpServer server;
// static int counter;

Led blueLED;
#define LED_PIN 16 //GPIO2

Button configButton;
#define CONFIG_BUTTON 5

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

	json["time"] = SystemClock.getSystemTimeString();
	json["timestamp"] = RTC.getRtcSeconds();

	response.sendJsonObject(stream);

	blueLED.flash(100);
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




//____________________
void OtaUpdate_CallBack(bool result) {
    if (result == true) {
        // success
        uint8 slot;
        slot = rboot_get_current_rom();
        if (slot == 0) slot = 1; else slot = 0;
        // set to boot new rom and then reboot
        Serial.printf("Firmware updated, rebooting to rom %d...\r\n", slot);
        rboot_set_current_rom(slot);
        System.restart();
    } else {
        // fail
        Serial.println("Firmware update failed!");
    }
}

void OtaUpdate() {

    uint8 slot;
    rboot_config bootconf;

    // need a clean object, otherwise if run before and failed will not run again
    if (otaUpdater) delete otaUpdater;
    otaUpdater = new rBootHttpUpdate();

    // select rom slot to flash
    bootconf = rboot_get_config();
    slot = bootconf.current_rom;
    if (slot == 0) slot = 1; else slot = 0;

    // flash rom to position indicated in the rBoot config rom table
    otaUpdater->addItem(bootconf.roms[slot], ROM_0_URL);

    // use user supplied values (defaults for 4mb flash in makefile)
    if (slot == 0) {
        otaUpdater->addItem(RBOOT_SPIFFS_0, SPIFFS_URL);
    } else {
        otaUpdater->addItem(RBOOT_SPIFFS_1, SPIFFS_URL);
    }

    // set a callback
    otaUpdater->setCallback(OtaUpdate_CallBack);

    // start update
    otaUpdater->start();
}

void fwUpdate(HttpRequest &request, HttpResponse &response)
{
	Serial.println("DGB:Trying fw update...");

    OtaUpdate();
}

void startWebServer()
{
	server.listen(80);
	server.addPath("/", onIndex);
	server.addPath("/temp", onTemp);
	server.addPath("/update", fwUpdate);

	Serial.println("\r\n=== WEB SERVER STARTED ===");
	Serial.println(WifiStation.getIP());
	Serial.println("==============================\r\n");
}

void doSmartConf()
{
	Serial.println("DBG: start SmartConfig");
	WifiStation.smartConfigStart(SCT_EspTouch);
}

// Will be called when WiFi station was connected to AP
void connectOk()
{
	Serial.println("WiFi CONNECTED");

	startWebServer();
	startmDNS();

	blueLED.blinkSTOP();
	blueLED.on();
}
void connectFail()
{
	debugf("WiFi NOT CONNECTED!");
	WifiStation.waitConnection(connectOk, 10, connectFail);

	// blueLED.blinkSTOP();
	// blueLED.off();
}

void init()
{
	// spiffs_mount();

	Serial.println("----------------");
	Serial.println("I'm the NEW FW3");

	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial

	/*We will use autoconnect (the ESP stores in FLASH the lastest working
	WiFi credentials) */
	// WifiStation.config(WIFI_SSID, WIFI_PWD);

	WifiStation.enable(true);
	WifiAccessPoint.enable(false);

	wifi_set_broadcast_if(STATIONAP_MODE);

	WifiStation.waitConnection(connectOk, 30, connectFail);

	blueLED.init(LED_PIN);
	blueLED.blinkSTART(100);


	// configButton.init(CONFIG_BUTTON, OtaUpdate); //GPIO5
    configButton.init(CONFIG_BUTTON, doSmartConf); //GPIO5
}
