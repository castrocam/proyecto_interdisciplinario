#define BLYNK_TEMPLATE_ID "TMPL2NiUWdbIU"
#define BLYNK_TEMPLATE_NAME "Pastillero"
#define BLYNK_AUTH_TOKEN "Sn2XCwbcd1JWb4JSs4iMzZelZzWC5Plz"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

// --- CONFIGURACIÓN DE TU WIFI ---
char ssid[] = "Dormitorio de Ina";
char pass[] = "RU16763856";

BlynkTimer timer;
WidgetRTC rtc;

// --- PINES DEL MOTOR ULN2003 ---
#define IN1 D0
#define IN2 D3
#define IN3 D4
#define IN4 D5

// Variables para guardar el horario programado
int horaProgramada = -1;
int minutoProgramado = -1;

int horaProgramada2 = -1;
int minutoProgramado2 = -1;

BLYNK_CONNECTED() {
  rtc.begin();
  Serial.println("RTC iniciado. Esperando horario desde la app...");
}

// Recibe horarios desde la app Blynk
BLYNK_WRITE(V1) {
  TimeInputParam horario(param);

  if (horario.hasStartTime()) {
    horaProgramada = horario.getStartHour();
    minutoProgramado = horario.getStartMinute();

    Serial.print("Horario recibido desde la app: ");
    Serial.print(horaProgramada);
    Serial.print(":");
    Serial.println(minutoProgramado);
  } else {
    Serial.println("No hay horario configurado.");
    horaProgramada = -1;
    minutoProgramado = -1;
  }
}

BLYNK_WRITE(V2) {
  TimeInputParam horario(param);

  if (horario.hasStartTime()) {
    horaProgramada2 = horario.getStartHour();
    minutoProgramado2 = horario.getStartMinute();

    Serial.print("Horario 2 (izquierda) recibido: ");
    Serial.print(horaProgramada2);
    Serial.print(":");
    Serial.println(minutoProgramado2);
  } else {
    Serial.println("No hay horario 2 configurado.");
    horaProgramada2 = -1;
    minutoProgramado2 = -1;
  }
}


// Secuencia simple para girar el motor paso a paso
void girarMotorDerecha() {
  Serial.println("Girando motor a la derecha...");
  for (int i = 0; i < 200; i++) {
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); digitalWrite(IN3, LOW); digitalWrite(IN4, LOW); delay(5);
    digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); delay(5);
    digitalWrite(IN2, LOW);  digitalWrite(IN3, HIGH); delay(5);
    digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); delay(5);
  }
  apagarMotor();
}

// 🔄 Giro a la izquierda
void girarMotorIzquierda() {
  Serial.println("Girando motor a la izquierda...");
  for (int i = 0; i < 200; i++) {
    digitalWrite(IN4, HIGH); digitalWrite(IN3, LOW); digitalWrite(IN2, LOW); digitalWrite(IN1, LOW); delay(5);
    digitalWrite(IN4, LOW);  digitalWrite(IN3, HIGH); delay(5);
    digitalWrite(IN3, LOW);  digitalWrite(IN2, HIGH); delay(5);
    digitalWrite(IN2, LOW);  digitalWrite(IN1, HIGH); delay(5);
  }
  apagarMotor();
}

// Apaga el motor
void apagarMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Chequea la hora actual y compara con la programada
void checkSchedule() {
  int h = hour();
  int m = minute();

  Serial.print("Hora actual: ");
  Serial.print(h);
  Serial.print(":");
  Serial.println(m);

  // Comparar con horario 1 (derecha)
  if (horaProgramada1 != -1 && minutoProgramado1 != -1) {
    if (h == horaProgramada1 && m == minutoProgramado1) {
      Serial.println("⏩ Coincidencia con horario 1 → girar derecha");
      girarMotorDerecha();
    }
  }

  // Comparar con horario 2 (izquierda)
  if (horaProgramada2 != -1 && minutoProgramado2 != -1) {
    if (h == horaProgramada2 && m == minutoProgramado2) {
      Serial.println("⏪ Coincidencia con horario 2 → girar izquierda");
      girarMotorIzquierda();
    }
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  apagarMotor();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(60000L, checkSchedule); // revisa cada minuto
}

void loop() {
  Blynk.run();
  timer.run();
}

