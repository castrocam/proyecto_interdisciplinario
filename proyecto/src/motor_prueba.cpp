#define BLYNK_TEMPLATE_ID "TMPL2NiUWdbIU"
#define BLYNK_TEMPLATE_NAME "Pastillero"
#define BLYNK_AUTH_TOKEN "Sn2XCwbcd1JWb4JSs4iMzZelZzWC5Plz"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

char ssid[] = "WifiVicky";
char pass[] = "louaylor";

BlynkTimer timer;
WidgetRTC rtc;

<<<<<<< HEAD
// --- PINES DEL MOTOR ULN2003 ---
#define IN1 D0
#define IN2 D3
#define IN3 D4
#define IN4 D5
=======
// Motor A (Primer motor)
#define IN1_A D0
#define IN2_A D3
#define IN3_A D4
#define IN4_A D5
>>>>>>> ebd707e (Actualizado para dos motores)

// Motor B (Segundo motor)
#define IN1_B D6
#define IN2_B D7
#define IN3_B D8
#define IN4_B RX 

// Estructura para almacenar horarios
struct Horario {
  int hora;
  int minuto;
};

Horario horariosA_derecha[6];
Horario horariosA_izquierda[6];
Horario horariosB_derecha[6];
Horario horariosB_izquierda[6];

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
    }
    if(horariosA_izquierda[i].hora == h && horariosA_izquierda[i].minuto == m){
      moverA(false);
    }
    if(horariosB_derecha[i].hora == h && horariosB_derecha[i].minuto == m){
      moverB(true);
    }
    if(horariosB_izquierda[i].hora == h && horariosB_izquierda[i].minuto == m){
      moverB(false);
    }
  }
}

void setup(){
  Serial.begin(9600);

  pinMode(IN1_A,OUTPUT); pinMode(IN2_A,OUTPUT); pinMode(IN3_A,OUTPUT); pinMode(IN4_A,OUTPUT);
  pinMode(IN1_B,OUTPUT); pinMode(IN2_B,OUTPUT); pinMode(IN3_B,OUTPUT); pinMode(IN4_B,OUTPUT);

  resetHorarios(horariosA_derecha);
  resetHorarios(horariosA_izquierda);
  resetHorarios(horariosB_derecha);
  resetHorarios(horariosB_izquierda);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(60000L, checkSchedule);
}

void loop(){
  Blynk.run();
  timer.run();
}
<<<<<<< HEAD

=======
>>>>>>> ebd707e (Actualizado para dos motores)
