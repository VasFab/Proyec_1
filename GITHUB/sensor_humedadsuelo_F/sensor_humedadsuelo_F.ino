const int sensorPin = 34;  
const int tiempoLectura = 15000; 
int valorLectura = 0;
float promedio = 0;

void setup() {
  Serial.begin(9600);  
}

void loop() {
  unsigned long tiempoInicio = millis();
  int suma = 0;
  int contador = 0;

  // Recoge datos durante 15 segundos
  while (millis() - tiempoInicio < tiempoLectura) {
    valorLectura = analogRead(sensorPin);  // Lee el valor del sensor
    suma += valorLectura;
    contador++;
    delay(100);  // Un pequeño retraso para evitar leer demasiado rápido
  }

  // Promedia los valores
  promedio = suma / contador;

  // Si el valor promedio es 4095, el sensor está fuera de la tierra
  if (promedio == 4095) {
    Serial.println("El sensor no está en contacto con la tierra.");
  } else {
    // Mapea el valor promedio a un porcentaje de humedad (0 a 100%)
    int porcentajeHumedad = map(promedio, 4095, 1709, 0, 100);
    porcentajeHumedad = constrain(porcentajeHumedad, 0, 100);  // Asegura que el valor esté entre 0 y 100
    
    // Imprime el porcentaje de humedad
    Serial.print("Porcentaje de humedad: ");
    Serial.print(porcentajeHumedad);
    Serial.println("%");
    
    // Imprime la condición del suelo basada en el porcentaje de humedad
    if (porcentajeHumedad <= 10) {
      Serial.println("Condición del suelo: Muy seco. Riego urgente necesario.");
    } else if (porcentajeHumedad <= 30) {
      Serial.println("Condición del suelo: Seco. Riego necesario.");
    } else if (porcentajeHumedad <= 50) {
      Serial.println("Condición del suelo: Moderadamente seco. Riego recomendado.");
    } else if (porcentajeHumedad <= 70) {
      Serial.println("Condición del suelo: Húmedo. Ideal para el cultivo.");
    } else if (porcentajeHumedad <= 90) {
      Serial.println("Condición del suelo: Muy húmedo. Monitorear para evitar enfermedades.");
    } else {
      Serial.println("Condición del suelo: Saturado. Drenaje necesario.");
    }
  }

  delay(5000);  // Pausa de 5 segundos antes de la siguiente lectura
}

