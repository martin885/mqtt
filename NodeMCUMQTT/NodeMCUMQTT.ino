#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <ArduinoJson.h>

Servo myservo;

int contconexion=0;

const char *ssid="";
const char *password="";
char SERVER[]="mqtt.flespi.io";
int SERVERPORT=1883;
//String USERNAME= "use-token-auth"; //username del cliente creado en la nube para cloudmqtt, en maqiatto va nombre de usuario
char PASSWORD[]="";   //password del cliente creado en la nube para cloudmqtt, en maqiatto va password de usuario
char PLACA[]="";//username
char id[]="esp8266";

unsigned long previousMillis=0;

char charPulsador [15];
String strPulsador;
String strPulsadorUltimo;
String movimientoAnterior;
String movimientoActual;   



char valueStr[20];
String strtemp="";
char TEMPERATURA[50];
char PULSADOR[50];
char SALIDADIGITAL[50]="probando/digital";
char SALIDAANALOGICA[50]="probando/analogica";
char SALIDAMOVIMIENTO[50]="probando/movimiento";



String serial;
char serialChar[5];
StaticJsonDocument<50> jsonBuffer;

WiFiClient espClient;
PubSubClient client(espClient);

//-------------------------------------------CALLBACK---------------------------------------------------

void callback(char* topic,byte* payload, unsigned int length){
  char PAYLOAD[5]="";
  Serial.print("Mensaje Recibido: [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++){
    PAYLOAD[i]=(char)payload[i];
  }
  Serial.println(PAYLOAD);
  DeserializationError error =deserializeJson(jsonBuffer,payload);
  
  /*if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }*/

JsonObject root=jsonBuffer.as<JsonObject>();
  
 /* if(String(topic)==String(SALIDADIGITAL)){
    if(root["data"]=="N"){
      digitalWrite(2,HIGH);
    }
    if(root["data"]=="F"){
      digitalWrite(2,LOW);
    }
  }*/
    if(String(topic)==String(SALIDADIGITAL)){
    if(String(PAYLOAD)=="N"){
      digitalWrite(2,HIGH);
    }
    if(String(PAYLOAD)=="F"){
      digitalWrite(2,LOW);
    }
  }
else if(String(topic)==String(SALIDAANALOGICA)){
  myservo.write(String(PAYLOAD).toInt());
}
  if(String(topic)==String(SALIDAANALOGICA)){
    int valor=String(PAYLOAD).toInt();
    valor=map(valor,0,100,0,180);
    Serial.println(valor);
    analogWrite(13,valor);
  }
}

//----------------------------RECONNECT----------------------------------------------------------

void reconnect(){
  uint8_t retries=3;
  //Loop hasta que estamos conectados
  
  while(!client.connected()){
    Serial.print("Intentando conexión MQTT...");
    //Crea un ID de cliente al azar

    if(client.connect(id,PLACA,PASSWORD)){
      Serial.println("conectado");
      client.subscribe(SALIDADIGITAL);
      client.subscribe(SALIDAANALOGICA);
    }else{
      Serial.print("fallo, rc=");
      Serial.print(client.state());
      Serial.println(" intenta nuevamente en 5 segundos");
      //espera 5 segundos antes de reintentar
      delay(5000);
    }
 /*   retries--;
    if(retries==0){
      //esperar que el WDT lo reinicie
      while(1);
    }    */
  }
}

//---------------------------------------SETUP------------------------------------------------------------------

void setup() {   
//prepara GPI13 y 12 como salidas
pinMode(13,OUTPUT);//D7 salida analogica
analogWrite(13,0);
pinMode(2,OUTPUT);//D6 salida digital
digitalWrite(2,HIGH);
pinMode(D3,INPUT);
//Entradas
pinMode(14,INPUT);//D5

//Inicia Serial
Serial.begin(115200);


//Conexión WIFI
WiFi.begin(ssid,password);
while(WiFi.status()!=WL_CONNECTED and contconexion<50){//hace 50 intentos de conexión
  ++contconexion;
  delay(500);
  Serial.print(".");
 }
 if(contconexion<50){
  //para usar con ip fija
  IPAddress ip(192,168,0,11);
  IPAddress gateway(192,168,0,1);
  IPAddress subnet(255,255,255,0);
  WiFi.config(ip,gateway,subnet);

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println(WiFi.localIP());
 }
 else{
  Serial.println("");
  Serial.println("Error de conexión");
 }

 client.setServer(SERVER,SERVERPORT);
 client.setCallback(callback);

myservo.attach(13);



/* String temperatura ="/"+USERNAME+"/"+"temperatura";
 temperatura.toCharArray(TEMPERATURA,50);
 String pulsador ="/" + USERNAME + "/" + "pulsador";
 pulsador.toCharArray(PULSADOR,50);
 String salidaDigital= "/"+ USERNAME+"/"+"salidaDigital";
 salidaDigital.toCharArray(SALIDADIGITAL,50);
 String salidaAnalogica="/"+USERNAME+"/"+"salidaAnalogica";
 salidaAnalogica.toCharArray(SALIDAANALOGICA,50);*/

}

//---------------------------------------------------------------------LOOP----------------------------------------------

void loop() {
   if(!client.connected()){
    
    reconnect();
   }
   
   client.loop();

  unsigned long currentMillis = millis();
    
  /*if (currentMillis - previousMillis >= 10000) { //envia la temperatura cada 10 segundos
    previousMillis = currentMillis;
    int analog = analogRead(17);
    float temp = analog*0.322265625;
    strtemp = String(temp, 1); //1 decimal
    strtemp.toCharArray(valueStr, 15);
    Serial.println("Enviando: [" +  String(TEMPERATURA) + "] " + strtemp);
    client.publish(TEMPERATURA, valueStr);
  }*/
  
  if (digitalRead(14) == 0) {
    strPulsador = "presionado";
  } else {
    strPulsador = "NO presionado";
  }

  if(Serial.available()){
    serialChar[0]=Serial.read();
    String payload= "{\"data\":\"" + String(serialChar[0]) + "\"}";
    payload.toCharArray(valueStr,20);
    Serial.println("Enviando: [" +  String(SALIDADIGITAL) + "] " +valueStr);
    client.publish(SALIDADIGITAL, valueStr);
  }

if(digitalRead(D3)==1){
movimientoActual="Hay movimiento";
}else{
  movimientoActual="No Hay Movimiento";
}
if(movimientoActual!=movimientoAnterior){
  movimientoActual.toCharArray(valueStr,20);
  client.publish(SALIDAMOVIMIENTO, valueStr);
}
movimientoAnterior=movimientoActual;
/*  if (strPulsador != strPulsadorUltimo) { //envia el estado del pulsador solamente cuando cambia.
    strPulsadorUltimo = strPulsador;
    strPulsador.toCharArray(valueStr, 15);
    Serial.println("Enviando: [" +  String(PULSADOR) + "] " + strPulsador);
    client.publish(PULSADOR, valueStr);
  }*/
/*  String payload="{Martin: \"HOLA AMIGOS\"}";
  payload.toCharArray(valueStr,20);
if(client.publish(EVENTOIBM,valueStr)){
  Serial.println("Publicado");
  }
  else{
    Serial.println("No publicado");
  }*/
}
