# PHV_ESP8266_LittleFS
Librería con interprete PHV servido desde LittleFS (PHV, Preprocesador de Hipertexto en Vuelo). Convierte paginas estáticas almacenadas como archivos en LittleFS en páginas dinámicas con datos del ESP durante la transmisión de la pagina (por ello “en vuelo”), sin requerir modificaciones en el código del ESP (no se requiere actualizar firmware si no se añaden nuevos datos internos a representar) si la página debe ser modificada para mejorar la gráfica de la página o presentación de los datos. 

### Ejemplo de página sin interpretar.

![Screenshot](https://raw.githubusercontent.com/gonzalocargut/PHV_ESP8266_LittleFS/main/ejemplo%20sin%20interpretar.png)

### Ejemplo interpretado con PHV.

![Screenshot](https://raw.githubusercontent.com/gonzalocargut/PHV_ESP8266_LittleFS/main/ejemplo%20interpretado.png)

## Descripción

Se tienen cuatro tipos de datos a interpretar: enteros, flotantes, cadenas (String) y funciones, en estas últimas se entrega una función miembro para enviar datos al cliente que realiza la solicitud. Se hace uso de matrices de los tipos de datos indicados, estos deben ser entregados al interprete para su uso, y el usuario debe darles valores adecuados en los momentos oportunos, por ejemplo, justo antes de cargar valores en una página ejecutar una función llamada desde PHV ($$P000X$$) que actualice los valores, o bien que el ESP de forma asíncrona genere los datos según la aplicación. Para entregar las matrices deben llamarse a las funciones miembro adecuadas según el tipo, por ejemplo para números enteros se tiene **”cargarMatrizInt”**, entregándole la matriz más su tamaño. Si se intenta leer un índice que esta fuera de la matriz, no se transmitirán datos borrando el comando durante la transmisión.

### Los códigos que PHV lee son los siguientes:
 - Entero: $$EXYYZ$$
   * X = 0 sin modificar, 1 tamaño numero (rellena a la izquierda con ceros) hasta alcanzar Y
- Flotante: $$FXYYZ$$
   * X = 0 dos decimales, 1 decimales especificados por Y
 - Cadena: $$C000Z$$
 - Función: $$P000Z$$
 - Función de Sistema: $$SXYYZ$$
   * X = 0, aun sin asignar
   * Y = para Z = 2 (millis() en formato HH:MM), 1 con segundos, 2 con segundos y milisegundos

Donde X es la opción del tipo de dato, Y las opciones según X, y Z el índice de la matriz entregada al interprete que este deberá leer y reemplazar. Z puede tener tantos dígitos sean necesarios para representar al índice. Solo se admiten números del 0 al 9 en los campos, siendo X e Y fijos en largo, y Z variable. Si Y solo tiene un digito, se debe colocar un 0 a la izquierda.

#### Índices de funciones de sistema
 | Índice | Equivalente | 
 | :---: | :--- | 
 | 1 | millis() | 
 | 2 | millis() (formateado HH:MM) | 
 | 5 | tiempo ejecucion interprete (us) | 
 | 6 | tiempo transmision (us) | 
 | 10 | ESP.getChipId() | 
 | 11 | ESP.getCoreVersion() | 
 | 12 | ESP.getCpuFreqMHz() | 
 | 13 | ESP.getFlashChipId() | 
 | 14 | ESP.getFlashChipSize() | 
 | 15 | ESP.getFlashChipSpeed() | 
 | 16 | ESP.getFreeHeap() | 
 | 17 | ESP.getFreeSketchSpace() | 
 | 18 | ESP.getFullVersion() | 
 | 19 | ESP.getResetInfo() | 
 | 20 | ESP.getResetReason() | 
 | 21 | ESP.getSdkVersion() | 
 | 22 | ESP.getVcc() | 

Se deben especificar las extensiones a ser interpretadas, por defecto la extensión **".phv"** será interpretada, en el ejemplo ejemploPHVLittleFS.ino se añade la extensión **".html"**, las nuevas extensiones pueden ser agregadas en múltiples llamadas a la función miembro **"cargarExtensionAInterpretar"** o bien realizando una sola llamada separando las extensiones con puntos, por ejemplo **"html.htm.phv.etc"**, no importa que las extensiones se repitan, solo se desperdiciara un poco de RAM. Cualquier archivo que no sea interpretado, será transmitido tal cual según su tipo.

También se provee de una función para especificar un archivo de página no encontrada **”cargarPaginaNoEncontrada”**, así es posible tener una página personalizada que permita volver al inicio con un botón u otra función útil, se debe especificar la ruta absoluta hacia el archivo en el almacenamiento. Antes de llamar a esta función el sistema de archivos debe estar inicializado.

El código de envío desde LittleFS se basa en el ejemplo SDWebServer que viene en el núcleo para ESP8266, al igual que el editor, el que fue modificado para mostrar más información de los archivos almacenados como su tamaño y la capacidad utilizada y disponible de la unidad.
https://github.com/esp8266/ESPWebServer/tree/master/examples/SDWebServer
