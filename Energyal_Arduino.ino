#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SocketIoClient.h>

/*const char* ssid = "APRENDIZ";
const char* password = "Dvbew89*UBuc70%RCuc95=";*/

const char* ssid = "Melo"; // Nombre de la red Wifi
const char* password = "12345678";// Contraseña de la red WIfi
SocketIoClient webSocket; // Instacia de socketClient

float Sensibilidad=0.100; //sensibilidad en V/A para nuestro sensor
float offset=0.065; // Equivale a la amplitud del ruido
String serial = "2";
int interval =5; // Tiempo de duracion de  las mediciones 

void setup () {
  pinMode(2, OUTPUT);
  Serial.print("Hili");
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  digitalWrite(2, HIGH);    // turn the LED off by making the voltage LOW        
    delay(1000);
    Serial.print("Connecting..");
   digitalWrite(2, LOW);   // turn the LED on (HIGH is the voltage level)
  }
  Serial.print("Connect");
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW   

  webSocket.begin("192.168.137.1",3000); // Inicio de la conecion con el servidor
}
void loop() {
float Ip = 0;
  for(int i = 0 ; i< interval ; i++){
    Ip += get_corriente();
    delay(50);
  }
 /* float Ip=get_corriente();//obtenemos la corriente pico*/
  float Irms=(Ip/interval)*0.707; //Intensidad RMS = Ipico/(2^1/2) 
  String post = "";
    post += "meter="+String(serial);
    post += "&irms="+String(Irms);
   post += "&interval="+String(interval);
 if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
   WiFiClient client;
  HTTPClient Post;
  if( client, Post.begin("http://192.168.137.1:3000/measurement") ){ 
    Post.addHeader("Content-Type", "application/x-www-form-urlencoded");
    Post.POST(post);
    String payload = Post.getString();
    Post.end();
    Serial.println("0"); 
  }else {
    Serial.println("No pudo establecer conexion con el servidor");
  }
   
  }  else {
     Serial.println("No wifi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(2, HIGH);    // turn the LED off by making the voltage LOW     
    Serial.print(".");
    delay(100); 
    digitalWrite(2, LOW);   // turn the LED on (HIGH is the voltage level)
    yield();
  }
  }
}

float get_corriente()
{
  float voltajeSensor;
  float corriente=0;
  long tiempo=millis();
  float Imax=0;
  float Imin=0;
  while(millis()-tiempo < 1000)//realizamos mediciones durante 0.5 segundos
  { 
    voltajeSensor = analogRead(A0) * (3.3/ 1023.0);//lectura del sensor
    corriente=0.9*corriente+0.1*((voltajeSensor-1.74)/Sensibilidad); //Ecuación  para obtener la corriente
    if(corriente>Imax)Imax=corriente;
    if(corriente<Imin)Imin=corriente;
    yield();
  }
  return(((Imax-Imin))-offset);
}
