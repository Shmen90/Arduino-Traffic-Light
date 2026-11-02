#include <Arduino.h>
#include <RCSwitch.h>

RCSwitch mySwitch;


// ======================================================
// НАСТРОЙКИ ВРЕМЕНИ
// ======================================================

const unsigned long T_RED_PED = 20000;       // 20 секунд

// Мигание зелёного пешеходного
const unsigned long T_PED_BLINK = 3500;      // 3.5 секунды

// Пауза после пешеходного
const unsigned long T_PED_OFF = 1000;        // 1 секунда

// Красный + жёлтый
const unsigned long T_RED_YELLOW = 3000;     // 3 секунды

// Основной зелёный
const unsigned long T_GREEN = 20000;         // 20 секунд

// Мигание основного зелёного
const unsigned long T_GREEN_BLINK = 3500;    // 3.5 секунды

// Жёлтый
const unsigned long T_YELLOW = 2000;         // 2 секунды

// Красный перед новым циклом
const unsigned long T_RED_PAUSE = 1000;      // 1 секунда

// Интервал мигания
const unsigned long BLINK_TIME = 500;        // 0.5 секунды


// ======================================================
// КОДЫ ПУЛЬТА
// ======================================================

const unsigned long CODE_RED        = 3473600;
const unsigned long CODE_YELLOW     = 3473456;
const unsigned long CODE_RED_YELLOW = 3473648;
const unsigned long CODE_GREEN      = 3473420;
const unsigned long CODE_AUTO       = 3473411;


// ======================================================
// ПИНЫ
// ======================================================

#define MAIN_RED     5
#define MAIN_YELLOW  6
#define MAIN_GREEN   7

#define PED_RED      8
#define PED_GREEN    9


// ======================================================
// СОСТОЯНИЯ
// ======================================================

#define ST_RED_PED        0
#define ST_PED_BLINK      1
#define ST_PED_OFF        2
#define ST_RED_YELLOW     3
#define ST_GREEN          4
#define ST_GREEN_BLINK    5
#define ST_YELLOW         6
#define ST_RED_PAUSE      7


// ======================================================
// ПЕРЕМЕННЫЕ
// ======================================================

byte state = ST_RED_PED;

bool autoMode = true;
bool blinkState = true;

unsigned long stateTime = 0;
unsigned long blinkTime = 0;


// ======================================================
// СВЕТОФОР
// ======================================================

void lights(
    bool red,
    bool yellow,
    bool green,
    bool pedRed,
    bool pedGreen
)
{
    digitalWrite(MAIN_RED, red);
    digitalWrite(MAIN_YELLOW, yellow);
    digitalWrite(MAIN_GREEN, green);

    digitalWrite(PED_RED, pedRed);
    digitalWrite(PED_GREEN, pedGreen);

    Serial.print(F("MAIN: "));

    if (red)    Serial.print(F("RED "));
    if (yellow) Serial.print(F("YELLOW "));
    if (green)  Serial.print(F("GREEN "));

    if (!red && !yellow && !green)
        Serial.print(F("OFF "));

    Serial.print(F("| PED: "));

    if (pedRed)   Serial.print(F("RED "));
    if (pedGreen) Serial.print(F("GREEN "));

    if (!pedRed && !pedGreen)
        Serial.print(F("OFF"));

    Serial.println();
}


// ======================================================
// УСТАНОВКА СОСТОЯНИЯ
// ======================================================

void setState(byte s)
{
    state = s;

    stateTime = millis();
    blinkTime = stateTime;
    blinkState = true;

    switch (state)
    {
        // ------------------------------------------------
        // КРАСНЫЙ + ПЕШЕХОДНЫЙ ЗЕЛЁНЫЙ
        // ------------------------------------------------

        case ST_RED_PED:

            Serial.println(F("AUTO -> RED + PED GREEN"));

            lights(
                HIGH,
                LOW,
                LOW,
                LOW,
                HIGH
            );

            break;


        // ------------------------------------------------
        // МИГАНИЕ ПЕШЕХОДНОГО
        // ------------------------------------------------

        case ST_PED_BLINK:

            Serial.println(F("AUTO -> PED GREEN BLINK"));

            lights(
                HIGH,
                LOW,
                LOW,
                LOW,
                HIGH
            );

            break;


        // ------------------------------------------------
        // ПЕШЕХОДНЫЙ ПОГАС
        // ------------------------------------------------

        case ST_PED_OFF:

            Serial.println(F("AUTO -> RED / PED OFF"));

            lights(
                HIGH,
                LOW,
                LOW,
                LOW,
                LOW
            );

            break;


        // ------------------------------------------------
        // КРАСНЫЙ + ЖЁЛТЫЙ
        // ------------------------------------------------

        case ST_RED_YELLOW:

            Serial.println(F("AUTO -> RED + YELLOW"));

            lights(
                HIGH,
                HIGH,
                LOW,
                HIGH,
                LOW
            );

            break;


        // ------------------------------------------------
        // ЗЕЛЁНЫЙ + ПЕШЕХОДНЫЙ КРАСНЫЙ
        // ------------------------------------------------

        case ST_GREEN:

            Serial.println(F("AUTO -> GREEN + PED RED"));

            lights(
                LOW,
                LOW,
                HIGH,
                HIGH,
                LOW
            );

            break;


        // ------------------------------------------------
        // МИГАНИЕ ОСНОВНОГО ЗЕЛЁНОГО
        // ------------------------------------------------

        case ST_GREEN_BLINK:

            Serial.println(F("AUTO -> GREEN BLINK"));

            lights(
                LOW,
                LOW,
                HIGH,
                HIGH,
                LOW
            );

            break;


        // ------------------------------------------------
        // ЖЁЛТЫЙ
        // ------------------------------------------------

        case ST_YELLOW:

            Serial.println(F("AUTO -> YELLOW + PED RED"));

            lights(
                LOW,
                HIGH,
                LOW,
                HIGH,
                LOW
            );

            break;


        // ------------------------------------------------
        // КРАСНЫЙ
        // ------------------------------------------------

        case ST_RED_PAUSE:

            Serial.println(F("AUTO -> RED + PED RED"));

            lights(
                HIGH,
                LOW,
                LOW,
                HIGH,
                LOW
            );

            break;
    }
}


// ======================================================
// РУЧНЫЕ РЕЖИМЫ
// ======================================================

void manualRed()
{
    autoMode = false;

    Serial.println(F("MANUAL -> RED + PED GREEN"));

    lights(HIGH, LOW, LOW, LOW, HIGH);
}


void manualYellow()
{
    autoMode = false;

    Serial.println(F("MANUAL -> YELLOW + PED RED"));

    lights(LOW, HIGH, LOW, HIGH, LOW);
}


void manualRedYellow()
{
    autoMode = false;

    Serial.println(F("MANUAL -> RED + YELLOW"));

    lights(HIGH, HIGH, LOW, HIGH, LOW);
}


void manualGreen()
{
    autoMode = false;

    Serial.println(F("MANUAL -> GREEN + PED RED"));

    lights(LOW, LOW, HIGH, HIGH, LOW);
}


// ======================================================
// ПУЛЬТ
// ======================================================

void checkRemote()
{
    if (!mySwitch.available())
        return;

    unsigned long code = mySwitch.getReceivedValue();

    Serial.print(F("RF: "));
    Serial.println(code);

    if (code == CODE_RED)
    {
        manualRed();
    }
    else if (code == CODE_YELLOW)
    {
        manualYellow();
    }
    else if (code == CODE_RED_YELLOW)
    {
        manualRedYellow();
    }
    else if (code == CODE_GREEN)
    {
        manualGreen();
    }
    else if (code == CODE_AUTO)
    {
        autoMode = true;

        Serial.println(F("AUTO MODE"));

        setState(ST_RED_PED);
    }

    mySwitch.resetAvailable();
}


// ======================================================
// АВТОМАТ
// ======================================================

void runAuto()
{
    unsigned long now = millis();

    switch (state)
    {
        // ------------------------------------------------
        // КРАСНЫЙ + ПЕШЕХОДНЫЙ ЗЕЛЁНЫЙ
        // ------------------------------------------------

        case ST_RED_PED:

            if (now - stateTime >= T_RED_PED)
                setState(ST_PED_BLINK);

            break;


        // ------------------------------------------------
        // МИГАЕТ ПЕШЕХОДНЫЙ ЗЕЛЁНЫЙ
        // ------------------------------------------------

        case ST_PED_BLINK:

            if (now - blinkTime >= BLINK_TIME)
            {
                blinkTime = now;

                blinkState = !blinkState;

                digitalWrite(PED_GREEN, blinkState);

                Serial.print(F("PED GREEN: "));

                if (blinkState)
                    Serial.println(F("ON"));
                else
                    Serial.println(F("OFF"));
            }

            if (now - stateTime >= T_PED_BLINK)
                setState(ST_PED_OFF);

            break;


        // ------------------------------------------------
        // ПЕШЕХОДНЫЙ ПОГАС
        // ------------------------------------------------

        case ST_PED_OFF:

            if (now - stateTime >= T_PED_OFF)
                setState(ST_RED_YELLOW);

            break;


        // ------------------------------------------------
        // КРАСНЫЙ + ЖЁЛТЫЙ
        // ------------------------------------------------

        case ST_RED_YELLOW:

            if (now - stateTime >= T_RED_YELLOW)
                setState(ST_GREEN);

            break;


        // ------------------------------------------------
        // ЗЕЛЁНЫЙ
        // ------------------------------------------------

        case ST_GREEN:

            if (now - stateTime >= T_GREEN)
                setState(ST_GREEN_BLINK);

            break;


        // ------------------------------------------------
        // МИГАЕТ ОСНОВНОЙ ЗЕЛЁНЫЙ
        // ------------------------------------------------

        case ST_GREEN_BLINK:

            if (now - blinkTime >= BLINK_TIME)
            {
                blinkTime = now;

                blinkState = !blinkState;

                digitalWrite(MAIN_GREEN, blinkState);

                Serial.print(F("MAIN GREEN: "));

                if (blinkState)
                    Serial.println(F("ON"));
                else
                    Serial.println(F("OFF"));
            }

            if (now - stateTime >= T_GREEN_BLINK)
                setState(ST_YELLOW);

            break;


        // ------------------------------------------------
        // ЖЁЛТЫЙ
        // ------------------------------------------------

        case ST_YELLOW:

            if (now - stateTime >= T_YELLOW)
                setState(ST_RED_PAUSE);

            break;


        // ------------------------------------------------
        // КРАСНЫЙ
        // ------------------------------------------------

        case ST_RED_PAUSE:

            if (now - stateTime >= T_RED_PAUSE)
                setState(ST_RED_PED);

            break;
    }
}


// ======================================================
// SETUP
// ======================================================

void setup()
{
    Serial.begin(9600);

    pinMode(MAIN_RED, OUTPUT);
    pinMode(MAIN_YELLOW, OUTPUT);
    pinMode(MAIN_GREEN, OUTPUT);

    pinMode(PED_RED, OUTPUT);
    pinMode(PED_GREEN, OUTPUT);

    // Всё выключить
    digitalWrite(MAIN_RED, LOW);
    digitalWrite(MAIN_YELLOW, LOW);
    digitalWrite(MAIN_GREEN, LOW);

    digitalWrite(PED_RED, LOW);
    digitalWrite(PED_GREEN, LOW);

    // 433 МГц
    // INT0 = D2
    mySwitch.enableReceive(0);

    Serial.println(F("================================"));
    Serial.println(F("TRAFFIC LIGHT ATmega8"));
    Serial.println(F("RCSwitch 433 MHz"));
    Serial.println(F("AUTO START"));
    Serial.println(F("================================"));

    // Автомат по умолчанию
    autoMode = true;

    setState(ST_RED_PED);
}


// ======================================================
// LOOP
// ======================================================

void loop()
{
    checkRemote();

    if (autoMode)
        runAuto();
}