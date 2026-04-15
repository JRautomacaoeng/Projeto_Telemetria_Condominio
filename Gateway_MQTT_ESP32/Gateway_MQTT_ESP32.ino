#include <WiFi.h>
#include <WiFiMulti.h> // Biblioteca para gerenciar múltiplas redes
#include <PubSubClient.h>

// Instancia o objeto WiFiMulti
WiFiMulti wifiMulti;

const char* mqtt_server = "broker.hivemq.com";

// Pinos ajustados conforme seu teste de sucesso
#define PIN_RX 16 
#define PIN_TX 17

WiFiClient espClient;
PubSubClient client(espClient);
HardwareSerial loraSerial(2);

String comandoAtual = "";
String idMsgAtual = "";
String bufferLora = ""; 

unsigned long tempoEnvio = 0;
int tentativas = 0;

const int MAX_RETRY = 5;
const int TIMEOUT = 2000;

void setup_wifi() {
  delay(10);
  Serial.println("\nConfigurando WiFi...");

  // Adiciona as duas redes na lista de prioridades
  wifiMulti.addAP("TP-Link_AFF3", "87374711");
  wifiMulti.addAP("Jf", "Asenhaefacil");

  Serial.println("Conectando...");
  
  // Tenta conectar em uma das redes cadastradas
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi OK");
  Serial.print("Conectado em: ");
  Serial.println(WiFi.SSID()); // Mostra qual rede foi escolhida
}

String gerarID() {
  return String(millis());
}

void enviarComando(String bloco, String acao) {
  idMsgAtual = gerarID();
  comandoAtual = "CMD;" + idMsgAtual + ";" + bloco + ";" + acao;
  loraSerial.println(comandoAtual);
  tempoEnvio = millis();
  tentativas = 1;
  Serial.println("TX LORA CMD: " + comandoAtual);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) msg += (char)payload[i];
  Serial.println("MQTT RX: " + msg);

  int sep = msg.indexOf(':');
  if (sep != -1) {
    String bloco = msg.substring(0, sep);
    String acao = msg.substring(sep + 1);
    enviarComando(bloco, acao);
  }
}

void reconnect() {
  while (!client.connected()) {
    // Verifica o WiFi antes de tentar o MQTT
    if (wifiMulti.run() != WL_CONNECTED) {
      setup_wifi();
    }
    
    Serial.print("Tentando MQTT...");
    if (client.connect("Gateway_Industrial_Peruibe")) {
      client.subscribe("predio/comando/#");
      Serial.println("CONECTADO");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  loraSerial.begin(9600, SERIAL_8N1, PIN_RX, PIN_TX);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Serial.println("=== RECEPTOR INDUSTRIAL PRONTO ===");
}

void loop() {
  // O wifiMulti.run() no loop garante que se uma rede cair, 
  // ele tentará a outra automaticamente
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi Perdido. Reconectando...");
  } else {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
  }

  // --- Lógica de Retry LoRa ---
  if (comandoAtual != "" && millis() - tempoEnvio > TIMEOUT) {
    if (tentativas < MAX_RETRY) {
      loraSerial.println(comandoAtual);
      tentativas++;
      tempoEnvio = millis();
      Serial.println("RETRY " + String(tentativas) + " -> " + comandoAtual);
    } else {
      Serial.println("ERRO: Bloco nao respondeu ao comando.");
      comandoAtual = ""; 
    }
  }

  // --- Recebimento LoRa ---
  while (loraSerial.available()) {
    char c = loraSerial.read();
    if (c == '\n') {
      bufferLora.trim();
      if (bufferLora.length() > 0) {
        Serial.println("LORA RX RAW: " + bufferLora);

       if (bufferLora.startsWith("ACK;")) {
  int p1 = bufferLora.indexOf(';');
  int p2 = bufferLora.indexOf(';', p1 + 1);
  int p3 = bufferLora.indexOf(';', p2 + 1);

  if (p1 != -1 && p2 != -1 && p3 != -1) {

    String bloco = bufferLora.substring(p1 + 1, p2);
    String id = bufferLora.substring(p2 + 1, p3);

    if (id == idMsgAtual) {
      Serial.println("ACK CONFIRMADO DO BLOCO " + bloco);

      // 🔥 AQUI entra o tópico com bloco
      String topico = "predio/ack/bloco" + bloco + "/" + id;
      String payload = "OK";

      client.publish(topico.c_str(), payload.c_str());

      comandoAtual = "";
    }
  }
}
        else if (bufferLora.startsWith("DATA;")) {
          int p1 = bufferLora.indexOf(';');
          int p2 = bufferLora.indexOf(';', p1 + 1);
          if (p1 != -1 && p2 != -1) {
            String bloco = bufferLora.substring(p1 + 1, p2);
            String topico = "predio/status/bloco" + bloco;
            client.publish(topico.c_str(), bufferLora.c_str());
            Serial.println("MQTT PUBLICADO: " + topico);
          }
        }
      }
      bufferLora = ""; 
    } else {
      if (bufferLora.length() < 60) bufferLora += c;
    }
  }
}

