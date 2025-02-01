#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Configuração do ponto de acesso
const char *ssid = "PedalPro";
const char *password = "PedalPro";

// Pino de entrada para o sensor de voltas
const int Voltas = 2;  // Pino onde o sensor está conectado
int contador = 0;

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
    html += "<head><title>Contador de Voltas</title><meta charset=\"UTF-8\"></head>";
    html += "<body><h1>Contador de Voltas</h1>";
    html += "<p id='contador'>Voltas: 0</p>";
    html += "<script>";
    html += "function atualizarContador() {";
    html += "  fetch('/leitura')"; // Faz a requisição para a rota '/leitura'
    html += "  .then(response => response.text())";
    html += "  .then(data => {";
    html += "    document.getElementById('contador').innerText = 'Voltas: ' + data;";
    html += "  });";
    html += "}";
    html += "setInterval(atualizarContador, 200);"; // Atualiza a cada 500ms
    html += "</script>";
    html += "</body>";
    html += "</html>";

    server.send(200, "text/html", html);
  });

  // Rota para retornar o valor das voltas
  server.on("/leitura", []() {
    String response = String(contador);
    server.send(200, "text/plain", response);
  });

  // Inicia o servidor
  server.begin();
  Serial.println("Servidor web iniciado.");
}

void loop() {
  server.handleClient(); // Trata requisições do servidor

  // Lê o valor do sensor a cada 100ms
  int valor = digitalRead(Voltas);
  if (valor == LOW) {  // Se o sensor indicar uma volta (LOW)
    contador++;  // Incrementa o contador
    delay(200);  // Debounce simples (evitar múltiplas contagens rápidas)
  }

  // Pequena pausa para evitar poluição no monitor serial
  delay(100);
}
