#include "hal.h"
#include "textbox.h"

textbox tb;

SPIClass hspi(HSPI);

Adafruit_ST7796S tft = Adafruit_ST7796S(&hspi, DISP_CS, DISP_DC, DISP_RST);

IoExpanderXL9555 io; 

Adafruit_TCA8418 keypad;

typedef struct
{
  uint8_t mode;
  uint8_t value;
} pinconfig;

pinconfig pinconfigs_ext[] = 
{
  {OUTPUT, LOW}, //Haptic en
  {OUTPUT, LOW}, //Amp en
  {OUTPUT, LOW}, //KEY RST
  {OUTPUT, LOW}, //lora en
  {OUTPUT, LOW}, //gps en
  {OUTPUT, LOW}, //rf en
  {OUTPUT, LOW}, //lcd rst
  {INPUT, LOW}, //gps rst
  {OUTPUT, HIGH}, //KEY EN
  {INPUT, LOW}, //NRF CE
  {INPUT, LOW}, //SD DET
  {OUTPUT, HIGH}, //SPI PULLUP EN
  {OUTPUT, HIGH}, //SD EN
  {INPUT, LOW},
  {INPUT, LOW},
  {INPUT, LOW},
};

#define NUM_PINCONFIGS_EXT (sizeof(pinconfigs_ext)/sizeof(pinconfigs_ext[0]))

uint8_t cspins[] = 
{
  SD_CS,
  NFC_CS,
  LORA_CS,
  DISP_CS,
};

#define NUM_CSPINS (sizeof(cspins)/sizeof(cspins[0]))

void hal_init()
{
  int i;
  Wire.begin(SDA, SCL);

  io.begin(Wire, 0x20);

  for(i=0;i<NUM_PINCONFIGS_EXT;i++)
  {
    delay(1);
    io.pinMode(i, pinconfigs_ext[i].mode);
    if(pinconfigs_ext[i].mode == OUTPUT)
    {
      io.digitalWrite(i, pinconfigs_ext[i].value);
    }
  }

  for(i=0;i<NUM_CSPINS;i++)
  {
    pinMode(cspins[i],OUTPUT);
    digitalWrite(cspins[i],HIGH);
  }

  hspi.begin(SCK,MISO,MOSI,-1);
  hspi.setFrequency(100000);

  pinMode(42,OUTPUT);
  analogWrite(42,128);

  tft.init(222, 480, 0, 49, ST7796S_BGR);
  tb.begin(tft, 0, 0, 480, 222);
  tft.setRotation(3);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.fillScreen(ST77XX_BLACK);
  
  tb.printf("Starting up...\n");

  io.digitalWrite(2, HIGH);

  delay(100);
  //io.digitalWrite(EXPANDS_SD_EN, HIGH);
  if (!SD.begin(SD_CS, hspi))
  {
    tb.printf("sd not loaded\n");
    Serial.println("Failed to detect SD Card!!");
  }
  else
  {
    tb.printf("sd found\n");
    Serial.println("detected SD card\n");
  }

  if (! keypad.begin(TCA8418_DEFAULT_ADDR, &Wire)) {
    Serial.println("keypad not found, check wiring & pullups!");
    while (1);
  }
}