/*
 * PHV_ESP8266_LittleFS.h
 *
 *  Created on: 11-04-2021
 *      Author: Gonzalo
 */

#include <ESP8266WebServer.h>
#include <FS.h>
#include <LittleFS.h>

#ifndef LIBRARIES_PHV_ESP8266_LittleFS_PHV_ESP8266_H_LittleFS_
#define LIBRARIES_PHV_ESP8266_LittleFS_PHV_ESP8266_H_LittleFS_


class PHVclassLittleFS {
	public:
		PHVclassLittleFS();
		PHVclassLittleFS(ESP8266WebServer &servidorWeb, FS &Unidad);

		void inicializar(bool UnidadOK);
		void inicializar(ESP8266WebServer &servidorWeb, FS &Unidad, bool UnidadOK);

		void PHV();

		void cargarMatrizInt(int Matriz[], int TamMatriz);
		void cargarMatrizFloat(float Matriz[], int TamMatriz);
		void cargarMatrizFunciones(void (*Matriz[])(), int TamMatriz);
		void cargarMatrizString(String Matriz[], int TamMatriz);
		void cargarPaginaNoEncontrada(String ruta);

		void cargarExtensionAInterpretar(String extencion);

		template<typename Z> void enviarHTTP(Z&& x) {
//			Serial.print(x);
			servidor->client().print(x);
		}
		template<typename F, typename ... Args> void enviarHTTP(F&& x, Args&&... args) {
//			Serial.print(x);
			servidor->client().print(x);
			enviarHTTP(args...);
		}

		void enviarCabecera(int codigo, String tipoDatos);
		void detenerCliente();
	private:
		int *MatrizInt;
		int TamMatrizInt=0;
		float *MatrizFloat;
		int TamMatrizFloat=0;
		void (**MatrizFunciones)();
		int TamMatrizFunciones=0;
		String *MatrizString;
		int TamMatrizString=0;
		unsigned long inicioEjecucionPHV=0;
		unsigned long inicioTransmision=0;
		String extencionAInterpretar=".phv.";
		String rutaPaginaNoEncontrada ="";
		bool rutaPaginaNoEncontradaDefinida=0;
		bool UnidadFSInicializado = false;

		FS *UnidadFS;
		ESP8266WebServer *servidor;

		struct datosComando{
				char tipoComando = 0;
				int argumento[4] = { 0, 0, 0, 0 };
				int indice = 0;
		};

		bool cargarDesdeSD(String path, bool paginaNoEncontrada=0);
		void interpretePHV(File &archivo);
		void descComando(String &comando);
		void ejecComando(datosComando &comando);

		String fechaHoraFormat(long _milisegundos, int formato);
		String DosDigitos(int valor);
		const char marcador='$';
		bool debug = 0;
};

#define TamFileBufferIn 256
#define TamFileBufferOut 256

class fileBuffer{
	public:
		fileBuffer(File &archivo,ESP8266WebServer *servidorWeb);
		int available();
		uint8_t read();
		void write(uint8_t dato);
		void enviarBufferSalida();
	private:
		int puntbufferIn=0;
		uint8_t bufferIn[TamFileBufferIn+5] = { 0 };
		int bufferInLeido=0;
		File *archivoTrabajo=NULL;
		int puntbufferOut=0;
		uint8_t bufferOut[TamFileBufferOut+5] = { 0 };
		ESP8266WebServer *servidor;
};


#endif /* LIBRARIES_PHV_ESP8266_PHV_ESP8266_H_ */
