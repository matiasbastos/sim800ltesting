#include <SoftwareSerial.h>
#include "SSD1306.h"

String usb_data, gsm_data, gsm_temp_data, usb_last_data, gsm_last_data;

SoftwareSerial gsm(D7, D6); // RX, TX

// Initialize the OLED display using Wire library
SSD1306 display(0x3c, D3, D5);

void send_sms() {
  Serial.println("Sending SMS...");
   
  //Set SMS format to ASCII
  gsm.write("AT+CMGF=1\r\n");
  delay(10000);
  if(gsm.available())
  {
    gsm_data = gsm.readString();
    Serial.println(gsm_data);
  } 
  //Send new SMS command and message number
  gsm.write("AT+CMGS=\"+549XXXX\"\r\n");
  delay(5000);
  if(gsm.available())
  {
    gsm_data = gsm.readString();
    Serial.println(gsm_data);
  }   
  //Send SMS content
  gsm.write("TEST");
  delay(5000);
  if(gsm.available())
  {
    gsm_data = gsm.readString();
    Serial.println(gsm_data);
  }   
  //Send Ctrl+Z / ESC to denote SMS message is complete
  gsm.write((char)26);
  delay(10000);
  if(gsm.available())
  {
    gsm_data = gsm.readString();
    Serial.println(gsm_data);
  }  
  if(Serial.available())
  {
    usb_data = Serial.readString();
    gsm.println(usb_data);
    Serial.println("SMS Sent!");
  }    
}

void setup() {
  usb_data = "";
  gsm_data = "";
  gsm_temp_data = "";
  usb_last_data = "-";
  gsm_last_data = "-";  
  
  // Initialize serial with computer
  Serial.begin(115200);
  Serial.println("Testing GSM SIM800L");

  // Initialize GSM serial 
  gsm.begin(9600);

  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  
  delay(1000);
  
  Serial.println("Setup Complete.");
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 22, "Setup Complete.");
  display.display();

  delay(3000);
}

void loop() {
  if(gsm.available()){
    gsm_temp_data = gsm.readString();
    if(gsm_temp_data.length() > 1){
      gsm_data = gsm_temp_data;
    }
    gsm_last_data = String(millis());
    Serial.println(gsm_temp_data);
  }

  if(Serial.available()){
    Serial.println("sending...");
    usb_data = Serial.readString();
    usb_last_data = String(millis());
    if(usb_data == "ctrlz\r\n"){
      gsm.println((char)26);
      usb_data = "ctrlz sent...";  
    } else {
      gsm.println(usb_data);
    }
  }

  // update oled
  update_display();
}

void update_display() {
  // clear the display
  display.clear();
   
  // The coordinates define the left starting point of the text
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 8, "USB Input:");
  display.drawString(0, 18, String(usb_data));
  display.drawString(0, 30, "GSM Output:");
  display.drawString(0, 40, String(gsm_data));
  
  // write millis
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 0, String(millis()));
  display.drawString(128, 8, usb_last_data);
  display.drawString(128, 30, gsm_last_data);
  
  // write the buffer to the display
  display.display();

  delay(10);  
}

