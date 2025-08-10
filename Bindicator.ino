#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <NetworkClientSecure.h>
#include <XPT2046_Touchscreen.h>	
#include <TFT_eSPI.h>

// UPDATE THESE VALUES
char  AP[] = "ACCESS_POINT";
char KEY[] = "SECURITY_KEY";
String url = "https://script.google.com/macros/paste-your-own-published-script-url/exec"; // the google script that returns Calendar details in JSON format


struct cc { // colour coding structure
  String textvalue;
  int colourcode;
};
struct cc ccodes[10];// COLOUR CODING for the Entries - edit these yourself within the SETUP function below



// Touchscreen pins
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);	


TFT_eSPI tft = TFT_eSPI();
TFT_eSPI_Button key;

String items[10]; // array of items
String dates[10]; // array of dates for the items
int count=0; // count of valid items
String myData = "Welcome to Bindicator";
int tx = TFT_WHITE;
const int default_bg = TFT_BLACK;
int bg = TFT_BLACK; // whole background colour

    int fontNum = 2; 
    int refreshrate = 60; // number of seconds to update
int tz = 60*60*9.5; // number of seconds for the current timezone


  struct tm timeinfo;
  int nexttime=0;



// Google Root Certificate
const char *rootCACertificate = R"string_literal(
-----BEGIN CERTIFICATE-----
MIIFVzCCAz+gAwIBAgINAgPlk28xsBNJiGuiFzANBgkqhkiG9w0BAQwFADBHMQsw
CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU
MBIGA1UEAxMLR1RTIFJvb3QgUjEwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAw
MDAwWjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp
Y2VzIExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjEwggIiMA0GCSqGSIb3DQEBAQUA
A4ICDwAwggIKAoICAQC2EQKLHuOhd5s73L+UPreVp0A8of2C+X0yBoJx9vaMf/vo
27xqLpeXo4xL+Sv2sfnOhB2x+cWX3u+58qPpvBKJXqeqUqv4IyfLpLGcY9vXmX7w
Cl7raKb0xlpHDU0QM+NOsROjyBhsS+z8CZDfnWQpJSMHobTSPS5g4M/SCYe7zUjw
TcLCeoiKu7rPWRnWr4+wB7CeMfGCwcDfLqZtbBkOtdh+JhpFAz2weaSUKK0Pfybl
qAj+lug8aJRT7oM6iCsVlgmy4HqMLnXWnOunVmSPlk9orj2XwoSPwLxAwAtcvfaH
szVsrBhQf4TgTM2S0yDpM7xSma8ytSmzJSq0SPly4cpk9+aCEI3oncKKiPo4Zor8
Y/kB+Xj9e1x3+naH+uzfsQ55lVe0vSbv1gHR6xYKu44LtcXFilWr06zqkUspzBmk
MiVOKvFlRNACzqrOSbTqn3yDsEB750Orp2yjj32JgfpMpf/VjsPOS+C12LOORc92
wO1AK/1TD7Cn1TsNsYqiA94xrcx36m97PtbfkSIS5r762DL8EGMUUXLeXdYWk70p
aDPvOmbsB4om3xPXV2V4J95eSRQAogB/mqghtqmxlbCluQ0WEdrHbEg8QOB+DVrN
VjzRlwW5y0vtOUucxD/SVRNuJLDWcfr0wbrM7Rv1/oFB2ACYPTrIrnqYNxgFlQID
AQABo0IwQDAOBgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4E
FgQU5K8rJnEaK0gnhS9SZizv8IkTcT4wDQYJKoZIhvcNAQEMBQADggIBAJ+qQibb
C5u+/x6Wki4+omVKapi6Ist9wTrYggoGxval3sBOh2Z5ofmmWJyq+bXmYOfg6LEe
QkEzCzc9zolwFcq1JKjPa7XSQCGYzyI0zzvFIoTgxQ6KfF2I5DUkzps+GlQebtuy
h6f88/qBVRRiClmpIgUxPoLW7ttXNLwzldMXG+gnoot7TiYaelpkttGsN/H9oPM4
7HLwEXWdyzRSjeZ2axfG34arJ45JK3VmgRAhpuo+9K4l/3wV3s6MJT/KYnAK9y8J
ZgfIPxz88NtFMN9iiMG1D53Dn0reWVlHxYciNuaCp+0KueIHoI17eko8cdLiA6Ef
MgfdG+RCzgwARWGAtQsgWSl4vflVy2PFPEz0tv/bal8xa5meLMFrUKTX5hgUvYU/
Z6tGn6D/Qqc6f1zLXbBwHSs09dR2CQzreExZBfMzQsNhFRAbd03OIozUhfJFfbdT
6u9AWpQKXCBfTkBdYiJ23//OYb2MI3jSNwLgjt7RETeJ9r/tSQdirpLsQBqvFAnZ
0E6yove+7u7Y/9waLd64NnHi/Hm3lCXRSHNboTXns5lndcEZOitHTtNCjv0xyBZm
2tIMPNuzjsmhDYAPexZ3FL//2wmUspO8IFgV6dtxQ/PeEMMA3KgqlbbC1j+Qa3bb
bP6MvPJwNQzcmRk13NfIRmPVNnGuV/u3gm3c
-----END CERTIFICATE-----
)string_literal";

void updateTimer(){
  // update timer at bottom of the screen
   tft.setTextColor(tx, bg);
  int x = SCREEN_WIDTH/2-60;
  int y = 10;
  tft.setCursor(x, y); // Set the cursor
  tft.setTextSize(1);
  int now = time(NULL);
  time_t nowt = time(NULL) + tz;
  gmtime_r(&nowt, &timeinfo);
  tft.println(asctime(&timeinfo));
  x = SCREEN_WIDTH/2-80;
  y += 20;
  tft.setCursor(x, y); // Set the cursor
  tft.setTextSize(3);
  tft.println("Bindicator");
   y=235;
// draw progress line from 1 to 320*(nexttime-now)refreshrate
  tft.drawLine(0,y, 320, y, bg);
  tft.drawLine(0,y, 320*(nexttime-now)/refreshrate, y, TFT_WHITE);

}

int findcolour(int i)
{ // check for colour code in items[i]
  String tmpdata = items[i];
      tmpdata.toLowerCase();
      for (int j=0; j<sizeof(ccodes)/sizeof(ccodes[0]); j++)
        {

            if(tmpdata.indexOf(ccodes[j].textvalue) >= 0)
            {
              return ccodes[j].colourcode;
            }
        }
  return -1;
}

void updateDisplay(){
// Clear the screen before writing to it
  bg = default_bg;
  for (int i=0; i<=count; i++)
  { // find bg colour
      if (findcolour(i))
      {
        bg = findcolour(i);
        break;
      }
  }
  tft.fillScreen(bg);

  key.initButton(&tft,
                      SCREEN_WIDTH/2,
                      SCREEN_HEIGHT-30,
                      100,
                      40,
                      TFT_BLACK, // Outline
                      TFT_BLUE, // Fill
                      TFT_WHITE, // Text
                      "REFRESH",
                      2);
  key.drawButton();

  tft.setTextWrap(true, false);
  tft.setTextPadding(5);
  int tmpbg = bg; // change it as we go if necessary
  int x = 5;
  int y = 60;
  tft.setCursor(x, y); // Set the cursor
  Serial.print("Count = ");
  Serial.println(count);
  for (int i=0; i<=count; i++)
  {
    
    if (findcolour(i)>-1){tmpbg = findcolour(i);}
    //else { bg = TFT_BLACK;}
    tft.setTextColor(tx, tmpbg);
    tft.setTextSize(2);
    tft.println(items[i]);
    //tft.print(" - ");
    tft.setTextSize(1);
    tft.println(dates[i]);
    //y+=25;
    Serial.println(items[i]);
    //Read date into format of date
    char date[11];
    //strftime(date, sizeof(date), "%Y-%m-%d", &weekago);
  }

  updateTimer();
  
}

// Not sure if NetworkClientSecure checks the validity date of the certificate.
// Setting clock just to be sure...
void setClock() {
  // Start the tft display and set it to black
  tft.init();
  tft.setRotation(1); //This is the display in landscape
  
  configTime(0, 0, "pool.ntp.org");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();

  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
  
}

WiFiMulti wf;

void setup() {

// CUSTOM COLOUR CODES - feel free to modify these!
ccodes[0].textvalue="recycl";
ccodes[0].colourcode = TFT_YELLOW;
ccodes[1].textvalue="green";
ccodes[1].colourcode = TFT_OLIVE;
ccodes[2].textvalue="love";
ccodes[2].colourcode = TFT_RED;
// add your own codes and colours to colourcode the entries


  Serial.begin(115200);
  // Serial.setDebugOutput(true);
// Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }

  // Start the SPI for the touchscreen and init the touchscreen
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  // Set the Touchscreen rotation in landscape mode
  // Note: in some displays, the touchscreen might be upside down, so you might need to set the rotation to 3: touchscreen.setRotation(3);
  touchscreen.setRotation(1);


  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.mode(WIFI_STA);
  wf.addAP(AP, KEY);

  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  while ((wf.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(" connected");
  updateDisplay();
  setClock();
  updateDisplay();
}

void getURL() {
   NetworkClientSecure *client = new NetworkClientSecure;
  if (client) {
    client->setCACert(rootCACertificate);

    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before NetworkClientSecure *client is
      HTTPClient https;
      https.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); // follow redirects is required for the google script


      Serial.print("[HTTPS] begin...\n");
            
      if (https.begin(*client, url)) {  // HTTPS
        Serial.print("[HTTPS] GET...");
        Serial.println(url);
        // start connection and send HTTP header
        //int httpCode = https.GET();
        int httpCode = https.sendRequest("GET");
        

        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.println(payload);
            // parse payload and create data
            const size_t CAPACITY = JSON_ARRAY_SIZE(30);
            StaticJsonDocument<CAPACITY> doc;
            deserializeJson(doc, payload);
            Serial.println(doc.as<String>());
            JsonArray array = doc.as<JsonArray>();
            int i=0;
            for(JsonVariant v: array){
              //Serial.println(v.as<String>());
              JsonDocument val;
              for (JsonPair kv : v.as<JsonObject>())
              {
                if (i<10)
              {
              items[i] = kv.key().c_str();
              dates[i] = kv.value().as<String>();
              count=i;
              }    
              }
            i++;
            
            }
            myData=payload;
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }

        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }

      // End extra scoping block
    }

    delete client;
  } else {
    Serial.println("Unable to create client");
  }
    int now = time(NULL);
    nexttime = now + refreshrate; // store the next lookup time (just in seconds, not timestructure)

    Serial.print(F("Next time: "));
  Serial.print(nexttime);
}

void loop() {
  
  int x,y,z; // touchscreen positions

   // Check if the button is pressed
  if (touchscreen.tirqTouched() && touchscreen.touched()) {
   // Get Touchscreen points
    TS_Point p = touchscreen.getPoint();
    // Calibrate Touchscreen points with map function to the correct width and height
    x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
    z = p.z;

    if (key.contains(p.x, p.y)) {
Serial.println("BUTTON RELEASED");
    }
    getURL();
    updateTimer();
    updateDisplay();
  }  

  int now = time(NULL);
  if (now > nexttime || nexttime==0)
  {
  getURL();
  updateTimer();
  updateDisplay();
  }
  delay(100);
  updateTimer();
}
