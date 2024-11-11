#include "WiFiEsp.h"            //WiFi library for ESP8266
#include "SoftwareSerial.h"     //SoftwareSerial library

SoftwareSerial Serial1(2, 3);   // RX, TX
#define ESP_BAUDRATE  9600      //ESP Baud Rate
char ssid[] = "JUN";            //Network SSID (name) 
char pass[] = "87654321";       //Network password
WiFiEspClient client;           //Set client as WiFi instance

//const int so that the pin won’t change after initialize
const int redPin = 13;
const int amberPin = 12;
const int greenPin = 11;
const int stopPin = 10;
const int walkPin = 9;
const int buzzerPin = 8;
const int buttonPin = 7;
const int txPin = 3;
const int rxPin = 2;
String line;

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(amberPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(stopPin, OUTPUT);
  pinMode(walkPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  
  digitalWrite(redPin,LOW);
  digitalWrite(amberPin,LOW);
  digitalWrite(greenPin,LOW);
  digitalWrite(stopPin,LOW);
  digitalWrite(walkPin,LOW);
  digitalWrite(buzzerPin,LOW);
  Serial.begin(9600);                           //Serial Monitor
  Serial1.begin(ESP_BAUDRATE);                  //Begin ESP8266 with the defined Baud Rate
  Serial.print("Searching for ESP8266...");     //Print Text on Serial Monitor
  WiFi.init(&Serial1);                          //Use Wifi library for Serial1(ESP8266)
  connectToWiFi();                   //Function to Connect ESP8266 to WiFi
}//setup end

void loop() {
  readPhone();                  //Read and set Phone value to line
  if (line == "1"){             //Check if line is 1
    longerTraffic();            //Longer Green Traffic Light
  }
  if (digitalRead(buttonPin) == HIGH){       //Check if Pedestrian Button is Pressed
    pedestrianCrossing();                    //Traffic Light change for pedestrian
  }
  normalTraffic();
}//loop end


void connectToWiFi() {
  if (WiFi.status() != WL_CONNECTED) {               //Check if Wifi is not connected
    Serial.print("Attempting to connect to SSID: "); //Serial Monitor Print Text
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED) {          //While WiFi is not connect
      WiFi.begin(ssid, pass);                        //Connect to Wifi with the ssid and pass set on top
      Serial.println(".");
      delay(500);     
    } 
    Serial.println("\nConnected");
  }
}//connectToWiFi end


void readPhone() {  //Read ThingSpeak, which is connected to phone button
  if (client.connect("api.thingspeak.com", 80)) {                         //Conect to ThingSpeak
    client.println("GET /channels/2406152/fields/1/last.txt HTTP/1.1");   //Send HTTP GET request to ThingSpeak
    client.println("Host: api.thingspeak.com");                           //Indicates who to request to
    client.println("Connection: close");                                  //Close connection after sending
    client.println();
    boolean headerReceived = false;                       //Set headerReceived to false
    while (client.connected()) {                          //Do this while ESP8266 is connected to ThingSpeak
      if (client.available()) {                           //Check if any data can be read from ThingSpeak
        line = client.readStringUntil('\n');       //Read line until '\n'
        if (!headerReceived) {                            //If headerReaceived is false
          if (line == "\r") {                             // Check if it's the end of headers
            headerReceived = true;                        //Set headerReaceived to true
          }
        } else {
          Serial.print("Phone Button State = ");          //Print Button State = {1 or 0}
          Serial.println(line);                           //Print line which is the last text of the request
        }
      }
    }
    client.stop();    //Close connection to ThingSpeak
  } else {            //Else print text
    Serial.println("Failed to connect to ThingSpeak");
  }
}//readLastTextValue end


void pedestrianCrossing() {  //Traffic Light change for pedestrian
  Serial.print("Pedestrian Button is Clicked!\n");
  delay(500);
  digitalWrite(greenPin,LOW);
  digitalWrite(amberPin,HIGH);    //Change Traffic Light to Amber
  
  readPhone();                    //Read and set Phone value to line using it as a delay
  if (line == "1"){               //Check if line is 1
    longerTraffic();              //Longer Green Traffic Light
  }
  digitalWrite(amberPin,LOW);
  digitalWrite(redPin,HIGH);      //Change Traffic Light to Red
  
  readPhone();                    //Read and set Phone value to line using it as a delay
  if (line == "1"){               //Check if line is 1
    longerTraffic();              //Longer Green Traffic Light
    digitalWrite(greenPin,LOW);
    digitalWrite(amberPin,HIGH);
    delay(5000);
  }
  
  digitalWrite(redPin,HIGH);
  digitalWrite(stopPin,LOW);
  digitalWrite(walkPin,HIGH);     //Turn Pedestrian Light to Green for 30 secs
  Serial.print("Pedestrian Light on!\n");
  
  for (int i = 0; i <= 6; i++){    //Buzzer slow beeping for 30 secs
    tone(buzzerPin, 100);
    delay(500);
    noTone(buzzerPin);
    readPhone();                    //Read and set Phone value to line using it as a delay
    if (line == "1"){               //Check if line is 1
      i = 30;                       //End for loop
      }
  }
  
  Serial.print("Binking!\n");
  for (int i = 0; i <= 20; i++){    //blink walk led and buzzer beeping for 10 secs
    digitalWrite(walkPin,LOW);
    noTone(buzzerPin);
    delay(250);
    digitalWrite(walkPin,HIGH);
    tone(buzzerPin, 255);
    delay(250);
  }
  noTone(buzzerPin);
  digitalWrite(walkPin,LOW);
  digitalWrite(stopPin,HIGH);
  delay(1000);
}//pedestrianCossing End


void normalTraffic() {  //Green Traffic light is on, everything else if off
  Serial.print("Normal Traffic\n");
  digitalWrite(greenPin,HIGH);
  digitalWrite(redPin,LOW);
  digitalWrite(amberPin,LOW);
  digitalWrite(stopPin,HIGH);
  digitalWrite(walkPin,LOW);
  digitalWrite(buzzerPin,LOW);
}

void longerTraffic() {  //Green Traffic light is on, everything else if off for longer
  Serial.print("Longer Traffic\n");
  digitalWrite(greenPin,HIGH);
  digitalWrite(redPin,LOW);
  digitalWrite(amberPin,LOW);
  digitalWrite(stopPin,HIGH);
  digitalWrite(walkPin,LOW);
  digitalWrite(buzzerPin,LOW);
  delay(10000);
}
