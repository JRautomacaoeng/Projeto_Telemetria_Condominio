#include <Arduino.h>

const int ID_BLOCO = 2; 

#define PIN_RX 16  
#define PIN_TX 17  
#define PIN_PRESSOSTATO 36 
#define PIN_BOIA 15
#define PIN_RELE_SISTEMA 14 
#define PIN_RELE_BOMBA 12   

HardwareSerial loraSerial(2); 

unsigned long ultimoEnvio = 0;
const unsigned long intervaloEnvio = 5000;
String comandoRaw = ""; 
String ultimoIDRecebido = "";

// Variável para armazenar o desejo do usuário via rádio
bool statusComandoManual = false; 

void setup() {
  Serial.begin(115200);
  loraSerial.begin(9600, SERIAL_8N1, PIN_RX, PIN_TX);

  pinMode(PIN_BOIA, INPUT);
  pinMode(PIN_RELE_SISTEMA, OUTPUT);
  pinMode(PIN_RELE_BOMBA, OUTPUT);

  digitalWrite(PIN_RELE_SISTEMA, HIGH);
  digitalWrite(PIN_RELE_BOMBA, LOW);

  Serial.println("=== TRANSMISSOR BLOCO " + String(ID_BLOCO) + " PRONTO ===");
}

void loop() {

  // --- LÓGICA DE CONTROLE DA BOMBA (Prioridades) ---
  int estadoBoia = digitalRead(PIN_BOIA);

  // Se a Boia for 1, liga a bomba independente do manual.
  // Se a Boia for 0, a bomba segue o que o comando manual mandou por último.
  if (estadoBoia == 1) {
    digitalWrite(PIN_RELE_BOMBA, HIGH);
  } else {
    // Se a boia é 0, o relé obedece ao comando manual (BOMBA_ON ou BOMBA_OFF)
    digitalWrite(PIN_RELE_BOMBA, statusComandoManual ? HIGH : LOW);
  }

  // --- RECEBE COMANDO ---
  while (loraSerial.available()) {
    char c = loraSerial.read();

    if (c == '\n') {
      comandoRaw.trim();
      Serial.println("RX RAW: " + comandoRaw);

      if (comandoRaw.startsWith("CMD;")) {
        int p1 = comandoRaw.indexOf(';');
        int p2 = comandoRaw.indexOf(';', p1 + 1);
        int p3 = comandoRaw.indexOf(';', p2 + 1);

        if (p1 != -1 && p2 != -1 && p3 != -1) {
          String idMsg = comandoRaw.substring(p1 + 1, p2);
          String blocoStr = comandoRaw.substring(p2 + 1, p3);
          String acao = comandoRaw.substring(p3 + 1);

          if (blocoStr.toInt() == ID_BLOCO) {
            if (idMsg != ultimoIDRecebido) {
              ultimoIDRecebido = idMsg;
              Serial.println("EXECUTANDO: " + acao);

              if (acao == "SYS_ON")    digitalWrite(PIN_RELE_SISTEMA, HIGH);
              if (acao == "SYS_OFF")   digitalWrite(PIN_RELE_SISTEMA, LOW);
              
              // Em vez de acionar o pino direto, alteramos a intenção manual
              if (acao == "BOMBA_ON")  statusComandoManual = true;
              if (acao == "BOMBA_OFF") statusComandoManual = false;
              
              delay(100); 
              loraSerial.println("ACK;" + String(ID_BLOCO) + ";" + idMsg + ";OK");
            }
          }
        }
      }
      comandoRaw = ""; 
    } else {
      if (comandoRaw.length() < 50) comandoRaw += c; 
    }
  }

  // --- TELEMETRIA ---
  if (millis() - ultimoEnvio > intervaloEnvio) {
    int leitura = analogRead(PIN_PRESSOSTATO);
    int nivel = map(leitura, 0, 4095, 0, 100);
    nivel = constrain(nivel, 0, 100);
    int boia = digitalRead(PIN_BOIA);

    String pacote = "DATA;" + String(ID_BLOCO) + ";" + String(nivel) + ";" + String(boia);
    loraSerial.println(pacote);

    Serial.print("TX DATA: " + pacote);
    Serial.println(statusComandoManual ? " (Manual: ON)" : " (Manual: OFF)");
    ultimoEnvio = millis();
  } 
}

