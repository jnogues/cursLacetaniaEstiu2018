#include <DHT22.h>
DHT22 sensor(D1); //declarem una variable del tipus DHTxx i assignem pin de dades a on es connectarà el sensor (D1)

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint32_t res = sensor.leer(); // hem de executar aquesta funció abans de llegir les dades
                                // cada segon com a mínim
//  Serial.println(res, HEX);
//  Serial.print((res >> 24));
//  Serial.print("|");
//  Serial.print((res & 0x00FF0000) >> 16);
//  Serial.print("|");
//  Serial.print((res & 0x0000FF00) >> 8);
//  Serial.print("|");
//  Serial.println((res & 0x000000FF));
  
  Serial.print("Temperatura (C): ");
  Serial.println(sensor.temperatura_C()); // temperatura en graus Celsius
  Serial.print("Temperatura (F): ");
  Serial.println(sensor.temperatura_F()); // temperatura en graus Fahrenheit
  Serial.print("Temperatura (K): ");
  Serial.println(sensor.temperatura_K()); // temperatura en graus Kelvin
  Serial.print("Humitat: ");
  Serial.println(sensor.humedad()); // humitat en %
  delay(1000);
}
