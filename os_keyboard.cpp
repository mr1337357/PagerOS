#include "os_keyboard.h"
#include "os_process.h"

Adafruit_TCA8418 keypad;

uint8_t wheel_state = 0;
uint8_t new_wheel_state = 0;
uint8_t clicked = 0;
int8_t wheel_count = 0;

void os_event_send(uint8_t event);

void keyboard_interrupt()
{
  os_event_send(0);
}

void wheel_interrupt()
{
  new_wheel_state = digitalRead(40) | digitalRead(41) << 1;
  clicked |= !digitalRead(7);
  os_event_send(1);
}

void keyboard_init()
{
  Serial.printf("keyboard init\n");
  if (! keypad.begin(TCA8418_DEFAULT_ADDR, &Wire))
  {
    Serial.println("keypad not found, check wiring & pullups!");
    while (1);
  }
  pinMode(6,INPUT_PULLUP);
  attachInterrupt(6,keyboard_interrupt, CHANGE);
  keypad.matrix(4, 10);
  keypad.flush();
  keypad.enableInterrupts();

  pinMode(40,INPUT_PULLUP);
  pinMode(41,INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  attachInterrupt(40,wheel_interrupt, CHANGE);
  attachInterrupt(41,wheel_interrupt, CHANGE);
  attachInterrupt(7 ,wheel_interrupt, CHANGE);
}
char keys[] = "~qwertyuiopasdfghjkl~~zxcvbnm~~~";
char syms[] = "-1234567890*/+-=:'\"@--_$;?!,.---";

bool cap = false;
bool sym = false;

void keyboard_handle_event()
{
  char letter = 0;
  bool press = false;
  Serial.printf("KB\n");
  int intstat = keypad.readRegister(TCA8418_REG_INT_STAT);
  while(intstat)
  {
    Serial.printf("KB\n");
    letter = 0;
    press = false;
    if(intstat & 1)
    {
      uint8_t event = keypad.getEvent();
      keypad.writeRegister(TCA8418_REG_INT_STAT, 1);
      if(event & 0x80)
      {
        press = true;
        event &= 0x7F;
      }
      if(event < sizeof(keys))
      {
        letter = keys[event];
      }
      else
      {
        continue;
      }
      if(letter == '~')
      {
        switch(event)
        {
          case 20:
            letter = '\n';
            break;
          case 21:
            sym = press;
            continue;
            break;
          case 29:
            cap = press;
            continue;
            break;
          case 30:
            letter = '\b';
            break;
          case 31:
            letter = ' ';
            break;
        }
      }
      //Serial.printf("letter %d %c\n", letter, letter);
      if(press)
      {
        if(sym)
        {
          if(event == 31)
          {
            process_switch_active_app();
            continue;
          }
          letter = syms[event];
        }
        else if(cap)
        {
          if(isalpha(letter))
          {
            letter &= ~' ';
          }
        }
        //Serial.printf("press %d %c\n", letter, letter);
        //send keypress to app
      }
    }
    if(intstat & 2)
    {
      uint8_t event = keypad.readRegister(TCA8418_REG_GPIO_INT_STAT_1);
      //Serial.printf("gpio %02X\n",event);
      keypad.writeRegister(TCA8418_REG_INT_STAT, 2);
    }
    intstat = keypad.readRegister(TCA8418_REG_INT_STAT);
  }
  
}

void wheel_handle_event()
{
  if(new_wheel_state != wheel_state)
  {
    if(wheel_state == 0x02 && new_wheel_state == 0x00)
    {
      wheel_count -= 1;
      process_send_event_active(EVENT_WHEEL,(void *)1);
    }
    if(wheel_state == 0x00 && new_wheel_state == 0x02)
    {
      wheel_count += 1;
      process_send_event_active(EVENT_WHEEL,(void *)2);
    }
    //Serial.printf("count %d\n",wheel_count);
    if((clicked & 0x0F) == 1)
    {
      Serial.printf("click\n");
      process_send_event_active(EVENT_WHEEL,(void *)4);
    }
    clicked <<= 1;
    wheel_state = new_wheel_state;
  }
}