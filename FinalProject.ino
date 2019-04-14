  /*
   
                             ********************************************* 
                                       PLANT MONITORING PROJECT
                             ********************************************* 

   This is an ESP32 and ESP8266 project for monitoring plants.
   This is how it works : the DHT11 sensor absorbs the room temperature and humidity
   data and the UltraSonic sensor absorbs the distance between the sensor and the water
   in the water tank. All the data is uploaded to CloutMQTT and then transferred to MongoDB
   database via Node-Red.
   The Node red responsible for transferring the data from the database to BigML services 
   in order to analyze it and make predictions and models.
   Whenever the distance between the UltraSonic sensor and the water in the water tank reach 10 cm,
   (which means the water are about to end), the user will get an SMS to his smartphon (with the IFTTT services).
   Whenever the BigML predict the user should irrigate the plant, the user will get an email says 
   "Time to water the plant!". 
   
   Sources:
   SMS using IFTTT : https://circuitdigest.com/microcontroller-projects/sending-sms-using-esp8266
   Ultrasonic Code : https://www.instructables.com/id/Pocket-Size-Ultrasonic-Measuring-Tool-With-ESP32/
   DHT11 Code : https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-web-server-arduino-ide/
   MQTT publish and subscribe : https://techtutorialsx.com/2017/04/24/esp32-publishing-messages-to-mqtt-topic/
   
  */

  
  #include <WiFi.h>              //if using esp8266 replace it with <ESP8266WiFi.h>
  #include <PubSubClient.h>            
  #include <WiFiClientSecure.h>
  #include "DHT.h"
  
  #define DHTTYPE DHT11   
  
// Replace with your network credentials

  const char* ssid     = "";
  const char* password = "";

// Connecting to IFTTT for SMS service

// Replace with your IFTTT acount credentials
  const char* host = "";
  const int httpsPort = ;

  
// Replace with your CloudMQTT credentials

  const char* mqttServer = "";
  const int mqttPort = ;
  const char* mqttUser = "";
  const char* mqttPassword = "";

  int Bool = 0;  // This variable is for the SMS function

  
// DHT Sensor
  const int DHTPin = 16;  // you can change to the eps32 pin you want
  
// Defines pins numbers for lights red and green
  const int redLed = 19;
  const int greenLed = 18;
    
// Defines pins numbers for the UltraSonic sensor
  const int trigPin = 2;
  const int echoPin = 5;
  
// Initialize DHT sensor.
  DHT dht(DHTPin, DHTTYPE);

// Defines variables for the UltraSonic sensor
  long duration;
  int d; // distance
  

  WiFiClient espClient;
  PubSubClient client(espClient);

  void Send_Distance_SMS(){
    WiFiClientSecure client;
    Serial.print("connecting to ");
    Serial.println(host);
    if (!client.connect(host, httpsPort)) {
      Serial.println("connection failed");
      return;
    }

   String url = "";                        //Enter your IFTTT account url
    Serial.print("requesting URL: ");
    Serial.println(url);
  
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: BuildFailureDetectorESP8266\r\n" +
                 "Connection: close\r\n\r\n");
  
    Serial.println("request sent");
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    
    String line = client.readStringUntil('\n');
  
    Serial.println("reply was:");
    Serial.println("==========");
    Serial.println(line);
    Serial.println("==========");
    Serial.println("closing connection");
   } // END OF Send_Distance_SMS
 

 
    
// The callback function is executed when MQTT message is received

  void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  
//Checks if the subscribed topic is "temerature" or "distance"
        int strcomparison = strcmp(topic, "temerature");   // The strcmp() function compares two strings and returns 0 if both strings are identical.
          if (strcomparison == 0) {
           Serial.println("Matched temerature Topic");
               Send_Distance_SMS();
                             } //end If
          strcomparison = strcmp(topic, "distance");
         if (strcomparison == 0) {
           Serial.println("Matched distance Topic");
                             } //end If
          
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
 } //END OF callback


  
  void setup() {
    Serial.begin(115200);
    // Connecting to a WiFi network
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  
      client.setServer(mqttServer, mqttPort);
      client.setCallback(callback);

      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      

  
  //initialize the UltraSonic sensor
   pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
   pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
  //initialize the lights
   pinMode(redLed, OUTPUT);
   pinMode(greenLed, OUTPUT);
   
  // initialize the DHT sensor
      dht.begin();

  // subscribing 
      client.subscribe("temperature");
      client.subscribe("distance");

} //END OF SETUP


  
  void reconnect() {
    // Loop until we're reconnected
  
    Serial.println("In reconnect...");
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (client.connect("ESP32Client",mqttUser,mqttPassword)) {   //If using esp8266 change to 'if (client.connect("ESP8266Client",mqttUser,mqttPassword))'
        Serial.println("connected");
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
      }
    }
  } // END OF RECONNECT


  
  void loop() {
    if (!client.connected()) {
      reconnect();
    }
  
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculating the distance
              d = duration*0.034/2;

              float h = dht.readHumidity();
              // Read temperature as Celsius (the default)
              float t = dht.readTemperature();
              // Check if any reads failed and exit early (to try again).
              if (isnan(h) || isnan(t)) {
                Serial.println("Failed to read from DHT sensor!");
                return;    
              }
  
    String temperature = String(t);
    String humidity = String(h);
    String distance = String (d);


// If the water tank is about to end, an SMS will be sent to the user and the red light will turn on
    if(d > 10){
     digitalWrite(redLed, HIGH);
     digitalWrite(greenLed, LOW);


// If the user has already got SMS, then he won't get another one
      if(Bool = 0){ Send_Distance_SMS(); 
                    Bool = 1;  }
   
  }else{
     digitalWrite(redLed, LOW);
     digitalWrite(greenLed, HIGH);  
    }
    
  
 // Prints to Serial Monitor for debug
    Serial.print( "Sending temperature and humidity : [" );
    Serial.print( temperature ); Serial.print( "," );
    Serial.print( humidity );
    Serial.print( "]\n" );
    Serial.print( "Sending distance : [" );
    Serial.print( distance );
    Serial.print( "]\n" );
     
    
  // Prepare a JSON payload string for temerature
    String temerature_payload = "{";
      temerature_payload += "\"temperature\":";
      temerature_payload += temperature;
      temerature_payload += ",";
      temerature_payload += "\"humidity\":";
      temerature_payload += humidity; 
      temerature_payload += "}";
    
    // Prepare a JSON payload string for distance
    String distance_payload = "{";
    distance_payload += "\"distance\":"; 
      distance_payload += distance; 
      distance_payload += "}";
      
    // Send temerature payload
    char temperature_attributes[100];
    temerature_payload.toCharArray( temperature_attributes, 100 );
      client.publish( "temperature", temperature_attributes );

      // Send distance payload
    char distance_attributes[100];
    distance_payload.toCharArray( distance_attributes, 100 );
      client.publish( "distance", distance_attributes );
 
        
    delay(120000); // wait for 2 minutes   //  Arduino pauses your program during the delay()

    
  } // END OF LOOP
