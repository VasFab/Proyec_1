#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "ThingSpeak.h"

// Configuración Wi-Fi
const char* ssid = "FVS";
const char* password = "3138587602";

// Configuración ThingSpeak
unsigned long myChannelNumber = 2672621;  // Número de canal
const char* myWriteAPIKey = "TTMB6BKLOQ5R1D3Z";  // Write API Key

WiFiClient client;

// Configuración del sensor de lluvia
const int pinSensorLluvia = 34;
int valorLluvia = 0;
int sumaValoresLluvia = 0;
int contadorLecturasLluvia = 0;
const int duracionPromedioLluvia = 15000;
unsigned long tiempoInicioLluvia = 0;

// Configuración del sensor de humedad del suelo
const int pinSensorHumedad = 32;
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

void setup() {
  Serial.begin(9600);
  setup_wifi();

  ThingSpeak.begin(client);

  pinMode(pinSensorLluvia, INPUT);
  pinMode(pinSensorHumedad, INPUT);
  
  sensors.begin();
  
  Serial.println("Iniciando lectura de sensores...");
}

void loop() {
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

    // Enviar a ThingSpeak (Campo 1: sensor_lluvia)
    ThingSpeak.setField(1, valorPromedioLluvia);
    
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

  if (promedioHumedad == 4095) {
    Serial.println("El sensor no está en contacto con la tierra.");
    // Enviar un valor especial o cero a ThingSpeak si el sensor está desconectado
    ThingSpeak.setField(2, 0);  // Enviar 0 si el sensor no está en contacto
  } else {
    // Convertir el valor a porcentaje de humedad
    int porcentajeHumedad = map(promedioHumedad, 4095, 1709, 0, 100);
    porcentajeHumedad = constrain(porcentajeHumedad, 0, 100);
    Serial.println("Humedad del suelo: " + String(porcentajeHumedad) + "%");

    // Enviar a ThingSpeak (Campo 2: sensor_humedad_suelo)
    ThingSpeak.setField(2, porcentajeHumedad);
  }

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

    // Enviar a ThingSpeak (Campo 3: sensor_temperatura)
    ThingSpeak.setField(3, promedioTemperatura);
  } else {
    Serial.println("No se obtuvieron lecturas válidas del sensor de temperatura.");
  }

  // Enviar los datos a ThingSpeak
  int resultado = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (resultado == 200) {
    Serial.println("Datos enviados correctamente a ThingSpeak");
  } else {
    Serial.println("Error al enviar los datos a ThingSpeak. Código de error: " + String(resultado));
  }

  delay(5000);  // Pausa de 5 segundos antes de la siguiente lectura
}

