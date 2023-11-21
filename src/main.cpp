
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

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "marcoantonio"
#define WIFI_PASSWORD "12345678"

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

const int trigPin = 19;
const int echoPin = 18;


#define RED_LED  2
#define YELLOW_LED 4
#define GREEN_LED 5

#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

void piscarLed(int led, int tempoDelay);
void inicioPortao();


void setup()
{
    pinMode(RED_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);

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
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
  
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);

    // Calculate the distance
    distanceCm = duration * SOUND_SPEED/2;

    if(distanceCm < 5.00 && distanceCm != 0){
        
        piscarLed(RED_LED, 0);
        Serial.println("Abrindo o portão");
        piscarLed(YELLOW_LED, 2500);
        Serial.println("Portão Aberto");
        piscarLed(GREEN_LED, 5000);
        Serial.println("Fechando o portão");
        piscarLed(YELLOW_LED, 2500);
        
        Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, "/portao/back/", distanceCm) ? "ok" : fbdo.errorReason().c_str());
    }
    
    // Prints the distance in the Serial Monitor
    Serial.print("Distance (cm): ");
    Serial.println(distanceCm);

    delay(1000);
    /*
    int currentState = digitalRead(BUTTON_PIN);
    Serial.println(currentState);
    delay(1000);
    
    if(!digitalRead(BUTTON_PIN))
    {
        buttonPressed = true;
        Serial.printf("Button pressed: %s\n", buttonPressed);
    }
    else
    {
        buttonPressed = false;
        Serial.printf("Button pressed: %s\n", buttonPressed);
    }
    if(buttonPressed)digitalWrite(RED_LED, HIGH);
    else digitalWrite(RED_LED, LOW);
    */

    /*if (millis() - dataMillis > 5000)
    {
        estado = !estado;

        digitalWrite(RED_LED, estado);
        digitalWrite(YELLOW_LED, estado);
        digitalWrite(GREEN_LED, estado);
        dataMillis = millis();
        Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, "/test/int", count++) ? "ok" : fbdo.errorReason().c_str());
    }*/
}

void piscarLed(int led, int tempoDelay){
    bool estado = false;

    digitalWrite(led, !estado);
    delay(tempoDelay);
    digitalWrite(led, estado);
}

void inicioPortao(){
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    Serial.println("Portão Fechado");
}
