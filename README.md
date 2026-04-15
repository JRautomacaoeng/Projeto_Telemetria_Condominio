# 📡 Sistema de Telemetria de Reservatórios via LoRa (ESP32)

Este projeto faz parte de um sistema de telemetria industrial para monitoramento de nível de água e controle de bombas em tempo real.<br>
Utiliza comunicação LoRa, ESP32 e integração com MQTT + Node-RED.

---

# 🧩 1. Nodo Transmissor (Bloco 1)

## 📋 Descrição do Funcionamento

O Nodo Transmissor é responsável pela coleta de dados e controle local:

- 🔹 **Leitura de Nível**  
Leitura de pressostato (entrada analógica) convertida em porcentagem (0 a 100%)

- 🔹 **Sensor de Boia**  
Monitoramento do nível crítico (digital)

- 🔹 **Prioridade Inteligente**  
Se a boia indicar nível baixo → bomba liga automaticamente<br>
Caso contrário → segue comando manual via rádio

- 🔹 **Comunicação Bidirecional**  
Envia dados a cada 5 segundos<br>
Recebe comandos remotos<br>
Responde com ACK

---

## 🛠️ Hardware Utilizado

**Microcontrolador:** ESP32<br>
**Comunicação:** Módulo LoRa (Serial2)

### Sensores
- Pressostato → GPIO 36 (Analógico)  
- Boia → GPIO 15 (Digital)

### Atuadores
- Relé do Sistema → GPIO 14  
- Relé da Bomba → GPIO 12  

---

## 📌 Pinagem

| Componente     | Pino | Função                  |
|---------------|------|-------------------------|
| LoRa RX       | 16   | Recebimento             |
| LoRa TX       | 17   | Transmissão             |
| Pressostato   | 36   | Analógico               |
| Boia          | 15   | Nível crítico           |
| Relé Sistema  | 14   | Ativação geral          |
| Relé Bomba    | 12   | Acionamento da bomba    |

---

## 📡 Protocolo de Comunicação

### Envio de Dados

DATA;ID_BLOCO;NIVEL;BOIA


### Recebimento de Comando

CMD;ID_MSG;ID_BLOCO;ACAO


### Resposta (ACK)

ACK;ID_MSG;OK


---

## 🚀 Como usar

1. Instale as bibliotecas necessárias  
2. Configure o LoRa para 9600 baud  
3. Envie o código para o ESP32  
4. Monitor Serial em 115200 baud  

---

# 🌐 2. Gateway LoRa → MQTT (ESP32)

Responsável por conectar os nodos à internet.

---

## 🚀 Funcionalidades

- 🔹 Redundância de WiFi (WiFiMulti)  
- 🔹 Comunicação LoRa ↔ MQTT  
- 🔹 Sistema de retry (até 5 tentativas)  
- 🔹 Identificação única por millis()  

---

## 🛠️ Hardware

- ESP32  
- Módulo LoRa (GPIO 16/17)  
- Broker MQTT (ex: HiveMQ)

---

## 📌 Bibliotecas

- PubSubClient  
- WiFi  
- WiFiMulti  

---

## ⚙️ Configuração

```cpp
wifiMulti.addAP("NOME_DA_REDE", "SENHA");
📡 Fluxo de Dados

Entrada MQTT

1:BOMBA_ON

Processamento

CMD;ID;BLOCO;ACAO

Saída LoRa
Envio via rádio

Confirmação
Se receber ACK → OK<br>
Se não → tenta novamente após 2 segundos

🖥️ 3. Interface Node-RED + MQTT

Interface de monitoramento e controle.

📤 Comandos (Node-RED → Gateway)

Tópico

predio/comando/#

Formato

ID_BLOCO:ACAO
Ações
Ação	Exemplo	Descrição
Ligar Sistema	1:SYS_ON	Liga painel
Desligar Sistema	1:SYS_OFF	Desliga painel
Ligar Bomba	1:BOMBA_ON	Liga bomba
Desligar Bomba	1:BOMBA_OFF	Desliga bomba
📥 Status (Gateway → Node-RED)

Tópico

predio/status/bloco1

Formato

DATA;ID_BLOCO;NIVEL;BOIA
⚙️ Função Node-RED
// Exemplo: DATA;1;85;0
var dados = msg.payload.split(';');

var nivel = parseInt(dados[2]);
var boia = parseInt(dados[3]);

return [
    { payload: nivel },
    { payload: boia }
];
✅ Resumo
📡 Comunicação LoRa
🌐 Integração MQTT
🖥️ Dashboard Node-RED
🔁 Sistema com ACK e retry
⚡ Controle automático e manual