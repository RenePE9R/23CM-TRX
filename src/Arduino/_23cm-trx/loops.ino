/*
 * 
 */
 void loopVfo() {
  Serial.println("--- Loop: VFO ---");
  lcd.clear();
  setRxFreq(rxFreqHz);

  while (1) {
    updateSmeterDisplay();
    
    /* Handle turning of the rotary encoder */
    long up = getRotaryTurn() * rasterHz;
    if (up != 0) {
      rxFreqHz += up;
      setRxFreq(rxFreqHz);
    }
  
    /* Exit if rotary pushed */
    if (getRotaryPush()) {
      return;
    }
  
    /* Handle PTT */
    if (isPTTPressed()) {
      digitalWrite(MUTE, true);
      startCTCSS();
      setTxFreq(rxFreqHz - 28000000);
  
      while(isPTTPressed()) {
        // wait
      }
    
      stopCTCSS();
      setRxFreq(rxFreqHz);
    }
  }
}

void loopMenu() {
  Serial.println("--- Loop: Menu ---");
  byte menuitem = 0;
  boolean exit = false;

  while(!exit) {   
    switch(menuitem) {
      case 0 : // Squelch menu
        Serial.println("Menu 0.");
        lcd.clear();
        lcd.setCursor(2,0); lcd.print("Squelch level");
        lcd.setCursor(2,1); lcd.print((int)squelchlevel);
        
        while(!exit && menuitem == 0) {
         readRSSI();                  // Mute/unmute audio based on squelch.

          // Highlight top row
          lcd.setCursor(0,0); lcd.print(">");
          lcd.setCursor(0,1); lcd.print(" ");

          byte push = getRotaryPush();
          menuitem += getRotaryTurn();

          if (push == 2) { exit = true; } // long push, exit
          if (push == 1) {
            // Menu item selected, rotary now selects squelch            
            // Highlight bottom row
            lcd.setCursor(0,0); lcd.print(" ");
            lcd.setCursor(0,1); lcd.print(">");
            
            while (0 == getRotaryPush()) { // until rotary is pushed again
              readRSSI();                  // Mute/unmute audio based on squelch.
              int turn = getRotaryTurn();
              if (turn != 0) {
                squelchlevel = constrain(squelchlevel + turn, 0, 9);                                
                lcd.setCursor(2,1);
                lcd.print((int)squelchlevel);
              }
            }
            exit = true;
          }
        }
        break;
      default : menuitem = constrain(menuitem,0,0);
    }
    writeAllToEEPROM();
  }
}

