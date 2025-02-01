#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Configuração do ponto de acesso
const char *ssid = "PedalPro";
const char *password = "PedalPro";

// Pino de entrada para o sensor de voltas
const int Voltas = 2;  // Pino onde o sensor está conectado
int contador = 0;
unsigned long tempoUltimaVolta = 0; // Para o cálculo de tempo
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
    html += "  fetch('/leitura')"; // Faz a requisição para a rota '/leitura'
    html += "  .then(response => response.text())";
    html += "  .then(data => {";
    html += "    document.getElementById('contador').innerText = 'Voltas: ' + data;";
    html += "  });";
    html += "  fetch('/velocidade')"; // Faz a requisição para a rota '/velocidade'
    html += "  .then(response => response.text())";
    html += "  .then(data => {";
    html += "    document.getElementById('velocidade').innerText = 'Velocidade: ' + data + ' km/h';";
    html += "  });";
    html += "}";  
    html += "setInterval(atualizarContador, 200);"; // Atualiza a cada 200ms
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

  // Rota para retornar o valor da velocidade
  server.on("/velocidade", []() {
    // Calcula a velocidade
    unsigned long tempoDecorrido = millis() - tempoUltimaVolta; // Tempo desde a última volta (em ms)
    if (tempoDecorrido > 0 && contador > 0) {
      // Calcula a velocidade em metros por segundo
      float distancia = contador * distanciaPorVolta; // Distância total em metros
      float velocidadeMs = distancia / (tempoDecorrido / 1000.0); // Velocidade em m/s
      velocidade = velocidadeMs * 3.6; // Converte para km/h
    }
    
    // Calculando a média da velocidade para melhorar a precisão
    tempoAcumulado += tempoDecorrido;
    voltasAcumuladas++;
    if (voltasAcumuladas > 5) {  // Após 5 voltas, calculamos a média
      velocidadeMedia = (contador * distanciaPorVolta) / (tempoAcumulado / 1000.0) * 3.6;
      tempoAcumulado = 0;  // Reseta o tempo acumulado para a próxima média
      voltasAcumuladas = 0; // Reseta o contador de voltas
    }
    
    String response = String(velocidadeMedia, 2); // Retorna a média da velocidade formatada
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
    tempoUltimaVolta = millis(); // Marca o tempo da última volta
    delay(200);  // Debounce simples (evitar múltiplas contagens rápidas)
  }

  // Pequena pausa para evitar poluição no monitor serial
  delay(100);
}
