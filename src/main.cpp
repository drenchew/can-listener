#include <SPI.h>
#include <mcp_can.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

const int SPI_CS_PIN = 5;
const int CAN_INT_PIN = 4;

#define TFT_CS     15
#define TFT_RST    2
#define TFT_DC     16

MCP_CAN CAN(SPI_CS_PIN); 
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int displayLine = 0;
const int MAX_LINES = 10; 

void setup() {
  Serial.begin(115200);
  while (!Serial);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.println("CAN Analyzer");
  tft.println("Initializing...");

  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("CAN BUS Shield init OK!");
    tft.setTextColor(ST77XX_GREEN);
    tft.println("CAN: OK");
  } else {
    Serial.println("CAN BUS Shield init FAIL");
    tft.setTextColor(ST77XX_RED);
    tft.println("CAN: FAIL");
    while (1);
  }

  CAN.setMode(MCP_NORMAL);
  pinMode(CAN_INT_PIN, INPUT);
  Serial.println("CAN Sniffer Ready");
  
  delay(2000);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.println("CAN Messages:");
  displayLine = 1;
}

void loop() {
  if (!digitalRead(CAN_INT_PIN)) {
    long unsigned int id;
    unsigned char len = 0;
    unsigned char buf[8];

    if (CAN.readMsgBuf(&id, &len, buf) == CAN_OK) {
      Serial.print("ID: 0x");
      Serial.print(id, HEX);
      Serial.print(" DLC: ");
      Serial.print(len);
      Serial.print(" Data: ");
      for (int i = 0; i < len; i++) {
        Serial.print(buf[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      
      displayCANMessage(id, len, buf);
    }
    else{
      Serial.println("Couldn't read any can msg!");
      delay(1000);
    }
  }
}

void displayCANMessage(long unsigned int id, unsigned char len, unsigned char* data) {
  if (displayLine >= MAX_LINES) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.println("CAN Messages:");
    displayLine = 1;
  }
  
  tft.setCursor(0, displayLine * 10);
  
  tft.setTextColor(ST77XX_CYAN);
  tft.print("ID:");
  tft.print(id, HEX);
  
  tft.setTextColor(ST77XX_YELLOW);
  tft.print(" L:");
  tft.print(len);
  
  displayLine++;
  tft.setCursor(0, displayLine * 10);
  
  tft.setTextColor(ST77XX_WHITE);
  for (int i = 0; i < len && i < 8; i++) {
    if (data[i] < 0x10) tft.print("0");
    tft.print(data[i], HEX);
    tft.print(" ");
  }
  
  displayLine++;
}
