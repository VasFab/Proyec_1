#include <OneWire.h>
#include <DallasTemperature.h>

// Definir el pin donde está conectado el DS18B20
#define ONE_WIRE_BUS 15

// Configurar la instancia de OneWire
OneWire oneWire(ONE_WIRE_BUS);

// Pasar la instancia de OneWire a la clase DallasTemperature
DallasTemperature sensors(&oneWire);

// Definir el tiempo total para el promedio (15 segundos)
#define TIEMPO_PROMEDIO 15000

// Definir los rangos de temperatura para el cultivo de maíz
#define TEMP_MINIMA 18.0
#define TEMP_MAXIMA 30.0

void setup() {
  Serial.begin(9600); // Iniciar la comunicación serial a 9600 baudios
  sensors.begin();    // Iniciar la comunicación con el sensor
}

void loop() {
  unsigned long startTime = millis(); // Obtener el tiempo de inicio
  float sumaTemperaturas = 0;
  int contadorLecturas = 0;

  // Leer y sumar valores de temperatura durante 15 segundos
  while (millis() - startTime < TIEMPO_PROMEDIO) {
    sensors.requestTemperatures();               // Solicitar la lectura del sensor
    float temperatura = sensors.getTempCByIndex(0); // Leer la temperatura en °C

    if (temperatura != DEVICE_DISCONNECTED_C) {  // Verificar que el sensor esté conectado
      sumaTemperaturas += temperatura;           // Sumar la temperatura a la suma total
      contadorLecturas++;                        // Incrementar el contador de lecturas
    }

    delay(2000); // Esperar 2 segundos entre lecturas
  }

  // Calcular el promedio de las temperaturas
  if (contadorLecturas > 0) {
    float promedioTemperatura = sumaTemperaturas / contadorLecturas;
    Serial.print("Temperatura promedio de los últimos 15 segundos: ");
    Serial.print(promedioTemperatura);
    Serial.println(" °C");

    // Verificar si la temperatura está dentro del rango óptimo para el cultivo de maíz
    if (promedioTemperatura < TEMP_MINIMA) {
      Serial.println("Advertencia: La temperatura está por debajo del rango óptimo para el maíz.");
    } else if (promedioTemperatura > TEMP_MAXIMA) {
      Serial.println("Advertencia: La temperatura está por encima del rango óptimo para el maíz.");
    } else {
      Serial.println("La temperatura está dentro del rango óptimo para el cultivo de maíz.");
    }
  } else {
    Serial.println("No se obtuvieron lecturas válidas del sensor.");
  }

  // Esperar 1 segundo antes de iniciar el próximo ciclo de lectura
  delay(1000);
}
