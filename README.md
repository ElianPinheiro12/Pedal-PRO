# 🚴‍♂️ Pedal-PRO

**SUPER: Pedal PRO - Project ICT - JR**  
Computador de bordo para bicicletas desenvolvido no âmbito do Projeto SUPER 2024 da Universidade Federal do Amazonas (UFAM).

---

## 📌 Descrição

O **Pedal-PRO** é um sistema embarcado de baixo custo para monitoramento de desempenho ciclístico. Desenvolvido como parte do projeto **SUPER 2024**, o objetivo é promover inovação tecnológica e incentivar a prática do ciclismo inteligente e seguro.

O dispositivo utiliza sensores para coletar dados em tempo real, fornecendo informações como:

- Velocidade
- Distância percorrida
- Tempo de percurso

## 🧠 Tecnologias Utilizadas

- **ESP-8266**
- **Sensor magnetico (Hall Effect)**
- **Desenvolvimento do aplicativo Android**
- **Bateria recarregável Li-ion**
- **Linguagem C/C++ (Arduino IDE)**

## Funcionamento Geral
O sistema utiliza um (a)sensor magnético instalado no garfo dianteiro da bicicleta e (b)ímãs fixados nos raios da roda dianteira. A passagem dos ímãs pelo sensor gera pulsos que são detectados pelo (c) microcontrolador ESP8266, responsável por processar os dados e transmiti-los via Wi-Fi para um aplicativo em um (d) smartphone Android. O aplicativo, além de exibir informações como velocidade e distância percorrida, também permite o envio de dados do usuário como: peso, altura e taxa metabólica, ao ESP8266, possibilitando uma análise mais personalizada do desempenho.

![unnamed](https://github.com/user-attachments/assets/430514cc-cac4-4105-89fc-9a0397417eb8)

## APLICATIVO ANDROID
O sistema PedalPro App é um aplicativo Android desenvolvido em Java, cuja estrutura de software é organizada de forma simples e funcional. A lógica principal está concentrada na MainActivity, que é responsável por inicializar a interface, configurar e exibir uma WebView com o conteúdo hospedado pelo microcontrolador ESP8266. O app utiliza SharedPreferences para armazenar o IP do dispositivo, permitindo uma conexão personalizada. Um menu de navegação com Toolbar oferece acesso a configurações e outras ações. A biblioteca OkHttp está integrada para futuras requisições HTTP, facilitando a comunicação com o hardware. A estrutura prioriza a praticidade, sendo adequada para aplicações embarcadas de controle e monitoramento via rede local.

![image](https://github.com/user-attachments/assets/eeab98d9-378c-48c3-bddd-ff90e0c9cd7c)

![image](https://github.com/user-attachments/assets/3ed62e8e-8d2a-4279-a035-54a49d244906)

## Autores

ELIAN DA ROCHA PINHEIRO @ElianPinheiro12 (https://github.com/ElianPinheiro12),

LUIZ VINICIUS COSTA REIS, 

ANA KAROLINE HIGINO LIMA
