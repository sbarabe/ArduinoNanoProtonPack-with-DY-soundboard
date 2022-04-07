//Ghostbuster_proton_pack_V1.0
// https://github.com/sbarabe/ArduinoNanoProtonPack-with-DY-soundboard
// (Inspired from https://github.com/CountDeMonet/ArduinoProtonPack)
// but modified for DY-SV8F soundboard and MAX7219 shift register for bar graph
// Features included :
// - DY-SV8F sound board with flash : 
//      Tracks called by path and file names, you should keep them as short as possible, like "/xx.mp3"
//      You can add themes tracks fill the empty flash memory left.
//      There is 3 themes tracks in this exemple.
//      Themes track should be name 16.mp3, 17.mp3, etc.
//      You need to change the "themes pack state" cases 
//      and values if your number of themes tracks number is not 3...
// - wand BAR GRAPH controled by shift register MAX7219 with the LedControl librairy
// - There is 2 push buttons for volume increase/decrease, you can ignore them and set the volume in setup loop.
// - There is a function to auto power off on "POWER_ON_PIN" the pack with a transistor/mosfet circuit, but it can be ignore is the circuit is absent.
// - There is the following regular switches and buttons :
//      Startup switch :  power on/shutting down the pack by calling starting up / shutting down sequence
//      Safety switch :   safety ON means non shooting, Safety OFF means shooting possible
//      Theme switch :    for playing themes song instead of the regular pack function.
//                        You can switch themes by toggling ON/OFF
//      Fire button :     It does what it says...

//********************************************
// MAIN LOOP IS AT THE BOTTOM OF THIS SKETCH
//********************************************
    

#include <QueueArray.h>
#include <Wire.h> // Include the I2C library (required)
//#include <Button.h> // Help to simplify push buttons managing
// for the sound board
#include <SoftwareSerial.h>
#include "DYPlayerArduino.h" //for the DY-SV8F audio board
// For WS2812 leds
#include <Adafruit_NeoPixel.h>
// For BAR GRAPH with the MAX7219 shift register
#include "LedControl.h" // Include LedControl for the bargraph


//Arduino pins definition
#define DY_RX             2     // for serial with DY-SV8F Player (Tx pin)
#define DY_TX             3     // for serial with DY-SV8F Player (Rx pin)
#define DY_BUSY           4     // DY-SV8F Player "BUSY" pin
#define NEO_POWER         5     // for cyclotron and powercell
#define NEO_WAND          8     // for nose of wand
#define BARGRAPH_DATA_IN  9     // connected to the MAX7219 DataIn (pin 1)
#define BARGRAPH_CLK      10    // connected to the MAX7219 CLK (pin 13)
#define BARGRAPH_LOAD     11    // connected to MAX7219 LOAD (pin 12)
#define POWER_ON_PIN      12    // must be HIGH to keep power on with the mosfet and transistor auto off circuit
// inputs for switches and buttons
#define THEME_SWITCH      A0
#define STARTUP_SWITCH    A1
#define SAFETY_SWITCH     A2
#define FIRE_BUTTON       A3
#define VOLUME_UP_PIN     A4
#define VOLUME_DOWN_PIN   A5



//  Bar graph with shift register MAX7219 :
//  Now we need a LedControl to work with the shift register : LedControl(DataIn, Clock, Load, Adress)
//  Nano pin 9 is connected to the MAX7219 DataIn (pin 1)
//  Nano pin 10 is connected to the MAX7219 CLK (pin 13)
//  Nano pin 11 is connected to MAX7219 LOAD (pin 12)
//  We have only a single MAX7219, so adress "0".
//  Nano pins could change in your project
LedControl bargraph = LedControl(BARGRAPH_DATA_IN, BARGRAPH_CLK, BARGRAPH_LOAD, 0);
// bargraph helper variables
int8_t bargraphSeqOneCurrent = 1;  // current led in sequence 1
#define bargraphTotalLeds (int8_t)10  // total number of leds in bar graph

// neopixel pins / setup
// for cyclotron and powercell
#define totalPackLedsNumber (uint8_t)43
#define totalWandLedsNumber (uint8_t)12
//Leds brightness, 0 to 255,  to be use has a factor in set led fucntions to adjust brightness
#define packLedsBrightness  (uint8_t)75
#define wandLedsBrightness  (uint8_t)75
#define noseLedsBrightness  (uint8_t)255
Adafruit_NeoPixel packLeds = Adafruit_NeoPixel(totalPackLedsNumber, NEO_POWER, NEO_GRB + NEO_KHZ800);
// for nose of wand
Adafruit_NeoPixel wandLeds = Adafruit_NeoPixel(totalWandLedsNumber, NEO_WAND, NEO_GRB + NEO_KHZ800);

// LED indexes into the neopixel packLeds chain for the cyclotron Powercell. Each stick has 8 neopixels for a total of
// 8 (just one sitck) with an index starting at 0. This offset are because my powercell window only shows 8 leds. If you can show more
// change the offset index and powercell count to get more or less lit.
#define powercellLedCount     (int8_t)8           // total number of led's in the animation
#define powercellIndexOffset  (int8_t)0        // first led offset into the led chain for the animation

// packLeds leds index
// These are the indexes for the led's on the chain. Each jewel has 7 LEDs. If you are using a single neopixel or
// some other neopixel configuration you will need to update these indexes to match where things are in the chain
#define ventStart   (int8_t)0
#define ventEnd     (int8_t)6
#define c1Start     (int8_t)7
#define c1End       (int8_t)13
#define c2Start     (int8_t)14
#define c2End       (int8_t)20
#define c3Start     (int8_t)21
#define c3End       (int8_t)27
#define c4Start     (int8_t)28
#define c4End       (int8_t)34
#define pcStart     (int8_t)35
#define pcEnd       (int8_t)42
#define LastPackLed (int8_t)42

// wandLeds leds index
// These are the indexes for the led's on the chain. Each jewel has 7 LEDs. If you are using a single neopixel or
// some other neopixel configuration you will need to update these indexes to match where things are in the chain
#define backOrange  (int8_t)0
#define body        (int8_t)1
#define topWhite    (int8_t)2
#define topYellow   (int8_t)3
#define sloblow     (int8_t)4
#define noseStart   (int8_t)5
#define noseEnd     (int8_t)11
#define lastWandLed (int8_t)11

// DY-SV8F Player definition
SoftwareSerial SoftSerial(DY_RX, DY_TX);
DY::Player player(&SoftSerial);
bool playing = false; //variable for DY-SV8F activity status
#define volumeMin (int8_t)0
#define volumeMax (int8_t)25
int8_t volume = 20;
int8_t prevVolume = volume ;
//to switch btween themes track
int8_t themeFlag = 1;
// theme 1 = Ghostbuster Track
// theme 2 = Thriller Track
// theme 3 = Party up track
//Define push buttons for volume setting
bool volumeDownButton = false;
bool volumeUpButton = false;

// audio track files path definition for soundboard DY-SV8F
// (they are played in the order they have been put on the flash drive, not by trackname or number)
const char  startupTrack[]       = "/01.mp3";
const char  blastTrack[]         = "/02.mp3";
const char  endOneTrack[]        = "/03.mp3";
const char  endTwoTrack[]        = "/04.mp3";
const char  shutdownTrack[]      = "/05.mp3";
const char  loadingTrack[]       = "/06.mp3";
const char  unloadingTrack[]     = "/07.mp3";
const char  ventTrack[]          = "/08.mp3";
// Dialogue tracks for Hotfiring ending if dialogue tracks option is set to TRUE
// See "available option below"
const char  texTrack[]           = "/09.mp3";
const char  choreTrack[]         = "/10.mp3";
const char  toolsTrack[]         = "/11.mp3";
const char  listenTrack[]        = "/12.mp3";
const char  thatTrack[]          = "/13.mp3";
const char  neutronizedTrack[]   = "/14.mp3";
const char  boxTrack[]           = "/15.mp3";
// Themes tracks
// You can choose and add your own tracks, but update should be made to 
// the "theme stage" in the main loop to fit the number of themes tracks
const char  ghostbustersTrack[]  = "/16.mp3";
const char  thrillerTrack[]      = "/17.mp3";
const char  partyupTrack[]       = "/18.mp3";

// this queue holds a shuffled list of dialog tracks we can pull from so we don't
// play the same ones twice
QueueArray <int> dialogQueue;
#define numDialog (int)7


// ##############################
// available options
// ##############################
const bool useGameCyclotronEffect = true;   // set this to true to get the fading previous cyclotron light in the idle sequence
const bool useCyclotronFadeInEffect = true; // Instead of the yellow alternate flashing on boot/vent this fades the cyclotron in from off to red
const bool useDialogTracks = true;          // set to true if you want the dialog tracks to play after firing for 5 seconds


// Possible Pack states for main loop switch cases
int8_t packState = 1 ;      // Initial pack state in the main loop
int8_t prevPackState = 1;   // State tracking for some sequences
int8_t packStateFlag = 0;   // state flag to implement different state stages in main loop
#define themes            0  // themes playing mode
#define poweredDown       1  // the pack is powered down
#define startingUp        2  // pack is in process of power booting
#define idlingSafetyOn    3  // has the pack booted up and is idling with safety on
#define idlingSafetyOff   4  // has the pack booted up and is idling with safety off ready to fire
#define isCharging        5  // Safety removed and pack charging
#define isUnloading      6  // Safety is put on and pack uncharging
#define isFiring          7  // firing state
#define isFiringHot      8  // firing state becoming Hot (also triggering dialogs tracks)
#define isFiringWarning   9  // firing state with warning sound (triggering venting)
#define endFiring        10  // end of firing sequence
#define venting          11  // the pack is venting
#define shuttingDown     12  // the pack is in the process of shutting down

// physical switch states to be update with digitalRead
bool theme_switch = false;
bool startup_switch = false;
bool safety_switch = false;
bool fire_button = false;
bool front_button_1 = false;
bool front_button_2 = false;

// General timer tracker
unsigned long previousMillis = 0 ;    // general time tracker for functions timers and delays


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Arduino SETUP LOOP
void setup() {
  //set the auto power off pin to high to maintain power on
  pinMode(POWER_ON_PIN, OUTPUT);
  digitalWrite(POWER_ON_PIN, true);

  // DY-SV8F audio board setup
  // set board BUSY pin
  pinMode(DY_BUSY, INPUT);
  player.begin(); // initialize player
  delay(500);
  player.setEq(DY::Eq::Pop);
  delay(100);
  player.setCycleMode(DY::PlayMode::OneOff); //set cylce mode set to "One Off"
  delay(100);
  player.setVolume(volume);

  // configure powercell/cyclotron
  packLeds.begin();
  delay(200);
  packLeds.setBrightness(255);
  packLeds.show(); // Initialize all pixels to 'off'

  // configure wand lights
  wandLeds.begin();
  delay(200);
  wandLeds.setBrightness(255);
  wandLeds.show();

  // set the modes for the switches/buttons
  pinMode(THEME_SWITCH, INPUT_PULLUP);
  pinMode(STARTUP_SWITCH, INPUT_PULLUP);
  pinMode(SAFETY_SWITCH, INPUT_PULLUP);
  pinMode(FIRE_BUTTON, INPUT_PULLUP);
  pinMode(VOLUME_UP_PIN, INPUT_PULLUP);
  pinMode(VOLUME_DOWN_PIN, INPUT_PULLUP);
  //  volumeUpButton.begin();//Increase volume
  //  volumeDownButton.begin();//Decrease volume

  // configuration for the bargraph LED's
  //wake up the MAX72XX from power-saving mode
  bargraph.shutdown(0, false);
  //set a max brightness for the Leds (min =0, max = 15)
  bargraph.setIntensity(0, 15);
  // set everything off initially
  bargraph_clear_leds();
}
// End Arduino SETUP LOOP
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////
//************** Audio Board Helper Functions *************

// use fo a small delay for debouncing in audioplayback calling
// because there is a small lag between teh command
// and the status update from the audioboard
#define audioDelay (uint16_t)500

// Aufio playing function to play a track by name on the audio board
void playAudio( char trackpath[]) {
  player.playSpecifiedDevicePath(DY::Device::Flash, trackpath);
}

// Audio playing function for the dialog tracks after the "Is Firing Hot" state.
void playDialogTrack() {
  // if the queue is empty reseed it
  if ( dialogQueue.isEmpty() ) {
    for (int i = 1; i <= numDialog; i++) {
      dialogQueue.enqueue(i);
    }
  }
  switch (dialogQueue.dequeue()) {
    case 1:
      playAudio(texTrack);
      break;
    case 2:
      playAudio(listenTrack);
      break;
    case 3:
      playAudio(choreTrack);
      break;
    case 4:
      playAudio(boxTrack);
      break;
    case 5:
      playAudio(thatTrack);
      break;
    case 6:
      playAudio(neutronizedTrack);
      break;
    case 7:
      playAudio(toolsTrack);
      break;
    default:
      playAudio(endTwoTrack);
      break;
  }
}

// END Audio Board Helpers and Functions
///////////////////////////////////////////////



///////////////////////////////////////////////////
//LEDS color trackers resets into limits
int16_t trackerLimits(int16_t tracker) {
  if (tracker > 255) {
    tracker = 255;
  }
  else if (tracker < 0) {
    tracker = 0;
  }
  return tracker;
}
//END LEDS color trackers resets into limits
////////////////////////////////////////////


//////////////////////////////////////////////////////////
//************** Wand Light Helpers and functions*********
unsigned long prevFlashMillis = 0; // last time we changed a flashing wand light
unsigned long prevFadeMillis = 0;  // last time we changed a fadinf wand light
bool flashState = false;
int16_t bodyRedTracker = 0;
int16_t bodyGreenTracker = 0;
int16_t bodyBlueTracker = 0;

#define wandFastFlashInterval     (uint16_t)100 // interval at which we flash the top led on the wand
#define wandMediumFlashInterval   (uint16_t)500 // interval at which we flash the top led on the wand


void setWandLightState(int8_t lednum, int8_t state) {

  switch ( state ) {
    case 0: // set led red
      wandLeds.setPixelColor(lednum, 255, 0, 0);
      break;
    case 1: // set led white
      wandLeds.setPixelColor(lednum, 255, 255, 255);
      break;
    case 2: // set led orange
      wandLeds.setPixelColor(lednum, 255, 127, 0);
      break;
    case 3: // set led blue
      if (lednum == body) {// set body led tracker to blue
        bodyRedTracker = 0;
        bodyGreenTracker = 0;
        bodyBlueTracker = 255;
      }
      wandLeds.setPixelColor(lednum, 0, 0, 255);
      break;
    case 4: // set led off
      wandLeds.setPixelColor(lednum, 0, 0, 0);
      break;
    case 5: // fast white flashing
      if ((millis() - prevFlashMillis) >= wandFastFlashInterval) {
        prevFlashMillis = millis();
        if ( flashState == false ) {
          wandLeds.setPixelColor(lednum, 255, 255, 255);
          flashState = true;
        } else {
          wandLeds.setPixelColor(lednum, 0, 0, 0);
          flashState = false;
        }
      }
      break;
    case 6: // slower orange flashing
      if ((millis() - prevFlashMillis) >= wandMediumFlashInterval) {
        prevFlashMillis = millis();
        if ( flashState == false ) {
          wandLeds.setPixelColor(lednum, 255, 127, 0);
          flashState = true;
        } else {
          wandLeds.setPixelColor(lednum, 0, 0, 0);
          flashState = false;
        }
      }
      break;
    case 7: // medium red flashing
      if ((millis() - prevFlashMillis) >= wandMediumFlashInterval) {
        prevFlashMillis = millis();
        if ( flashState == false ) {
          wandLeds.setPixelColor(lednum, 255, 0, 0);
          flashState = true;
        } else {
          wandLeds.setPixelColor(lednum, 0, 0, 0);
          flashState = false;
        }
      }
      break;
    case 8: // fast red flashing
      if ((millis() - prevFlashMillis) >= wandFastFlashInterval) {
        prevFlashMillis = millis();
        if ( flashState == false ) {
          wandLeds.setPixelColor(lednum, 255, 0, 0);
          flashState = true;
        } else {
          wandLeds.setPixelColor(lednum, 0, 0, 0);
          flashState = false;
        }
      }
      break;
    case 9://BODY light fade in to blue
      if ((millis() - prevFadeMillis) >= 8) {
        prevFadeMillis = millis();
        if (bodyBlueTracker <= 254) {
          wandLeds.setPixelColor(lednum, bodyRedTracker, bodyGreenTracker, bodyBlueTracker);
          bodyRedTracker--;
          bodyGreenTracker--;
          bodyBlueTracker++;
        }
        else {
          bodyRedTracker = 0;
          bodyGreenTracker = 0;
          bodyBlueTracker = 255;
          wandLeds.setPixelColor(lednum, bodyRedTracker, bodyGreenTracker, bodyBlueTracker);
          prevFadeMillis = 0;
        }
      }
      break;
    case 10://BODY Fade out
      if ((millis() - prevFadeMillis) >= 8) {
        prevFadeMillis = millis();
        if (bodyBlueTracker <= 254) {
          wandLeds.setPixelColor(lednum, bodyRedTracker, bodyGreenTracker, bodyBlueTracker);
          bodyRedTracker--;
          bodyGreenTracker--;
          bodyBlueTracker++;
        }
        else {
          bodyRedTracker = 0;
          bodyGreenTracker = 0;
          bodyBlueTracker = 0;
          wandLeds.setPixelColor(lednum, bodyRedTracker, bodyGreenTracker, bodyBlueTracker);
          prevFadeMillis = 0;
        }
      }
      break;
    case 11://BODY light blue  to purple
      if ((millis() - prevFadeMillis) >= 15) {
        prevFadeMillis = millis();
        if (bodyRedTracker < 85) {
          wandLeds.setPixelColor(lednum, bodyRedTracker, bodyGreenTracker, bodyBlueTracker);
          if (bodyRedTracker < 85) {
            bodyRedTracker++;
          }
          if (bodyGreenTracker > 0) {
            bodyGreenTracker--;
          }
          if (bodyBlueTracker > 170) {
            bodyBlueTracker--;
          }
        }
        else {
          bodyRedTracker = 85;
          bodyGreenTracker = 0;
          bodyBlueTracker = 170;
          wandLeds.setPixelColor(lednum, bodyRedTracker, bodyGreenTracker, bodyBlueTracker);
          prevFadeMillis = 0;
        }
      }
      break;
    case 12://BODY light purple  to blue
      if ((millis() - prevFadeMillis) >= 20) {
        prevFadeMillis = millis();
        if (bodyRedTracker > 1) {
          wandLeds.setPixelColor(lednum, bodyRedTracker, bodyGreenTracker, bodyBlueTracker);
          if (bodyRedTracker > 0) {
            bodyRedTracker--;
          }
          if (bodyGreenTracker > 0) {
            bodyGreenTracker--;
          }
          if (bodyBlueTracker < 255) {
            bodyBlueTracker++;
          }
        }
        else {
          bodyRedTracker = 0;
          bodyGreenTracker = 0;
          bodyBlueTracker = 255;
          wandLeds.setPixelColor(lednum, bodyRedTracker, bodyGreenTracker, bodyBlueTracker);
          prevFadeMillis = 0;
        }
      }
      break;
  }

  wandLeds.show();
  bodyRedTracker = trackerLimits(bodyRedTracker);
  bodyGreenTracker = trackerLimits(bodyGreenTracker);
  bodyBlueTracker = trackerLimits(bodyBlueTracker);
}
// END Wand Light Helpers and functions
/////////////////////////////////////////////////////////


///////////////////////////////////////////////
//**** Vent Light helpers and functions********

//uint16_t ventFadeInTracker = 0;
//uint16_t ventFadeOutTracker = 0;
int16_t ventBlueTracker = 0;
int16_t ventRedTracker = 0;
int16_t ventGreenTracker = 0;
unsigned long ventPrevFadingMillis = 0;



void setVentLightState(int8_t startLed, int8_t endLed, int8_t state ) {
  ventRedTracker = trackerLimits(ventRedTracker);
  ventGreenTracker = trackerLimits(ventGreenTracker);
  ventBlueTracker = trackerLimits(ventBlueTracker);
  switch ( state ) {
    case 0: // set all leds to purple
      ventRedTracker = 85;
      ventGreenTracker = 0;
      ventBlueTracker = 170;
      for (int8_t i = startLed; i <= endLed; i++) {
        packLeds.setPixelColor(i, ventRedTracker, ventGreenTracker, ventBlueTracker);
      }
      break;
    case 1: // set all leds to blue
      ventRedTracker = 0;
      ventGreenTracker = 0;
      ventBlueTracker = 255;
      for (int8_t i = startLed; i <= endLed; i++) {
        packLeds.setPixelColor(i, ventRedTracker, ventGreenTracker, ventBlueTracker);
      }
      break;
    case 2: // set all leds off
      ventRedTracker = 0;
      ventGreenTracker = 0;
      ventBlueTracker = 0;
      for (int8_t i = startLed; i <= endLed; i++) {
        packLeds.setPixelColor(i, ventRedTracker, ventGreenTracker, ventBlueTracker);
      }
      break;
    case 3://fade in blue
      if ((millis() - ventPrevFadingMillis) >= 7) {
        ventPrevFadingMillis = millis();
        if (ventBlueTracker <= 254) {
          for (int8_t i = startLed; i <= endLed; i++) {
            packLeds.setPixelColor(i, ventRedTracker, ventGreenTracker, ventBlueTracker);
          }
          ventRedTracker--;
          ventGreenTracker--;
          ventBlueTracker++;
        }
        else {
          ventRedTracker = 0;
          ventGreenTracker = 0;
          ventBlueTracker = 255;
          for (int8_t i = startLed; i <= endLed; i++) {
            packLeds.setPixelColor(i, ventRedTracker, ventGreenTracker, ventBlueTracker);
          }
          ventPrevFadingMillis = 0;
        }
      }
      break;
    case 4://fade out
      if ((millis() - ventPrevFadingMillis) >= 7) {
        ventPrevFadingMillis = millis();
        if (ventRedTracker > 0 || ventGreenTracker > 0 || ventBlueTracker > 0) {
          for (int8_t i = startLed; i <= endLed; i++) {
            packLeds.setPixelColor(i, ventRedTracker, ventGreenTracker, ventBlueTracker);
          }
          ventRedTracker--;
          ventGreenTracker--;
          ventBlueTracker--;
        }
        else {
          ventRedTracker = 0;
          ventGreenTracker = 0;
          ventBlueTracker = 0;
          for (int8_t i = startLed; i <= endLed; i++) {
            packLeds.setPixelColor(i, ventRedTracker, ventGreenTracker, ventBlueTracker);
          }
          ventPrevFadingMillis = 0;
        }
      }
      break;
    case 5://moving from blue to purple
      if ((millis() - ventPrevFadingMillis) >= 12) {
        ventPrevFadingMillis = millis();
        if (ventRedTracker < 85) {
          for (int8_t i = startLed; i <= endLed; i++) {
            packLeds.setPixelColor(i, ventRedTracker, ventGreenTracker, ventBlueTracker);
          }
          if (ventRedTracker < 85) {
            ventRedTracker++;
          }
          if (ventGreenTracker > 0) {
            ventGreenTracker--;
          }
          if (ventBlueTracker > 170) {
            ventBlueTracker--;
          }
        }
        else {
          ventRedTracker = 85;
          ventGreenTracker = 0;
          ventBlueTracker = 170;
          for (int8_t i = startLed; i <= endLed; i++) {
            packLeds.setPixelColor(i, ventRedTracker, ventGreenTracker, ventBlueTracker);
          }
          ventPrevFadingMillis = 0;
        }
      }
      break;
    case 6://moving purple to blue
      if ((millis() - ventPrevFadingMillis) >= 12) {
        ventPrevFadingMillis = millis();
        if (ventRedTracker > 0) {
          for (int8_t i = startLed; i <= endLed; i++) {
            packLeds.setPixelColor(i, ventRedTracker, ventGreenTracker, ventBlueTracker);
          }
          if (ventRedTracker > 0) {
            ventRedTracker--;
          }
          if (ventGreenTracker > 0) {
            ventGreenTracker--;
          }
          if (ventBlueTracker < 255) {
            ventBlueTracker++;
          }
        }
        else {
          ventRedTracker = 0;
          ventGreenTracker = 0;
          ventBlueTracker = 255;
          for (int8_t i = startLed; i <= endLed; i++) {
            packLeds.setPixelColor(i, ventRedTracker, ventGreenTracker, ventBlueTracker);
          }
          ventPrevFadingMillis = 0;
        }
      }
      break;
    case 7://move from purple to red
      if ((millis() - ventPrevFadingMillis) >= 80) {
        ventPrevFadingMillis = millis();
        if (ventRedTracker <= 254) {
          for (int8_t i = startLed; i <= endLed; i++) {
            packLeds.setPixelColor(i, ventRedTracker, ventGreenTracker, ventBlueTracker);
          }
          ventRedTracker++;
          ventGreenTracker--;
          ventBlueTracker--;
        }
        else {
          ventRedTracker = 255;
          ventGreenTracker = 0;
          ventBlueTracker = 0;
          for (int8_t i = startLed; i <= endLed; i++) {
            packLeds.setPixelColor(i, ventRedTracker, ventGreenTracker, ventBlueTracker);
          }
          ventPrevFadingMillis = 0;
        }
      }
      break;
    case 8://move from red to blue
      if ((millis() - ventPrevFadingMillis) >= 15) {
        ventPrevFadingMillis = millis();

        if (ventBlueTracker <= 254) {
          for (int8_t i = startLed; i <= endLed; i++) {
            packLeds.setPixelColor(i, ventRedTracker, ventGreenTracker, ventBlueTracker);
          }
          ventRedTracker--;
          ventGreenTracker--;
          ventBlueTracker++;
        }
        else {
          ventRedTracker = 0;
          ventGreenTracker = 0;
          ventBlueTracker = 255;
          for (int8_t i = startLed; i <= endLed; i++) {
            packLeds.setPixelColor(i, ventRedTracker, ventGreenTracker, ventBlueTracker);
          }
          ventPrevFadingMillis = 0;
        }
      }
      break;
  }
}
// END Vent Light helpers and fucntions
//////////////////////////////////////////////


//////////////////////////////////////////////////////////
//************** Powercell/Cyclotron Animations *********

// timer helpers and intervals for the animations
unsigned long prevPwrBootMillis = 0;    // the last time we changed a powercell light in the boot sequence
#define pwr_boot_interval     (uint16_t)60       // interval at which to cycle lights (milliseconds). 




unsigned long prevPwrMillis = 0;        // last time we changed a powercell light in the idle sequence
unsigned long prevCycMillis = 0;        // last time we changed a cyclotron light in the idle sequence
unsigned long prevFadeCycMillis = 0;    // last time we changed a cyclotron light in the idle sequence

// LED tracking variables
int8_t powerSeqTotal = powercellLedCount;                                    // total number of led's for powercell 0 based
int8_t powerSeqNum = powercellIndexOffset;                                   // current running powercell sequence led
int8_t powerShutdownSeqNum = powercellLedCount - powercellIndexOffset;       // shutdown sequence counts down

// animation level trackers for the boot and shutdown
int8_t currentBootLevel = powercellIndexOffset;                              // current powercell boot level sequence led
int8_t currentLightLevel = powercellLedCount - powercellIndexOffset;         // current powercell boot light sequence led

uint8_t cyclotronRunningFadeIn = 0;     // we reset this to 0 to fade the cyclotron in from nothing
uint8_t cyclotronRunningFadeOut = 255;  // we reset this to 255 to have the cyclotron running fade out effect
uint8_t cyclotronShDnFadeOut = 255;     // we reset this to 255 to have the cyclotron fade out effect

void setCyclotronLightState(int8_t startLed, int8_t endLed, int8_t state ) {
  switch ( state ) {
    case 0: // set all leds to red
      for (int8_t i = startLed; i <= endLed; i++) {
        packLeds.setPixelColor(i, 255, 0, 0);
      }
      break;
    case 1: // set all leds to orange
      for (int8_t i = startLed; i <= endLed; i++) {
        packLeds.setPixelColor(i, 255, 106, 0);
      }
      break;
    case 2: // set all leds off
      for (int8_t i = startLed; i <= endLed; i++) {
        packLeds.setPixelColor(i, 0, 0, 0);
      }
      break;
    case 3: // fade all leds from red
      for (int8_t i = startLed; i <= endLed; i++) {
        if ( cyclotronRunningFadeOut >= 0 ) {
          packLeds.setPixelColor(i, cyclotronRunningFadeOut, 0, 0);
          cyclotronRunningFadeOut--;
        } else {
          packLeds.setPixelColor(i, 0, 0, 0);
        }
      }
      break;
    case 4: // fade all leds to red
      for (int8_t i = startLed; i <= endLed; i++) {
        if ( cyclotronRunningFadeIn < 255 ) {
          packLeds.setPixelColor(i, cyclotronRunningFadeIn, 0, 0);
          cyclotronRunningFadeIn++;
        } else {
          packLeds.setPixelColor(i, 255, 0, 0);
        }
      }
      break;
  }
}


/////////////////////////////////////////////
// boot animation on the powercell/cyclotron
bool reverseBootCyclotron = false;

unsigned long prevCycBootMillis = 0;    // the last time we changed a cyclotron light in the boot sequence
#define cyc_boot_interval     (uint16_t)500      // interval at which to cycle lights (milliseconds).
#define cyc_boot_alt_interval (uint16_t)50      // interval at which to cycle lights (milliseconds).

void powerSequenceBoot() {

  // START CYCLOTRON
  if ( useCyclotronFadeInEffect == false ) {
    if ((millis() - prevCycBootMillis) >= cyc_boot_interval) {
      prevCycBootMillis = millis();

      if ( reverseBootCyclotron == false ) {
        setCyclotronLightState(c1Start, c1End, 1);
        setCyclotronLightState(c2Start, c2End, 2);
        setCyclotronLightState(c3Start, c3End, 1);
        setCyclotronLightState(c4Start, c4End, 2);
        reverseBootCyclotron = true;
      } else {
        setCyclotronLightState(c1Start, c1End, 2);
        setCyclotronLightState(c2Start, c2End, 1);
        setCyclotronLightState(c3Start, c3End, 2);
        setCyclotronLightState(c4Start, c4End, 1);
        reverseBootCyclotron = false;
      }
    }
  } else {
    if ((millis() - prevCycBootMillis) >= cyc_boot_alt_interval) {
      prevCycBootMillis = millis();
      setCyclotronLightState(c1Start, c4End, 4);
    }
  }
  // END CYCLOTRON


  // START POWERCELL
  if ( currentBootLevel != powerSeqTotal ) {

    if ((millis() - prevPwrBootMillis) >= pwr_boot_interval) {
      // save the last time you blinked the LED
      prevPwrBootMillis = millis();
      if ( currentBootLevel == currentLightLevel) {
        if (currentLightLevel + 1 <= powerSeqTotal) {
          packLeds.setPixelColor(pcStart + currentLightLevel + 1, 0);
        }
        packLeds.setPixelColor(pcStart + currentBootLevel, 0, 0, 255);
        currentLightLevel = powerSeqTotal;
        currentBootLevel++;
      } else {
        if (currentLightLevel + 1 <= powerSeqTotal) {
          packLeds.setPixelColor(pcStart + currentLightLevel + 1, 0, 0, 0);
        }
        packLeds.setPixelColor(pcStart + currentLightLevel, 0, 0, 255);
        currentLightLevel--;
      }
    } else {
      // for startingUp sequence to go final
      packStateFlag = 2; //to finish the startup pack stage
      currentBootLevel = powercellIndexOffset;
      currentLightLevel = powercellLedCount - powercellIndexOffset;
    }
    // END POWERCELL
  }
  packLeds.show(); // commit all of the changes
}
// END of boot animation on the powercell/cyclotron
/////////////////////////////////////////////////


////////////////////////////////////////////////////
// POWERCELL/CYCLOTRON idle/firing animation

// intervals/states for timer and delays
unsigned long firingStateMillis = 0;
#define firingHotWaitTime (uint16_t)5000    // how long to hold down fire for lights to speed up
#define firingWarnWaitTime (uint16_t)5000  // how long to hold down fire before warning sounds
#define firingForceStopTime (uint16_t)5000 // how long before the gun shutdown anbd recharge

// intervals that can be adjusted in real time to speed up animations
uint16_t pwr_interval = 60;       // interval at which to cycle lights for the powercell. We update this in the loop to speed up the animation so must be declared here (milliseconds)
uint16_t cyc_interval = 1000;     // interval at which to cycle lights for the cyclotron.
uint16_t cyc_fade_interval = 15;  // fade the inactive cyclotron to light to nothing
uint16_t firing_interval = 90;   // interval at which to cycle firing lights on the bargraph. We update this in the loop to speed up the animation so must be declared here (milliseconds).
int8_t cycOrder = 0;              // which cyclotron led will be lit next
int8_t cycFading = -1;            // which cyclotron led is fading out for game style


void powerSequenceOne( uint16_t anispeed, uint16_t cycspeed, uint16_t cycfadespeed) {

  // START CYCLOTRON
  if ( useGameCyclotronEffect == true ) { // if we are doing the video game style cyclotron
    if ((unsigned long)(millis() - prevCycMillis) >= cycspeed) {
      prevCycMillis = millis();

      switch ( cycOrder ) {
        case 0:
          setCyclotronLightState(c1Start, c1End, 0);
          setCyclotronLightState(c2Start, c2End, 2);
          setCyclotronLightState(c3Start, c3End, 2);
          cycFading = 0;
          cyclotronRunningFadeOut = 255;
          cycOrder = 1;
          break;
        case 1:
          setCyclotronLightState(c2Start, c2End, 0);
          setCyclotronLightState(c3Start, c3End, 2);
          setCyclotronLightState(c4Start, c4End, 2);
          cycFading = 1;
          cyclotronRunningFadeOut = 255;
          cycOrder = 2;
          break;
        case 2:
          setCyclotronLightState(c1Start, c1End, 2);
          setCyclotronLightState(c3Start, c3End, 0);
          setCyclotronLightState(c4Start, c4End, 2);
          cycFading = 2;
          cyclotronRunningFadeOut = 255;
          cycOrder = 3;
          break;
        case 3:
          setCyclotronLightState(c1Start, c1End, 2);
          setCyclotronLightState(c2Start, c2End, 2);
          setCyclotronLightState(c4Start, c4End, 0);
          cycFading = 3;
          cyclotronRunningFadeOut = 255;
          cycOrder = 0;
          break;
      }
    }

    // now figure out the fading light
    if ( (unsigned long)( millis() - prevFadeCycMillis) >= cycfadespeed ) {
      prevFadeCycMillis = millis();
      if ( cycFading != -1 ) {
        switch ( cycFading ) {
          case 0:
            setCyclotronLightState(c4Start, c4End, 3);
            break;
          case 1:
            setCyclotronLightState(c1Start, c1End, 3);
            break;
          case 2:
            setCyclotronLightState(c2Start, c2End, 3);
            break;
          case 3:
            setCyclotronLightState(c3Start, c3End, 3);
            break;
        }
      }
    }
  } else { // otherwise this is the standard version
    if ((unsigned long)(millis() - prevCycMillis) >= cycspeed) {
      prevCycMillis = millis();

      switch ( cycOrder ) {
        case 0:
          setCyclotronLightState(c4Start, c4End, 2);
          setCyclotronLightState(c1Start, c1End, 0);
          setCyclotronLightState(c2Start, c2End, 2);
          setCyclotronLightState(c3Start, c3End, 2);
          cycFading = 0;
          cyclotronRunningFadeOut = 255;
          cycOrder = 1;
          break;
        case 1:
          setCyclotronLightState(c1Start, c1End, 2);
          setCyclotronLightState(c2Start, c2End, 0);
          setCyclotronLightState(c3Start, c3End, 2);
          setCyclotronLightState(c4Start, c4End, 2);
          cycFading = 1;
          cyclotronRunningFadeOut = 255;
          cycOrder = 2;
          break;
        case 2:
          setCyclotronLightState(c1Start, c1End, 2);
          setCyclotronLightState(c2Start, c2End, 2);
          setCyclotronLightState(c3Start, c3End, 0);
          setCyclotronLightState(c4Start, c4End, 2);
          cycFading = 2;
          cyclotronRunningFadeOut = 255;
          cycOrder = 3;
          break;
        case 3:
          setCyclotronLightState(c1Start, c1End, 2);
          setCyclotronLightState(c2Start, c2End, 2);
          setCyclotronLightState(c3Start, c3End, 2);
          setCyclotronLightState(c4Start, c4End, 0);
          cycFading = 3;
          cyclotronRunningFadeOut = 255;
          cycOrder = 0;
          break;
      }
    }
  }
  // END CYCLOTRON

  // START POWERCELL
  if ((unsigned long)(millis() - prevPwrMillis) >= anispeed) {
    // save the last time you blinked the LED
    prevPwrMillis = millis();

    for ( int8_t i = powercellIndexOffset; i <= powerSeqTotal; i++) {
      if ( i <= powerSeqNum ) {
        packLeds.setPixelColor(pcStart + i, 0, 0, 150);
      } else {
        packLeds.setPixelColor(pcStart + i, 0, 0, 0);
      }
    }
    if ( powerSeqNum <= powerSeqTotal) {
      powerSeqNum++;
    } else {
      powerSeqNum = powercellIndexOffset;
    }
  }
  // END POWERCELL

  packLeds.show();
}
// END of POWERCELL/CYCLOTRON idle/firing animation
//////////////////////////////////////////////


//////////////////////////////////////////////////
// POWERCELL/CYCLOTRON shutdown animation
unsigned long prevShtdMillis = 0;                    // last time we changed a light in the idle sequence
#define pwr_shutdown_interval (uint16_t)250    // interval at which to cycle lights (milliseconds)

void powerSequenceShutdown() {
  if ((millis() - prevShtdMillis) >= pwr_shutdown_interval) {
    prevShtdMillis = millis();
    // CYCLOTRON shutdown
    for (int16_t i = c1Start; i <= c4End; i++) {
      if ( cyclotronShDnFadeOut > 0 ) {
        packLeds.setPixelColor(i, cyclotronShDnFadeOut, 0, 0);
        cyclotronShDnFadeOut--;
      } else {
        packLeds.setPixelColor(i, 0, 0, 0);
      }
    }
    // POWERCELL shutdown
    for (int16_t i = powerSeqTotal; i >= powercellIndexOffset; i--) {
      if ( i <= powerShutdownSeqNum ) {
        packLeds.setPixelColor(pcStart + i, 0, 0, 150);
      } else {
        packLeds.setPixelColor(pcStart + i, 0, 0, 0);
      }
    }

    if ( powerShutdownSeqNum >= powercellIndexOffset) {
      powerShutdownSeqNum--;
    }
  }
  // Update leds
  packLeds.show();
}

// END of POWERCELL/CYCLOTRON
///////////////////////////////////////////////


//////////////////////////////////////////////
//***** WAND NOSE Firing Animations *********

uint8_t noseRedTrack = 0;
uint8_t noseGreenTrack = 0;
uint8_t noseBlueTrack = 0;

void clearFireStrobe() {
  noseRedTrack = 0;
  noseGreenTrack = 0;
  noseBlueTrack = 0;
  for ( int8_t i = noseStart; i <= noseEnd; i++) {
    wandLeds.setPixelColor(i, noseRedTrack, noseGreenTrack, noseBlueTrack);
  }
  wandLeds.show();
}

void fireStrobe() {
  for (int8_t i = noseStart; i <= noseEnd; i++) {
    noseRedTrack = random(0, 255);
    noseGreenTrack = random(0, 100);
    noseBlueTrack = random(0, 255);
    wandLeds.setPixelColor(i, noseRedTrack, noseGreenTrack, noseBlueTrack);
  }
  wandLeds.show();
}
// END of WAND NOSE Firing Animations
/////////////////////////////////////////


//////////////////////////////////////////////////
// SHUTOFF all leds for wand and pack, EXCEPT bar graph
// and resets some trackers
void clearAllLights() {
  //reset variables
  powerSeqNum = powercellIndexOffset;
  powerShutdownSeqNum = powercellLedCount - powercellIndexOffset;
  currentLightLevel = powercellLedCount;
  currentBootLevel = powercellIndexOffset;
  cyclotronRunningFadeIn = 0;

  // shutoff cyclotron leds
  for ( int8_t i = c1Start; i <= c4End; i++) {
    packLeds.setPixelColor(i, 0, 0, 0);
  }
  // shutoff powercell leds
  for (int8_t i = pcStart; i <= pcEnd; i++) {
    packLeds.setPixelColor(i, 0, 0, 0);
  }
  //shutoff venting leds
  setVentLightState(ventStart, ventEnd, 2);
  // shutoff wand leds
  for ( int8_t i = 0; i <= totalWandLedsNumber; i++ ) {
    wandLeds.setPixelColor(i, 0, 0, 0);
  }
  //Update Leds
  packLeds.show();
  wandLeds.show();
}
// END of shuts off and resets the powercell/cyclotron leds
///////////////////////////////////////////



////////////////////////////////////////////////
////////////////////////////////////////////////
//************ BAR GRAPH Animations ************

/////////////////////////////
// BAR GRAPH idle sequence
unsigned long prevBarMillis_on = 0;          // bargraph on tracker
#define pwrcl_interval (uint16_t)100     // interval at which to cycle lights (milliseconds).
bool reverseSequenceOne = false;

void barGraphIdleSequenceOne() {//In Idle mode safety ON
  // normal sync animation on the bar graph while safety ON
  if ((unsigned long)(millis() - prevBarMillis_on) > pwrcl_interval) {
    // save the last time you blinked the LED
    prevBarMillis_on = millis();

    if ( reverseSequenceOne == false ) {
      switch_graph_led(bargraphSeqOneCurrent, true);
      bargraphSeqOneCurrent++;
      if ( bargraphSeqOneCurrent > bargraphTotalLeds ) {
        reverseSequenceOne = true;
      }
    } else {
      switch_graph_led(bargraphSeqOneCurrent, false);
      bargraphSeqOneCurrent--;
      if ( bargraphSeqOneCurrent < 0  ) {
        reverseSequenceOne = false;
      }
    }
  }
}

void barGraphIdleSequenceTwo() {
  // normal sync animation on the bar graph while safety OFF
  if ((unsigned long)(millis() - prevBarMillis_on) > (pwrcl_interval - 30)) {
    // save the last time you blinked the LED
    prevBarMillis_on = millis();

    if ( reverseSequenceOne == false ) {
      for (int8_t i = 1; i <= bargraphTotalLeds; i++) {
        if (i == bargraphSeqOneCurrent) {
          switch_graph_led(i, true);
        }
        else {
          switch_graph_led(i, false);
        }
      }
      bargraphSeqOneCurrent++;
      if ( bargraphSeqOneCurrent > bargraphTotalLeds ) {
        bargraphSeqOneCurrent = bargraphTotalLeds - 1;
        reverseSequenceOne = true;
      }

    } else {
      switch_graph_led(bargraphSeqOneCurrent, false);
      for (int8_t i = 1; i <= bargraphTotalLeds; i++) {
        if (i == bargraphSeqOneCurrent) {
          switch_graph_led(i, true);
        }
        else {
          switch_graph_led(i, false);
        }
      }
      bargraphSeqOneCurrent--;
      if ( bargraphSeqOneCurrent < 1  ) {
        bargraphSeqOneCurrent = 2;
        reverseSequenceOne = false;
      }
    }
  }
}


/////////////////////////////////
// BAR GRAPH shutting down sequence
#define shtcl_interval (uint16_t)200     // interval at which to cycle lights (milliseconds).

void barGraphshuttingDown() {
  if ((millis() - prevBarMillis_on) > shtcl_interval) {
    // save the last time you blinked the LED
    prevBarMillis_on = millis();
    if ( bargraphSeqOneCurrent >= 1  ) {
      switch_graph_led(bargraphSeqOneCurrent, false);
      bargraphSeqOneCurrent--;
    }
  }
}

/////////////////////////////////
// BAR GRAPH firing leds function

unsigned long prevBarMillis_fire = 0; // bargraph firing tracker
int8_t fireSequenceNum = 1;

void barGraphFiringSequence() {
  if ((unsigned long)(millis() - prevBarMillis_fire) > firing_interval) {
    // save the last time you blinked the LED
    prevBarMillis_fire = millis();

    switch (fireSequenceNum) {
      case 1:
        switch_graph_led(2, false);
        switch_graph_led(9, false);
        switch_graph_led(1, true);
        switch_graph_led(10, true);
        fireSequenceNum++;
        break;
      case 2:
        switch_graph_led(1, false);
        switch_graph_led(10, false);
        switch_graph_led(2, true);
        switch_graph_led(9, true);
        fireSequenceNum++;
        break;
      case 3:
        switch_graph_led(2, false);
        switch_graph_led(9, false);
        switch_graph_led(3, true);
        switch_graph_led(8, true);
        fireSequenceNum++;
        break;
      case 4:
        switch_graph_led(3, false);
        switch_graph_led(8, false);
        switch_graph_led(4, true);
        switch_graph_led(7, true);
        fireSequenceNum++;
        break;
      case 5:
        switch_graph_led(4, false);
        switch_graph_led(7, false);
        switch_graph_led(5, true);
        switch_graph_led(6, true);
        fireSequenceNum++;
        break;
      case 6:
        switch_graph_led(5, false);
        switch_graph_led(6, false);
        switch_graph_led(4, true);
        switch_graph_led(7, true);
        fireSequenceNum++;
        break;
      case 7:
        switch_graph_led(4, false);
        switch_graph_led(7, false);
        switch_graph_led(3, true);
        switch_graph_led(8, true);
        fireSequenceNum++;
        break;
      case 8:
        switch_graph_led(3, false);
        switch_graph_led(8, false);
        switch_graph_led(2, true);
        switch_graph_led(9, true);
        fireSequenceNum = 1;
        break;
    }
  }
}

////////////////////////////////
//BAR GRAPH clearing leds function
void bargraph_clear_leds() {
  // reset the sequence
  bargraphSeqOneCurrent = 1;
  fireSequenceNum = 1;

  // shut all led's off
  for (int8_t i = 1; i <= bargraphTotalLeds; i++) {
    switch_graph_led(i, false);
  }
}

///////////////////////////////
// BAR GRAPH Updating leds fucntion
void switch_graph_led(int8_t num, bool state) {
  switch (num) {
    case 1:
      bargraph.setLed(0, 0, 0, state);
      break;
    case 2:
      bargraph.setLed(0, 0, 1, state);
      break;
    case 3:
      bargraph.setLed(0, 0, 2, state);
      break;
    case 4:
      bargraph.setLed(0, 0, 3, state);
      break;
    case 5:
      bargraph.setLed(0, 0, 4, state);
      break;
    case 6:
      bargraph.setLed(0, 0, 5, state);
      break;
    case 7:
      bargraph.setLed(0, 0, 6, state);
      break;
    case 8:
      bargraph.setLed(0, 0, 7, state);
      break;
    case 9:
      bargraph.setLed(0, 1, 1, state);
      break;
    case 10:
      bargraph.setLed(0, 1, 2, state);
      break;
  }
}
// END of BAR GRAPH Animations
////////////////////////////////////////
///////////////////////////////////////


////////////////////////////////////////
////////////////////////////////////////
////////////////////////////////////////
//************** MAIN LOOP ************

bool prev_theme_switch = false;
bool prev_startup_switch = false;
bool prev_safety_switch = false;
#define PowerOffDelayShort (unsigned long)180000  //delay (3 minute) before auto power off when startup switch is off and no theme playing
#define PowerOffDelayLong (unsigned long)1200000  //delay (20 minutes) before auto power off when startup switch ON or theme playing
unsigned long PreviousPowerOffMillis = 0;
unsigned long PreviousVolumeSet = 0;


void loop() {

  // get the current switches and buttons states
  // switches and buttons read LOW when activated, so digitalRead need to be reverse to facilitate program reading (true/false)...
  theme_switch = !digitalRead(THEME_SWITCH);
  startup_switch = !digitalRead(STARTUP_SWITCH);
  safety_switch = !digitalRead(SAFETY_SWITCH);
  fire_button = !digitalRead(FIRE_BUTTON);
  volumeUpButton = !digitalRead(VOLUME_UP_PIN);
  volumeDownButton = !digitalRead(VOLUME_DOWN_PIN);

  //////////////////////////////
  // Auto power off checking
  // Power ON is maintained by a hardware mosfet and transistor circuit when POWER_ON_PIN is HIGH
  if (theme_switch != prev_theme_switch || startup_switch != prev_startup_switch || safety_switch != prev_safety_switch || fire_button == true ) {
    PreviousPowerOffMillis = millis();
  }
  if (theme_switch == true) {// Long power off delay, but after theme playing is done
    if (millis() - PreviousPowerOffMillis > PowerOffDelayLong && playing == false) {
      digitalWrite(POWER_ON_PIN, false);
    }
  }
  else if (startup_switch == true) {// Long  power off delay, pack is in action
    if (millis() - PreviousPowerOffMillis > PowerOffDelayLong) {
      digitalWrite(POWER_ON_PIN, false);
    }
  } else if (millis() - PreviousPowerOffMillis > PowerOffDelayShort) {//Short power off delay, pack is out of action anyway
    digitalWrite(POWER_ON_PIN, false);
  }
  //END Auto power off checking
  ////////////////////////////////



  ///////////////////////////////
  // Get playing info and set volume parameters
  // find out of the audio board is playing audio
  playing = !digitalRead(DY_BUSY); //pin reads LOW if player's busy
  //Check if volume buttons are used

  if (millis() - PreviousVolumeSet > 200) {
    if (volumeDownButton == true) {
      ++volume;//Increase volume
    }
    else if (volumeUpButton == true) {
      --volume;//Decrease volume
    }
    //Checking if volume is inside limitis
    if (volume > volumeMax) {
      volume = volumeMax;
    }
    else if (volume < volumeMin) {
      volume = volumeMin;
    }
    //Setting volume if volume has changed
    if (volume != prevVolume) {
      player.setVolume(volume);
      prevVolume = volume ;
      PreviousVolumeSet = millis();
    }
  }



  ///////////////////////////////////////////////////////////////
  //Actions for different packs states


  switch (packState) {

    ////////////////////////
    // "themes" pack state
    // themes playing mode
    case 0:
      // Play the thems song while the theme_switch is on
      // should play the full ghostbusters theme song
      switch (packStateFlag) {
        case 0:
          if ((millis() - previousMillis) > audioDelay) {
            switch (themeFlag) {
              case 1 :
                playAudio(ghostbustersTrack);
                break;
              case 2 :
                playAudio(thrillerTrack);
                break;
              case 3 :
                playAudio(partyupTrack);
                break;
            }
            themeFlag++;
            if (themeFlag > 3) {
              themeFlag = 1;
            }
            previousMillis = millis();
            packStateFlag = 1;
          }
          break;
        case 1:
          powerSequenceOne( pwr_interval, cyc_interval, cyc_fade_interval);
          barGraphIdleSequenceOne();
          if ( (playing == false) && ((millis() - previousMillis) > audioDelay) ) {
            switch (themeFlag) {
              case 1 :
                playAudio(ghostbustersTrack);
                break;
              case 2 :
                playAudio(thrillerTrack);
                break;
              case 3 :
                playAudio(partyupTrack);
                break;
            }
            themeFlag++;
            if (themeFlag > 3) {
              themeFlag = 1;
            }
            previousMillis = millis();
          }
          if (theme_switch == false) {
            player.stop();
            packState = prevPackState;
          }
          break;
      }
      break;


    ////////////////////////
    // "poweredDown" pack state
    // the the pack is powered down
    case 1:
      // Check if themes_switch ON, go to themes state
      if (theme_switch == true) {
        prevPackState = packState;
        packState = themes;
        packStateFlag = 0;
      }
      // Pack is powering up : calling booting up sequence
      else if ( (startup_switch == true) && ((millis() - previousMillis) > audioDelay) ) {
        packState = startingUp;
        packStateFlag = 0;
      }
      // pack is already powerdowned
      else {
        clearAllLights();//clear cyclotron, powercell, vent and wands LEDS
        bargraph_clear_leds();//clear the bar graph leds
      }
      break;


    ////////////////////////
    //"startingUp" pack state
    // pack is in process of power booting
    case 2:
      switch (packStateFlag) {
        case 0:
          playAudio(startupTrack);
          previousMillis = millis();
          cyclotronRunningFadeIn = 0;// reset cyclotron fade in effet
          packStateFlag = 1;
          break;

        case 1://While powerSequenceBoot is not finished
          powerSequenceBoot();
          setWandLightState(sloblow, 7);            //sloblow flashing red
          setWandLightState(body, 9);               //body leds fade in blue
          setVentLightState(ventStart, ventEnd, 3); //vent fade in blue
          break;

        case 2://After powerSequenceBoot is finished (packStateFlag = 2 from powerSequenceBoot)
          setWandLightState(sloblow, 7);//sloblow flashing red
          setWandLightState(body, 9);//body leds fade in blue
          powerSequenceOne( pwr_interval, cyc_interval, cyc_fade_interval);
          barGraphIdleSequenceOne();
          if ( (millis() - previousMillis) > 3500 ) {
            packState = idlingSafetyOn;
            packStateFlag = 0;
            break;
          }
      }
      break;


    ///////////////////////
    //"idlingSafetyOn" pack state
    // has the pack booted up and is idling with safety on
    case 3:

      // Set leds
      setWandLightState(sloblow, 0);            // set sloblow red
      setWandLightState(backOrange, 4);         // set backOrange OFF
      setWandLightState(topWhite, 4);           // set topWhite led OFF
      setWandLightState(body, 3);               // set body led blue
      setVentLightState(ventStart, ventEnd, 1); // set venting light to blue
      powerSequenceOne(pwr_interval, cyc_interval, cyc_fade_interval);
      barGraphIdleSequenceOne();

      // Check if themes_switch ON, go to themes state
      if (theme_switch == true) {
        prevPackState = packState;
        packState = themes;
        packStateFlag = 0;
      }
      // Check if the pack is shutting down
      else if (startup_switch == false) {
        packState = shuttingDown ;
        packStateFlag = 0;
      }
      // check if safety is turned OFF : calling charging up sequence
      else if (safety_switch == false && ((millis() - previousMillis) > audioDelay) ) {
        packState = isCharging ;
        packStateFlag = 0;
      }
      break;


    ///////////////////////
    //"idlingSafetyOff" pack state
    // has the pack booted up and is idling with safety off ready to fire
    case 4:

      //Set leds
      powerSequenceOne( pwr_interval, cyc_interval, cyc_fade_interval);
      barGraphIdleSequenceTwo();
      setWandLightState(backOrange, 2);         // set back light orange
      setWandLightState(topWhite, 1);           // set topWhite led white
      setWandLightState(body, 11);              // set body led purple
      setVentLightState(ventStart, ventEnd, 0); // set vent to purple

      // Check if themes_switch ON, go to themes state
      if (theme_switch == true) {
        prevPackState = packState;
        packState = themes;
        packStateFlag = 0;
      }
      // Check if the pack is shutting down
      else if (startup_switch == false) {
        packState = shuttingDown ;
        packStateFlag = 0;
      }
      // Check if safety is turned ON, pack going to Idle One with safety ON and no firing : calling uncharging sequence
      else if (safety_switch == true) {
        packState = isUnloading ;
        packStateFlag = 0;
      }
      // Check if firing button is pushed, calling firing sequence
      else if (fire_button == true) {
        packState = isFiring ;
        packStateFlag = 0;
      }
      break;


    /////////////////////////
    //"isLoading" pack state
    //Safety removed and pack is loading
    case 5:
      switch (packStateFlag) {
        case 0:
          powerSequenceOne(pwr_interval, cyc_interval, cyc_fade_interval);
          barGraphIdleSequenceOne();
          if ( (playing == false) && ((millis() - previousMillis) > audioDelay) ) {
            playAudio(loadingTrack);
            previousMillis = millis();
            packStateFlag = 1;
          }
          break;

        case 1:
          setWandLightState(topWhite, 5);           //  set back topWhite light flashing
          setVentLightState(ventStart, ventEnd, 5); // vent shift from blue to purple
          setWandLightState(body, 11);              // body shift from blue to purple
          barGraphIdleSequenceOne();
          powerSequenceOne( pwr_interval, cyc_interval, cyc_fade_interval);
          if ( (playing == false) && ((millis() - previousMillis) > audioDelay) ) {
            packState = idlingSafetyOff;
            packStateFlag = 0;
          }
          break;
      }
      break;

    /////////////////////////
    //"isUnloading" pack state
    // Safety is put on and pack uncharging to idling with safety on
    case 6:
      switch (packStateFlag) {
        case 0:
          if ( (playing == false) && ((millis() - previousMillis) > audioDelay) ) {
            playAudio(unloadingTrack);
            previousMillis = millis();
            packStateFlag = 1;
          }
          break;
        case 1:
          setWandLightState(backOrange, 4);         //  set backOrange OFF
          setWandLightState(topWhite, 5);           //  set topWhite led flashing
          setWandLightState(body, 12);              //  set body shifting from purple to blue
          setVentLightState(ventStart, ventEnd, 6); //  shift from purple to blue
          barGraphIdleSequenceTwo();
          powerSequenceOne( pwr_interval, cyc_interval, cyc_fade_interval);
          if ( (playing == false) && ((millis() - previousMillis) > audioDelay) ) {
            packState = idlingSafetyOn;
            bargraph_clear_leds();
            packStateFlag = 0;
          }
          break;
      }
      break;


    /////////////////////////
    //"isFiring" pack state
    // normal firing state
    case 7:
      switch (packStateFlag) {
        case 0:
          if ( (playing == false) && ((millis() - previousMillis) > audioDelay) ) {
            playAudio(blastTrack);
            firingStateMillis = millis();
            previousMillis = millis();
            bargraph_clear_leds();
            packStateFlag = 1;
          }
          break;

        case 1:
          // show the firing bargraph sequence
          barGraphFiringSequence();

          // powercell and cyclotron
          powerSequenceOne( pwr_interval, cyc_interval, cyc_fade_interval);

          // strobe the nose pixels
          fireStrobe();

          // vent lights warming
          setVentLightState(ventStart, ventEnd, 7); //shifting from purple to red

          //Check if firing is in Hot stage
          if ( (millis() - firingStateMillis) > firingHotWaitTime) { // if we are in the fire Hot interval
            firingStateMillis = millis();
            packState = isFiringHot ;
            packStateFlag = 0 ;
          }

          //Firing is button released, pack goes in firing ending state
          if ( (fire_button == false) && ((millis() - previousMillis) > audioDelay) ) {
            playAudio(endOneTrack);
            previousMillis = millis();
            clearFireStrobe();
            packStateFlag = 2;
            break;
          }
          break;

        case 2:
          barGraphIdleSequenceTwo();
          powerSequenceOne( pwr_interval, cyc_interval, cyc_fade_interval);
          setWandLightState(topYellow, 4);          // set topYellow OFF
          setWandLightState(backOrange, 6);         // set backOrange flashing
          setVentLightState(ventStart, ventEnd, 8); // shift vent leds from red top blue
          if ((millis() - previousMillis) > 1500) {
            packState = endFiring ;
            packStateFlag = 0; ;
          }
          break;
      }
      break;

    /////////////////////////
    //"isFiringHot" pack state
    // firing state becoming Hot (also triggering dialogs tracks)
    case 8:
      switch (packStateFlag) {

        case 0:
          pwr_interval = 30;      // speed up the powercell animation
          firing_interval = 50;   // speed up the bar graph animation
          cyc_interval = 200;     // speed up cyclotron
          cyc_fade_interval = 10; // speed up the fade of the cyclotron
          packStateFlag = 1;
          break;

        case 1:
          // Hot ligth flashing
          setWandLightState(topYellow, 6); // set topYellow flashing

          // show the firing bargraph sequence
          barGraphFiringSequence();

          // powercell and cyclotron
          powerSequenceOne( pwr_interval, cyc_interval, cyc_fade_interval);

          // strobe the nose pixels
          fireStrobe();

          // vent lights warming
          setVentLightState(ventStart, ventEnd, 7); // keep shifting from purple to red

          //Check if firing is too long aka warning stage
          if ( (millis() - firingStateMillis) > firingWarnWaitTime) { // if we are in the fire warn interval
            firingStateMillis = millis();
            packState = isFiringWarning ;
            packStateFlag = 0;
          }

          //Firing is button released, pack goes in firing ending state
          else if (fire_button == false && (millis() - previousMillis) > audioDelay) {
            if ( useDialogTracks == true ) {
              playDialogTrack();
            } else {
              playAudio(endTwoTrack);
            }
            previousMillis = millis();
            bargraph_clear_leds();
            clearFireStrobe();
            pwr_interval = 60;
            firing_interval = 90;
            cyc_interval = 1000;
            cyc_fade_interval = 15;
            packStateFlag = 2;
          }
          break;
        case 2:
          barGraphIdleSequenceTwo();
          powerSequenceOne( pwr_interval, cyc_interval, cyc_fade_interval);
          setWandLightState(topYellow, 4);          // set topYellow to OFF
          setWandLightState(backOrange, 6);         // set backOrange to flashing
          setVentLightState(ventStart, ventEnd, 8); // vent shifting from red to blue
          if ((millis() - previousMillis) > 2000) {
            packState = endFiring ;
            packStateFlag = 0; ;
          }
          break;
      }
      break;


    /////////////////////////
    //"isFiringWarning" pack state
    // firing state with warning sound (triggering venting)
    case 9:
      switch (packStateFlag) {

        case 0:
          pwr_interval = 10;      // speed up the powercell animation
          firing_interval = 30;   // speed up the bar graph animation
          cyc_interval = 50;      // really speed up cyclotron
          cyc_fade_interval = 5;  // speed up the fade of the cyclotron
          packStateFlag = 1;
          break;

        case 1:
          // set top Yellow light red flashing fast
          setWandLightState(topYellow, 8); // set topYellow to fast flashing red

          // show the firing bargraph sequence
          barGraphFiringSequence();

          // powercell and cyclotron
          powerSequenceOne( pwr_interval, cyc_interval, cyc_fade_interval);

          // strobe the nose pixels
          fireStrobe();

          // vent lights warming
          setVentLightState(ventStart, ventEnd, 7); // keep shifting from purple to red

          //Firing is button released, pack goes in firing ending state
          if ( (fire_button == false && (millis() - previousMillis) > audioDelay) || ((millis() - firingStateMillis) > firingForceStopTime) ) {
            playAudio(endTwoTrack);
            previousMillis = millis();
            bargraph_clear_leds();
            clearFireStrobe();
            pwr_interval = 60;
            firing_interval = 75;
            cyc_interval = 1000;
            cyc_fade_interval = 15;
            packState = endFiring ;
            packStateFlag = 1;//Special venting ending selected
          }
          break;
      }
      break;

    /////////////////////////
    //"endFiring" pack state
    // end of firing sequence
    case 10:
      switch (packStateFlag) {
        case 0:  // Normal End Firing
          barGraphIdleSequenceTwo();
          powerSequenceOne( pwr_interval, cyc_interval, cyc_fade_interval);
          setWandLightState(topYellow, 4);
          setWandLightState(backOrange, 6);
          setVentLightState(ventStart, ventEnd, 5);
          if ( (playing == false) && ((millis() - previousMillis) > audioDelay) ) {
            packState = idlingSafetyOff;
            packStateFlag = 0;
          }
          break;
        case 1://Firing end with venting
          barGraphIdleSequenceTwo();
          powerSequenceOne( pwr_interval, cyc_interval, cyc_fade_interval);
          setWandLightState(topYellow, 4);          // set topYellow to OFF
          setWandLightState(backOrange, 6);         // set backOrange to flashing
          setVentLightState(ventStart, ventEnd, 8); // vent shifting from red to blue
          if ( (playing == false) && ((millis() - previousMillis) > audioDelay) ) {
            packState = venting;
            packStateFlag = 0;
          }
          break;
      }
      break;

    /////////////////////////
    //"venting" pack state
    // the pack is venting
    case 11:
      switch (packStateFlag) {
        case 0:
          playAudio(ventTrack);
          previousMillis = millis();
          packStateFlag = 1;
          break;
        case 1:
          setWandLightState(backOrange, 6);         // set backOrange to flashing
          setWandLightState(topYellow, 4);          // set topYellow to OFF
          setVentLightState(ventStart, ventEnd, 8); // vent shifting from red to blue
          barGraphIdleSequenceTwo();
          powerSequenceOne( pwr_interval, cyc_interval, cyc_fade_interval);

          if ((millis() - previousMillis) > 4000) {//delay for vent cooling leds sequence
            packStateFlag = 2 ;
          }
          break;
        case 2:
          setWandLightState(backOrange, 6);         // set backOrange to flashing
          setVentLightState(ventStart, ventEnd, 5); //vent shifting from blue to purple
          barGraphIdleSequenceTwo();
          powerSequenceOne( pwr_interval, cyc_interval, cyc_fade_interval);
          if ( (playing == false) && ((millis() - previousMillis) > audioDelay) ) {
            packState = idlingSafetyOff;
            packStateFlag = 0;
          }
          break;
      }
      break;


    /////////////////////////
    //"shuttingDown" pack state
    // the pack is in the process of shutting down
    case 12:
      switch (packStateFlag) {
        case 0:
          playAudio(shutdownTrack);
          previousMillis = millis();
          cyclotronShDnFadeOut = 255; //reset cyclotron shutdown fade out
          packStateFlag = 1 ;
          break;
        case 1:
          powerSequenceShutdown();
          barGraphshuttingDown();
          setWandLightState(body, 10);              // body fading out
          setWandLightState(sloblow, 7);            // sloblow falshing red
          setVentLightState(ventStart, ventEnd, 4); // vent fading out
          if ( (playing == false) && ((millis() - previousMillis) > audioDelay) ) {
            packState = poweredDown ;
            packStateFlag = 0;
            break;
          }
      }
      break;
  }

  //END Actions for different packs states
  ///////////////////////////////////////////////////////////////

}
//
//************END MAIN LOOP*****************
////////////////////////////////////////////
////////////////////////////////////////////
