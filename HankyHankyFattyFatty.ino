/* 
 * Hanky Hanky Fatty Fatty
 */
 
int PIN_LED = 3;
int PIN_SWITCH_RESET = 4;
int PIN_SWITCH_LID = 5;
int PIN_LED_INTERNAL = 13;  

int TICK = 100;
unsigned long REMINDER_TIMEOUT = (60 * 60 * 2); // 2 hours

long time_end = 0;
boolean fed_state = false;

int led_brightness = 0;
int led_fade_ammount = 10;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_SWITCH_RESET, INPUT);
  pinMode(PIN_SWITCH_LID, INPUT);
}

void loop() {
  // Main loop
  update_led(fed_state);
  
  if (fed_state) {
    /* Check to see if we need to reset the light based on time */
    if (time_end - ((long)millis()/1000) <= 0 ) {
      fed_state = false;
    }
      
    /* Check to see if anyone pushed the reset button */
    if (digitalRead(PIN_SWITCH_RESET) == HIGH) {
      fed_state = false;
    }
  } 
    
  if (!fed_state && digitalRead(PIN_SWITCH_LID) == HIGH) {
    time_end = ((long)millis()/1000) + REMINDER_TIMEOUT;
    fed_state = true;
  }
  
  delay(TICK);
}

void update_led(int state) {
  if (state) { 
    analogWrite(PIN_LED, led_brightness);
    
    led_brightness = led_brightness + led_fade_ammount;
    
    if (led_brightness > 255) 
      led_brightness = 255;
    else if (led_brightness < 0)
      led_brightness = 0;
    
    if (led_brightness <= 0 || led_brightness >= 255)
       led_fade_ammount = -led_fade_ammount;

  } else {
    analogWrite(PIN_LED, 0);
    led_brightness = 0;
  }
}
