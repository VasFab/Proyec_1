#include <WiFi.h>
#include <PubSubClient.h>

// Configuración Wi-Fi
const char* ssid = "camilo";         
const char* password = "3138587602"; 

// Configuración MQTT
const char* mqtt_server = "192.168.59.96"; 
const int mqtt_port = 1883;                
const char* topic = "sensor_lluvia";       

WiFiClient espClient;
PubSubClient client(espClient);

int pinSensor = 32;  
int valorLluvia = 0;  
int sumaValores = 0;  
int contadorLecturas = 0;  

// Duración para calcular el promedio (en milisegundos)
const int duracionPromedio = 15000;  
unsigned long tiempoInicio = 0;  


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
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

  pinMode(pinSensor, INPUT);
  
  Serial.println("Iniciando lectura del sensor YL83 para calcular el promedio cada 15 segundos...");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long tiempoActual = millis();  

 
  if (tiempoActual - tiempoInicio < duracionPromedio) {
    valorLluvia = analogRead(pinSensor);  
    sumaValores += valorLluvia;  
    contadorLecturas++;  

    delay(1000);  
  } else {
    
    int valorPromedio = sumaValores / contadorLecturas;

    // Muestra el valor promedio
    Serial.print("Valor promedio en 15 segundos: ");
    Serial.println(valorPromedio);

    String mensaje;

    
    if (valorPromedio == 4095) {
      mensaje = "No se presentan lluvias.";
    } else if (valorPromedio >= 3000 && valorPromedio <= 4094) {
      mensaje = "Leves lluvias.";
    } else if (valorPromedio >= 1900 && valorPromedio <= 2970) {
      mensaje = "Moderadas lluvias.";
    } else if (valorPromedio > 1072 && valorPromedio < 1900) {
      mensaje = "Lluvias moderadas a fuertes.";
    } else if (valorPromedio <= 1072) {
      mensaje = "Se presentan fuertes lluvias.";
    }

    // Publica el mensaje en el tópico MQTT
    client.publish(topic, mensaje.c_str());

    
    sumaValores = 0;
    contadorLecturas = 0;
    tiempoInicio = millis();  
  }
}
