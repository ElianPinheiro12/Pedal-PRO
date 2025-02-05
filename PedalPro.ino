#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Configuração do ponto de acesso
const char *ssid = "PedalPro";
const char *password = "PedalPro";

// Pino de entrada para o sensor de voltas
const int Voltas = 2;  // Pino onde o sensor está conectado
int contador = 0;
unsigned long tempoUltimaVolta = 0; // Para evitar contagens múltiplas
float distanciaPorVolta = 2.31; // Distância de uma volta em metros
float velocidade = 0.0; // Velocidade em metros por segundo

// Variáveis para calcular a média
unsigned long tempoAcumulado = 0;
int voltasAcumuladas = 0;
float velocidadeMedia = 0.0;

// Criação do servidor web na porta 80
ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);

  // Configura o pino de Voltas como entrada
  pinMode(Voltas, INPUT);

  // Configura o ESP como ponto de acesso
  Serial.println("Configurando o ponto de acesso...");
  WiFi.softAP(ssid, password);

  // Exibe o IP do ponto de acesso
  Serial.print("Endereço IP do AP: ");
  Serial.println(WiFi.softAPIP());

  // Rota principal (página HTML)
  server.on("/", []() {
    String html = "<!DOCTYPE html>";
    html += "<html>";
    html += "<head><title>Contador de Voltas e Velocímetro</title><meta charset=\"UTF-8\"></head>";
    html += "<body><h1>Contador de Voltas e Velocímetro</h1>";
    html += "<p id='contador'>Voltas: 0</p>";
    html += "<p id='velocidade'>Velocidade: 0 km/h</p>";
    html += "<script>";
    html += "function atualizarContador() {";
    html += "  fetch('/leitura')"; 
    html += "  .then(response => response.text())";
    html += "  .then(data => {";
    html += "    document.getElementById('contador').innerText = 'Voltas: ' + data;";
    html += "  });";
    html += "  fetch('/velocidade')"; 
    html += "  .then(response => response.text())";
    html += "  .then(data => {";
    html += "    document.getElementById('velocidade').innerText = 'Velocidade: ' + data + ' km/h';";
    html += "  });";
    html += "}";  
    html += "setInterval(atualizarContador, 200);"; 
    html += "</script>";
    html += "</body>";
    html += "</html>";

    server.send(200, "text/html", html);
  });

  // Rota para retornar o valor das voltas
  server.on("/leitura", []() {
    server.send(200, "text/plain", String(contador));
  });

  // Rota para retornar o valor da velocidade
  server.on("/velocidade", []() {
    unsigned long tempoDecorrido = millis() - tempoUltimaVolta;
    if (tempoDecorrido > 0 && contador > 0) {
      float distancia = contador * distanciaPorVolta; 
      float velocidadeMs = distancia / (tempoDecorrido / 1000.0); 
      velocidade = velocidadeMs * 3.6; 
    }

    tempoAcumulado += tempoDecorrido;
    voltasAcumuladas++;
    if (voltasAcumuladas > 5) {  
      velocidadeMedia = (contador * distanciaPorVolta) / (tempoAcumulado / 1000.0) * 3.6;
      tempoAcumulado = 0;
      voltasAcumuladas = 0;
    }

    server.send(200, "text/plain", String(velocidadeMedia, 2));
  });

  // Inicia o servidor
  server.begin();
  Serial.println("Servidor web iniciado.");
}

void detectarVolta() {
  static bool sensorAtivado = false; // Armazena se o sensor já estava ativado
  int valor = digitalRead(Voltas);

  if (valor == LOW && !sensorAtivado) { // Se o sensor detectar o imã e ainda não foi contado
    if (millis() - tempoUltimaVolta > 500) { // Garante um intervalo de pelo menos 500ms
      contador++; 
      tempoUltimaVolta = millis(); 
      sensorAtivado = true; 
    }
  } else if (valor == HIGH) { // Reseta quando o imã sai do sensor
    sensorAtivado = false;
  }
}

void loop() {
  server.handleClient();
  detectarVolta();
  delay(5); // Reduz o uso da CPU
}
