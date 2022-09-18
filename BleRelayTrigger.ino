#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>

const int PIN = 2;
const int RELAY_UNLOCK = 16;
const int RELAY_LOCK = 17;
const int CUTOFF = -120;

const int LOCK_TIMEOUT = 10;

void setup()
{
  pinMode(PIN, OUTPUT);
  pinMode(RELAY_UNLOCK, OUTPUT);
  pinMode(RELAY_LOCK, OUTPUT);
  BLEDevice::init("");

  // Serial.begin(115200);
  // Serial.println("Starting...");
}

int time_tracker = 0;
bool in_range = true;
bool is_unlock = true;

void loop()
{
  BLEScan *scan = BLEDevice::getScan();
  scan->setActiveScan(true);
  BLEScanResults results = scan->start(1);

  bool blue_in_range = false;
  bool is_blue = false;

  for (int i = 0; i < results.getCount(); i++)
  {
    BLEAdvertisedDevice device = results.getDevice(i);
    if (device.getAddress().toString() == "<BLEDEVICEMAC>" && device.getName() == "<BLEDEVICENAME>")
    {
      is_blue = true;

      if (device.getRSSI() > CUTOFF)
      {
        blue_in_range = true;
        break;
      }
    }
  }

  if (blue_in_range && is_blue)
  {
    if (in_range == false)
    {
      in_range = true;
      time_tracker = 0;
    }

    if (time_tracker <= LOCK_TIMEOUT)
    {
      time_tracker++;
    }
  }
  else
  {
    if (in_range == true)
    {
      in_range = false;
      time_tracker = 0;
    }

    if (time_tracker <= LOCK_TIMEOUT)
    {
      time_tracker++;
    }
    else
    {
      digitalWrite(RELAY_UNLOCK, LOW);
      digitalWrite(RELAY_LOCK, LOW);
    }
  }

  // Serial.printf("Blue: %d, InRange: %d, Tracker: %d, direction: %d\n", is_blue, blue_in_range, time_tracker, in_range);

  digitalWrite(PIN, blue_in_range ? HIGH : LOW);

  if (in_range && time_tracker >= 3 && !is_unlock)
  {
    // Serial.println("UNLOCK TRIGGER");

    digitalWrite(RELAY_UNLOCK, HIGH);
    delay(250);
    digitalWrite(RELAY_UNLOCK, LOW);
    delay(350);
    digitalWrite(RELAY_UNLOCK, HIGH);
    delay(250);
    digitalWrite(RELAY_UNLOCK, LOW);

    is_unlock = true;
  }

  if (!in_range && time_tracker == 7)
  {
    // Serial.println("LOCK");

    digitalWrite(RELAY_LOCK, HIGH);
    delay(300);
    digitalWrite(RELAY_LOCK, LOW);

    is_unlock = false;
  }
}
