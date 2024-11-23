

//IR Sensor acting as WakeUp Button
#define button 23

// RGB LEDs for status indication
#define led_1 15
#define led_2 2
#define led_3 4
#define led_4 5  //wifi comform



// UART Pins of Other ESP32 for Text to Speech
#define RXp2 16
#define TXp2 17

// Necessary Libraries
#include "Audio.h"
#include "CloudSpeechClient.h"

const char *ssid = "cserobo";       // WiFi SSID Name
const char *password = "12345678";


int i = 0;
void setup()
{
  pinMode(button, INPUT);
  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);
  pinMode(led_3, OUTPUT);
  pinMode(led_4, OUTPUT);
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXp2, TXp2);
  Serial2.println("Intialising");
  //  Serial.println(My_Data);

  WiFi.begin(ssid, password);
  //  while (WiFi.status() == WL_CONNECTED){ digitalWrite(led_3,1);}
  while (WiFi.status() != WL_CONNECTED){ digitalWrite(led_4,1);} delay(1000);
}

void loop()
{

  digitalWrite(led_1, 0);
  digitalWrite(led_2, 0);
  digitalWrite(led_3, 0);

  if (i == 0) {
    Serial.println("Press button");
    i = 1;
  }
  //  if(i==1){delay(1);}

  delay(500);
  if (digitalRead(button) == 0)
  {
   // Serial2.println("\r\nPlease Ask!\r\n");
    // Green LED ON
    digitalWrite(led_1, 1);
    digitalWrite(led_2, 0);
    digitalWrite(led_3, 0);
   // delay(500);
    delay(1000);
    Serial.println("\r\nRecord start!\r\n");
    Audio* audio = new Audio(ADMP441);
    audio->Record();
    Serial.println("Processing your Audio File");
    // Blue LED ON
    digitalWrite(led_1, 0);
    digitalWrite(led_2, 1);
    digitalWrite(led_3, 0);

    CloudSpeechClient* cloudSpeechClient = new CloudSpeechClient(USE_APIKEY);
    cloudSpeechClient->Transcribe(audio);
    delete cloudSpeechClient;
    delete audio;
    i = 0;
  }
  if (digitalRead(button) == 1)
  {
    delay(1);   
  }
}
