#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>

//#define SSID  "WPI Sailbot"
//#define PASS   "YJKFMP6B8D"
//#define WEBHOOK "https://discord.com/api/webhooks/1057187391338729502/Ouv3SCZVQcniGfEuoDIc8ryEyzVlT--8vy5JdpkK2KpTGojrpdZgwd1Ugj2twUTDYaTP"

#define SSID "Verizon_NRLT97"
#define PASS "creed4-pod-dna"
#define WEBHOOK "https://discordapp.com/api/webhooks/1057796742545944677/DS4RZJDaAtrRw6LJQ5V0F5i3x0Zv4FSUscBg0w4_bfLB1Z95mtjWK9KOIantbvD--Sii"

#define TTS "false"
#define B_LED 2
#define PIR 4

WiFiClientSecure client;
HTTPClient https;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "north-america.pool.ntp.org", -4*60*60, 6000);

bool last_pir = false;

void setup() {
  pinMode(B_LED, OUTPUT);
  pinMode(PIR, INPUT);
  timeClient.begin();
  connectWIFI();
  setupDiscord();
  Serial.begin(9600);
}

void loop() {  
  timeClient.update(); //TODO: update less frequently  
  if (!client.connected()) https.begin(client, WEBHOOK);

  bool pir = digitalRead(PIR);
  digitalWrite(B_LED, !pir);
  Serial.println(pir);
  if(pir && !last_pir) sendDiscord("Movement Detected: " + timeClient.getFormattedTime());
  last_pir = pir;
}


//#############
//## HELPERS ##
//#############
void connectWIFI() {
  WiFi.begin(SSID, PASS);
  WiFi.mode(WIFI_STA);
  while (WiFi.status() != WL_CONNECTED){
    digitalWrite(B_LED, HIGH);
    delay(500);
    digitalWrite(B_LED, LOW);
    delay(500);
  }
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

void setupDiscord(){
  timeClient.update();
  client.setInsecure();
  https.begin(client, WEBHOOK);
  sendDiscord("Alarm Connected: " + timeClient.getFormattedTime());
}

//https://www.instructables.com/Send-a-Message-on-Discord-Using-Esp32-Arduino-MKR1/
//https://github.com/maditnerd/discord_test/blob/master/discord_test_esp8266/discord.h
void sendDiscord(String content){
  https.addHeader("Content-Type", "application/json");
  int httpCode = https.POST("{\"content\":\"" + content + "\",\"tts\":" + TTS + "}");
}
