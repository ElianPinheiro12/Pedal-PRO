#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Configuração do ponto de acesso
const char *ssid = "PedalPro";
const char *password = "PedalPro";

// Pino do sensor de voltas
const int Voltas = 2;
int contador = 0;
unsigned long tempoUltimaVolta = 0;
float distanciaPorVolta = 2.31; // metros
float velocidade = 0.0;

// Variáveis para cálculo de calorias
const float pesoUsuario = 70.0; // Peso em kg (ajuste conforme necessário)
const float METS = 6.8; // METS para ciclismo moderado
float caloriasQueimadas = 0.0;
unsigned long inicioExercicio = 0;
bool emMovimento = false;

// Servidor web
ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(Voltas, INPUT);

  WiFi.softAP(ssid, password);
  Serial.print("Endereço IP do AP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/velocidade", []() { server.send(200, "text/plain", String(velocidade, 1)); });
  server.begin();
  Serial.println("Servidor web iniciado.");
}

void calcularVelocidade(unsigned long tempoEntreVoltas) {
  if (tempoEntreVoltas > 100) {
    velocidade = (distanciaPorVolta / (tempoEntreVoltas / 1000.0)) * 3.6; // Convertendo para km/h
  }
}

void reduzirVelocidadeGradualmente() {
  if (millis() - tempoUltimaVolta > 2000) { // Se passaram 2 segundos sem novas voltas
    velocidade *= 0.98; // Redução de 2% a cada iteração para suavizar a queda
    if (velocidade < 0.1) velocidade = 0; // Evita valores muito pequenos
  }
}

void detectarVolta() {
  static bool sensorAtivado = false;
  int valor = digitalRead(Voltas);

  if (valor == LOW && !sensorAtivado) {
    unsigned long tempoAtual = millis();
    unsigned long tempoEntreVoltas = tempoAtual - tempoUltimaVolta;

    if (tempoEntreVoltas > 50) {
      contador++;
      tempoUltimaVolta = tempoAtual;
      calcularVelocidade(tempoEntreVoltas);
    }
    sensorAtivado = true;
  } else if (valor == HIGH) {
    sensorAtivado = false;
  }
}

void loop() {
  server.handleClient();
  detectarVolta();
  reduzirVelocidadeGradualmente();
  delay(50);
}
