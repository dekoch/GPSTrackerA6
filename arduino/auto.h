
#define STEP_IDLE 0
#define STEP_GPS 100
#define STEP_GPS_ERROR -100
#define STEP_GPS_FIX 105
#define STEP_GPS_END 199
#define STEP_SMS 200
#define STEP_SMS_ERROR -200
#define STEP_SMS_SEND 205
#define STEP_SMS_WAIT 210
#define STEP_SMS_END 299

class Auto
{
public:
  void setup()
  {
    step(STEP_GPS, 0);

    _tStep.Start();
    _tTimeOut.Start();
    _tTimeOutAuto.Interval(mToMillis(5));
    _tTimeOutAuto.Start();
    _tRefreshLoc.Interval(mToMillis(60));
    _tRefreshLoc.Start();
  }

  void loop()
  {
    if (_tTimeOutAuto.elapsed())
    {
      state.Put(State_WARNING, 3, 1, "auto TimeOut");
      step(STEP_IDLE, 0);
    }

    if (_tStep.elapsed())
    {
      uintStep = _uintNextStep;

      switch (uintStep)
      {
      case STEP_IDLE:
        byRxSelect = DEV_A6;

        _tTimeOutAuto.Reset();

        if (strCallerNumber != "" || _tRefreshLoc.elapsed())
        {
          step(STEP_GPS, 0);
        }
        break;

      case STEP_GPS:
        byRxSelect = DEV_GPS;

        _tTimeOut.Interval(mToMillis(3));
        _tTimeOut.Reset();

        locCurrent.valid = false;

        step(STEP_GPS_FIX, 0);
        break;

      case STEP_GPS_FIX:
        if (_tTimeOut.elapsed())
        {
          step(STEP_GPS_ERROR, 0);
        }

        if (locCurrent.valid)
        {
          locLastFix = locCurrent;

          step(STEP_GPS_END, 0);
        }
        break;

      case STEP_GPS_ERROR:
        state.Put(State_ERROR, 3, 100, "GPS");

        step(STEP_GPS_END, 0);
        break;

      case STEP_GPS_END:
        _tRefreshLoc.Reset();

        if (strCallerNumber == "")
        {
          step(STEP_IDLE, 0);
        }
        else
        {
          step(STEP_SMS, 0);
        }
        break;

      case STEP_SMS:
        byRxSelect = DEV_A6;

        step(STEP_SMS_SEND, 0);
        break;

      case STEP_SMS_SEND:
        byCommandA6 = A6_SENDSMS;
        byStateA6 = NOTOK;

        _tTimeOut.Interval(sToMillis(30));
        _tTimeOut.Reset();

        step(STEP_SMS_WAIT, 0);
        break;

      case STEP_SMS_WAIT:
        if (_tTimeOut.elapsed())
        {
          step(STEP_SMS_ERROR, 0);
        }

        if (byStateA6 == OK)
        {
          step(STEP_SMS_END, 0);
        }
        break;

      case STEP_SMS_ERROR:
        state.Put(State_ERROR, 3, 101, "SMS");

        step(STEP_SMS_END, 0);
        break;

      case STEP_SMS_END:
        strCallerNumber = "";

        step(STEP_IDLE, 0);
        break;

      default:
        state.Put(State_ERROR, 3, uintStep, "step");
        step(STEP_IDLE, 0);
        break;
      }
    }
  }

private:
  unsigned int _uintNextStep = 0;
  Timer _tStep = Timer();
  Timer _tTimeOut = Timer();
  Timer _tTimeOutAuto = Timer();
  Timer _tRefreshLoc = Timer();

  void step(unsigned int step, unsigned long time)
  {
    _uintNextStep = step;
    _tStep.Interval(time);
  }
};
