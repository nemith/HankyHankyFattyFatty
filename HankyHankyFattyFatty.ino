/* 
 * Hanky Hanky Fatty Fatty
 */
#include <SoftwareSerial.h>
#include <serLCD.h>
 
#define PIN_LED          3
#define PIN_SWITCH_RESET 4
#define PIN_SWITCH_LID   5
#define PIN_LCD_SERIAL   6
#define PIN_LED_INTERNAL 13

#define TICK 20
#define REMINDER_TIMEOUT (60 * 60 * 2)

serLCD lcd(PIN_LCD_SERIAL);

long time_fed = secs();

const char *lcd_text[][2] PROGMEM = {
//  1234567890123456  1234567890123456
  {"      Init",      ""               },
  {"    Lid Open!",   ""               },
  {"H was last fed",  "      hours agp"},
  {"Hanky Hanky",     "    Fatty Fatty"}
};

enum lcd_states {
  LCD_INIT = 0,
  LCD_LID_OPEN = 1,
  LCD_LAST_FED = 2,
  LCD_FATTY_FATTY = 3
};

lcd_states lcd_state = LCD_LID_OPEN;
lcd_states prev_lcd_state;
long last_lcd_state_change = secs();

enum lid_states {
  LID_OPEN,
  LID_CLOSED
};

lid_states lid_state = LID_OPEN;
lid_states prev_lid_state;

enum led_states {
  LED_OFF,
  LED_ON,
  LED_PULSE
};

led_states led_state = LED_OFF;
led_states prev_led_state;

int led_brightness = 0;
int led_fade_ammount = 10;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_SWITCH_RESET, INPUT);
  pinMode(PIN_SWITCH_LID, INPUT);
}

void loop() {
  update_lid_state();
  update_led();
  update_lcd();
  
  delay(TICK);
}

// Seconds since started
long secs() {
  return (long)millis() / 1000;
}

void update_lid_state() {
  prev_lid_state = lid_state;

  if (digitalRead(PIN_SWITCH_LID) == LOW) {
    /* Lid is open */
    lid_state = LID_OPEN;
  } else {
    lid_state = LID_CLOSED;
  }

  switch(lid_state) {
    case LID_OPEN:
       if (prev_lid_state != LID_OPEN) {
         led_state = LED_ON;
         lcd_state = LCD_LID_OPEN;
         time_fed = secs();
       }
       break;

    case LID_CLOSED:
       if (prev_lid_state != LID_CLOSED) {
         lcd_state = LCD_LAST_FED;
         led_state = LED_PULSE;
       }

       if (led_state != LED_OFF && time_fed + REMINDER_TIMEOUT - secs() <= 0) {
         led_state = LED_OFF;
       }

       if (digitalRead(PIN_SWITCH_RESET) == HIGH) {
         if (led_state != LED_OFF) {
           led_state = LED_OFF;
         }
         lcd_state = LCD_FATTY_FATTY;
       }
       break;
  }
}

void update_lcd() {
  lcd_states orig_lcd_state = lcd_state;

  if (prev_lcd_state != lcd_state) {
    lcd.clear();
    lcd.print(lcd_text[lcd_state][0]);
    lcd.selectLine(2);
    lcd.print(lcd_text[lcd_state][1]);

    last_lcd_state_change = secs();
  }

  float hours_since_fed = (secs() - time_fed) / 60.0 / 60.0;

  switch (lcd_state) {
    case LCD_LAST_FED:
      lcd.setCursor(2, 1);
      lcd.print(hours_since_fed, 2);
      break;

    case LCD_FATTY_FATTY:
      //Reset back to LAST_FED after 5 seconds
      if ((last_lcd_state_change + 5) - secs() <= 0) {
        lcd_state = LCD_LAST_FED;
      }
      break;
  }

  prev_lcd_state = orig_lcd_state;
}


void update_led() {
  
  switch (led_state) {
    case LED_OFF:
      if (prev_led_state != LED_OFF)
        analogWrite(PIN_LED, 0);
      break;

    case LED_ON:
      if (prev_led_state != LED_ON)
        analogWrite(PIN_LED, 255);
      break;

    case LED_PULSE:
      if (prev_led_state != LED_PULSE)
        led_brightness = 0;

      analogWrite(PIN_LED, led_brightness);

      led_brightness = led_brightness + led_fade_ammount;

      if (led_brightness > 255)
        led_brightness = 255;
      else if (led_brightness < 0)
        led_brightness = 0;

      if (led_brightness <= 0 || led_brightness >= 255)
         led_fade_ammount = -led_fade_ammount;

      break;
  }
  prev_led_state = led_state
}
