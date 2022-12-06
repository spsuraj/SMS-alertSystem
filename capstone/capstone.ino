//Import library for the RFID reader
#include <SPI.h>
#include <MFRC522.h>
//Function to communicate with the GSM module
#include <SoftwareSerial.h>

SoftwareSerial mySerial(7, 8); //SIM module Tx and RX connect to pin 7 and pin 8 
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);
 
void setup() 
{
  //Begin serial communication between board and the IDE
  Serial.begin(9600);   
  SPI.begin();
  //Start coms with the mfrc522 module
  mfrc522.PCD_Init();
  //Begin serial communication with Arduino and SIM900
  mySerial.begin(9600);

  Serial.println("Initializing..."); 
  delay(1000);
  mySerial.println("AT"); //Handshaking with SIM900
  updateSerial();
  Serial.println("Approximate your card to the reader...");
}

void loop() 
{
  // Look for new cards in affinity
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Scan the card
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }

  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "A3 03 A7 1D") 
  {
    Serial.println("Authorized access");
    mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
    updateSerial();
    mySerial.println("AT+CMGS=\"+15198729239\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
    updateSerial();
    delay(1000);
    mySerial.print(" You have a new bank statement available. Login securely online to view "); 
    updateSerial();
    mySerial.print("Thank you");
    updateSerial();
    mySerial.write(26); //The text message entered followed by a ‘Ctrl+z’ character is treated as SMS. ‘Ctrl+z’ is actually a 26th non-printing character described as ‘substitute’ in ASCII table. So, we need to send 26Dec (1AHex) once we send a message.
    Serial.println();
    delay(3000);
  }
 
 else  if(content.substring(1) == "A3 48 E3 1B") {
    Serial.println("Authorized access");
    mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
    updateSerial();
    mySerial.println("AT+CMGS=\"+15198729239\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
    updateSerial();
    mySerial.print(" Your OTP is 2054. Please call us immediately if this was not initiated by you. "); 
    updateSerial();
    mySerial.write(26);
    //The text message entered followed by a ‘Ctrl+z’ character is treated as SMS. ‘Ctrl+z’ is actually a 26th non-printing character described as ‘substitute’ in ASCII table. So, we need to send 26Dec (1AHex) once we send a message.
    Serial.println();
    delay(3000);
  }
  else{
    Serial.println("Please Try again");
  }
  delay(1000);
} 

//Serial function to check if there is any incoming signal to the arduino from GSM module

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}
