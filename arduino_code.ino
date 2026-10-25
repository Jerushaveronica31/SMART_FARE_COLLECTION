#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define RST_PIN  22  
#define SS_PIN   21  

MFRC522 mfrc522(SS_PIN, RST_PIN);   

// Wi-Fi விவரங்கள்
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// உங்கள் Firebase Realtime Database URL
const String firebaseURL = "https://smart-fare-collection-default-rtdb.firebaseio.com/";

bool isCheckedIn = false; 

void setup() {
  Serial.begin(115200);
  SPI.begin();          
  mfrc522.PCD_Init();   

  // Wi-Fi இணைப்பு
  Serial.println();
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected successfully!");
  Serial.println("IP address: " + WiFi.localIP().toString());
  
  Serial.println("Smart Fare Collection System Ready!");
  Serial.println("Tap your RFID card to Check-in / Check-out.");
}

void loop() {
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
      return;
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;
  }

  Serial.print("Card UID: ");
  String currentUID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
      currentUID += String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      currentUID += String(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println(currentUID);

  // Check-in / Check-out லாஜிக் மற்றும் Firebase கிளவுட் சின்க்
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String statusStr = "";
    
    if (!isCheckedIn) {
        statusStr = "Check-in";
        Serial.println("Status: Check-in Successful!");
        isCheckedIn = true;
    } else {
        statusStr = "Check-out";
        Serial.println("Status: Check-out Successful!");
        isCheckedIn = false;
    }

    // Firebase-க்கு டேட்டாவை அனுப்புதல் (JSON வடிவில்)
    String url = firebaseURL + "/fare_logs.json";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    
    String payload = "{\"cardUID\":\"" + currentUID + "\",\"status\":\"" + statusStr + "\"}";
    int httpResponseCode = http.PUT(payload);

    if (httpResponseCode > 0) {
      Serial.println("Cloud Sync: Data successfully sent to Firebase! Response: " + String(httpResponseCode));
    } else {
      Serial.println("Cloud Sync Error: " + String(httpResponseCode));
    }
    http.end();
  }
  
  Serial.println("-----------------------------------");
  mfrc522.PICC_HaltA();
  delay(2000);
}