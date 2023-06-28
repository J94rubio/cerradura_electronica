#include <Wire.h>
//librerias para controlar pantalla OLED
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//libreria para conectar a WIFI
#include <WiFi.h>
//Libreria para el envio de mensaje Whatsapp
#include <HTTPClient.h>
// importa libreria Keypad
#include <Keypad.h>
//  importa libreria Servo
#include <ESP32Servo.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid ="Jrubio";
const char* password = "Jose940203";
//TOKEn generado por META
String token ="Bearer EAASFq9LaT4sBABBcanfgGgpFx8HF8WO2rryAovBDZA5A0ur0pTarJA9cFoAg5TBK4rs7fyDTAKLTg7X0xLM3Kc7Hgrc6YMwY2XFCxkiVnCI2TyEGLFHWUuKZBTO721mvKKIlJcF3HrKa5hPj5PxiM24pngm1Ke5yEDtioh3xTevjBEYoUX8kOvEYKPxO6x2tE6jQSdJgZDZD";
//URL a donde se envian los mensajes de Whatsapp
String servidor = "https://graph.facebook.com/v16.0/101205759667657/messages";
//Creamos un json donde se coloca el numero de telefono y el mensaje
String payload = "{ \"messaging_product\": \"whatsapp\", \"to\": \"573132142789\", \"type\": \"text\", \"text\": { \"body\": \"Alerta de seguridad INTENTO DE ACCESO CON CLAVE ERRADA\"}}";
// define numero de filas
const uint8_t ROWS = 4;
// define numero de columnas
const uint8_t COLS = 4;
// define la distribucion de teclas
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
// pines correspondientes a las filas
uint8_t colPins[COLS] = { 16, 3, 23, 15 };
// pines correspondientes a las columnas
uint8_t rowPins[ROWS] = { 19, 18, 5, 17 };
// crea objeto con los prametros creados previamente
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//Declaramos la variable que almacena el password
char pass[7]="0709A";
//Varible que alamacena la clave que se va ingresando
char ingresaPass[7];
//Variable que define el numero de letra que se va ingresando
int indice=0;
//Inicializamos la variable del pin del servo
int pinServo= 2;
//Inicializamos la variable del pin del buzzer
int buzzerPin = 4;

//Instaciamos el servo
Servo servo_1;

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();

  servo_1.attach(pinServo, 500, 2500);
  //Movemos el servo_1 a 90 grados
  servo_1.write(90);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Ingrese clave");
  display.display();
  //Esperamos 1.5 segundos
  delay(1500);

  WiFi.begin(ssid,password);
  Serial.println("Conectando");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado a Wifi con IP: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  
  // obtiene tecla presionada y asigna a variable
  char key = keypad.getKey();
  // comprueba que se haya presionado una tecla
  if (key) {
    //Alamacenamos la letra ingresada en arreglo de letras ingresaPass
    ingresaPass[indice]=key;
    //Aumentamos la letra que se va a alamacenar
    indice++;
    // envia a monitor serial la tecla presionada
    Serial.println(key);
  }
  //Si ya se ingresaron 6 letras o mas
  if(indice==5){
    //Si el password coincide con la clave ingresada
    if(strcmp(pass,ingresaPass)==0){
      // Mostramos el mensaje de acceso permitido
      Serial.println("Acceso permitido");
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("ACCESO CONCEDIDO");
      display.display();
      tone(buzzerPin, 1000, 500);  // Emitir sonido de acceso
      //delay(5000);
      //Movemos el servo_1 a 0 grados
      servo_1.write(0);
      //Esperamos 5 segundos
      delay(5000);
      //Movemos el servo_1 a 90 grados
      servo_1.write(90);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Ingrese clave");
      display.display();
      //Esperamos 1.5 segundos
      //delay(1500);

    }
    //Si el password NO coincide con la clave ingresada
    else{
      // Mostramos el mensaje de acceso denegado
      Serial.println("Acceso denegado");
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("ACCESO DENEGADO");
        display.display();
        tone(buzzerPin, 2000, 500);  // Emitir sonido de error
        delay(2000);
         display.clearDisplay();
         display.setCursor(0, 0);
         display.println("Ingrese clave");
         display.display();
      delay(3000);
      if(WiFi.status()== WL_CONNECTED){
        //Iniciamos el objeto HTTP que enviara el mensaje posteriormente
        HTTPClient http;
        //Url del servidor a donde se enviara el mensaje
        http.begin(servidor.c_str());
        //cabecera que indicamos que sera un tipo json
        http.addHeader("Content-Type", "application/json");
        //Agregamos el TOKEN en la cabecera de los datos a enviar
        http.addHeader("Authorization", token);
        //Enviamos los datos via POST
        int httpPostCode = http.POST(payload);
        //si se logran enviar los datos
        if(httpPostCode > 0) {
          //recibimos la respuesta que brinda META
          int httpResponseCode = http.GET();
          //Si hay respuesta la mostramos
          if(httpPostCode > 0) {
            Serial.print("HTTP response code: ");
            Serial.println(httpResponseCode);
            String payload = http.getString();
            Serial.println(payload);
          }
        }
        http.end();
      }   
      else{
        Serial.println("WIFI DESCONECTADO");
      }  
    }
    //Regresamos el indice a 0 para ingresar nuevas letras al password
    indice=0;
  }
}
