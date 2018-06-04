#ifndef LED_H
#define LED_H

#define LED 5
#define LED_MSK (1 << LED)

class Led {

public:
  Led(void);
  void toggle(void);
  uint8_t is_led_set(void);
  void on(void);
  void off(void);
};


#endif // LED_H
