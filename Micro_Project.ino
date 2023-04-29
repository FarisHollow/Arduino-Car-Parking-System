#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);  //Change the HEX address
#include <Servo.h> 

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define LED_G 7 //define green LED pin
#define LED_R 5 //define red LED
#define BUZZER 6 //buzzer pin

MFRC522 rfid(SS_PIN, RST_PIN);
Servo servo;
int Slot = 2;

const byte AUTHORIZED_CARDS[][4] = {
  {0x0A, 0x4F, 0xE4, 0x5C}, // Replace with authorized card UIDs
  {0x00, 0x00, 0x00, 0x00},
  // Add more authorized cards here as needed
};
const int NUM_AUTHORIZED_CARDS = sizeof(AUTHORIZED_CARDS) / sizeof(AUTHORIZED_CARDS[0]);

// int car_count = 3;
int ir_sensor_pin = 2;
int ir_sensor_pin2 = 4;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  servo.attach(3);
  pinMode(ir_sensor_pin, INPUT_PULLUP);
  // pinMode(ir_sensor_pin2, INPUT_PULLUP);
  // mfrc522.PCD_Init();   // Initiate MFRC522
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
  Serial.println("Put your card to the reader...");
  Serial.println();

  lcd.begin(16,2);
  lcd.backlight();

  
  lcd.setCursor (0,0);
  lcd.print("     ARDUINO    ");
  lcd.setCursor (0,1);
  lcd.print(" PARKING SYSTEM ");
  delay (2000);
  lcd.clear();  
}

bool is_authorized_card(byte *uid, byte uid_size) {
  for (int i = 0; i < NUM_AUTHORIZED_CARDS; i++) {
    if (memcmp(uid, AUTHORIZED_CARDS[i], uid_size) == 0) {
      return true;
    }
  }
  return false;
}

void loop() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    // RFID tag detected
    Serial.print("RFID tag UID: ");
    for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();
    if (is_authorized_card(rfid.uid.uidByte, rfid.uid.size)) {
      Serial.println("Authorized card detected.");
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();

      // Check if car slot is available
      if ( Slot > 0) {
        digitalWrite(LED_G, HIGH);
        Serial.println("Car slot available. Unlocking...");
        servo.write(180); // Unlock car slot
        Slot = Slot - 1;
        Serial.println("Slot Left");
        Serial.println(Slot);
        
        // Wait for car to enter the slot
        
        Serial.println("Car entered slot.");
              
      } else {
        Serial.println("Car slot is occupied. Try again later.");
        lcd.setCursor(0,0);
        lcd.print("SORRY! Try Later");
        lcd.setCursor(0,1);
        lcd.print("  Parking Full  ");
        digitalWrite(LED_R, HIGH);
        tone(BUZZER, 300);
        delay(4000);
        digitalWrite(LED_R, LOW);
        noTone(BUZZER); 
        // digitalWrite(BUZZER, LOW); 


        delay(3000);
        lcd.clear();
      }
    } else {
      Serial.println("Unauthorized card detected. Access denied.");
       lcd.setCursor(0,0);
        lcd.print(" Unauthorized ");
        lcd.setCursor(0,1);
        lcd.print(" Can't Enter ");
        digitalWrite(LED_R, HIGH);
        tone(BUZZER, 300);
        delay(4000);
        digitalWrite(LED_R, LOW);
        noTone(BUZZER); 
        // digitalWrite(BUZZER, LOW); 


        delay(3000);
        lcd.clear();
    }
  }

  if (digitalRead(ir_sensor_pin2) == LOW) {
          servo.write(0); // Enter and close the barrier
           lcd.setCursor (0,0);
         lcd.print("Greetings....");
        delay(2000);
        lcd.clear(); 
          digitalWrite(LED_G, LOW);
        }

  // Check if a car is leaving the slot
  if (digitalRead(ir_sensor_pin) == LOW ) {
    lcd.setCursor (0, 0);
    lcd.print("Car Leaving....");
   
    
    digitalWrite(LED_G, HIGH);
    Serial.println("Car leaving slot. Locking...");



    servo.write(180); // Open car slot
    delay(5000);
    servo.write(0); //Lock Car slot
    lcd.clear();     
    Slot = Slot + 1; 
    digitalWrite(LED_G, LOW);    
    
  
    Serial.println("Car slot is now available.");
    
     
    Serial.println("Slot Left");
    Serial.println(Slot);
   
    
  }
    lcd.setCursor(0,0);
    lcd.print("  HI THERE ^_^  ");
    lcd.setCursor(0,1);
    lcd.print("  SLOT LEFT  ");
    lcd.print(Slot);
}
