#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_BMP280.h>

#define BUTTON_SWITCH_PIN D5
#define BUTTON_LED_PIN D6

#define BUZZER_PIN D7

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define ATMOSPHERIC_O2_FRACTION 0.21


int button_state = LOW;
int previous_button_state = LOW;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_ADS1115 ads;
Adafruit_BMP280 bmp;

float atmospheric_cell_voltage = 0;
float atmospheric_pressure_bars = 0;

float read_cell_voltage () {
    float multiplier = 0.1875F;
    float adc_reading = ads.readADC_Differential_0_1();

    return adc_reading * multiplier;
}

float read_atmospheric_pressure_bars () {
    float atmospheric_pressure_pascals = bmp.readPressure();
    return atmospheric_pressure_pascals / 100000;
}

float get_O2_partial_pressure_from_voltage (float current_cell_voltage) {
    return ATMOSPHERIC_O2_FRACTION * atmospheric_pressure_bars * current_cell_voltage / atmospheric_cell_voltage;
}

void calibrate_cell() {
    atmospheric_cell_voltage = read_cell_voltage();
    atmospheric_pressure_bars = read_atmospheric_pressure_bars();
}

void print_calibrate_message() {
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);

    display.println("Calibrating...");
    display.display();
}

void setup() {
    Serial.begin(115200);

    // Button
    pinMode(BUTTON_SWITCH_PIN, INPUT);
    pinMode(BUTTON_LED_PIN, OUTPUT);

    // Buzzer
    pinMode(BUZZER_PIN, OUTPUT);

    // Display
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        while (1);
    }

    // ADS
    if (!ads.begin()) {
        Serial.println(F("Failed to initialize ADS."));
        while (1);
    }

    // BMP
    if (!bmp.begin(0x76)) {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring or try a different address!"));
        while (1);
    }

    /* Default settings from datasheet. */
    bmp.setSampling(Adafruit_BMP280::MODE_FORCED,
                    Adafruit_BMP280::SAMPLING_X2,
                    Adafruit_BMP280::SAMPLING_X16,
                    Adafruit_BMP280::FILTER_X16,
                    Adafruit_BMP280::STANDBY_MS_500);

    calibrate_cell();
    print_calibrate_message();
    delay(1000);
}

void loop() {
    button_state = digitalRead(BUTTON_SWITCH_PIN);

    if (button_state == HIGH) {
        digitalWrite(BUTTON_LED_PIN, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);

        if(button_state != previous_button_state) {
            calibrate_cell();
            print_calibrate_message();
            delay(1000);
        }
    }

    if (button_state == LOW) {
        digitalWrite(BUTTON_LED_PIN, LOW);
        digitalWrite(BUZZER_PIN, LOW);
    }

    previous_button_state = button_state;

    float cell_voltage = read_cell_voltage();
    float ppO2 = get_O2_partial_pressure_from_voltage(cell_voltage);

    Serial.print("Voltage: ");
    Serial.print(cell_voltage);
    Serial.println(" mV");

    Serial.print("ppO2: ");
    Serial.print(ppO2);
    Serial.println(" ppO2");

    display.setTextSize(2);

    display.clearDisplay();
    display.setCursor(0, 5);
    display.print(cell_voltage);
    display.println(" mV");

    display.setCursor(0, 25);
    display.print(ppO2);
    display.println(" ppO2");

    display.setCursor(0, 45);
    display.print(atmospheric_pressure_bars);
    display.println(" bar");
    display.display();

    //////


    if (bmp.takeForcedMeasurement()) {
        float atmospheric_pressure_pascals = bmp.readPressure();

        Serial.print(F("Temperature: "));
        Serial.print(bmp.readTemperature());
        Serial.println(" *C");

        Serial.print(F("Pressure: "));
        Serial.print(atmospheric_pressure_pascals);
        Serial.println(" Pa");

        Serial.print(F("Pressure: "));
        Serial.print(atmospheric_pressure_bars);
        Serial.println(" bar");

        Serial.print(F("Approx altitude: "));
        Serial.print(bmp.readAltitude(1013.25));
        Serial.println(" m");

        Serial.println();
    } else {
        Serial.println("Forced measurement failed!");
    }

    delay(200);
}