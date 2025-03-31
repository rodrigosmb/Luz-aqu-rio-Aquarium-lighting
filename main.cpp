#include <Servo.h>
#include <Bounce2.h>
#include <EEPROM.h>
#include <virtuabotixRTC.h>

// Pins
#define BUTTON_PIN 2
#define SERVO_PIN 9
#define LED_PIN 13

#define RTC_CLK 3       // CLK and DAT pins are switched in the RTC used for the project
#define RTC_DAT 7

#define RTC_RST 8

// Timing
#define LONG_PRESS_MS      1000       // 1 second long press threshold
#define DIAGNOSTIC_MS      5000       // 5 second diagnostic press
#define ADJUST_DURATION_S  10          // 10 second fade for short press
#define FADE_DURATION_S    3600UL       // 3600 second full on/off transition
#define UPDATE_INTERVAL_MS 20         // 20 ms loop update interval

// EEPROM address for brightness (0..100)
#define EEPROM_BRIGHTNESS_ADDR  0

// State machine
enum States { OFF, FADE_ON, ON, FADE_OFF, ADJUSTING };
States state = OFF;

// Globals
Servo servo;
Bounce button = Bounce();

unsigned long pressStartTime     = 0;
bool buttonActive                = false;

int currentBrightness            = 0;
int initialBrightness            = 0;
int targetBrightness             = 0;
int longPressTarget              = 0;

unsigned long fadeStartTime      = 0;
unsigned long fadeDurationMs     = 0;

unsigned long lastLEDToggle      = 0;
unsigned long lastLoopUpdate     = 0;

bool increasing = true;


virtuabotixRTC myRTC(RTC_DAT, RTC_CLK, RTC_RST);
int lastHourChecked = -1;


int constrainValue(int value, int min_val, int max_val) {
  if (value < min_val) return min_val;
  if (value > max_val) return max_val;
  return value;
}

unsigned long calcProportionalFadeDuration(int startVal, int endVal, unsigned long fullDurationS) {
  int distance = abs(endVal - startVal);
  if (distance == 0) return 0;
  float fraction = (float)distance / 100.0f;
  return (unsigned long)(fraction * fullDurationS + 0.5f);
}

void startFade(int startVal, int endVal, unsigned long duration_s) {
  if (startVal == endVal) {
    fadeDurationMs = 0;
    return;
  }
  initialBrightness = startVal;
  targetBrightness  = endVal;
  fadeStartTime     = millis();
  fadeDurationMs    = duration_s * 1000UL;
}

void setup() {
  servo.attach(SERVO_PIN);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  button.attach(BUTTON_PIN, INPUT_PULLUP);
  button.interval(10);

  int savedBrightness = EEPROM.read(EEPROM_BRIGHTNESS_ADDR);
  savedBrightness = constrainValue(savedBrightness, 0, 100);
  longPressTarget = (savedBrightness == 0) ? 0 : 100;
  
  currentBrightness = savedBrightness;
  servo.write(map(currentBrightness, 0, 100, 0, 180));

  startFade(savedBrightness, 0, 10);
  state = (fadeDurationMs == 0) ? ((currentBrightness == 0) ? OFF : ON) : FADE_OFF;

  // Optional: set time only once if needed
  myRTC.setDS1302Time(0, 0, 18, 6, 29, 3, 2025);
}

void loop() {
  unsigned long now = millis();
  if (now - lastLoopUpdate >= UPDATE_INTERVAL_MS) {
    lastLoopUpdate = now;
    button.update();
    handleButton();
    updateBrightness();
    updateLED();
    checkAutomaticFade();
  }
}

void handleButton() {
  if (button.rose()) {
    buttonActive = true;
    pressStartTime = millis();
  }

  if (button.fell() && buttonActive) {
    buttonActive = false;
    unsigned long pressDuration = millis() - pressStartTime;

    if (pressDuration >= DIAGNOSTIC_MS) {
      performDiagnosticBlink();
    } else if (pressDuration >= LONG_PRESS_MS) {
      togglePowerState();
    } else {
      adjustBrightness();
    }
  }
}

void togglePowerState() {
  if (currentBrightness == longPressTarget) {
    longPressTarget = (longPressTarget == 0) ? 100 : 0;
  }

  unsigned long needed_s = calcProportionalFadeDuration(currentBrightness, longPressTarget, FADE_DURATION_S);

  startFade(currentBrightness, longPressTarget, needed_s);

  if (longPressTarget == 100) {
    state = FADE_ON;
    increasing = true;
  } else {
    state = FADE_OFF;
    increasing = false;
  }
}

void adjustBrightness() {
  if (increasing) {
    targetBrightness = constrainValue(currentBrightness + 20, 0, 100);
    if (targetBrightness == 100) increasing = false;
  } else {
    targetBrightness = constrainValue(currentBrightness - 20, 0, 100);
    if (targetBrightness == 0) increasing = true;
  }

  startFade(currentBrightness, targetBrightness, ADJUST_DURATION_S);

  if (fadeDurationMs == 0) {
    EEPROM.update(EEPROM_BRIGHTNESS_ADDR, currentBrightness);
    state = (currentBrightness == 0) ? OFF : ON;
  } else {
    if (targetBrightness > currentBrightness) {
      longPressTarget = 100;
      increasing = true;
    } else {
      longPressTarget = 0;
      increasing = false;
    }

    state = ADJUSTING;
  }
}

void updateBrightness() {
  if (fadeDurationMs == 0) return;

  unsigned long now = millis();
  unsigned long elapsed = now - fadeStartTime;
  float progress = (float)elapsed / (float)fadeDurationMs;
  if (progress > 1.0f) progress = 1.0f;

  float interpolated = initialBrightness + (targetBrightness - initialBrightness) * progress;
  currentBrightness = constrainValue((int)(interpolated + 0.5f), 0, 100);

  servo.write(map(currentBrightness, 0, 100, 0, 180));

  if (progress >= 1.0f || currentBrightness == targetBrightness) {
    currentBrightness = targetBrightness;
    fadeDurationMs = 0;
    EEPROM.update(EEPROM_BRIGHTNESS_ADDR, currentBrightness);

    if (currentBrightness == longPressTarget) {
      state = (currentBrightness == 0) ? OFF : ON;
    } else {
      state = (currentBrightness > 0) ? ON : OFF;
    }
  }
}

void updateLED() {
  switch (state) {
    case FADE_ON:
    case FADE_OFF:
      digitalWrite(LED_PIN, HIGH);
      break;
    case ADJUSTING:
      if (millis() - lastLEDToggle > 250) {
        lastLEDToggle = millis();
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      }
      break;
    default:
      digitalWrite(LED_PIN, LOW);
      break;
  }
}

void checkAutomaticFade() {
  myRTC.updateTime();
  int currentHour = myRTC.hours;

  if (currentHour == lastHourChecked) return;

  if (currentHour == 11 && state != FADE_ON && currentBrightness < 100) {
    unsigned long duration_s = calcProportionalFadeDuration(currentBrightness, 100, FADE_DURATION_S);
    startFade(currentBrightness, 100, duration_s);
    state = FADE_ON;
    increasing = true;
    longPressTarget = 100;
  }

  if (currentHour == 20 && state != FADE_OFF && currentBrightness > 0) {
    unsigned long duration_s = calcProportionalFadeDuration(currentBrightness, 0, FADE_DURATION_S);
    startFade(currentBrightness, 0, duration_s);
    state = FADE_OFF;
    increasing = false;
    longPressTarget = 0;
  }

  lastHourChecked = currentHour;
}

void performDiagnosticBlink() {
  int blinkCount = 0;

  if (currentBrightness == 0) blinkCount = 2;
  else if (currentBrightness == 100) blinkCount = 4;
  else return;

  for (int i = 0; i < blinkCount; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
}
