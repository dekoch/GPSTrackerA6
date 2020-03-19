
#define DEV_NONE 0
#define DEV_A6 1
#define DEV_GPS 2

#define RESET_FULL 1
#define RESET_CPU 2
#define RESET_A6 3

#define OK 1
#define NOTOK 0
#define UNDEFINED -1
#define FAILURE -2
#define TIMEOUT -3

#define A6_UNDEFINED 0
#define A6_SENDSMS 1

#define A6_CMD_TIMEOUT 1000

bool boDebugger = true;
State state = State();
bool boError = false;

unsigned int uintStep = 0;

int intTest = 0;

enum enumState
{
  State_Undefined,
  State_Starting,
  State_Ready,
  State_Loop,
  State_Standbye
} stateBoard = State_Undefined;

unsigned long ulTimeCycle = 0;

byte byRxSelect = DEV_NONE;

byte byStateA6 = UNDEFINED;
byte byStateGPS = UNDEFINED;

byte byCommandA6 = A6_UNDEFINED;

struct location
{
  String date;
  String time;
  double lat;
  double lng;
  double speed;
  int sv;
  bool valid;
};

location locCurrent;
location locLastFix;

String strCallerNumber = "";

void Print(String str)
{
  if (boDebugger)
  {
    Serial.print(str);
  }
}

void Println(String str)
{
  if (boDebugger)
  {
    Serial.println(str);
  }
}