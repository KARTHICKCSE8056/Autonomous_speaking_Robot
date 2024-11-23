#include "CloudSpeechClient.h"
#include "network_param.h"
#include "Credentials.h"

#include <base64.h>
#include <ArduinoJson.h>
#define USE_SERIAL Serial
#include <Arduino.h>
#include <HTTPClient.h>
//#define uart_en 15
#define led_3 4
#define led_1 15
#define led_2 2
#define led_4 10
//#include <SoftwareSerial.h>

CloudSpeechClient::CloudSpeechClient(Authentication authentication) {
  this->authentication = authentication;
  client.setCACert(root_ca);
  if (!client.connect(server, 443)) 
                  {
                     Serial.println("Connection failed!");
                     digitalWrite(led_3, 1);
                     digitalWrite(led_1, 1);
                     digitalWrite(led_2, 1);
                  }
           else   {
                     Serial.println("Connected to server!");
                     digitalWrite(led_3, 1);
                   //  digitalWrite(led_1, 1);
                   //  digitalWrite(led_2, 1);
                  }
}

String ans;

CloudSpeechClient::~CloudSpeechClient() {
  client.stop();
 // WiFi.disconnect();
}

void CloudSpeechClient::PrintHttpBody2(Audio* audio)
{
  String enc = base64::encode(audio->paddedHeader, sizeof(audio->paddedHeader));
  enc.replace("\n", "");  // delete last "\n"
  client.print(enc);      // HttpBody2
  char** wavData = audio->wavData;
  for (int j = 0; j < audio->wavDataSize / audio->dividedWavDataSize; ++j) {
    enc = base64::encode((byte*)wavData[j], audio->dividedWavDataSize);
    enc.replace("\n", "");// delete last "\n"
    client.print(enc);    // HttpBody2
  }
}

void CloudSpeechClient::Transcribe(Audio* audio) {
  String HttpBody1 = "{\"config\":{\"encoding\":\"LINEAR16\",\"sampleRateHertz\":16000,\"languageCode\":\"" + SpeechtoText_Language + "\"},\"audio\":{\"content\":\"";
  String HttpBody3 = "\"}}\r\n\r\n";
  int httpBody2Length = (audio->wavDataSize + sizeof(audio->paddedHeader)) * 4 / 3; // 4/3 is from base64 encoding
  String ContentLength = String(HttpBody1.length() + httpBody2Length + HttpBody3.length());
  String HttpHeader;
  // if (authentication == USE_APIKEY)
  HttpHeader = String("POST /v1/speech:recognize?key=") + ApiKey
               + String(" HTTP/1.1\r\nHost: speech.googleapis.com\r\nContent-Type: application/json\r\nContent-Length: ") + ContentLength + String("\r\n\r\n");
  //  else if (authentication == USE_ACCESSTOKEN)
  //    HttpHeader = String("POST /v1beta1/speech:syncrecognize HTTP/1.1\r\nHost: speech.googleapis.com\r\nContent-Type: application/json\r\nAuthorization: Bearer ")
  //   + AccessToken + String("\r\nContent-Length: ") + ContentLength + String("\r\n\r\n");
  client.print(HttpHeader);
  client.print(HttpBody1);
  PrintHttpBody2(audio);
  client.print(HttpBody3);
  String My_Answer = "";
  while (!client.available());

  while (client.available())
  {
    char temp = client.read();
    My_Answer = My_Answer + temp;
    // Serial.write(client.read());
  }
client.stop();
   Serial.print("My Answer - ");Serial.println(My_Answer);
  int postion = My_Answer.indexOf('{');
   Serial.println(postion);
  ans = My_Answer.substring(postion);
  Serial.print("Json daata--");
  Serial.print(ans);

  DynamicJsonDocument doc(384);

  //StaticJsonDocument<384> doc;

  DeserializationError error = deserializeJson(doc, ans);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  JsonObject results_0 = doc["results"][0];
  //const char*
  const char* chatgpt_Q = results_0["alternatives"][0]["transcript"];
  const char* a = "light on";
  const char* b = "light off";
  //String chatgpt_Q = a+ans+b;
  //Serial.println(ans);
  Serial.print(chatgpt_Q); Serial.println("-");
  ///////////////////////////////////////////////////////////
 
  {
    Serial.println("Asking Chat Gemini");



String res = "";
res = res + chatgpt_Q;
 int len = res.length();
 //res = res.substring(0, (len - 1));
  res = res.substring(0, (len));
  res = "\"" + res + "\"";
  Serial.println("");
  Serial.println(res);

HTTPClient https;

  //Serial.print("[HTTPS] begin...\n");
  if (https.begin("https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + (String)Gemini_Token)) {  // HTTPS

    https.addHeader("Content-Type", "application/json");
    String payload = String("{\"contents\": [{\"parts\":[{\"text\":" + res + "}]}],\"generationConfig\": {\"maxOutputTokens\": " + (String)Gemini_Max_Tokens + "}}");

    //Serial.print("[HTTPS] GET...\n");

    // start connection and send HTTP header
    int httpCode = https.POST(payload);

    // httpCode will be negative on error
    // file found at server

    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String payload = https.getString();
      //Serial.println(payload);

      DynamicJsonDocument doc(1024);


      deserializeJson(doc, payload);
      String Answer = doc["candidates"][0]["content"]["parts"][0]["text"];

Answer.trim();  // Trims the Answer string first
String filteredAnswer = "";
bool lastWasSpace = false;
for (size_t i = 0; i < Answer.length(); i++) {
  char c = Answer[i];
  if (isalnum(c)) {
    filteredAnswer += c;
    lastWasSpace = false;
  } else if (isspace(c) && !lastWasSpace) {
    filteredAnswer += ' ';
    lastWasSpace = true;
  }
}
filteredAnswer.trim();  // Trim the result of filteredAnswer
Answer = filteredAnswer;  // Assign it back to Answer




      Serial.println("");
      Serial.println("Here is your Answer: ");
      const char* only_ans = Answer.c_str();


      Serial.println("Only ans:-"); Serial.print(only_ans);
        Serial2.print(only_ans);
        digitalWrite(led_1, 1);
        digitalWrite(led_2, 1);
      // digitalWrite(uart_en, LOW);
       delay(1);

    } else {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }
    https.end();
  } else {
    Serial.printf("[HTTPS] Unable to connect\n");
  }

  res = "";

    Serial.print("To ask again");
    delay(10000);

  }
}
