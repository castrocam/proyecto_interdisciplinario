#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>


#define BLYNK_TEMPLATE_ID "TMPL2NiUWdbIU"
#define BLYNK_TEMPLATE_NAME "Pastillero"
#define BLYNK_AUTH_TOKEN "Sn2XCwbcd1JWb4JSs4iMzZelZzWC5Plz"

char ssid[] = "WifiVicky";
char pass[] = "louaylor";

BlynkTimer timer;
WidgetRTC rtc;


// Motor A (Primer motor)
#define IN1_A D0
#define IN2_A D4
#define IN3_A D5
#define IN4_A D6


// Motor B (Segundo motor)
#define IN1_B D7
#define IN2_B D8
#define IN3_B RX
#define IN4_B TX 

// Botón
const int pinBoton = D3;

//LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Estructura para almacenar horarios
struct Horario {
  int hora;
  int minuto;
};


Horario horariosA_derecha[6];
Horario horariosA_izquierda[6];
Horario horariosB_derecha[6];
Horario horariosB_izquierda[6];


// Prototipos de funciones
void initializeMotorsAndBlynk();
void initializeDisplay();
void showTime();
void resetHorarios(Horario arr[]);
void moverA(bool derecha);
void moverB(bool derecha);
void apagarMotores();
void checkSchedule();
void leerBoton();


// --- Variables para el Botón ---
bool mensajePendiente = false; // Flag para el mensaje "Retirar Pastilla"

// Variables para el anti-rebote (debounce)
unsigned long ultimoDebounce = 0;
unsigned long delayDebounce = 50;
bool estadoBotonAnterior = HIGH;

void setup() {
  initializeMotorsAndBlynk();
  initializeDisplay();

  // Configura el pin del botón con la resistencia pull-up interna
  pinMode(pinBoton, INPUT_PULLUP);
}

void loop() {
  Blynk.run();
  timer.run();
  leerBoton();
}

void initializeMotorsAndBlynk(){
  Serial.begin(9600);

  pinMode(IN1_A,OUTPUT); pinMode(IN2_A,OUTPUT); pinMode(IN3_A,OUTPUT); pinMode(IN4_A,OUTPUT);
  pinMode(IN1_B,OUTPUT); pinMode(IN2_B,OUTPUT); pinMode(IN3_B,OUTPUT); pinMode(IN4_B,OUTPUT);

  resetHorarios(horariosA_derecha);
  resetHorarios(horariosA_izquierda);
  resetHorarios(horariosB_derecha);
  resetHorarios(horariosB_izquierda);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(60000L, checkSchedule);   // revisa cada minuto
  timer.setInterval(1000L, showTime);         // actualiza el LCD cada segundo
}

void initializeDisplay() {
    // Inicializa el LCD
    lcd.init();
    // Enciende la luz de fondo
    lcd.backlight();
}

// Resetea horarios
void resetHorarios(Horario arr[]) {
  for(int i=0; i<6; i++){
    arr[i].hora = -1;
    arr[i].minuto = -1;
  }
}

BLYNK_CONNECTED() {
  rtc.begin();
  Serial.println("RTC iniciado");
}

// --- V1 a V6 ---> Motor A derecha
// --- V7 a V12 ---> Motor A izquierda
// --- V13 a V18 ---> Motor B derecha
// --- V19 a V24 ---> Motor B izquierda

BLYNK_WRITE_DEFAULT() {
  int pin = request.pin;
  int idx = (pin-1) % 6; // índice 0-5

  TimeInputParam t(param);

  if (!t.hasStartTime()) {
    Serial.println("Horario borrado");
    return;
  }

  int h = t.getStartHour();
  int m = t.getStartMinute();

  if(pin >=1 && pin <=6){
    horariosA_derecha[idx] = {h,m};
    Serial.printf("Motor A derecha [%d] %02d:%02d\n",idx,h,m);
  }
  if(pin >=7 && pin <=12){
    horariosA_izquierda[idx] = {h,m};
    Serial.printf("Motor A izquierda [%d] %02d:%02d\n",idx,h,m);
  }
  if(pin >=13 && pin <=18){
    horariosB_derecha[idx] = {h,m};
    Serial.printf("Motor B derecha [%d] %02d:%02d\n",idx,h,m);
  }
  if(pin >=19 && pin <=24){
    horariosB_izquierda[idx] = {h,m};
    Serial.printf("Motor B izquierda [%d] %02d:%02d\n",idx,h,m);
  }
}

// FUNCIÓN DE MOVIMIENTO MOTOR A
void moverA(bool derecha){
  for(int i=0; i<200; i++){
    if(derecha){
      digitalWrite(IN1_A,HIGH); digitalWrite(IN2_A,LOW); digitalWrite(IN3_A,LOW); digitalWrite(IN4_A,LOW); delay(4);
      digitalWrite(IN1_A,LOW); digitalWrite(IN2_A,HIGH); delay(4);
      digitalWrite(IN2_A,LOW); digitalWrite(IN3_A,HIGH); delay(4);
      digitalWrite(IN3_A,LOW); digitalWrite(IN4_A,HIGH); delay(4);
    } else {
      digitalWrite(IN4_A,HIGH); digitalWrite(IN3_A,LOW); digitalWrite(IN2_A,LOW); digitalWrite(IN1_A,LOW); delay(4);
      digitalWrite(IN4_A,LOW); digitalWrite(IN3_A,HIGH); delay(4);
      digitalWrite(IN3_A,LOW); digitalWrite(IN2_A,HIGH); delay(4);
      digitalWrite(IN2_A,LOW); digitalWrite(IN1_A,HIGH); delay(4);
    }
  }
  apagarMotores();
}

// FUNCIÓN DE MOVIMIENTO MOTOR B
void moverB(bool derecha){
  for(int i=0; i<200; i++){
    if(derecha){
      digitalWrite(IN1_B,HIGH); digitalWrite(IN2_B,LOW); digitalWrite(IN3_B,LOW); digitalWrite(IN4_B,LOW); delay(4);
      digitalWrite(IN1_B,LOW); digitalWrite(IN2_B,HIGH); delay(4);
      digitalWrite(IN2_B,LOW); digitalWrite(IN3_B,HIGH); delay(4);
      digitalWrite(IN3_B,LOW); digitalWrite(IN4_B,HIGH); delay(4);
    } else {
      digitalWrite(IN4_B,HIGH); digitalWrite(IN3_B,LOW); digitalWrite(IN2_B,LOW); digitalWrite(IN1_B,LOW); delay(4);
      digitalWrite(IN4_B,LOW); digitalWrite(IN3_B,HIGH); delay(4);
      digitalWrite(IN3_B,LOW); digitalWrite(IN2_B,HIGH); delay(4);
      digitalWrite(IN2_B,LOW); digitalWrite(IN1_B,HIGH); delay(4);
    }
  }
  apagarMotores();
}

// APAGAR TODOS
void apagarMotores(){
  digitalWrite(IN1_A,LOW); digitalWrite(IN2_A,LOW); digitalWrite(IN3_A,LOW); digitalWrite(IN4_A,LOW);
  digitalWrite(IN1_B,LOW); digitalWrite(IN2_B,LOW); digitalWrite(IN3_B,LOW); digitalWrite(IN4_B,LOW);
}

// REVISAR HORARIOS CADA MINUTO
void checkSchedule(){
  int h = hour();
  int m = minute();

  for(int i=0;i<6;i++){
    if(horariosA_derecha[i].hora == h && horariosA_derecha[i].minuto == m){
      moverA(true);
      mensajePendiente = true;
      Blynk.logEvent("horario_pastilla", "Pastilla 1 lista");
    }
    if(horariosA_izquierda[i].hora == h && horariosA_izquierda[i].minuto == m){
      moverA(false);
      mensajePendiente = true;
      Blynk.logEvent("horario_pastilla", "Pastilla 2 lista");
    }
    if(horariosB_derecha[i].hora == h && horariosB_derecha[i].minuto == m){
      moverB(true);
      mensajePendiente = true;
      Blynk.logEvent("horario_pastilla", "Pastilla 3 lista");
    }
    if(horariosB_izquierda[i].hora == h && horariosB_izquierda[i].minuto == m){
      moverB(false);
      mensajePendiente = true;
      Blynk.logEvent("horario_pastilla", "Pastilla 4 lista");
    }
  }
}

void showTime() {
  int h = hour();
  int m = minute();

  // Lógica de la Línea 0 (Mensaje)
  if (mensajePendiente) {
    lcd.setCursor(0, 0);
    lcd.print("Retirar Pastilla");
  } else {
    // Si no hay mensaje, limpia la línea 0
    lcd.setCursor(0, 0);
    lcd.print("                "); // 16 espacios
  }

  lcd.setCursor(6, 1); // Columna 6, Fila 1
  if (h < 10) lcd.print('0'); // Añade un cero delante si es menor de 10
  lcd.print(h);
  lcd.print(':');
  if (m < 10) lcd.print('0'); // Añade un cero delante si es menor de 10
  lcd.print(m);
}

void leerBoton() {
  bool lecturaActual = digitalRead(pinBoton);

  if (lecturaActual != estadoBotonAnterior) {
    ultimoDebounce = millis();
  }

  if ((millis() - ultimoDebounce) > delayDebounce) {
    if (lecturaActual != estadoBotonAnterior) {
      estadoBotonAnterior = lecturaActual;

      // Actuar SÓLO cuando se PRESIONA (transición de HIGH a LOW)
      if (lecturaActual == LOW) {
        // Si se presiona el botón, bajamos la bandera
        if (mensajePendiente) {
          Serial.println("Botón presionado. Borrando mensaje.");
          mensajePendiente = false;
        }
      }
    }
  }
}