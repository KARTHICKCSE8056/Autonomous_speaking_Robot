#include "Arduino.h"
#include "WiFi.h"
#include "Audio.h"                   // Download this Library -> https://github.com/schreibfaul1/ESP32-audioI2S

#define uart_en 15
#define RXp2 16      
#define TXp2 17
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

#define led_1 5

// Your WiFi Credentials
const char *ssid = "cserobo";       // WiFi SSID Name
const char *password = "12345678";// WiFi Password ( Keep it blank if your WiFi router is open )

Audio audio;


void setup()
{
  pinMode(led_1, OUTPUT);

  Serial.begin(115200);
  Serial2.begin(115200,SERIAL_8N1, RXp2, TXp2);

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin( ssid, password);

  //while (WiFi.status() != WL_CONNECTED)
   WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){ digitalWrite(led_1,1);} delay(1000);
    delay(1500);

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(100);
  audio.connecttospeech("Starting", "en"); // Google TTS
}


void loop()

{

  if (Serial2.available()) {
    String Answer = Serial2.readString();
  
    //-----
            // Split the answer into chunks and send each chunk to connecttospeech
        size_t chunkSize = 80;  // Define chunk size (adjust if necessary)
        for (size_t i = 0; i < Answer.length(); i += chunkSize) {
        
          String chunk = Answer.substring(i, (i + chunkSize));
          Serial.println(chunk);
          audio.connecttospeech(chunk.c_str(), "en");

          while(audio.isRunning()){
            audio.loop();
        

          }
    //--------
   
  }
 

}
audio.loop();

}

void audio_info(const char *info) {

}
