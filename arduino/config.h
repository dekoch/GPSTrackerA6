
struct struSettings
{
  char *DefaultNumber;
  char *Vin;
} settings;

class Settings
{
public:
  bool setup()
  {
    settings.DefaultNumber = "00491234";
    settings.Vin = "1234";
  }
};
