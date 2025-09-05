#include <SPI.h>
#include <mcp_can.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

const int SPI_CS_PIN = 5;
const int CAN_INT_PIN = 4;

#define TFT_CS     15
#define TFT_RST    2
#define TFT_DC     16

MCP_CAN CAN(SPI_CS_PIN); 
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int displayLine = 0;
const int MAX_LINES = 10;

typedef struct {
  uint32_t id;
  uint8_t len;
  uint8_t data[8];
} CANMessage;

QueueHandle_t canMessageQueue;
SemaphoreHandle_t displayMutex; 

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

  canMessageQueue = xQueueCreate(10, sizeof(CANMessage));
  displayMutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(canReaderTask, "CAN Reader", 4096, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(displayTask, "Display", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(serialTask, "Serial", 2048, NULL, 1, NULL, 0);
}

void loop() {
  vTaskDelete(NULL);
}

void canReaderTask(void *parameter) {
  while (1) {
    if (!digitalRead(CAN_INT_PIN)) {
      CANMessage msg;
      unsigned char buf[8];
      long unsigned int id;
      unsigned char len;
      
      if (CAN.readMsgBuf(&id, &len, buf) == CAN_OK) {
        msg.id = id;
        msg.len = len;
        memcpy(msg.data, buf, len);
        xQueueSend(canMessageQueue, &msg, portMAX_DELAY);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

void displayTask(void *parameter) {
  CANMessage msg;
  
  while (1) {
    if (xQueueReceive(canMessageQueue, &msg, portMAX_DELAY) == pdTRUE) {
      if (xSemaphoreTake(displayMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        displayCANMessage(msg.id, msg.len, msg.data);
        xSemaphoreGive(displayMutex);
      }
    }
  }
}

void serialTask(void *parameter) {
  CANMessage msg;
  
  while (1) {
    if (xQueueReceive(canMessageQueue, &msg, pdMS_TO_TICKS(100)) == pdTRUE) {
      Serial.print("ID: 0x");
      Serial.print(msg.id, HEX);
      Serial.print(" DLC: ");
      Serial.print(msg.len);
      Serial.print(" Data: ");
      for (int i = 0; i < msg.len; i++) {
        Serial.print(msg.data[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void displayCANMessage(uint32_t id, uint8_t len, uint8_t* data) {
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
