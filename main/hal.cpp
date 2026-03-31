#include "hal.h"

SPIClass hspi(HSPI);

Adafruit_ST7796S tft = Adafruit_ST7796S(&hspi, DISP_CS, DISP_DC, DISP_RST);

IoExpanderXL9555 io; 

typedef struct
{
  uint8_t mode;
  uint8_t value;
} pinconfig;

pinconfig pinconfigs[] = 
{
  {OUTPUT, LOW}, //Haptic en
  {OUTPUT, LOW}, //Amp en
  {OUTPUT, LOW}, //KEY RST
  {OUTPUT, LOW}, //lora en
  {OUTPUT, LOW}, //gps en
  {OUTPUT, LOW}, //rf en
  {OUTPUT, LOW}, //lcd rst
  {INPUT, LOW}, //gps rst
  {INPUT, LOW}, //KEY EN
  {INPUT, LOW}, //NRF CE
  {INPUT, LOW}, //SD DET
  {OUTPUT, HIGH}, //SPI PULLUP EN
  {OUTPUT, HIGH}, //SD EN
  {INPUT, LOW},
  {INPUT, LOW},
  {INPUT, LOW},
};

#define NUM_PINCONFIGS (sizeof(pinconfigs)/sizeof(pinconfigs[0]))

void hal_init()
{
  int rv;
  int i;
  Wire.begin(SDA, SCL);

  io.begin(Wire, 0x20);
  delay(1);

  for(i=0;i<NUM_PINCONFIGS;i++)
  {
    io.pinMode(i, pinconfigs[i].mode);
    delay(1);
    if(pinconfigs[i].mode == OUTPUT)
    {
      io.digitalWrite(i, pinconfigs[i].value);
      delay(1);
    }
  }

  rv = hspi.begin(SCK,MISO,MOSI,-1);
  ESP_LOGI("PagerOS", "hspi status %d",rv);

  pinMode(42,OUTPUT);
  analogWrite(42,128);

  tft.init(222, 480, 0, 49, ST7796S_BGR);
  tft.setRotation(3);
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.fillScreen(ST77XX_RED);

  delay(100);
  //io.digitalWrite(EXPANDS_SD_EN, HIGH);
  tft.setCursor(0, 0);
  delay(100);
  if (!SD.begin(SD_CS, hspi))
  {
    tft.printf("SD: Fail\n");
    ESP_LOGI("PagerOS", "Failed to detect SD Card!!");
  }
  else
  {
    tft.printf("SD: Pass\n");
    ESP_LOGI("PagerOS", "detected SD card");
  }
}
