# Proyecto de Monitoreo de Cultivos con Sensores
Este proyecto permite monitorear diferentes variables ambientales en un cultivo utilizando sensores de humedad del suelo, lluvia y temperatura. Los datos se envían a servicios en la nube y se visualizan utilizando diferentes protocolos de comunicación. La estructura del proyecto facilita el monitoreo remoto y en tiempo real de condiciones que impactan la salud del cultivo.

## Estructura del Repositorio
### Carpeta: CODIGO_MQTT
Contiene el código necesario para realizar pruebas de comunicación con el protocolo MQTT. Este código permite el envío y recepción de datos en tiempo real desde los sensores hasta un servidor MQTT, útil para la visualización remota.

### Carpeta: CODIGO_THINGSPEAKFI
Este directorio incluye el código para enviar datos al servicio ThingSpeak, que permite la visualización y almacenamiento de datos en la nube. ThingSpeak se utiliza para monitorizar los datos en tiempo real y generar gráficos de las variables ambientales recolectadas.

### Carpeta: PCB
Contiene un diseño sugerido para la placa de circuito impreso (PCB) del proyecto. Este diseño facilita la integración de todos los sensores y el microcontrolador en una estructura compacta y organizada, simplificando el ensamblaje del sistema.

### Carpeta: sensor_humedadsuelo_F
Este código es específico para el sensor de humedad del suelo HD-38. Permite obtener y procesar los datos de humedad del suelo y enviar la información al servidor para el monitoreo del cultivo. Los valores de humedad se interpretan de acuerdo con las condiciones de riego necesarias para el tipo de cultivo en cuestión.

### Carpeta: sensor_lluvia_F
Incluye el código para el sensor de lluvia YL-83, que detecta la presencia de lluvia y envía el estado al sistema. Dentro de esta carpeta, el subdirectorio sensor_lluvia_ESP_EMQX contiene configuraciones adicionales para conectarse a un servidor MQTT (EMQX) si se requiere un protocolo de comunicación específico.

### Carpeta: sensor_temperatura_F
Aquí se encuentra el código que permite la lectura de la temperatura utilizando el sensor LM35-DZ. Los datos de temperatura son capturados y procesados para su envío a la plataforma de monitoreo, permitiendo un seguimiento preciso de la temperatura en el ambiente del cultivo.

## Requerimientos
- **Microcontrolador**: ESP32 u otro compatible
- **Sensores**:
  - HD-38 para medir la humedad del suelo
  - YL-83 para detectar lluvia
  - LM35-DZ para medir la temperatura
- **Conexión a Internet** para la transmisión de datos (Wi-Fi para el ESP32)
- **Cuenta en ThingSpeak** (opcional, si se desea usar esta plataforma para visualización de datos)

## Instalación
1. Clonar el repositorio:
   ```bash
   git clonehttps://github.com/tu_usuario/Proyec_1.git
   ```
2. Subir el código correspondiente a tu ESP32 o microcontrolador compatible usando el entorno de desarrollo adecuado (ej. Arduino IDE).
3. Configurar el servidor MQTT o la cuenta de ThingSpeak según el código proporcionado en cada carpeta. Los detalles de configuración específicos para ThingSpeak y MQTT están en los comentarios del código.

## Uso del Proyecto
- **Monitoreo de Humedad del Suelo**: El sensor HD-38 mide la humedad en el suelo y envía datos periódicos al servidor, que luego se visualizan en tiempo real. Este monitoreo permite identificar niveles de riego óptimos y evitar el exceso de agua en el cultivo.
- **Detección de Lluvia**: El sensor de lluvia YL-83 detecta la presencia de lluvia y envía una alerta. Esta información se puede utilizar para desactivar el riego automático durante lluvias.
- **Monitoreo de Temperatura**: El sensor LM35-DZ mide la temperatura del ambiente del cultivo. Esto ayuda a los agricultores a controlar las condiciones de temperatura y ajustar la ventilación o la sombra según sea necesario.
