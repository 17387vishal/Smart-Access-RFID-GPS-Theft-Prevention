#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32  

#define SS_PIN 10
#define RST_PIN 9

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

SoftwareSerial gpsSerial(RXPin, TXPin);
TinyGPSPlus gps;

// Define SIM800 module's TX and RX pins
static const int simTX = 7;  // SIM800 TX pin connected to Arduino
static const int simRX = 8;  // SIM800 RX pin connected to Arduino
SoftwareSerial sim800(simRX, simTX);

const String phoneNumber = "+1234567890";  // Replace with the recipient's phone number

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

const int relay = 2;
bool theftMode = false;

// Function declarations
void gpsreader();
void displaydata();
void sendSMS(String message);
void sendLocationSMS();
void checkIncomingSMS();
void RFID_Reader();


// Function to read GPS location and display it
void gpsreader() {
  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();
    if (gps.encode(c) && gps.location.isUpdated()) {
      displaydata();
    }
  }
}

// RFID Reader function
void RFID_Reader() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();

  // Array of authorized card UIDs
  String authorizedUIDs[] = {"F5 37 54 0C", "CD 03 07 85"}; //Replace the RFID UIDS

  bool accessGranted = false;
  for (int i = 0; i < sizeof(authorizedUIDs) / sizeof(authorizedUIDs[0]); i++) {
    if (content.substring(1) == authorizedUIDs[i]) {
      accessGranted = true;
      break;
    }
  }

  if (accessGranted) {
    Serial.println("Authorized access");
    digitalWrite(relay, HIGH);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.println("ACCESSED");
    display.display();
    delay(1000);
  } else {
    Serial.println("Access denied");
    digitalWrite(relay, LOW);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.println("DENIED!!");
    display.display();
    delay(1000);
  }
}

// Function to check incoming SMS and handle commands
void checkIncomingSMS() {
  if (sim800.available()) {
    String incomingSMS = sim800.readString();
    Serial.println("Incoming SMS: " + incomingSMS);

    // Check for specific commands in the incoming SMS
    if (incomingSMS.indexOf("CURRENT LOCATION") >= 0) {
      sendLocationSMS();  // Send location if "CURRENT LOCATION" is received
    }
    else if (incomingSMS.indexOf("ACTIVATE THEFT MODE") >= 0) {
      theftMode = true;  // Activate theft mode
      sendSMS("Theft mode activated. Sending location...");
      digitalWrite(relay, LOW);  // Turn off relay in theft mode
    }
    else if (incomingSMS.indexOf("DEACTIVATE THEFT MODE") >= 0) {
      theftMode = false;  // Deactivate theft mode
      sendSMS("Theft mode deactivated.");
      digitalWrite(relay, HIGH);  // Turn on relay when theft mode is deactivated
    }
  }
}

// Function to send the current GPS location via SMS
void sendLocationSMS() {
  String lat = String(gps.location.lat(), 6);
  String lng = String(gps.location.lng(), 6);
  String message = "Current Location:\nLatitude: " + lat + "\nLongitude: " + lng + "\n";
  message += "Google Maps: https://www.google.com/maps?q=" + lat + "," + lng;
  sendSMS(message);
}

// Function to send SMS with GPS location
void sendSMS(String message) {
  sim800.println("AT+CMGS=\"" + phoneNumber + "\"");  // Send command to send SMS
  delay(1000);
  sim800.println(message);  // The SMS message
  delay(100);
  sim800.write(26);  // ASCII code of CTRL+Z to indicate SMS end
  delay(5000);
  Serial.println("SMS sent successfully");
}


// Function to display data on the OLED screen
void displaydata() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  String lat = String(gps.location.lat(), 6);
  String lng = String(gps.location.lng(), 6);

  // Display Latitude and Longitude
  display.setCursor(0, 0);
  display.println(F("Latitude:"));
  display.setCursor(60, 0);
  display.println(lat);
  
  display.setCursor(0, 10);
  display.println(F("Longitude:"));
  display.setCursor(60, 10);
  display.println(lng);

  display.display();
}


void setup() {
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);
  SPI.begin();
  mfrc522.PCD_Init();
  delay(4);

  // Initialize GSM SIM800
  sim800.begin(9600);  // SIM800 baud rate
  Serial.println("Initializing GSM module...");
  sim800.println("AT");  // Send AT command to check if SIM800 is responding
  delay(1000);
  sim800.println("AT+CMGF=1");  // Set SMS mode to text
  delay(1000);
  sim800.println("AT+CNMI=1,2,0,0,0"); // Enable unsolicited incoming SMS notification
  delay(1000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);  // Infinite loop on failure
  }

  display.clearDisplay();
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);  // Ensure relay is off initially
}

void loop() {
  gpsreader();
  RFID_Reader();
  checkIncomingSMS();  // Check for incoming SMS
  if (theftMode) {
    sendLocationSMS(); // If theft mode is on, continuously send location
    digitalWrite(relay, LOW);  // Disable relay in theft mode
    delay(5000);  // Send location every 5 seconds
  }
}






