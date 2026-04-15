# 🖥️ Configuração do Fluxo Node-RED

Esta pasta contém o backup do fluxo utilizado no dashboard de monitoramento e controle dos reservatórios.

---

## 📁 Arquivos

- **fluxo_dashboard.json**  
Contém todos os nós, lógicas de tratamento de dados e interface gráfica (Dashboard)

---

## 📥 Como importar

1. Abra o Node-RED no navegador  
2. Clique no menu superior direito (ícone ☰)  
3. Selecione **Importar**  
4. Clique em **select a file to import**  
5. Escolha o arquivo `.json` desta pasta  
6. Clique em **Import**  
7. Clique em **Deploy** no canto superior direito  

---

## 🛠️ Dependências necessárias

Para o funcionamento correto do dashboard, instale os seguintes pacotes (via **Manage Palette**):

- **node-red-dashboard**  
Interface gráfica (gráficos, botões, medidores)

- **node-red-contrib-mqtt-broker**  
Necessário caso utilize broker MQTT local

- **node-red-telegrambot**  
Integração para envio de mensagens via Telegram

---

## ⚠️ Observação de Segurança

Por motivos de segurança, as credenciais de acesso ao broker MQTT foram removidas do arquivo JSON.<br>
Após importar o fluxo, configure manualmente o nó MQTT com os dados do seu servidor.

Exemplos:
- Broker público (HiveMQ)  
- Broker local (rede interna)

---