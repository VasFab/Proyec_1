#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Configuración Wi-Fi
const char* ssid = "camilo";
const char* password = "3138587602";

// Configuración MQTT
const char* mqtt_server = "192.168.147.96";
const int mqtt_port = 1883;
const char* topicLluvia = "sensor_lluvia";
const char* topicHumedad = "sensor_humedad_suelo";
const char* topicTemperatura = "sensor_temperatura";

WiFiClient espClient;
PubSubClient client(espClient);

// Configuración del sensor de lluvia
const int pinSensorLluvia = 32;
int valorLluvia = 0;
int sumaValoresLluvia = 0;
int contadorLecturasLluvia = 0;
const int duracionPromedioLluvia = 15000;
unsigned long tiempoInicioLluvia = 0;

// Configuración del sensor de humedad del suelo
const int pinSensorHumedad = 34;
const int tiempoLecturaHumedad = 15000;
int valorHumedad = 0;
float promedioHumedad = 0;

// Configuración del sensor de temperatura
#define ONE_WIRE_BUS 15
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
#define TIEMPO_PROMEDIO_TEMPERATURA 15000
#define TEMP_MINIMA 18.0
#define TEMP_MAXIMA 30.0

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("...");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando al broker MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado.");
    } else {
      Serial.print("Falló, rc=");
      Serial.print(client.state());
      Serial.println(" intentando de nuevo en 5 segundos.");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  
  client.setServer(mqtt_server, mqtt_port);

  pinMode(pinSensorLluvia, INPUT);
  pinMode(pinSensorHumedad, INPUT);
  
  sensors.begin();
  
  Serial.println("Iniciando lectura de sensores...");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Lectura del sensor de lluvia
  unsigned long tiempoActualLluvia = millis();
  if (tiempoActualLluvia - tiempoInicioLluvia < duracionPromedioLluvia) {
    valorLluvia = analogRead(pinSensorLluvia);
    sumaValoresLluvia += valorLluvia;
    contadorLecturasLluvia++;
    delay(1000);
  } else {
    int valorPromedioLluvia = sumaValoresLluvia / contadorLecturasLluvia;
    Serial.print("Valor promedio de lluvia en 15 segundos: ");
    Serial.println(valorPromedioLluvia);

    String mensajeLluvia;
    if (valorPromedioLluvia == 4095) {
      mensajeLluvia = "No se presentan lluvias.";
    } else if (valorPromedioLluvia >= 3000 && valorPromedioLluvia <= 4094) {
      mensajeLluvia = "Leves lluvias.";
    } else if (valorPromedioLluvia >= 1900 && valorPromedioLluvia <= 2970) {
      mensajeLluvia = "Moderadas lluvias.";
    } else if (valorPromedioLluvia > 1072 && valorPromedioLluvia < 1900) {
      mensajeLluvia = "Lluvias moderadas a fuertes.";
    } else if (valorPromedioLluvia <= 1072) {
      mensajeLluvia = "Se presentan fuertes lluvias.";
    }
    client.publish(topicLluvia, mensajeLluvia.c_str());
    
    sumaValoresLluvia = 0;
    contadorLecturasLluvia = 0;
    tiempoInicioLluvia = millis();
  }

  // Lectura del sensor de humedad del suelo
  unsigned long tiempoInicioHumedad = millis();
  int sumaHumedad = 0;
  int contadorHumedad = 0;
  while (millis() - tiempoInicioHumedad < tiempoLecturaHumedad) {
    valorHumedad = analogRead(pinSensorHumedad);
    sumaHumedad += valorHumedad;
    contadorHumedad++;
    delay(100);
  }
  promedioHumedad = sumaHumedad / contadorHumedad;

  String mensajeHumedad;
  if (promedioHumedad == 4095) {
    mensajeHumedad = "El sensor no está en contacto con la tierra.";
  } else {
    int porcentajeHumedad = map(promedioHumedad, 4095, 1709, 0, 100);
    porcentajeHumedad = constrain(porcentajeHumedad, 0, 100);
    mensajeHumedad = "Humedad del suelo: " + String(porcentajeHumedad) + "%";
  }
  client.publish(topicHumedad, mensajeHumedad.c_str());

  // Lectura del sensor de temperatura
  unsigned long startTime = millis();
  float sumaTemperaturas = 0;
  int contadorLecturasTemperatura = 0;
  while (millis() - startTime < TIEMPO_PROMEDIO_TEMPERATURA) {
    sensors.requestTemperatures();
    float temperatura = sensors.getTempCByIndex(0);

    if (temperatura != DEVICE_DISCONNECTED_C) {
      sumaTemperaturas += temperatura;
      contadorLecturasTemperatura++;
    }
    delay(2000);
  }

  if (contadorLecturasTemperatura > 0) {
    float promedioTemperatura = sumaTemperaturas / contadorLecturasTemperatura;
    Serial.print("Temperatura promedio de los últimos 15 segundos: ");
    Serial.print(promedioTemperatura);
    Serial.println(" °C");

    String mensajeTemperatura;
    if (promedioTemperatura < TEMP_MINIMA) {
      mensajeTemperatura = "Advertencia: La temperatura está por debajo del rango óptimo para el maíz.";
    } else if (promedioTemperatura > TEMP_MAXIMA) {
      mensajeTemperatura = "Advertencia: La temperatura está por encima del rango óptimo para el maíz.";
    } else {
      mensajeTemperatura = "La temperatura está dentro del rango óptimo para el cultivo de maíz.";
    }

    // Publicar el mensaje con la temperatura y la advertencia en el tópico MQTT
    String mensajeCompleto = "Temperatura: " + String(promedioTemperatura) + " °C. " + mensajeTemperatura;
    client.publish(topicTemperatura, mensajeCompleto.c_str());
  } else {
    Serial.println("No se obtuvieron lecturas válidas del sensor de temperatura.");
  }

  delay(5000);  // Pausa de 5 segundos antes de la siguiente lectura
}
