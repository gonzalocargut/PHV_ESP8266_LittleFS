# PHV_ESP8266_LittleFS
Librería con interprete PHV servido desde LittleFS (PHV, Preprocesador de Hipertexto en Vuelo).

Ejemplo de página sin interpretar.

![Screenshot](https://raw.githubusercontent.com/gonzalocargut/PHV_ESP8266_LittleFS/main/ejemplo%20sin%20interpretar.png)

Ejemplo interpretado con PHV.

![Screenshot](https://raw.githubusercontent.com/gonzalocargut/PHV_ESP8266_LittleFS/main/ejemplo%20interpretado.png)

Se tienen cuatro tipos de datos a interpretar: enteros, flotantes, cadenas (String) y funciones, en estas últimas se entrega una función para enviar datos al cliente que realiza la solicitud.

Se deben especificar las extensiones a ser interpretadas, por defecto la extensión **".phv"** será interpretada, en el ejemplo ejemploPHVLittleFS.ino se añade la extensión **".html"**, las nuevas extensiones pueden ser agregadas en múltiples llamadas a la función miembro **"cargarExtencionAInterpretar"** o bien realizando una sola llamada separando las extensiones con puntos, por ejemplo **"html.htm.phv.etc"**, no importa que las extensiones se repitan, solo se desperdiciara un poco de RAM. Cualquier archivo que no sea interpretado, será transmitido tal cual según su tipo.

También se provee de una función para especificar un archivo de página no encontrada **”cargarPaginaNoEncontrada”**, así es posible tener una pagina personalizada que permita volver al inicio con un botón u otra función útil, se debe especificar la ruta absoluta hacia el archivo en el almacenamiento. Antes de llamar a esta función el sistema de archivos debe estar inicializado.

El código de envío desde LittleFS se basa en el ejemplo SDWebServer que viene en el núcleo para ESP8266, al igual que el editor, el que fue modificado para mostrar más información de los archivos almacenados como su tamaño y la capacidad utilizada y disponible de la unidad.
https://github.com/esp8266/ESPWebServer/tree/master/examples/SDWebServer
