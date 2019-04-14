# IOT-Planet-Monitoring-Project
   
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
