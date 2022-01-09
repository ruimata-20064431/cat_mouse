#ifndef divs_h
#define divs_h

#define BOARD_LED                         13                                    // internal board LED

/* -Blink-------------------------------------------------------------------- */

void blink(int led, int times, int duration){
    for (int n = 0; n < times; n++){
      digitalWrite(led, HIGH);
      delay(duration);
      digitalWrite(led, LOW);
      delay(duration);
    }
}



/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

#endif
