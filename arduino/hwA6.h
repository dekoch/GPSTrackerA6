
SoftwareSerial SerialA6(PIN_A6_RX, PIN_A6_TX);

class HardwareA6
{
public:
  void setup()
  {
    pinMode(PIN_A6_POWER, OUTPUT);
    digitalWrite(PIN_A6_POWER, LOW);
    pinMode(PIN_A6_POWER_LOW, OUTPUT);
    digitalWrite(PIN_A6_POWER_LOW, LOW);
    pinMode(PIN_A6_POWER_SIG, OUTPUT);
    digitalWrite(PIN_A6_POWER_SIG, LOW);
    pinMode(PIN_A6_RESET, OUTPUT);
    digitalWrite(PIN_A6_RESET, LOW);
    pinMode(PIN_A6_INT, OUTPUT);
    digitalWrite(PIN_A6_INT, HIGH);

    Reset(RESET_A6, "startup");

    _tWatchDog.Interval(sToMillis(30));
    _tWatchDog.Start();
  }

  void loop()
  {
    // byRxSelect = DEV_A6;

    if (byRxSelect == DEV_A6)
    {
      if (SerialA6.isListening() == false)
      {
        SerialA6.listen();
      }

      // A6 watchdog
      if (_tWatchDog.elapsed())
      {
        byStateA6 = A6command("AT", "\r\nOK\r\n", "yy", A6_CMD_TIMEOUT, 3, NULL);

        if (byStateA6 != OK)
        {
          state.Put(State_ERROR, 101, 2, "A6 WatchDog");
          Reset(RESET_A6, "A6 error");
        }
      }
      // A6 watchdog

      // incoming call
      String strA6Input = "";

      while (SerialA6.available())
      {
        char c = SerialA6.read();
        strA6Input += c;
      }

      if (strA6Input != "")
      {
        Println(strA6Input);

        if (strA6Input.indexOf("\r\nRING\r\n") != -1)
        {
          String strTempNumber = callerNumber();

          if (strTempNumber != "")
          {
            strCallerNumber = strTempNumber;
            Println("Incoming Call from : " + strCallerNumber);
          }
          else if (strTempNumber == "")
          {
            strCallerNumber = settings.DefaultNumber;
          }

          delay(5000);
          // Hangup
          A6command("ATH", "\r\nOK\r\n", "yy", A6_CMD_TIMEOUT, 1, NULL);
        }

        _tWatchDog.Reset();
      }
      // incoming call

      // send SMS
      if (byCommandA6 == A6_SENDSMS)
      {
        byCommandA6 = A6_UNDEFINED;

        byStateA6 = sendSMS(strCallerNumber, createSMS(locLastFix));

        /*Serial.println("sending SMS");
        byStateA6 = OK;*/
      }
      // send SMS
    }
  }

private:
  byte byState = NOTOK;

  Timer _tWatchDog = Timer();

  unsigned int uintA6ResetCnt = 0;
  unsigned int uintSMSCnt = 0;
  unsigned int uintSMSErrorCnt = 0;

  String callerNumber()
  {
    String strNumber = "";
    String strResponse = "";

    // Issue the command and wait for the response.
    A6command("AT+CLCC", "+CLCC:", "+ERROR", A6_CMD_TIMEOUT, 2, &strResponse);

    // Parse the response if it contains a valid +CLCC:
    if (strResponse.indexOf("+CLCC:") != -1)
    {
      strResponse.remove(0, strResponse.indexOf("\"") + 1);
      strResponse.remove(strResponse.lastIndexOf("\""), strResponse.lastIndexOf("\"") - strResponse.length());

      strNumber = "00" + strResponse;
    }

    return strNumber;
  }

  String createSMS(location loc)
  {
    String strRetVal = "VIN:" + String(settings.Vin) + "\r\n";
    strRetVal += loc.date + " " + loc.time + "\r\n";
    // link
    strRetVal += "http://maps.google.com/maps?q=" + String(loc.lat, 6) + "," + String(loc.lng, 6) + "\r\n";
    strRetVal += "SV:" + String(loc.sv) + " SP:" + String(loc.speed, 1) + "km/h" + "\r\n";
    strRetVal += "lat:" + String(loc.lat, 6) + " lng:" + String(loc.lng, 6) + "\r\n";
    // stats
    strRetVal += "R" + String(uintA6ResetCnt) + ",S" + String(uintSMSCnt) + ",SE" + String(uintSMSErrorCnt);

    return strRetVal;
  }

  byte sendSMS(String number, String text)
  {
    byte byState;

    if (number.length() == 0)
    {
      Println("no number");
      return NOTOK;
    }

    if (text.length() > 159)
    {
      Println("SMS was to long");
      text.remove(160, text.length() - 160);
    }

    Println(String(text.length()));

    Println("Sending SMS to " + number);
    Println(text);

    uintSMSCnt += 1;

    byState = A6command("AT+CMGF=1", "\r\nOK\r\n", "yy", A6_CMD_TIMEOUT, 2, NULL);

    //byState = NOTOK;

    if (byState == OK)
    {
      SerialA6.flush();
      SerialA6.print("AT+CMGS=\"");
      SerialA6.print(number);
      SerialA6.write(0x22);
      SerialA6.write(0x0D); // hex equivalent of Carraige return
      SerialA6.write(0x0A); // hex equivalent of newline
      delay(2000);
      SerialA6.print(text);
      delay(500);
      SerialA6.println(char(26)); //the ASCII code of the ctrl+z is 26
      delay(5000);
      Println("SMS sent");
    }
    else
    {
      uintSMSErrorCnt += 1;
    }

    return byState;
  }

  // Issue a command
  byte A6command(const char *command, const char *resp1, const char *resp2, int timeOut, int repetitions, String *response)
  {
    byte retVal = NOTOK;
    byte count = 0;

    // Get rid of any buffered output.
    SerialA6.flush();

    while (count < repetitions && retVal != OK)
    {
      Print("Issuing command: ");
      Println(command);

      SerialA6.println(command);

      retVal = A6waitFor(resp1, resp2, timeOut, response);

      count++;
    }

    return retVal;
  }

  // Wait for responses
  byte A6waitFor(const char *resp1, const char *resp2, int timeOut, String *response)
  {
    int count = 0;
    String reply = "";
    byte retVal = 99;

    Timer tTimeOut = Timer();
    tTimeOut.Interval(timeOut);
    tTimeOut.Start();

    do
    {
      if (SerialA6.available())
      {
        char c = SerialA6.read();
        reply += c;
      }
    } while ((((reply.indexOf(resp1) + reply.indexOf(resp2)) == -2) || (reply.endsWith("\r\n") == false)) && (tTimeOut.elapsed() == false));

    if (reply != "")
    {
      Println("Reply in " + String(tTimeOut.elapsedTime()) + "ms: ");
      Println(reply);
    }

    if (response != NULL)
    {
      *response = reply;
    }

    if (tTimeOut.elapsed())
    {
      retVal = TIMEOUT;
      Println("Timed out");
      Println(reply);
    }
    else
    {
      if (reply.indexOf(resp1) + reply.indexOf(resp2) > -2)
      {
        Println("Reply OK");
        retVal = OK;
      }
      else
      {
        Println("Reply NOTOK");
        retVal = NOTOK;
      }
    }

    return retVal;
  }

  void Reset(int resetmode, String cause)
  {
    Serial.println("Reset " + String(resetmode) + " " + cause);

    switch (resetmode)
    {
    case RESET_FULL:
      Reset(RESET_A6, cause);
      Reset(RESET_CPU, cause);
      break;

    case RESET_CPU:

      break;

    case RESET_A6:
      Println("A6 reset");

      uintA6ResetCnt += 1;

      SerialA6.begin(19200);

      digitalWrite(PIN_A6_POWER_SIG, LOW);
      digitalWrite(PIN_A6_RESET, HIGH);

      delay(2000);
      Println("A6 power on");

      digitalWrite(PIN_A6_POWER_SIG, HIGH);
      digitalWrite(PIN_A6_RESET, LOW);

      unsigned int uintAttempts = 0;

      Println("A6 115200");
      SerialA6.begin(115200);
      do
      {
        byState = A6command("AT", "\r\nOK\r\n", "yy", A6_CMD_TIMEOUT, 1, NULL);

        uintAttempts += 1;

      } while ((byState != OK) && (uintAttempts < 30));

      // switch A6 to 19200 baud
      byState = A6command("AT+IPR=19200", "\r\nOK\r\n", "yy", A6_CMD_TIMEOUT, 3, NULL);

      uintAttempts = 0;

      Println("A6 19200");
      SerialA6.begin(19200);
      do
      {
        byState = A6command("AT", "\r\nOK\r\n", "yy", A6_CMD_TIMEOUT, 1, NULL);

        uintAttempts += 1;

      } while ((byState != OK) && (uintAttempts < 30));

      digitalWrite(PIN_A6_POWER_SIG, LOW);

      if (byState != OK)
      {
        state.Put(State_ERROR, 101, 1, "A6 reset");
      }

      // restores factory profile
      A6command("AT&F0", "\r\nOK\r\n", "yy", A6_CMD_TIMEOUT, 2, NULL);
      // Request manufacturer specific information about the TA
      A6command("ATI", "\r\nOK\r\n", "yy", A6_CMD_TIMEOUT, 1, NULL);
      // identification of the SIM card
      A6command("AT+CCID", "\r\nOK\r\n", "yy", A6_CMD_TIMEOUT, 1, NULL);
      // query the network register status
      A6command("AT+CREG?", "\r\nOK\r\n", "yy", A6_CMD_TIMEOUT, 1, NULL);
      // query the network quality of the signal
      A6command("AT+CSQ", "\r\nOK\r\n", "yy", A6_CMD_TIMEOUT, 1, NULL);
      break;
    }
  }
};
