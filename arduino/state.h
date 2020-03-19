

class BoardState
{
public:
  void setup()
  {
    pinMode(PIN_LED1, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);

    digitalWrite(PIN_LED1, LOW);
    digitalWrite(PIN_LED2, LOW);

    _timerLED.Interval(200);
    _timerLED.Start();

    _timerInfo.Interval(500);
    _timerInfo.Start();
  }

  void loop()
  {
    if (stateBoard != _stateBoardOld)
    {
      _stateBoardOld = stateBoard;

      Serial.print("State:\t");

      switch (stateBoard)
      {
      case State_Starting:
        Serial.println("starting...");

        digitalWrite(PIN_LED1, HIGH);
        digitalWrite(PIN_LED2, LOW);

        delay(500);

        state.Put(State_INFO, 4, 1, "starting...");
        break;

      case State_Ready:
        Serial.println("ready");

        digitalWrite(PIN_LED1, HIGH);
        digitalWrite(PIN_LED2, HIGH);

        delay(500);

        digitalWrite(PIN_LED1, LOW);
        digitalWrite(PIN_LED2, LOW);

        delay(500);

        digitalWrite(PIN_LED1, HIGH);
        digitalWrite(PIN_LED2, HIGH);

        delay(1500);

        state.Put(State_INFO, 4, 2, "ready");
        break;

      case State_Loop:
        Serial.println("loop");

        digitalWrite(PIN_LED1, LOW);
        digitalWrite(PIN_LED2, LOW);

        state.Put(State_INFO, 4, 3, "loop");
        break;
      }
    }

    if (_timerLED.elapsed())
    {
      digitalWrite(PIN_LED1, digitalRead(PIN_LED2));
      digitalWrite(PIN_LED2, !digitalRead(PIN_LED2));
    }

    if (_timerInfo.elapsed())
    {
      Serial.println("");

      Serial.print("Uptime:\t");
      Serial.println(millis());

      Serial.print("Cycle:\t");
      Serial.println(ulTimeCycle);

      Serial.print("Step:\t");
      Serial.println(uintStep);

      state.Write();
    }
  }

  void SetState(enumState s)
  {
    stateBoard = s;

    _timerInfo.Reset();

    loop();
  }

private:
  Timer _timerLED = Timer();
  Timer _timerInfo = Timer();
  enumState _stateBoardOld = State_Undefined;
};
