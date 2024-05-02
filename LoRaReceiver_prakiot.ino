#include <SPI.h>
#include <LoRa.h>

//library oled
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND    915E6

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

String rssi = "RSSI --";
String packSize = "--";
String packet ;
int LoRaData;

void cbk(int packetSize) {
  packet ="";
  packSize = String(packetSize,DEC);
  for (int i = 0; i < packetSize; i++) { packet += (int) LoRa.read(); }
  rssi = "RSSI :" + String(LoRa.packetRssi(), DEC) ;
  Serial.println(rssi); // Output RSSI to Serial monitor
  Serial.println(packet);

  // Dsiplay information
   display.clearDisplay();
   display.setCursor(30,0);
   display.print("LORA RECEIVER");
   display.setCursor(0,20);
   display.print("Received packet:");
   display.setCursor(0,30);
   display.print(packet);
   //display.setCursor(0,50);
   //display.print("RSSI:");
   display.setCursor(0,40);
   display.print(rssi);
   display.display();
}

void setup() {
  Serial.begin(115200);

  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA RECEIVER ");
  display.display();


  while (!Serial);
  Serial.println();
  Serial.println("LoRa Receiver Callback");
  
  SPI.begin();
  LoRa.setPins(SS,RST,DI0);  
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.receive();
  LoRa.setSyncWord(0xF1);
  Serial.println("init ok");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) { cbk(packetSize); }

  delay(10);
}
