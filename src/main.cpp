
/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: k_suwatchai@hotmail.com
 *
 * Github: https://github.com/mobizt/Firebase-ESP8266
 *
 * Copyright (c) 2023 mobizt
 *
 */

/** This example will show how to authenticate using
 * the legacy token or database secret with the new APIs (using config and auth data).
 */

#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <time.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "uaifai-apolo"
#define WIFI_PASSWORD "bemvindoaocesar"

/* 2. If work with RTDB, define the RTDB URL and database secret */
#define DATABASE_URL "https://se-cesar-a3244-default-rtdb.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
#define DATABASE_SECRET "2GDvWnAKwIJnncIXppsKuQp1830n36Si94LAUPlr"

/* 3. Define the Firebase Data object */
FirebaseData fbdo;

/* 4, Define the FirebaseAuth data for authentication data */
FirebaseAuth auth;

/* Define the FirebaseConfig data for config data */
FirebaseConfig config;

unsigned long dataMillis = 0;
int count = 0;
bool buttonPressed = false;
long duration;
float distanceCm;
float distanceInch;
long timezone = -3;
byte daysavetime = 1;

const int trigPin = 19;
const int echoPin = 18;

#define RED_LED 2
#define YELLOW_LED 4
#define GREEN_LED 5
#define BIP_PIN 14

#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

void piscarLed(int led, int tempoDelay);
void inicioPortao();
String horatioAtual();
void bip();

void setup()
{
    pinMode(RED_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BIP_PIN, OUTPUT);

    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT);

    Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    Serial.println("Contacting Time Server");
    configTime(3600 * timezone, daysavetime * 3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");

    /* Assign the certificate file (optional) */
    // config.cert.file = "/cert.cer";
    // config.cert.file_storage = StorageType::FLASH;

    /* Assign the database URL and database secret(required) */
    config.database_url = DATABASE_URL;
    config.signer.tokens.legacy_token = DATABASE_SECRET;

    // Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager
    Firebase.reconnectNetwork(true);

    // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
    // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
    fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

    /* Initialize the library with the Firebase authen and config */
    Firebase.begin(&config, &auth);

    // Or use legacy authenticate method
    // Firebase.begin(DATABASE_URL, DATABASE_SECRET);
}

void loop()
{
    inicioPortao();
    horatioAtual();
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);

    // Calculate the distance
    distanceCm = duration * SOUND_SPEED / 2;

    if (distanceCm < 5.00 && distanceCm != 0)
    {
        Serial.printf("Set opening... %s\n", Firebase.setBool(fbdo, "/Gate/opening/", true) ? "ok" : fbdo.errorReason().c_str());
        Serial.printf("Set lastTimeOpened... %s\n", Firebase.setString(fbdo, "/Gate/lastTimeOpened/", horatioAtual()) ? "ok" : fbdo.errorReason().c_str());

        bip();
        piscarLed(RED_LED, 0);
        Serial.println("Abrindo o portão");
        piscarLed(YELLOW_LED, 2500);
        Serial.println("Portão Aberto");
        piscarLed(GREEN_LED, 5000);
        Serial.println("Fechando o portão");
        piscarLed(YELLOW_LED, 2500);
    }
    Serial.printf("Set opening... %s\n", Firebase.setBool(fbdo, "/Gate/opening/", false) ? "ok" : fbdo.errorReason().c_str());

    // Prints the distance in the Serial Monitor
    Serial.print("Distance (cm): ");
    Serial.println(distanceCm);

    delay(1000);
}

void piscarLed(int led, int tempoDelay)
{
    bool estado = false;

    digitalWrite(led, !estado);
    delay(tempoDelay);
    digitalWrite(led, estado);
}

void inicioPortao()
{
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    Serial.println("Portão Fechado");
}

String horatioAtual()
{
    struct tm tmstruct;

    tmstruct.tm_year = 0;
    getLocalTime(&tmstruct);

    // Criamos uma string formatada
    String time = String((tmstruct.tm_year) + 1900) + "-" + String((tmstruct.tm_mon) + 1) + "-" + String(tmstruct.tm_mday) + " " + String(tmstruct.tm_hour) + ":" + String(tmstruct.tm_min) + ":" + String(tmstruct.tm_sec);

    return time;
}

void bip()
{

    digitalWrite(BIP_PIN, HIGH);
    delay(500);
    digitalWrite(BIP_PIN, LOW);
    delay(500);
    digitalWrite(BIP_PIN, HIGH);
    delay(500);
    digitalWrite(BIP_PIN, LOW);
    delay(500);
}
