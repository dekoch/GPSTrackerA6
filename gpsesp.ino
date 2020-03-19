
#define BOARD_GPS

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#include "arduino/util.h"
#include "arduino/globals.h"
#include "arduino/config.h"
#include "arduino/boards.h"
#include "arduino/state.h"
#include "arduino/hwA6.h"
#include "arduino/hwGPS.h"
#include "arduino/background.h"
#include "arduino/auto.h"

Settings _sett;
BoardState _boardstate;
HardwareA6 _a6;
HardwareGPS _gps;
Background _bg;
Auto _auto;

Timer timerCycle = Timer(false, true);

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("");

  _boardstate.setup();
  _boardstate.SetState(State_Starting);

  _sett.setup();

  _a6.setup();
  _gps.setup();
  _bg.setup();
  _auto.setup();

  timerCycle.Interval(sToMillis(3) * 1000); // us
  timerCycle.Start();

  _boardstate.SetState(State_Ready);
  _boardstate.SetState(State_Loop);
}

void loop()
{
  timerCycle.Reset();

  // hardware
  _a6.loop();
  _gps.loop();
  // background
  _bg.loop();
  // program
  _auto.loop();

  ulTimeCycle = timerCycle.elapsedTime();

  // cycletime monitoring
  if (timerCycle.elapsed())
  {
    state.Put(State_WARNING, 1, 1, "Cyc");
  }

  // show board state
  _boardstate.loop();

  /*if (boDebugger)
  {
    digitalWrite(PIN_LED1, !digitalRead(PIN_LED1));
  }*/
}
