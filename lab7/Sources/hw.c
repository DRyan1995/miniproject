


  void analogWrite(int pin, int value){
    PWMCLK = 0; // Select Clock A as the clock source
    PWMPRCLK = 0; // Set clock A prescaler to 1
    // Set period value 24000000/3000/1 = 8000
    switch (pin) {
      case 0:
        PWMPER0 = 8000;
        PWMDTY0 = 8000 * value / 255;
        PWMCNT0 = 0; //reset counter
      break;
      case 1:
        PWMPER1 = 8000;
        PWMDTY1 = 8000 * value / 255;
        PWMCNT0 = 0; //reset counter
      break;
      case 2:
        PWMPER2 = 8000;
        PWMDTY2 = 8000 * value / 255;
        PWMCNT0 = 0; //reset counter
      break;
      case 3:
        PWMPER3 = 8000;
        PWMDTY3 = 8000 * value / 255;
        PWMCNT0 = 0; //reset counter
      break;
      case 4:
        PWMPER4 = 8000;
        PWMDTY4 = 8000 * value / 255;
        PWMCNT0 = 0; //reset counter
      break;
      case 5:
        PWMPER5 = 8000;
        PWMDTY5 = 8000 * value / 255;
        PWMCNT0 = 0; //reset counter
      break;
      case 6:
        PWMPER6 = 8000;
        PWMDTY6 = 8000 * value / 255;
        PWMCNT0 = 0; //reset counter
      break;
      case 7:
        PWMPER7 = 8000;
        PWMDTY7 = 8000 * value / 255;
        PWMCNT0 = 0; //reset counter
      break;
    }
    PWMPOL = (1 << pin); //set polarity
    PWME |= (1 << pin); // enable pinx
  }
