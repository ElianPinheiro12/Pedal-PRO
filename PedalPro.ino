#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Configuração do ponto de acesso
const char *ssid = "PedalPro";
const char *password = "PedalPro";

// Pino do sensor de voltas
const int Voltas = 2;
int contador = 0;
unsigned long tempoUltimaVolta = 0;
float distanciaPorVolta = 2.35; // metros
float velocidade = 0.0;
unsigned long tempoUltimaLeitura = 0;  // Para controlar o tempo de leitura da velocidade

// Variáveis para cálculo de calorias
const float pesoUsuario = 70.0; // Peso em kg (ajuste conforme necessário)
const float METS = 6.8; // METS para ciclismo moderado
float caloriasQueimadas;
unsigned long inicioExercicio = 0;
bool emMovimento = false;
unsigned long tempoUltimaCaloria = 0;  // Controle para evitar incremento excessivo

// Servidor web
ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(Voltas, INPUT);

  WiFi.softAP(ssid, password);
  Serial.print("Endereço IP do AP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", []() {
    String html = R"rawliteral(
      <!DOCTYPE html>
      <html lang="pt-BR">
      <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>PedalPro - Dashboard</title>
        <style>
          * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: 'Arial', sans-serif;
          }
          body {
            background: linear-gradient(to bottom, #001F3F, #003366);
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            color: white;
          }
          .dashboard {
            width: 90%;
            max-width: 350px;
            background: rgba(255, 255, 255, 0.15);
            backdrop-filter: blur(15px);
            border-radius: 15px;
            padding: 20px;
            box-shadow: 0px 10px 30px rgba(0, 0, 0, 0.3);
            text-align: center;
          }
          h1 {
            font-size: 22px;
            margin-bottom: 15px;
            text-transform: uppercase;
            letter-spacing: 1px;
          }
          .info {
            font-size: 20px;
            margin: 8px 0;
            padding: 12px;
            border-radius: 10px;
            background: rgba(255, 255, 255, 0.2);
            box-shadow: inset 0 0 10px rgba(255, 255, 255, 0.1);
          }
          .info span {
            font-weight: bold;
            font-size: 24px;
            color: #FFD700; /* Dourado para destaque */
          }
          @keyframes fadeIn {
            from { opacity: 0; transform: translateY(-10px); }
            to { opacity: 1; transform: translateY(0); }
          }
          .info { animation: fadeIn 0.5s ease-in-out; }
        </style>
      </head>
      <body>
        <div class="dashboard">
          <h1>Monitoramento de Ciclismo</h1>
          <p class="info">Velocidade: <span id="velocidade">0</span> km/h</p>
          <p class="info">Distância: <span id="distancia">0</span> km</p>
          <p class="info">Voltas: <span id="contador">0</span></p>
          <p class="info">Calorias: <span id="calorias">0</span> kcal</p>
        </div>

        <script>
          function atualizarDados() {
            fetch('/velocidade').then(r => r.text()).then(d => document.getElementById('velocidade').innerText = d);
            fetch('/distancia').then(r => r.text()).then(d => document.getElementById('distancia').innerText = d);
            fetch('/leitura').then(r => r.text()).then(d => document.getElementById('contador').innerText = d);
            fetch('/calorias').then(r => r.text()).then(d => document.getElementById('calorias').innerText = d);
          }
          setInterval(atualizarDados, 1000);
        </script>
      </body>
      </html>
    )rawliteral";
    server.send(200, "text/html", html);
  });

  server.on("/leitura", []() { server.send(200, "text/plain", String(contador)); });
  server.on("/velocidade", []() { server.send(200, "text/plain", String(velocidade, 2)); });
  server.on("/distancia", []() { server.send(200, "text/plain", String((contador * distanciaPorVolta) / 1000.0, 2)); });
  server.on("/calorias", []() { server.send(200, "text/plain", String(caloriasQueimadas, 2)); });

  server.begin();
  Serial.println("Servidor web iniciado.");
}

void detectarVolta() {
  static bool sensorAtivado = false;
  int valor = digitalRead(Voltas);

  // Detecta uma volta
  if (valor == LOW && !sensorAtivado && (millis() - tempoUltimaVolta > 50)) {
    contador++;
    tempoUltimaVolta = millis();

    // Calcula o tempo entre as voltas e a velocidade
    unsigned long tempoEntreVoltas = millis() - tempoUltimaLeitura;
    if (tempoEntreVoltas > 0) {
      velocidade = (distanciaPorVolta / tempoEntreVoltas) * 1000.0 * 3.6; // Velocidade em km/h (distância / tempo)
    }

    // Marca que o exercício começou
    if (!emMovimento) {
      inicioExercicio = millis();
      emMovimento = true; 
    }

    sensorAtivado = true;
    tempoUltimaLeitura = millis(); // Atualiza o tempo da última leitura
  } else if (valor == HIGH) {
    sensorAtivado = false;
  }

  // Verifica se o tempo sem voltas excedeu 5 segundos
  if (millis() - tempoUltimaVolta > 5000) {
    emMovimento = false;  // Define como "parado" se 5 segundos sem volta
  }
}

void calcularCalorias() {
  if (emMovimento) {
    // Incrementa calorias com base no tempo de exercício
    unsigned long tempoAtual = millis();
    unsigned long deltaTempo = tempoAtual - tempoUltimaCaloria;

    if (deltaTempo >= 1000) {  // Incrementa as calorias a cada segundo
      // Calcula as calorias queimadas
      float tempoHoras = deltaTempo / 3600000.0;  // Convertendo para horas
      caloriasQueimadas += METS * pesoUsuario * tempoHoras;  // Acumula calorias queimadas

      tempoUltimaCaloria = tempoAtual;  // Atualiza o tempo de cálculo das calorias
    }
  }
}

// Reduz a velocidade caso as voltas parem
void reduzirVelocidade() {
  if (millis() - tempoUltimaVolta > 3000 && !emMovimento) {
    velocidade *= 0.98;  // Reduz a velocidade suavemente
    if (velocidade <= 0) velocidade = 0;  // Garante que a velocidade não fique negativa
  }
}

void loop() {
  server.handleClient();
  detectarVolta();
  calcularCalorias();
  reduzirVelocidade();
  delay(10);
}
