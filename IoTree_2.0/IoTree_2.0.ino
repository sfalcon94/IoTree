/*
This project uses the Adafruit CC3000 and Blynk libraries. 
The CC3000 provides internet connectivity to the Arduino for 
pushing data to the user via the Blynk app. For this reason the
program requires the local access point SSID and password, 
security type, and your Blynk authentication token.

As it stands the project has one flaw: in the event of a power 
outage if the Arduino boots before the router it will fail to 
connect to the internet and require a manual reset. Because it
was a quick project meant to be used only for a couple of weeks 
during Christmas I didn't bother correcting it.
*/

//#define BLYNK_PRINT Serial    // Debugging prints.

// These are the interrupt and control pins for СС3000
#define ADAFRUIT_CC3000_IRQ   3
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10

#include <SPI.h>
#include <Adafruit_CC3000.h>
#include <BlynkSimpleCC3000.h>

//Blynk project token.
char auth[] = "BLYNK AUTHENTICATION TOKEN";

//WiFi credentials.
char ssid[] = "SSID";
char pass[] = "PASSWORD";
int wifi_sec = WLAN_SEC_WPA2; // Change accordingly. Consult CC3000 documentation.

// Time between measurements.
unsigned long int time;
const long interval = 1800000;
unsigned long previous_time = 0;

// Override timer for first measurement.
boolean override = 1;

//Sensor state array.
int sensor[3] = {0, 0, 0};

//Sensor pins.
int power = 8;
int hi = 2;
int mid = 4;
int lo = 6;

//Virtual displays.
WidgetLCD lcd(V0);

//Virtual LEDs
WidgetLED full(V2);
WidgetLED half(V3);
WidgetLED low(V4);
WidgetLED empty(V5);

//Track level to notify upon change.
int old_state = 4;
int current_state = 0;
String states[4] = {"empty", "low", "half", "full"};

//###############################################################//

void setup(){
  Serial.begin(115200);
  pinMode(power, OUTPUT);
  pinMode(hi, INPUT);
  pinMode(mid, INPUT);
  pinMode(lo, INPUT);
  Blynk.begin(auth, ssid, pass, wifi_sec);
}

void loop(){
  Blynk.run();
  if (checkTime() or override){
    //Reset LEDs.
    full.off();
    half.off();
    low.off();
    empty.off();
    //Read sensor values to sensor array.
    readSensor();
    //Sum of sensor array = state.
    current_state = sensor[0] + sensor[1] + sensor[2];
    //Check state, update it and notifications.
    old_state = updateState(old_state, current_state);
    Serial.print(sensor[0]);
    Serial.print(sensor[1]);
    Serial.print(sensor[2]);
    Serial.println("");
    override = 0;
  }
  updateLEDS(current_state); //Turn LEDs on constantly to update users connecting between measurements.
}

//###############################################################//

/*Set virtual button as override for timer. It does not reset 
the interval progress, only updates individual users. Refer to
checkTime() for implementation.*/
BLYNK_WRITE(V6){
  if (param.asInt() == 1){
    override = 1;
  }
  else{
    override = 0;
  }
}

/*Returns whether "interval" time has passed since last
measurement and updates previous measurement time.*/
boolean checkTime(){
  time = millis();
  if (time - previous_time >= interval){
    if (override == 0){
      previous_time = time;
    }
    return 1;
  }
  else{
    return 0;
  }
}

/*Turns sensor 5V pin on and reads each pin into an array. 
Then powers 5V line down.*/
void readSensor(){
  digitalWrite(power, HIGH);
  sensor[0] = digitalRead(hi);
  sensor[1] = digitalRead(mid);
  sensor[2] = digitalRead(lo);
  digitalWrite(power, LOW);
}

/*Returns new state and notifies if it is different from old state.
Pushes notification to user device if the state (water level) changed.
Sends e-mail only if low or empty.*/
int updateState(int old_state, int current_state){
  if (!states[old_state].equals(states[current_state]))
  {
    Blynk.notify("Tree water level: " + states[current_state]);
    if (current_state != 3)
    {
      Blynk.email("Christmas Tree", "I need water! Current level: " + states[current_state]);
    }
  }
  return current_state;
}

void updateLEDS(int state){
   switch (state){
     case 3:
       full.on();
       break;
     case 2:
       half.on();
       break;
     case 1:
       low.on();
       break;
     case 0:
       empty.on();
       break;
     default:
       break;
   }
}

