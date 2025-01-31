#include <ESP8266WiFi.h>  // Use <WiFi.h> para ESP32
#include <ESP8266WebServer.h>  // Use <WebServer.h> para ESP32

// Configuração do ponto de acesso
const char *ssid = "PedalPro";    // Nome do AP
const char *password = "PedalPro"; // Senha do AP

// comando vindo do site para o esp
//exemplo: ligar um led a partir site
const int ledPin = 4; // GPIO2 (D4 na maioria dos ESP8266)
int sensor =2;
int contador = 0;

//entrada do sensor
// exemplo: Configuração do pino do potenciômetro
const int potPin = A0; // Pino analógico do potenciômetro (A0 no ESP8266)

ESP8266WebServer server(80); // Criação do servidor web na porta 80

void setup() {
  Serial.begin(115200);

  // Configura o pino do LED como saída
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // LED inicia desligado

  pinMode(sensor, INPUT);


  // Configura o ESP como ponto de acesso
  Serial.println("Configurando o ponto de acesso...");
  WiFi.softAP(ssid, password);

  // Exibe o IP do ponto de acesso
  Serial.print("Endereço IP do AP: ");
  Serial.println(WiFi.softAPIP());

  // Rota para ligar o LED
  server.on("/ligar", []() {
    digitalWrite(ledPin, HIGH); // Liga o LED
    server.send(200, "text/plain", "LED ligado");
  });

  // Rota para desligar o LED
  server.on("/desligar", []() {
    digitalWrite(ledPin, LOW); // Desliga o LED
    server.send(200, "text/plain", "LED desligado");
  });

  // Rota para ler o valor do potenciômetro
  server.on("/leitura", []() {
    int valor = digitalRead(sensor); // Lê o valor do 
    if(valor == 0){
      contador = contador + 1;
    }
    String response = "Valor do sensor: " + String(contador);
    server.send(200, "text/plain", response);
  });

  // Rota principal com a página HTML
  server.on("/", []() {
    const char* html = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Velocímetro e Contador de Voltas</title>
    <style>
        body {
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            height: 100vh;
            background-color: #222;
            color: white;
            font-family: Arial, sans-serif;
        }
        canvas {
            background-color: black;
            border-radius: 50%;
        }
        .contador {
            margin-top: 20px;
            font-size: 24px;
        }
        button {
            margin: 10px;
            padding: 10px;
            font-size: 18px;
            cursor: pointer;
        }
        .timer {
            margin-top: 20px;
            font-size: 20px;
        }
    </style>
</head>
<body>
    <canvas id="velocimetro" width="200" height="200"></canvas>
    <div class="contador">
        Voltas: <span id="voltas">0</span>
        <br>
        Voltas por minuto: <span id="vpm">0</span>
    </div>
    <div class="timer">
        Tempo: <span id="timer">00:00</span>
    </div>
    <button onclick="incrementarVolta()">Adicionar Volta</button>
    <button onclick="resetarVoltas()">Resetar</button>

    <script>
        let voltas = 0;
        let velocidade = 0;
        let voltasPorMinuto = 0;
        let inicioTempo = Date.now();
        let ultimoVpm = 0; // Para comparar o VPM anterior e detectar diminuição
        let timerInterval;

        function desenharVelocimetro(valor) {
            const canvas = document.getElementById("velocimetro");
            const ctx = canvas.getContext("2d");
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            
            // Fundo
            ctx.fillStyle = "#444";
            ctx.beginPath();
            ctx.arc(100, 100, 90, 0, 2 * Math.PI);
            ctx.fill();
            
            // Indicador
            ctx.strokeStyle = "red";
            ctx.lineWidth = 5;
            ctx.beginPath();
            const angulo = (valor / 100) * Math.PI - Math.PI / 2;
            ctx.moveTo(100, 100);
            ctx.lineTo(100 + 70 * Math.cos(angulo), 100 + 70 * Math.sin(angulo));
            ctx.stroke();
            
            // Texto
            ctx.fillStyle = "white";
            ctx.font = "20px Arial";
            ctx.fillText(`${valor} km/h`, 60, 110);
        }

        function incrementarVolta() {
            voltas++;
            document.getElementById("voltas").textContent = voltas;
            atualizarVelocidade();
            calcularVPM();
        }
        
        function resetarVoltas() {
            voltas = 0;
            document.getElementById("voltas").textContent = voltas;
            document.getElementById("vpm").textContent = 0;
            document.getElementById("timer").textContent = "00:00";
            velocidade = 0;
            voltasPorMinuto = 0;
            inicioTempo = Date.now();
            ultimoVpm = 0; // Resetando o último VPM
            desenharVelocimetro(velocidade);
            clearInterval(timerInterval); // Parar o timer
            iniciarTimer(); // Reiniciar o timer
        }
        
        function atualizarVelocidade() {
            velocidade = Math.min(100, voltas * 10);
            desenharVelocimetro(velocidade);
        }
        
        function calcularVPM() {
            let tempoDecorrido = (Date.now() - inicioTempo) / 60000; // em minutos
            voltasPorMinuto = tempoDecorrido > 0 ? (voltas / tempoDecorrido).toFixed(2) : 0;
            document.getElementById("vpm").textContent = voltasPorMinuto;

            // Verificar se o VPM diminuiu
            if (ultimoVpm > 0 && voltasPorMinuto < ultimoVpm) {
                resetarVoltas(); // Resetar tudo se VPM cair
            } else {
                ultimoVpm = voltasPorMinuto; // Atualizar último VPM
            }
        }
        
        function atualizarTimer() {
            const tempoDecorrido = Date.now() - inicioTempo;
            const minutos = Math.floor(tempoDecorrido / 60000);
            const segundos = Math.floor((tempoDecorrido % 60000) / 1000);
            const timeString = `${minutos < 10 ? '0' + minutos : minutos}:${segundos < 10 ? '0' + segundos : segundos}`;
            document.getElementById("timer").textContent = timeString;
        }

        function iniciarTimer() {
            timerInterval = setInterval(atualizarTimer, 1000); // Atualiza o timer a cada segundo
        }

        desenharVelocimetro(velocidade);
        iniciarTimer(); // Inicia o timer assim que a página é carregada
    </script>
</body>
</html>
)rawliteral";

    server.send(200, "text/html", html);
  });

  // Inicia o servidor
  server.begin();
  Serial.println("Servidor web iniciado.");
}

void loop() {
  server.handleClient(); // Trata requisições ao servidor

  int potValue = analogRead(potPin);
  int ima2 = digitalRead(sensor);
  
  // Envia o valor para o monitor serial
  Serial.print("Valor do sensor: ");
  Serial.println(ima2);

  // Pequena pausa para evitar muita poluição no monitor serial
  delay(100); // Atualiza a cada 100ms
}
