// Prueba de UN motor con ESP8266
// Cambiar pines según conexión

#define MOTOR_A D1  // Giro horario
#define MOTOR_B D2  // Giro antihorario

void setup() {
  Serial.begin(115200);

  pinMode(MOTOR_A, OUTPUT);
  pinMode(MOTOR_B, OUTPUT);

  Serial.println("Prueba de un motor iniciada");
}

void loop() {
  // Giro horario
  Serial.println("Motor horario");
  digitalWrite(MOTOR_A, HIGH);
  digitalWrite(MOTOR_B, LOW);
  delay(2000);

  // Parar
  Serial.println("Parado");
  digitalWrite(MOTOR_A, LOW);
  digitalWrite(MOTOR_B, LOW);
  delay(1000);

  // Giro antihorario
  Serial.println("Motor antihorario");
  digitalWrite(MOTOR_A, LOW);
  digitalWrite(MOTOR_B, HIGH);
  delay(2000);

  // Parar
  Serial.println("Parado");
  digitalWrite(MOTOR_A, LOW);
  digitalWrite(MOTOR_B, LOW);
  delay(2000);
}
