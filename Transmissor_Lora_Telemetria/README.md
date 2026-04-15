# 📡 Monitoramento de Reservatórios via LoRa - Nodo Transmissor

Este projeto faz parte de um sistema de telemetria industrial para monitoramento de nível de água e controle de bombas em tempo real.<br>
Utiliza módulos LoRa e ESP32 para comunicação e controle.

---

## 📋 Descrição do Funcionamento

O código gerencia um **Nodo Transmissor (Bloco 1)** com as seguintes funcionalidades:

- 🔹 **Leitura de Nível**  
Leitura da pressão através de um pressostato (entrada analógica), convertida em porcentagem (0 a 100%)

- 🔹 **Sensor de Boia**  
Monitora o estado do nível crítico de segurança

- 🔹 **Prioridade Inteligente**  
Se a boia indicar nível baixo (estado 1) → a bomba liga automaticamente<br>
Caso contrário → a bomba segue comando manual via rádio (BOMBA_ON / BOMBA_OFF)

- 🔹 **Comunicação Bidirecional**  
Envia dados a cada 5 segundos<br>
Recebe comandos remotos<br>
Responde com confirmação (ACK)

---

## 🛠️ Hardware Utilizado

- **Microcontrolador:** ESP32  
- **Comunicação:** Módulo LoRa (Serial2)

### Sensores
- Pressostato → GPIO 36 (Analógico)  
- Boia de nível → GPIO 15 (Digital)

### Atuadores (Relés)
- Relé do Sistema → GPIO 14  
- Relé da Bomba → GPIO 12  

---

## 📌 Pinagem (Pinout)

| Componente      | Pino ESP32 | Função                         |
|----------------|-----------|--------------------------------|
| LoRa RX        | 16        | Recebimento de dados rádio     |
| LoRa TX        | 17        | Transmissão de dados rádio     |
| Pressostato    | 36        | Sensor de pressão (0–4095)     |
| Boia           | 15        | Nível crítico de segurança     |
| Relé Sistema   | 14        | Ativação geral do painel       |
| Relé Bomba     | 12        | Acionamento da bomba           |

---

## 📡 Protocolo de Mensagens

O sistema utiliza comunicação via Serial com mensagens formatadas:

### 🔹 Envio de Dados

DADOS;ID_BLOCO;NIVEL;BOIA


### 🔹 Recebimento de Comando

CMD;ID_MSG;ID_BLOCO;ACAO


### 🔹 Resposta (ACK)

ACK;ID_MSG;OK


---

## 🚀 Como usar

1. Instale as bibliotecas necessárias (Arduino IDE ou PlatformIO)  
2. Configure o módulo LoRa para **9600 baud**  
3. Carregue o código no ESP32  
4. Abra o Monitor Serial em **115200 baud** para acompanhar o funcionamento  

---