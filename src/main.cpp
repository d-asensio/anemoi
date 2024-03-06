#include <Arduino.h>

#include <Wire.h>
#include <LittleFS.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

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

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

AsyncWebServer server(80);

int button_state = LOW;
int previous_button_state = LOW;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_ADS1115 ads;
Adafruit_BMP280 bmp;

float atmospheric_cell_voltage = 0;
float atmospheric_pressure_bars = 0;

void init_i2c_display();
void init_i2c_bpm();
void init_wifi_connection();
void init_filesystem();
void init_http_server();
void init_button_switch();
void init_i2c_ads();
void init_buzzer();

float read_cell_voltage();
float read_atmospheric_pressure_bars();
float get_O2_partial_pressure_from_voltage(float current_cell_voltage);
void calibrate_cell();
void show_calibrting_message();

void setup()
{
    Serial.begin(115200);

    init_button_switch();
    init_buzzer();

    init_i2c_display();
    init_i2c_ads();
    init_i2c_bpm();

    init_filesystem();
    init_wifi_connection();
    init_http_server();

    calibrate_cell();
    show_calibrting_message();
}

void loop()
{
    button_state = digitalRead(BUTTON_SWITCH_PIN);

    if (button_state == HIGH && previous_button_state == LOW)
    {
        digitalWrite(BUZZER_PIN, HIGH);
        digitalWrite(BUTTON_LED_PIN, LOW);
        delay(100);
        digitalWrite(BUZZER_PIN, LOW);

        calibrate_cell();
        show_calibrting_message();
        delay(1000);
        digitalWrite(BUTTON_LED_PIN, HIGH);
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

    if (bmp.takeForcedMeasurement())
    {
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
    }
    else
    {
        Serial.println("Forced measurement failed!");
    }
}

void init_i2c_bpm()
{
    if (!bmp.begin(0x76))
    {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring or try a different address!"));
        while (1)
            ;
    }

    /* Default settings from datasheet. */
    bmp.setSampling(Adafruit_BMP280::MODE_FORCED,
                    Adafruit_BMP280::SAMPLING_X2,
                    Adafruit_BMP280::SAMPLING_X16,
                    Adafruit_BMP280::FILTER_X16,
                    Adafruit_BMP280::STANDBY_MS_500);
}

void init_wifi_connection()
{
    // connect to your local wi-fi network
    WiFi.begin(ssid, password);

    // check wi-fi is connected to wi-fi network
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected..!");
    Serial.print("Got IP: ");
    Serial.println(WiFi.localIP());
}

void init_http_server()
{
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html", "text/html");
    });

    server.serveStatic("/", LittleFS, "/");

    server.begin();
    Serial.println("HTTP server started");
}

void init_button_switch()
{
    pinMode(BUTTON_SWITCH_PIN, INPUT);
    pinMode(BUTTON_LED_PIN, OUTPUT);
    digitalWrite(BUTTON_LED_PIN, HIGH);
}

void init_i2c_ads()
{
    if (!ads.begin())
    {
        Serial.println(F("Failed to initialize ADS."));
        while (1)
            ;
    }
}

void init_buzzer()
{
    pinMode(BUZZER_PIN, OUTPUT);
}

void init_filesystem()
{
    if (!LittleFS.begin())
    {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }
}

void init_i2c_display()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        while (1)
            ;
    }
}

float read_cell_voltage()
{
    float multiplier = 0.1875F;
    float adc_reading = ads.readADC_Differential_0_1();

    return adc_reading * multiplier;
}

float read_atmospheric_pressure_bars()
{
    float atmospheric_pressure_pascals = bmp.readPressure();

    return atmospheric_pressure_pascals / 100000;
}

float get_O2_partial_pressure_from_voltage(float current_cell_voltage)
{
    return ATMOSPHERIC_O2_FRACTION * current_cell_voltage / atmospheric_cell_voltage;
}

void calibrate_cell()
{
    atmospheric_cell_voltage = read_cell_voltage();
    atmospheric_pressure_bars = read_atmospheric_pressure_bars();
}

void show_calibrting_message()
{
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);

    display.println("Calibrating...");
    display.display();
}