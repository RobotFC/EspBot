/*
  Código usado com o controlador do Esp

  Based in WifiTernetToSerial
  Autor: João Campos

*/
#include <ESP8266WiFi.h>
#include <Hbridge.h>
//how many clients should be able to tcp to this ESP8266
#define MAX_SRV_CLIENTS 1
// Dados do robô que está em modo station
const char* ssid     = "robotFC";
const char* password = "robotFC1234";

const int wifiPort = 23;

WiFiServer server(wifiPort);
WiFiClient serverClients[MAX_SRV_CLIENTS];
// En12, driver 1 e driver2
Hbridge motor1(15, 5,0);
// En34, driver 3 e driver 4
Hbridge motor2(0, 12, 1);

int motorLeftSpeed = 0;
int motorRightSpeed = 0;
int direction = 0;
int gun1 = 0;
int gun2 = 0;



void setup() {
  Serial.begin(115200);
  // Configurar ip fixo no ESP
  IPAddress ip(192, 168, 1, 5); // where xx is the desired IP Address
  IPAddress gateway(192, 168, 1, 1); // set gateway to match your network
  Serial.print(F("Setting static ip to : "));
  Serial.println(ip);
  IPAddress subnet(255, 255, 255, 0);
  // set subnet mask to match your network
  bool configAP = WiFi.softAPConfig(ip, gateway, subnet);
  if(!configAP){
    Serial.print("Config AP falhou");
  }


  // configurar wifi como access point
  bool AP = WiFi.softAP(ssid, password);
  if(!AP){
    Serial.print("AP falhou para iniciar");
  }

  Serial1.print(WiFi.localIP());
  Serial1.println(" Running in port 501");
}

void loop() {
  uint8_t i;
  //Checa se tem alguns clientes querendo se conectar
  if (server.hasClient()){
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      //find free/disconnected spot
      if (!serverClients[i] || !serverClients[i].connected()){
        if(serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        continue;
      }
    }
    //no free/disconnected spot so reject
    WiFiClient serverClient = server.available();
    serverClient.stop();
  }


  //Checa se os clientes estão com dados
  for(i = 0; i < MAX_SRV_CLIENTS; i++){
    if (serverClients[i] && serverClients[i].connected()){
      if(serverClients[i].available()){
        //get data from the scada and push it to the UART
        while(serverClients[i].available()){
            // chamar função para ler conteudo
            // ler o contéudo do controlador até ao caracter terminal
        String command = serverClients[i].readStringUntil("\n");
        // passar como referência
        handleProtocol(&command.c_str());

        }
      }
    }
  }

    //Função comentada, se quiser mandar algum conteudo de volta é só usar write
    // para o client
  /*  for(i = 0; i < MAX_SRV_CLIENTS; i++){
      if (serverClients[i] && serverClients[i].connected()){
        serverClients[i].write(sbuf, len);
        delay(1);
      }
    }
  }*/
}


void handleProtocol(char * msg){
  char * pch;
  pch = strtok (msg, ";");
  int i = 0;
  while (pch != NULL)
  {
    if(i==0){
      motorLeftSpeed = atoi(pch);
    }
    if(i==1){
      motorRightSpeed = atoi(pch);
    }
    if(i==2){
      direction = atoi(pch);
    }
    if(i==3){
      gun1 = atoi(pch);
    }
    if(i==4){
      gun1 = atoi(pch);
    }

    i++;
    pch = strtok (NULL, ";");
  }
  motor1.setSpeed(motorLeftSpeed);
  motor2.setSpeed(motorRightSpeed);


  if(direction){
    // se ambas os motores rodarem no mesmo sentido irão para  frente
    //  dependendo da disposição dos motores
    motor1.turnLeft();
    motor2.turnLeft();

  }else{
    motor1.turnRight();
    motor2.turnRight();
  }




}
