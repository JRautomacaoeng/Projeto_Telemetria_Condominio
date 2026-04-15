# 🌐 Gateway Industrial LoRa para MQTT (ESP32)

Este repositório contém o código do **Gateway Central**, responsável por conectar os nodos de campo (transmissores) à internet.<br>
O sistema recebe dados via rádio LoRa, publica em um servidor MQTT e permite o envio de comandos remotos.

---

## 🚀 Funcionalidades Principais

- 🔹 **Redundância de Wi-Fi**  
Utiliza a biblioteca WiFiMulti para conexão automática com múltiplas redes (principal e reserva), evitando perda de conexão

- 🔹 **Bridge LoRa ↔ MQTT**  
Converte pacotes recebidos via LoRa em publicações MQTT<br>
Publica no tópico: `predio/status/blocoX`<br>
Escuta comandos no tópico: `predio/comando/#` e retransmite via LoRa

- 🔹 **Sistema de Retry (Retentativa)**  
Caso o comando não seja confirmado (ACK), o Gateway tenta reenviar automaticamente até 5 vezes

- 🔹 **Identificação Única**  
Gera IDs baseados em `millis()` para garantir que cada comando seja único e rastreável

---

## 🛠️ Hardware e Conexões

- **Microcontrolador:** ESP32  
- **Comunicação:** Módulo LoRa (Serial2)  
- **Pinos:**  
  - RX → GPIO 16  
  - TX → GPIO 17  
- **Protocolo de Internet:** MQTT (Broker HiveMQ)

---

## 📌 Configuração de Software

Para utilizar o projeto, instale as seguintes bibliotecas no seu ambiente:

1. PubSubClient (Nick O'Leary)  
2. WiFi (nativa do ESP32)  
3. WiFiMulti  

---

## ⚙️ Configuração de Rede

No código, configure suas redes Wi-Fi:

```cpp
wifiMulti.addAP("NOME_DA_REDE", "SENHA");

📡 Fluxo de Dados
🔹 1. Entrada (MQTT)

O Gateway recebe comandos no formato:
ID_BLOCO:ACAO
Ex: 1:BOMBA_ON
``` id="y3g0x8"

---

### 🔹 2. Processamento
O comando é convertido para o formato interno:

CMD;ID;BLOCO;ACAO


---

### 🔹 3. Saída (LoRa)
O comando é enviado via rádio para o nodo correspondente

---

### 🔹 4. Feedback (ACK)
Se receber confirmação (ACK) → comando concluído<br>
Se não receber → nova tentativa após 2 segundos (até 5 vezes)

---