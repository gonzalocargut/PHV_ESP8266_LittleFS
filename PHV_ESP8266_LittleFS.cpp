/*
 * PHV_ESP8266_LittleFS.cpp
 *
 *  Created on: 11-04-2021
 *      Author: Gonzalo
 */
#include "Arduino.h"
#include "PHV_ESP8266_LittleFS.h"
#include <LittleFS.h>
#include "PRINT_Y_PRINTLN_MOD.h"

PHVclassLittleFS::PHVclassLittleFS() {
	UnidadFS = NULL;
	servidor = NULL;
	MatrizFunciones = NULL;
	MatrizInt = NULL;
	MatrizFloat = NULL;
	MatrizString = NULL;
}

PHVclassLittleFS::PHVclassLittleFS(ESP8266WebServer &servidorWeb, FS &Unidad) {
	UnidadFS = &Unidad;
	servidor = &servidorWeb;
	MatrizFunciones = NULL;
	MatrizInt = NULL;
	MatrizFloat = NULL;
	MatrizString = NULL;
}

void PHVclassLittleFS::inicializar(bool UnidadOK) {
	UnidadFSInicializado = UnidadOK;
}

void PHVclassLittleFS::inicializar(ESP8266WebServer &servidorWeb, FS &Unidad, bool UnidadOK) {
	UnidadFSInicializado = UnidadOK;
	UnidadFS = &Unidad;
	servidor = &servidorWeb;
	servidor->onNotFound([&]() {PHV();});
}

void PHVclassLittleFS::cargarMatrizInt(int Matriz[], int TamMatriz) {
	MatrizInt = Matriz;
	TamMatrizInt = TamMatriz;
}
void PHVclassLittleFS::cargarMatrizFloat(float Matriz[], int TamMatriz) {
	MatrizFloat = Matriz;
	TamMatrizFloat = TamMatriz;
}
void PHVclassLittleFS::cargarMatrizFunciones(void (*Matriz[])(), int TamMatriz) {
	MatrizFunciones = Matriz;
	TamMatrizFunciones = TamMatriz;
}

void PHVclassLittleFS::cargarMatrizString(String Matriz[], int TamMatriz) {
	MatrizString = Matriz;
	TamMatrizString = TamMatriz;
}

void PHVclassLittleFS::cargarExtencionAInterpretar(String extencion) {
	extencionAInterpretar = extencionAInterpretar + "." + extencion + ".";
}

void PHVclassLittleFS::cargarPaginaNoEncontrada(String ruta) {
	if (UnidadFSInicializado == 0) {
		rutaPaginaNoEncontradaDefinida = 0;
		rutaPaginaNoEncontrada = "";
		return;
	}
	File dataFile = UnidadFS->open(ruta.c_str(), "r");
	if (dataFile and dataFile.isFile()) {
		rutaPaginaNoEncontradaDefinida = 1;
		rutaPaginaNoEncontrada = ruta;
	}
	else {
		rutaPaginaNoEncontradaDefinida = 0;
		rutaPaginaNoEncontrada = "";
	}
}

void PHVclassLittleFS::detenerCliente() {
	servidor->client().stop();
}

void PHVclassLittleFS::enviarCabecera(int codigo, String tipoDatos) {
	switch (codigo) {
		case 200:
			enviarHTTP("HTTP/1.1 200 OK\r\n");
			break;
		case 404:
			enviarHTTP("HTTP/1.1 404 Not Found\r\n");
			break;
		default:
			enviarHTTP("HTTP/1.1 ", String(codigo), "\r\n");
			break;
	}
	enviarHTTP("Content-Type: ", tipoDatos, "\r\n");
	enviarHTTP("Accept-Ranges: none\r\n");
	enviarHTTP("Connection: close\r\n");
	if (tipoDatos == "application/javascript") {
		enviarHTTP("Cache-Control: public, max-age=604800, immutable\r\n");
	}

	enviarHTTP("\r\n");
}

bool PHVclassLittleFS::cargarDesdeSD(String path, bool paginaNoEncontrada) {

	bool interpretar = false;
	String dataType = "text/plain";
	if (path.endsWith("/")) {
		path += "index.htm";
	}
	printmod(path);
	String extencion = path.substring(path.lastIndexOf('.'));
	extencion = extencion + ".";

	if (extencionAInterpretar.indexOf(extencion) >= 0) {
		interpretar = true;
	}

	if (path.endsWith(".src"))
		path = path.substring(0, path.lastIndexOf("."));
	else
		if (path.endsWith(".htm"))
			dataType = "text/html";
		else
			if (path.endsWith(".html")) {
				dataType = "text/html";
			}
			else
				if (path.endsWith(".phv"))
					dataType = "text/html";
				else
					if (path.endsWith(".css"))
						dataType = "text/css";
					else
						if (path.endsWith(".js"))
							dataType = "application/javascript";
						else
							if (path.endsWith(".png"))
								dataType = "image/png";
							else
								if (path.endsWith(".gif"))
									dataType = "image/gif";
								else
									if (path.endsWith(".jpg"))
										dataType = "image/jpeg";
									else
										if (path.endsWith(".ico"))
											dataType = "image/x-icon";
										else
											if (path.endsWith(".xml"))
												dataType = "text/xml";
											else
												if (path.endsWith(".pdf"))
													dataType = "application/pdf";
												else
													if (path.endsWith(".zip")) dataType = "application/zip";

	File dataFile = UnidadFS->open(path.c_str(), "r");

	if (dataFile.isDirectory()) {
		path += "/index.htm";
		dataType = "text/html";
		dataFile = UnidadFS->open(path.c_str(), "r");
	}

	if (!dataFile) return false;

	if (servidor->hasArg("download")) {
		dataType = "application/octet-stream";
		interpretar = 0;
	}

	if (paginaNoEncontrada == 1) {
		enviarCabecera(404, dataType);
	}
	else {
		enviarCabecera(200, dataType);
	}

//	printlnmod(" --> ", dataType, " --> ", interpretar, " --> ", dataFile.size());

	if (interpretar == 1) {
		inicioEjecucionPHV = micros();
		interpretePHV(dataFile);
	}
	else {
		while (dataFile.available()) {
			uint8_t buffer[505] = { 0 };
			servidor->client().write(buffer, dataFile.read(buffer, 500));
		}
	}
	Serial.println();
	detenerCliente();
	dataFile.close();
	return true;
}

void PHVclassLittleFS::PHV() {
	if (UnidadFS == NULL or servidor == NULL or UnidadFSInicializado == 0) {
		return;
	}
	inicioTransmision = micros();
	String message;

	if (cargarDesdeSD(servidor->uri())) {
		return;
	}
	else {
		if (rutaPaginaNoEncontradaDefinida == 1) {
			cargarDesdeSD(rutaPaginaNoEncontrada, 1);
			return;
		}
		message = "No encontrado en Unidad\n\n";
	}

	message += "URI: ";
	message += servidor->uri();
	message += "\nMethod: ";
	message += (servidor->method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += servidor->args();
	message += "\n";
	for (uint8_t i = 0; i < servidor->args(); i++) {
		message += " NAME:" + servidor->argName(i) + "\n VALUE:" + servidor->arg(i) + "\n";
	}

	enviarCabecera(404, "text/plain");
	enviarHTTP(message);
	detenerCliente();
}

void PHVclassLittleFS::interpretePHV(File &archivo) {
	char caracter[3]={0};
	while (archivo.available()) {
		caracter[0] = char(archivo.read());
		if (caracter[0] == marcador) { //se busca el iniciador "$$" o doble marcador
			caracter[1] = char(archivo.read());
			if (caracter[1] == marcador) {
				descComando(archivo);
			}
			else {
				enviarHTTP(caracter);
			}
		}
		else {
			enviarHTTP(caracter[0]);
		}
	}

}

String PHVclassLittleFS::DosDigitos(int valor) {
	if (valor < 0) {
		return "00";
	}
	String Salida = "";
	if (valor < 10) {
		Salida = "0";
		Salida += valor;
	}
	else {
		Salida = valor;
	}
	return Salida;
}

String PHVclassLittleFS::fechaHoraFormat(long _milisegundos, int formato) {
	if (formato > 2 or formato < 0) {
		return String("");
	}
	unsigned long hor, minu, seg, mili, milisegundos;

	if (_milisegundos >= 0) {
		milisegundos = _milisegundos;
	}
	else {
		milisegundos = -_milisegundos;
	}

	mili = milisegundos % 1000;
	milisegundos = milisegundos / 1000;
	hor = (long) (milisegundos / 3600);
	minu = (long) ((milisegundos - hor * 3600) / 60);
	seg = milisegundos - (hor * 3600 + minu * 60);

	String horaCodificada = DosDigitos(hor) + ":" + DosDigitos(minu);

	if (formato == 1 or formato == 2) {
		horaCodificada = horaCodificada + ":" + DosDigitos(seg);
	}

	if (_milisegundos < 0) {
		horaCodificada = "(-" + horaCodificada + "?)";
	}

	if (formato == 2) {
		return horaCodificada + "." + String(mili);
	}
	else {
		return horaCodificada;
	}
}


void PHVclassLittleFS::descComando(File &archivo) {
	datosComando comandoDec;
	char caracter[3] = { 0, 0, 0 };
	//inicio descodificacion y reemplazo
	//extraer nombre codificado
	String variable;
	variable.reserve(100);
	variable = "";
	for (int i = 0; i < 100; i++) { //se busca el terminador "$$" o doble marcador
		caracter[0] = char(archivo.read());
		if (caracter[0] == marcador) {
			archivo.read(); //al encontrar uno, se borra el siguiente.
			break;
		}
		else {
			variable += caracter[0];
		}
	}
	//ahora variable almacena el comando
	//descodificar comando
	comandoDec.tipoComando = variable.charAt(0);
	char variableChar[3] = { 0, 0, 0 };
	variableChar[0] = variable.charAt(1);
	variableChar[1] = variable.charAt(2);
	variableChar[2] = variable.charAt(3);
	for (int i = 0; i < 3; i++) {
		comandoDec.argumento[i] = int(variableChar[i]) - 48;
		if (comandoDec.argumento[i] > 9 or comandoDec.argumento[i] < 0) {
			comandoDec.argumento[i] = 0;
		}
	}
	comandoDec.argumento[3] = comandoDec.argumento[1] * 10 + comandoDec.argumento[2];
	//extraer indice
	comandoDec.indice = variable.substring(4).toInt();
	//verificar indice valido
	if (comandoDec.indice < 0) {
		return;
	}
	//ejecutar
	ejecComando(comandoDec);
}

void PHVclassLittleFS::ejecComando(datosComando &comando){

	//ultima verificacion de indice y reemplazar
	String dato = "";
	switch (comando.tipoComando) {
		case 'E':
			if (comando.indice > TamMatrizInt) {
				return;
			}
			dato = MatrizInt[comando.indice];
			if (comando.argumento[0] == 1) {

				while (int(dato.length()) < comando.argumento[3]) {
					dato = '0' + dato;
				}
			}
			enviarHTTP(dato);
			break;
		case 'F':
			if (comando.indice > TamMatrizFloat) {
				return;
			}
			if (comando.argumento[0] == 1) {
				dato = String(MatrizFloat[comando.indice], comando.argumento[3]);
			}
			else {
				dato = String(MatrizFloat[comando.indice], 2);
			}
			enviarHTTP(dato);
			break;
		case 'C':
			if (comando.indice > TamMatrizString) {
				return;
			}
			enviarHTTP(MatrizString[comando.indice]);
			break;
		case 'P':
			if (comando.indice > TamMatrizFunciones) {
				return;
			}
			if (MatrizFunciones[comando.indice] != NULL) {
				MatrizFunciones[comando.indice]();
			}
			break;
		case 'S':
			switch (comando.indice) {
				case 1:
					enviarHTTP(millis());
					break;
				case 2:
					enviarHTTP(fechaHoraFormat(millis(), comando.argumento[3]));
					break;
				case 5:
					enviarHTTP(micros() - inicioEjecucionPHV);
					break;
				case 6:
					enviarHTTP(micros() - inicioTransmision);
					break;
				case 10:
					dato = String(ESP.getChipId(), HEX);
					dato.toUpperCase();
					enviarHTTP(dato);
					break;
				case 11:
					enviarHTTP(ESP.getCoreVersion());
					break;
				case 12:
					enviarHTTP(ESP.getCpuFreqMHz(), "MHz");
					break;
				case 13:
					dato = String(ESP.getFlashChipId(), HEX);
					dato.toUpperCase();
					enviarHTTP(dato);
					break;
				case 14:
					enviarHTTP(ESP.getFlashChipSize());
					break;
				case 15:
					enviarHTTP(String(float(ESP.getFlashChipSpeed()) / 1000000.0, 2), "MHz");
					break;
				case 16:
					enviarHTTP(ESP.getFreeHeap());
					break;
				case 17:
					enviarHTTP(ESP.getFreeSketchSpace());
					break;
				case 18:
					enviarHTTP(ESP.getFullVersion());
					break;
				case 19:
					enviarHTTP(ESP.getResetInfo());
					break;
				case 20:
					enviarHTTP(ESP.getResetReason());
					break;
				case 21:
					enviarHTTP(String(ESP.getSdkVersion()));
					break;
				case 22:
					enviarHTTP(ESP.getVcc());
					break;
				case 99:
					detenerCliente();
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
//	printlnmod(" tipoComando:", comando.tipoComando, " arg0:", comando.argumento[0], " arg1:", comando.argumento[1], " arg2:", comando.argumento[2], " indice:", comando.indice);
}
