#include <DHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <time.h>

const char* ssid = "Enter your Login";
const char* password = "Enter Your password";
char auth[] = "Insert Blynk auth code"; // Blynk authorization key
const char* Status = "Unknown";
int timezone = 140;

#define DHTPIN 4 // D2 pin on Nodemcu
#define LAMPPIN 16 // D0 Pin for Lamp
#define PUMPPIN 5 //D1 Pin for Pump 
#define MOISTUREPIN A0 //A0 ADC pin for moisture sensor
#define MOISTUREVCC 0 // D3 pin on Nodemcu

DHT dht(DHTPIN, DHT22, 6);

   BLYNK_CONNECTED() {
    Blynk.syncAll();
}

 BLYNK_WRITE(V5) // To Switch Pump On
{
    int i=param.asInt();
    if (i==1) 
    {
        digitalWrite(5, HIGH);
        delay(5000);
        digitalWrite(5, LOW);
    }
    else 
    {
        digitalWrite(5, LOW);
        
    }
}   

// BLYNK_WRITE(V6) // To Switch Lamp On
//{
//    int i=param.asInt();
//    if (i==1) 
//    {
//        digitalWrite(16, HIGH);
//        delay(5000);
//       
//    }
//    else 
//    {
    
//        digitalWrite(16, LOW);
//    }
//}   


void setup() {                
  Serial.begin(115200);
  dht.begin();
  pinMode(LAMPPIN, OUTPUT);
  pinMode(PUMPPIN, OUTPUT);
  pinMode(MOISTUREPIN,INPUT);
  pinMode(MOISTUREVCC,OUTPUT);


// Connect WiFi
//  Blynk.begin(auth, ssid, password);
  
//Test Pump at Switch On
  digitalWrite(PUMPPIN,HIGH);
  delay(1000); // Run Pump for 1 Sec to ensure it is working
  digitalWrite(PUMPPIN,LOW);
// End Pump Test


}

void loop() {   
  
   Serial.print("Connecting to ");
   Blynk.begin(auth, ssid, password);
   while (Blynk.connect() == false) {
    //Wait until connected
     }
   Serial.println("WiFi connected");

   Blynk.run();        // Main function to run Blynk app
   delay(100);
  // Collect Time from Web
   configTime(2 * 3600, 0, "pool.ntp.org", "time.nist.gov"); // Collect Time from web
   Serial.println("\nWaiting for time");
   while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
   Serial.println("");
  // End of collecting time

  // Read Humidity and Temp
  float h = dht.readHumidity();
  float t = dht.readTemperature(false); //read temp in Celsius use True for Farenheit
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
// read Moisture Sensor
   digitalWrite(MOISTUREVCC,HIGH);
   delay(2000);

// read the input on analog pin 0:
  int SensorValue = 0;
  digitalWrite (MOISTUREVCC, HIGH); //Place voltage onto sensor
  delay (150);

    SensorValue = analogRead(A0);  // Read Sensor Value 
    delay(150);

    digitalWrite (MOISTUREVCC, LOW); // Remove voltage from Sensor
  
  //Sensor value of 700 might have to change once sensor is in the soil
   if (SensorValue < 720) {
      digitalWrite(PUMPPIN,HIGH); // Switch the Pump On
      delay(2000);
      digitalWrite(PUMPPIN,LOW); // Switch the Pump Off
}

 //Check if Temp is Above or Below 25 Deg
  Serial.println("checking Temp");
   if (t > 25) {
      digitalWrite(LAMPPIN,LOW);
      Serial.println("Lamp OFF");
      Status = "Lamp Off";
}
   else if (t < 25) { 
     digitalWrite(LAMPPIN,HIGH);
     Serial.println("Lamp ON");
     Status = "Lamp On";

}
//get time
     time_t now = time(nullptr);

// send results to Blynk
     Blynk.virtualWrite(V1, t); // Write Temperature to Blynk
     Blynk.virtualWrite(V2, h); // Write Humidity to Blynk
     Blynk.virtualWrite(V3, SensorValue); // Write depth of water to Blynk
     Blynk.virtualWrite(V4,(ctime(&now)));// Write Date and time to Blynk
     Blynk.virtualWrite(V7,Status); //Write Status of the Lamp to Blynk
     delay(100);

// Print out all results     
     Serial.println(ctime(&now));
     Serial.print("Temperature: ");
     Serial.print(t);
     Serial.print(" degrees Celcius Humidity: "); 
     Serial.println(h);
     Serial.print("Soil Moisture :");
     Serial.println(SensorValue);
     Serial.println("Waiting...");    

  // time between updates
  delay(12000); // 2 mins
}




