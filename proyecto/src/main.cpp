#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// Def. de la dirección I2C y las dimensiones 
// del LCD --> investigar si es 0x27 o 0x3f
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Inicializa el LCD
    lcd.init();
    
    // Enciende la luz de fondo (backlight)
    lcd.backlight();
    
    // Muestra un mensaje inicial
    lcd.setCursor(0, 0); // Columna 0, Fila 0
    lcd.print("Hola, ESP8266");
    
    lcd.setCursor(0, 1); // Columna 0, Fila 1
    lcd.print("Con I2C y PIO!");
}

void loop() {
    static int counter = 0;
    
    // Mueve el cursor a la segunda línea, columna 13
    lcd.setCursor(13, 1); 
    lcd.print(counter);
    
    delay(1000); // Espera 1 segundo
    counter++;
}
