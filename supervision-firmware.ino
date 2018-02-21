#include <SPI.h>
#include <MFRC522.h> // RFID
#include <Ethernet.h> // Ethernet
#include <ArduinoHttpClient.h>

#define SS_PIN 6
#define RST_PIN 7
#define BUZZER_PIN 5

// ACORDS
const int c = 261;
const int d = 294;
const int e = 329;
const int f = 349;
const int g = 391;
const int gS = 415;
const int a = 440;
const int aS = 455;
const int b = 466;
const int cH = 523;
const int cSH = 554;
const int dH = 587;
const int dSH = 622;
const int eH = 659;
const int fH = 698;
const int fSH = 740;
const int gH = 784;
const int gSH = 830;
const int aH = 880;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "supervision-li.herokuapp.com";
int port = 80;

IPAddress ip(192, 168, 0, 177);

EthernetClient client;

MFRC522 mfrc522(SS_PIN, RST_PIN);
unsigned long uidDec, uidDecTemp;

void setup() {
 Serial.begin(9600);
 connectToEthernet();
 Serial.println("Waiting for card...");
 SPI.begin();
 mfrc522.PCD_Init();
}

void loop() {  
 if ( ! mfrc522.PICC_IsNewCardPresent()) {
  return;
 }
 
 if ( ! mfrc522.PICC_ReadCardSerial()) {
  return;
 }
 
 uidDec = 0;
 for (byte i = 0; i < mfrc522.uid.size; i++) {
  uidDecTemp = mfrc522.uid.uidByte[i];
  uidDec = uidDec * 256 + uidDecTemp;
 }
 
 Serial.println("Card UID: ");
 Serial.println(uidDec);
 
 sendTracking("cardId=" + String(uidDec));
 delay(3000);
}

void sendTracking(String postData) {
  HttpClient clientHttp = HttpClient(client, server, port);

  clientHttp.beginRequest();
  clientHttp.post("/api/tracking");
  clientHttp.sendHeader("Content-Type", "application/x-www-form-urlencoded");
  clientHttp.sendHeader("Content-Length", postData.length());
  clientHttp.sendHeader("X-Custom-Header", "custom-header-value");
  clientHttp.beginBody();
  clientHttp.print(postData);
  clientHttp.endRequest();

  int statusCode = clientHttp.responseStatusCode();
  
   if (statusCode == 200) {
    cardSuccessMelody();
   } else {
    cardFailedMelody();
   }

  clientHttp.stop();
}

void connectToEthernet() {
  Serial.println("Connecting to internet...");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }
}

void cardFailedMelody() {
  beep(eH, 500);
  beep(eH, 500);
}

void cardSuccessMelody() {
  beep(cH, 300);
}

void beep(int note, int duration)
{
  tone(BUZZER_PIN, note, duration);
  delay(duration);
  noTone(BUZZER_PIN);
  delay(50);
}

