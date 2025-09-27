#define BLYNK_TEMPLATE_ID "TU_TEMPLATE_ID"
#define BLYNK_DEVICE_NAME "Pastillero"
#define BLYNK_AUTH_TOKEN "TU_AUTH_TOKEN"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

// Credenciales WiFi
char ssid[] = "TU_WIFI";
char pass[] = "TU_PASS";

BlynkTimer timer;
WidgetRTC rtc;

// Pines de los motores (pueden cambiar según tu circuito)
#define MOTOR_A_DERECHA D1
#define MOTOR_A_IZQUIERDA D2
#define MOTOR_B_DERECHA D3
#define MOTOR_B_IZQUIERDA D4

// Variables para horarios
TimeInputParam horarios(V1);  // V1 = widget de entrada de tiempo

BLYNK_CONNECTED() {
  rtc.begin();
}

// Recibe horarios desde Blynk
BLYNK_WRITE(V1) {
  if (param.hasStartTime() && param.hasStopTime()) {
    Serial.println("Horarios configurados:");
    for (int i = 0; i < 7; i++) {
      if (param.isWeekdaySelected(i)) {
        Serial.print("Dia ");
        Serial.print(i);
        Serial.print(": ");
        Serial.print(param.getStartHour());
        Serial.print(":");
        Serial.print(param.getStartMinute());
        Serial.print(" - ");
        Serial.print(param.getStopHour());
        Serial.print(":");
        Serial.println(param.getStopMinute());
      }
    }
  } else {
    Serial.println("No hay horarios configurados.");
  }
}

// Chequea cada minuto si coincide la hora
void checkSchedule() {
  if (horarios.hasStartTime() && horarios.isWeekdaySelected(weekday() - 1)) {
    int hora = hour();
    int minuto = minute();

    if (hora == horarios.getStartHour() && minuto == horarios.getStartMinute()) {
      Serial.println("¡Hora de activar motores!");

      // Ejemplo: alternar entre pastillas
      // Aquí decides qué motor activar
      activarPastilla(1);  // Podés llamar a 1, 2, 3 o 4 según corresponda
    }
  }
}

// Función para activar motores según pastilla
void activarPastilla(int pastilla) {
  apagarMotores();  // seguridad

  switch (pastilla) {
    case 1: // Pastilla 1 → Motor A derecha
      digitalWrite(MOTOR_A_DERECHA, HIGH);
      delay(2000);  // tiempo de giro
      break;

    case 2: // Pastilla 2 → Motor A izquierda
      digitalWrite(MOTOR_A_IZQUIERDA, HIGH);
      delay(2000);
      break;

    case 3: // Pastilla 3 → Motor B derecha
      digitalWrite(MOTOR_B_DERECHA, HIGH);
      delay(2000);
      break;

    case 4: // Pastilla 4 → Motor B izquierda
      digitalWrite(MOTOR_B_IZQUIERDA, HIGH);
      delay(2000);
      break;
  }
  apagarMotores();
}

// Apaga todos los motores
void apagarMotores() {
  digitalWrite(MOTOR_A_DERECHA, LOW);
  digitalWrite(MOTOR_A_IZQUIERDA, LOW);
  digitalWrite(MOTOR_B_DERECHA, LOW);
  digitalWrite(MOTOR_B_IZQUIERDA, LOW);
}

void setup() {
  Serial.begin(9600);

  pinMode(MOTOR_A_DERECHA, OUTPUT);
  pinMode(MOTOR_A_IZQUIERDA, OUTPUT);
  pinMode(MOTOR_B_DERECHA, OUTPUT);
  pinMode(MOTOR_B_IZQUIERDA, OUTPUT);
  apagarMotores();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(60000L, checkSchedule);  // cada minuto chequea horarios
}

void loop() {
  Blynk.run();
  timer.run();
}
