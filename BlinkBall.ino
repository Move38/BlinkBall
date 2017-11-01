#include "blinklib.h"
#include "blinkstate.h"

bool hasBall = false;
bool justReceived = true;

byte neighborData = 0;
byte ballFace= 0;
uint32_t bounceTime = 1000;
uint32_t timeLastBounce = 0;
uint32_t handshakeDuration = 200;

void setup() {
  // put your setup code here, to run once:
  for(int i=0; i<6; i++) {
      // simple startup sequence with a ring of magenta
      setFaceColor( i , MAGENTA );
      delay(100);
      setFaceColor( i , OFF );
  }
}

void loop() {
  while(1) {
  // put your main code here, to run repeatedly:
  
  if(buttonDoubleClicked()) {
    hasBall = !hasBall;
  }

  if(hasBall) {
    // handle ball internally
    setFaceColor(ballFace, RED); 
    if(!justReceived) {
      irSendData(ballFace, 1);
    }
    if(getNeighborState(ballFace) == 2) {
      setFaceColor(ballFace, GREEN);
      uint32_t timer = millis();
      while(millis() - timer < handshakeDuration) {
        irSendData(ballFace, 3);
        if(getNeighborState(ballFace) == 3) {
          hasBall = false;
          break;
        }
      }
    }

    if (millis() - timeLastBounce > bounceTime) {
      setFaceColor(ballFace, OFF); 
      ballFace = (ballFace + 3) % 6;
      timeLastBounce = millis();
      justReceived = false;
    }
  }
  else {
    // wait to receive ball
    setColor(OFF);
    FOREACH_FACE(x) {
      if(getNeighborState(x) == 1) {
        uint32_t timer = millis();
        while(millis() - timer < handshakeDuration) {
          if(getNeighborState(x) == 1) {
            irSendData(x, 2);
          }
          else if(getNeighborState(x) == 3) {
            irSendData(x, 3);
            hasBall = true;
            justReceived = true;
            ballFace = x;       
          }
        }
      }
    }  
  }
  }
}
