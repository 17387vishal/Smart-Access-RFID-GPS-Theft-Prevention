# Smart RFID-Based Access Control with GPS Tracking and Theft Prevention via GSM

## Project Overview

This project integrates an RFID access control system with GPS tracking and theft prevention using a SIM800 GSM module. The system allows authorized users to access a secured area through RFID, while unauthorized access can trigger theft mode, which disables the attached relay and sends the current GPS location via SMS. Users can also request the current location by sending an SMS command to the module.

### Key Features

- **RFID Access Control:** Allows authorized users to unlock the system using RFID cards.
- **GPS Tracking:** Tracks the system's current location using the GPS module.
- **Theft Prevention Mode:** If theft mode is activated via SMS, the system sends the current location and turns off the relay to disable any connected device.
- **SMS Commands:** Users can request the system’s current location or activate/deactivate theft mode by sending specific SMS commands.
- **OLED Display:** Displays real-time latitude and longitude on an OLED screen.

---

## Components Used

- **Arduino UNO**
- **MFRC522 RFID Module**
- **SIM800 GSM Module**
- **Neo-6M GPS Module**
- **Relay Module**
- **Adafruit SSD1306 OLED Display**
- **RFID Cards**

---

## Hardware Connections

1. **RFID MFRC522:**

   - **SDA (SS_PIN):** Pin 10
   - **RST (RST_PIN):** Pin 9
   - **SCK:** Pin 13
   - **MOSI:** Pin 11
   - **MISO:** Pin 12
   - **GND:** Ground
   - **VCC:** 3.3V

2. **GPS Neo-6M:**

   - **TX:** Pin 4 (RX of Arduino)
   - **RX:** Pin 3 (TX of Arduino)
   - **GND:** Ground
   - **VCC:** 5V

3. **SIM800 GSM:**

   - **TX:** Pin 7 (RX of Arduino)
   - **RX:** Pin 8 (TX of Arduino)
   - **GND:** Ground
   - **VCC:** 5V

4. **OLED Display:**

   - **SCL:** A5
   - **SDA:** A4
   - **GND:** Ground
   - **VCC:** 3.3V

5. **Relay Module:**
   - **IN:** Pin 2
   - **VCC:** 5V
   - **GND:** Ground

---

## Software Libraries

Make sure to install the following libraries:

- **Adafruit GFX Library** (for OLED display)
- **Adafruit SSD1306** (for the OLED display control)
- **MFRC522** (for the RFID reader)
- **TinyGPS++** (for GPS data parsing)
- **SoftwareSerial** (for handling serial communication with GSM and GPS modules)

---

## Functionality

### RFID Access Control

- **Authorized Access:** The system checks the RFID card's UID against a list of authorized UIDs. If the card is authorized, the relay is activated, allowing access.
- **Unauthorized Access:** If the card is not authorized, the relay remains off, and access is denied.

### GPS Location Tracking

- The GPS module tracks the latitude and longitude in real-time and displays it on the OLED screen.

### Theft Mode

- **Activate Theft Mode:** When a user sends the SMS command "ACTIVATE THEFT MODE," the system activates theft mode, turning off the relay and continuously sending the GPS location every 5 seconds.
- **Deactivate Theft Mode:** Sending the SMS command "DEACTIVATE THEFT MODE" deactivates theft mode, restores normal operation, and turns the relay back on.

### SMS Commands

- **Request Current Location:** By sending the SMS command "CURRENT LOCATION," the system sends the current latitude and longitude, along with a Google Maps link, to the user's phone.
- **Activate/Deactivate Theft Mode:** Theft mode can be toggled on or off using the appropriate SMS commands.

---

## How to Use

1. **Setup Hardware:** Connect all the components as per the hardware connections mentioned above.
2. **Upload Code:** Load the Arduino code provided in the `main.cpp` file to your Arduino UNO.
3. **Test RFID Access:** Present authorized and unauthorized RFID cards to test the access control system.
4. **SMS Control:**
   - Send "CURRENT LOCATION" to get the device's current GPS coordinates.
   - Send "ACTIVATE THEFT MODE" to turn on theft mode.
   - Send "DEACTIVATE THEFT MODE" to turn off theft mode.
5. **Monitor OLED:** The OLED will display real-time GPS coordinates.

---

## Future Enhancements

- Adding real-time notifications for access attempts via GSM.
- Integration with a cloud platform for logging GPS locations and access events.
- Adding more security measures such as password protection for SMS commands.

---

## License

This project is open-source and licensed under the MIT License.

---

### Author

Vishal, an Embedded Systems and Web Development enthusiast.

---

## Acknowledgments

Special thanks to the open-source community for the fantastic libraries used in this project.
