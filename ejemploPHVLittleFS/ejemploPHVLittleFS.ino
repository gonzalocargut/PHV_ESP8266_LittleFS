//PHV, Preprocesador de Hipertexto en Vuelo
/*
- compilar y subir al ESP el codigo del ejemplo
- configurar la flash para 3MB de FS y el resto memoria de programa
- entrar a x.x.x.x/update (x segun la ip que obtuviera el ESP al conectarse)
- subir el archivo ejemploPHVLittleFS.bin en FileSystem
esto ultimo creara la carpeta edit junto con dos archivos html de ejemplo,
el .htm no sera interpretado, pero el .html si, como dice el ejemplo
mas abajo (al añadir la extencion al interprete)

el archivo .bin del sistema de archivos fue creado con la herramienta
https://github.com/earlephilhower/arduino-esp8266littlefs-plugin

si no quieren acceso al editor, solo comenten el añadido de la libreria y la
inicializacion en el codigo
*/

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

#include "PHV_ESP8266_LittleFS.h"
#include <FS.h>
#include <LittleFS.h>
#include "ESP8266EditorWebLittleFS.h"

//const char* ssid = "tu_SSID";
//const char* password = "tu_clave";
const char* ssid = "GSCG_ST";
const char* password = "17885031";
const char* host = "esp8266littlefs";

int matrizInt[100] = { 0 };
float matrizFloat[100] = { 0.0 };
String matrizString[100];

void (*matrizFun[10])()={NULL};

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

PHVclassLittleFS ServicioPHV;

void pruebaFuncion() {
	ServicioPHV.enviarHTTP("Ejecucion Funcion interna en ESP8266, millis()=",millis(),"ms");
}

void setup(void) {
	Serial.begin(115200);
	Serial.print("\n");
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	Serial.print("Connecting to ");
	Serial.println(ssid);

	uint8_t i = 0;
	while (WiFi.status() != WL_CONNECTED && i++ < 20) {  //wait 10 seconds
		delay(500);
	}
	if (i == 21) {
		Serial.print("Could not connect to");
		Serial.println(ssid);
		while (1)
			delay(500);
	}
	Serial.print("Connected! IP address: ");
	Serial.println(WiFi.localIP());

	LittleFSConfig cfg;
	cfg.setAutoFormat(false);
	LittleFS.setConfig(cfg);

	bool LttileFSOK;

	if (!LittleFS.begin()) {
		Serial.println("LittleFS no inicializado");
		LttileFSOK = false;
	}
	else {
		Serial.println("LittleFS inicializado");
		LttileFSOK = true;
	}

	matrizFun[2] = pruebaFuncion;

	inicializarEditorWeb(server,LittleFS);
	ServicioPHV.cargarMatrizInt(matrizInt, sizeof(matrizInt) - 1);
	ServicioPHV.cargarMatrizFloat(matrizFloat, sizeof(matrizFloat) - 1);
	ServicioPHV.cargarMatrizString(matrizString, sizeof(matrizString) - 1);
	ServicioPHV.cargarMatrizFunciones(matrizFun, sizeof(matrizFun) - 1);
	ServicioPHV.cargarExtencionAInterpretar("html");
	ServicioPHV.inicializar(server, LittleFS, LttileFSOK);
	httpUpdater.setup(&server);
	server.begin();
	Serial.println("HTTP server started");

	FSInfo datos;
	LittleFS.info(datos);
//	printlnmodsep(" | ", "Capacidad", datos.totalBytes, "Utilizado", datos.usedBytes, "TamPagina", datos.pageSize, "MaxPath", datos.maxPathLength);

	String str = "";
	Dir dir = LittleFS.openDir("/");
	while (dir.next()) {
		str += dir.fileName();
		str += " - ";
		str += dir.fileSize();
		Serial.println(str);
		str="";
	}
}

void loop(void) {
	server.handleClient();
	matrizInt[10]++;
	matrizFloat[3] = float(millis()) / 1000;
	matrizString[2] = "millis(): " + String(millis());
	matrizString[2] = matrizString[2] + "ms";
}