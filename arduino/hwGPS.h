
SoftwareSerial SerialGPS(PIN_GPS_RX, PIN_GPS_TX);

class HardwareGPS
{
public:
  void setup()
  {
    pinMode(PIN_GPS_POWER, OUTPUT);
    digitalWrite(PIN_GPS_POWER, LOW);
    pinMode(PIN_GPS_POWER_LOW, OUTPUT);
    digitalWrite(PIN_GPS_POWER_LOW, LOW);

    SerialGPS.begin(19200);
    SerialGPS.stopListening();

    _tPowerOff.Interval(mToMillis(5));
    _tPowerOff.Start();
  }

  void loop()
  {
    //byRxSelect = DEV_GPS;

    if (byRxSelect == DEV_GPS)
    {
      digitalWrite(PIN_GPS_POWER, HIGH);

      if (SerialGPS.isListening() == false)
      {
        SerialGPS.listen();
      }

      byStateGPS = readGPS(locCurrent);

      Serial.println("loc current");
      DisplayLocationInfo(locCurrent);
      Serial.println("loc last fix");
      DisplayLocationInfo(locLastFix);

      _tPowerOff.Reset();
    }

    if (_tPowerOff.elapsed())
    {
      digitalWrite(PIN_GPS_POWER, LOW);
    }
  }

private:
  Timer _tPowerOff = Timer();

  byte readGPS(location &loc)
  {
    TinyGPSPlus gps;
    byte byRetVal = OK;
    String strRX = "";

    SerialGPS.flush();

    Timer tTimeOut = Timer();
    tTimeOut.Interval(2500);
    tTimeOut.Start();

    do
    {
      if (SerialGPS.available())
      {
        char c = SerialGPS.read();
        strRX += c;
      }
    } while (tTimeOut.elapsed() == false);

    //Serial.println(strRX);

    for (uint16_t i = 0; i < strRX.length(); i++)
    {
      gps.encode(strRX.charAt(i));
    }

    // date
    if (gps.date.isValid())
    {
      loc.date = String(gps.date.month()) + "/" + String(gps.date.day()) + "/" + String(gps.date.year());
    }
    else
    {
      loc.date = "INVALID";

      byRetVal = NOTOK;
    }
    // time
    if (gps.time.isValid())
    {
      loc.time = "";

      if (gps.time.hour() < 10)
        loc.time += "0";
      loc.time += String(gps.time.hour()) + ":";

      if (gps.time.minute() < 10)
        loc.time += "0";
      loc.time += String(gps.time.minute()) + ":";

      if (gps.time.second() < 10)
        loc.time += "0";
      loc.time += String(gps.time.second()) + ".";

      if (gps.time.centisecond() < 10)
        loc.time += "0";
      loc.time += String(gps.time.centisecond());
    }
    else
    {
      loc.time = "INVALID";

      byRetVal = NOTOK;
    }
    // location
    if (gps.location.isValid())
    {
      loc.lat = gps.location.lat();
      loc.lng = gps.location.lng();
    }
    else
    {
      loc.lat = 0.0;
      loc.lng = 0.0;

      byRetVal = NOTOK;
    }

    loc.speed = gps.speed.kmph();
    loc.sv = gps.satellites.value();

    loc.valid = false;

    if (byRetVal == OK)
    {
      loc.valid = true;
    }

    return byRetVal;
  }

  void DisplayLocationInfo(location loc)
  {
    Print(loc.date + " " + loc.time);
    Print("  Location: lat:" + String(loc.lat, 6) + " lng:" + String(loc.lng, 6));
    Print("  Speed:" + String(loc.speed, 1) + "km/h");
    Print("  SV:" + String(loc.sv));
    Println("");
  }
};
