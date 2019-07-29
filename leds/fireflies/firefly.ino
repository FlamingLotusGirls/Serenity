//#include <AsyncMqttClient.h>
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
//#include <Ticker.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266TrueRandom.h>
#include <stdio.h>
#include "user_interface.h"
#include "wifi_credentials.h"

/* 
 *  FLG Firefly LED code. 
 *  
 *  For ESP8266, D1 Mini chips that we're using to light the small fireflies
 *  in the Serenity project.
 *  
 *  The program listens on a known multicast channel for UDP command packets. 
 *  These commands set the color of the lights, the pattern of blinking, and the
 *  rate of blinking.
 *  
 *  Note that the fireflies have been divided into three 'swarms' that can be 
 *  addressed individually. This is done by adding a voltage divider with 
 *  different resistances between the 3.3V and A0 pin on the D1 Mini board.
 *  
 *  Copyright Medea Software, Flaming Lotus Girls, 2019. 
 *  Released under the MIT license.
 */


// TODO - 
// Add randomization to OTA update

/*
#define BUFFER_LENGTH 512
char incomingPacket[BUFFER_LENGTH]; 
*/
// NB - variables ssid and password are intentionally kept in 
// 'wifi_credentials.h' so that they don't need to be checked in

/*
extern const char* ssid[NUM_WIFI_APS];
extern const char *password[NUM_WIFI_APS];
*/

#define VERSION 0.9

#define CLOCK_TIME 100 // base time unit, ms

#define NUM_LEDS 10
#define DATA_PIN 4   // GPIO4. D2 on the board
#define BLINK_PIN 2  // GPI02. D4 on the board

CRGB leds[NUM_LEDS];
static int board_id = 1;

/*********************************************
 *  OTA Update
 *********************************************/
//static AsyncMqttClient mqtt_client;
//static Ticker mqttReconnectTimer;
char fw_checksum[32 + 1];
bool do_reboot = false;
//bool ota_in_progress = false;
//#define MQTT_HOST IPAddress(192, 68, 4, 11) // XXX needs to be in a config file
//#define MQTT_PORT 5889
//#define OTA_TOPIC "/ota/firmware/"

void OtaClientInit() {
/*    Serial.print("OTA Client Init\n");
    mqtt_client.onConnect(onMqttConnect);
    mqtt_client.onDisconnect(onMqttDisconnect);
    mqtt_client.onMessage(onMqttMessage);
    mqtt_client.setServer(MQTT_HOST, MQTT_PORT);
*/
    
    strlcpy(fw_checksum, ESP.getSketchMD5().c_str(), sizeof(fw_checksum));
    fw_checksum[sizeof(fw_checksum) - 1] = '\0';
//    Update.runAsync(true);
    Serial.print("OTA Client Init Finishes\n");
}

/*
static void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqtt_client.connect();
}



static void onMqttConnect(bool session_present) {
  Serial.println("On Mqtt Connect\n");
  // Subscribe to all ota/firmware topics. The firmware hash is last position
  char ota_topic_plus[sizeof(OTA_TOPIC) + 1];
  strcpy(ota_topic_plus, OTA_TOPIC);
  strcat(ota_topic_plus, "+");
  mqtt_client.subscribe(ota_topic_plus, 1);
}

static void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("On Mqtt Disconnect\n");
  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
} 

static void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("On Mqtt Message\n");
  if (strncmp(topic, OTA_TOPIC, strlen(OTA_TOPIC)) == 0) {
    if (index == 0) {
      // First chunk. Grab the MD5 from the topic, and start the update.
      ota_in_progress = false;
      char *md5 = topic + strlen(OTA_TOPIC);
      if (strcmp(md5, fw_checksum) == 0) {
        Serial.println("OTA refused - already have this firmware");
        return;
      }
      Update.setMD5(md5);
      ota_in_progress = true;
      bool success = Update.begin(total); // XXX check return
      if (!success) {
        // XXX do something?
        return; 
      }
    }
    
    if (ota_in_progress && len > 0) {
      // Write chunk
      bool success = (Update.write((uint8_t *)payload, len) > 0);
      if (!success) {
        ota_in_progress = false;
        return;
      }
      // End OTA if we've gotten everything
      if (index + len >= total) {
        Update.end(false);
        ota_in_progress = false;
        do_reboot = true;
      }
    }
  }
}
*/



/*********************************************
 *  WIFI
 *********************************************/
static ESP8266WiFiMulti WifiMulti;
static WiFiUDP Udp;
static IPAddress multicastAddress(224,3,29,71);
#define MULTICAST_PORT 5010
//static auto wifi_ap_idx = 0;
static void FlashLEDs(int ntimes);

static WiFiEventHandler got_ip_event_handler, disconnect_event_handler;
//Ticker wifiReconnectTimer;

void WifiInit() {
  WiFi.mode(WIFI_STA);
  for (int i=0; i<NUM_WIFI_APS; i++) {
    WifiMulti.addAP(ssid[i], password[i]);
  }
  //WiFi.setAutoReconnect(true);
  got_ip_event_handler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP& event)
  {
    Serial.print("Wifi connected");
    WiFi.printDiag(Serial);
    Udp.beginMulticast(WiFi.localIP(), multicastAddress, MULTICAST_PORT);
    //connectToMqtt();
    Serial.print("End connect to mqtt\n");
    FlashLEDs(5);
    Serial.print("End wifi connect\n");
  });
  disconnect_event_handler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected& event)
  {
    Serial.println("Station disconnected\n");
    Udp.stop();
    //mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    //wifiReconnectTimer.once(2, WifiConnect);
  }); 
  //WifiConnect();
}

/*
void WifiConnect() {
  WiFi.begin(ssid[wifi_ap_idx], password[wifi_ap_idx]);
  wifi_init_time = millis();
}
*/

/*************************************************************************
 *  SEQUENCES 
 **************************************/
static char *default_sequence_str = "0001110011111";
#define MAX_COLOR_STR_LEN 16
#define MAX_SEQUENCE_STR_LEN 32
#define MAX_CLOCK_STR_LEN 8
static char current_color_str[MAX_COLOR_STR_LEN];
static char current_clock_str[MAX_CLOCK_STR_LEN];
static char current_sequence_str[MAX_SEQUENCE_STR_LEN];
static uint8 current_sequence_idx = 0;
static int current_sequence_len = 0;
static int  current_clock = 5; // 5* 100ms
static uint8_t current_red = 0xff;
static uint8_t current_blue = 0xff;
static uint8_t current_green = 0xff;

static bool doLEDs = true;

static bool validate_color(const char *color) {
  char temp_buf[MAX_COLOR_STR_LEN];
  char *red, *green, *blue;
  float r, g, b;
  char *endptr;
  
  if (strlen(color) > MAX_COLOR_STR_LEN - 1) {
    return false;
  }
  strcpy(temp_buf, color);
  red = strtok(temp_buf, ",");
  green = strtok(NULL, ",");
  blue = strtok(NULL, ",");

  r = strtof(red, &endptr);
  if (*endptr != '\0' 
       || r < 0 
       || r > 1.0 ) {
    char printbuf[256];
    sprintf(printbuf, "color string is %s\n", color);
    Serial.print(printbuf);
    Serial.print("red not correct\n");
    return false;
  }
  g = strtof(green, &endptr);
  if (*endptr != '\0' 
       || g < 0 
       || g > 1.0 ) {
   Serial.print("green not correct\n");
   return false;
  }
  b = strtof(blue, &endptr);
  if (*endptr != '\0' 
       || b < 0 
       || b > 1.0 ) {
    Serial.print("blue not correct\n");
    return false;
  }

  return true;
}
  
static bool validate_sequence(const char *sequence) {
  // only 0s and 1s allowed...
  if (strlen(sequence) > MAX_SEQUENCE_STR_LEN - 1) {
    Serial.print("Sequence too big\n");
    return false;
  }

  for (int i=0; i<strlen(sequence)-1; i++) {
    if (sequence[i] != '0' && sequence[i] != '1') {
      Serial.print("Sequence not ones and zeros\n");
      return false;
    }
  }

  return true;
}

static bool validate_clock(const char *clock_str) {
  long clock_time = 0;
  char *endptr;
  if (strlen(clock_str) > MAX_CLOCK_STR_LEN - 1) {
    Serial.print("Clock str too big\n");
    return false;
  }
  clock_time = strtol(clock_str, &endptr, 0);
  if (*endptr != '\0') {
    Serial.print("Clock not numeric\n");
    return false;
  }
  return true;
}

static void init_current_colors(const char *color) {
  // NB - validation is assumed to have happened previously
  char temp_buf[MAX_COLOR_STR_LEN];
  char *red, *green, *blue;
  long r, g, b;
  char *endptr;
  char msgBuf[256];

  strcpy(current_color_str, color);
  strcpy(temp_buf, color);
  red = strtok(temp_buf, ",");
  green = strtok(NULL, ",");
  blue = strtok(NULL, ",");
/*
  current_red = (uint8_t)(strtof(red, &endptr, 0) * 255);
  current_green = (uint8_t)(strtof(green, &endptr, 0) * 255);
  current_blue = (uint8_t)(strtof(blue, &endptr, 0) * 255);
*/
  current_red = (uint8_t)(strtof(red, &endptr) * 255);
  current_green = (uint8_t)(strtof(green, &endptr) * 255);
  current_blue = (uint8_t)(strtof(blue, &endptr) * 255);

  sprintf(msgBuf, "Set red: 0x%x, g: 0x%x, b: 0x%x\n", current_red, current_green, current_blue);
  Serial.print(msgBuf);
}

static void init_current_clock(const char *clock_time) {
  // NB - validation is assumed to have happened previously
  char msgBuf[256];
  char *endptr;

  strcpy(current_clock_str, clock_time);
  current_clock = strtol(current_clock_str, &endptr, 0);
  
  sprintf(msgBuf, "Setting clock to %d\n", current_clock);
  Serial.print(msgBuf);
}

static void init_current_sequence(const char *sequence) {
  char msgBuf[256];
  
  strcpy(current_sequence_str, sequence);
  current_sequence_idx = 0;
  current_sequence_len = strlen(current_sequence_str);

  sprintf(msgBuf, "Setting sequence to %s\n", current_sequence_str);
  Serial.print(msgBuf);
}

static void randomize_start() {
  /* Randomize the start of the current sequence by setting the index
   *  in the sequence we're currently at, and adding some delay so we're
   *  not always at the CLOCK_TIME edge
   */
  current_sequence_idx = ESP8266TrueRandom.random(1, current_sequence_len);
  int sleep_delay = ESP8266TrueRandom.random(1, current_clock) * CLOCK_TIME \
                + ESP8266TrueRandom.random(1, CLOCK_TIME);
  delay(sleep_delay);
  
}

static void set_blink_pattern(const char* color, const char *clock_time, const char *sequence) {
  if (!strcmp(color, current_color_str) \
     && !strcmp(clock_time, current_clock_str) \
     && !strcmp(sequence, current_sequence_str)) {
    // No change - do nothing
    return;
  }
  
  if (!validate_color(color) || !validate_clock(clock_time) || !validate_sequence(sequence)) {
    Serial.print("Input data invalid, ignoring\n");
    return;
  }

  init_current_colors(color);
  init_current_sequence(sequence);
  init_current_clock(clock_time);

  randomize_start();

  doLEDs = true;
}

static void blink_halt() {
  doLEDs = false;
}

static uint8_t simple_checksum(uint8_t *bytes, int len) {
  char msgBuf[256];
  uint8_t sum = 0;
  uint8_t *ptr = bytes;
  for (int i=0; i<len; i++, ptr++) {
    sum += *ptr;
    //sprintf(msgBuf, "Sum is %d\n", sum);
    //Serial.print(msgBuf);    
  }
  return sum;
}

/* 
 *  Firefly command packets
 *  
 *  Command packets are broadcast over UDP, and addressed to 
 *  a particular 'swarm'. The format of the command packet is as 
 *  follows:
 *  
 *  HEADER. 8 bytes. 
 *     0-3 : 'FLG2'
 *     4   : recipient id
 *     5   : payload_len.
 *     6   : pad
 *     7   : checksum.
 *     
 *  PAYLOAD. n bytes. All ASCII, with fields delimited by ':'
 *  Field 1: Command. Currently 'BL' (blink) and 'HALT'.
 *  Field 2: Color (for blink command). R,G,B - comma delimited.
 *  Field 3: Clock speed (100 ms units)
 *  Field 4: Sequence ('1's and '0's)
 *  
 *  So, for instance, the payload
 *  'BL:255,255,255:5:000111111000'
 *  will cause the fireflies to blink white for 3 seconds, then off for 3 seconds
 *     
 */
typedef struct FLG_Firefly_Packet_Header{
  char header_id[4]; // 'FLG2'
  uint8_t recipient_id; // board id, all boards is '0'
  uint8_t payload_len; // Does not include header
  uint8_t pad; 
  uint8_t checksum;  // simple checksum of packet data.
} FLG_FireFly_Packet_Header;

static bool validatePacket(char *packet, uint8_t packet_len) {
  int header_len = sizeof(FLG_Firefly_Packet_Header);
  if (packet_len < header_len) {
    return false;
  }
  FLG_Firefly_Packet_Header *hdr = (FLG_Firefly_Packet_Header *)packet;
  if (strncmp(hdr->header_id, "FLG2", 4)) {
    Serial.print("Bad packet header\n");
    return false;
  }

  Serial.printf("Board id %d\n", hdr->recipient_id);
//  if (hdr->recipient_id != 0 && hdr->recipient_id != board_id) {
  if (hdr->recipient_id == 0) {
    Serial.print("Board id zero\n");
  }
  if (hdr->recipient_id != board_id && hdr->recipient_id != 0) {
    Serial.print("Recipient not us\n");
    return false;
  }

  if (hdr->payload_len != packet_len - header_len) {
    Serial.print("Payload length invalid\n");
    return false;
  }

  if (hdr->checksum != simple_checksum((uint8_t*)(packet + header_len), packet_len - header_len)) {
    char msgBuf[256];
    /*memcpy(msgBuf, (uint8_t*)(packet + header_len), packet_len - header_len);
    msgBuf[packet_len - header_len] = '\0';
    Serial.print(msgBuf);
    sprintf(msgBuf, "Checksum invalid. Len is %d\n", packet_len - header_len);
    Serial.print(msgBuf);
    sprintf(msgBuf, "First character of checksum data is %c\n", (packet + header_len)[0]);
    Serial.print(msgBuf); */
    Serial.print("Checksum invalid\n");
    return false;
  }
  return true;
}

static void processPacket(char *packet, uint8_t packet_len) {
  char *packet_header;
  char *cmd;
  char *color;
  char *clock_time;
  char *sequence;
  char id_str[4];
  int recipient_id;

  if (!validatePacket(packet, packet_len)) {
    return;
  }

  // Initialize parse and skip past header
  char *payload_start = packet + sizeof(FLG_Firefly_Packet_Header);
  Serial.printf("payload is %s\n", payload_start);

  // Parse validated packet.
  cmd = strtok(payload_start, ":");
  if (!strcmp(cmd, "BL")) { // blink command
    color = strtok(NULL, ":");
    clock_time = strtok(NULL, ":");
    sequence = strtok(NULL, ":");
    if (color == NULL || clock_time == NULL || sequence == NULL) {
      Serial.print("Error parsing blink packet\n");
    }
    Serial.print("Received valid packet\n");
    set_blink_pattern(color, clock_time, sequence);
  } else if (!strcmp(cmd, "HALT")) { 
    blink_halt();
  } else if (!strcmp(cmd, "FW")) {
    //char url_buf[1024]; 
    // firmware update
    char *url  = strtok(NULL, ":");
    //char *url = payload_start+3;
    Serial.printf("Firmware update, url is %s\n", url);
    char *url_post_port = strtok(NULL, ":");
    char *stuff = strtok(NULL, ":");
    char *md5 = strtok(NULL, ":");
    url[strlen(url)] = ':';  // restore server
    url[strlen(url)] = ':';  // restore port
/*    char *md5 = get_last_section(url);
    if (md5 == NULL) {
      return;
    }
*/
    Serial.printf("Received firmware update packet, md5: %s, current: %s\n", md5, fw_checksum);
    //strcpy(url_buf, url);
    //strcat(url_buf, ":");
    //strcat(url_buf, url_post_port);
    Serial.printf("url is %s\n", url);
    //Serial.printf("Final url %s\n", url_buf);
    if (strcmp(md5, fw_checksum) != 0) {
      Serial.print("Attempting OTA update\n");
      // XXX - set a timer to do this. Not right now!
      WiFiClient wf_client;
   /*   if (wf_client.connect("google.com", 80)) {
        Serial.print("Connect success\n");
      } else {
        Serial.print("Connect failure\n");
      }
      */
      
      /*HTTPClient http;
      bool ret1 = http.begin("http://192.168.4.125:8080/firefly_leds");
      Serial.printf("http ret 1 is %d\n", ret1);
      int ret2 = http.GET();
      Serial.printf("http ret 2 is %d\n", ret2); */
      //url = "http://192.168.4.125:7000/firefly_leds";
      t_httpUpdate_return ret = ESPhttpUpdate.update(wf_client, url);
      //t_httpUpdate_return ret = ESPhttpUpdate.update("http://192.168.4.125", 7000, "firefly_leds");
      Serial.printf("update returns %d\n", ret);
      //WiFi.disconnect();
      // nb - we may have to turn WifiUdp back on after this XXX CHECK!
    } else {
      Serial.print("Update refused - already have firmware\n");
    }
  } 
}


int counter = 0;
int lasttime_ms = 0;
int onOff = 0;

static void set_LEDs() {
  char printBuf[256];
  if (!doLEDs) {
    return;
  }
//  sprintf(printBuf, "current_sequence_str is %s, idx is %d\n", current_sequence_str, current_sequence_idx);
//  Serial.print(printBuf);
  if (current_sequence_str[current_sequence_idx] == '1') {
    Serial.print("On\n");
    for (int i=0; i<NUM_LEDS; i++) {
      leds[i].setRGB(current_red, current_green, current_blue);
    }
  } else {
    Serial.print("Off\n");
    for (int i=0; i<NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
    }
  }
  FastLED.show();
  current_sequence_idx++;
  if (current_sequence_idx >= current_sequence_len) {
    current_sequence_idx = 0;
  }
}

static void get_board_id() {
  char statusBuf[512];
  uint32_t analogIn = analogRead(A0);

  if (analogIn < 340) {
    board_id = 1;
  } else if (analogIn < 680) {
    board_id = 2;
  } else {
    board_id = 3;
  }
   
   sprintf(statusBuf, "Reading %d on analog, board id %d\n", analogIn, board_id);
   Serial.print(statusBuf);
}


void setup() { 
    pinMode(2, OUTPUT);
    pinMode(4, OUTPUT);
    // Uncomment/edit one of the following lines for your leds arrangement.
    // FastLED.addLeds<TM1803, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1804, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1809, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    //FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    // FastLED.addLeds<APA104, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<UCS1903, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<UCS1903B, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<GW6205, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<GW6205_400, DATA_PIN, RGB>(leds, NUM_LEDS);
      
    // FastLED.addLeds<WS2801, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<SM16716, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<LPD8806, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<P9813, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<APA102, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<DOTSTAR, RGB>(leds, NUM_LEDS);

    // FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<SM16716, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<P9813, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  
    Serial.begin(9600);  // yeah. Slow but reliable...
    Serial.print("...Init...\n");
    Serial.print("   v4  \n");

    init_current_sequence(default_sequence_str);
      
    WifiInit();
    

    OtaClientInit();

    delay(500);

    get_board_id();

    char serialBuf[256];

    sprintf(serialBuf, "...Board id is %d\n", board_id);
}

static void FlashLEDs(int times) {
  // Quickly flash the onboard LED - use as a status indicator. Will block
  // For some reason, this was crashing the ESP8266. WTF.
  Serial.print("Begin flash LEDs\n");
  /*
  digitalWrite(BLINK_PIN, LOW); 
  delay(20);
  for (int i=0; i<times; i++) {
    digitalWrite(BLINK_PIN, HIGH);
    delay(20);
    digitalWrite(BLINK_PIN,LOW);
    delay(20);
  }
  digitalWrite(BLINK_PIN, onOff ? HIGH : LOW);
  */
  Serial.print("End flash LEDs\n");
}

static char *get_last_section(const char *url){
  char *slash_ptr = NULL;
  char *cur_ptr = (char *)url;
  while(*cur_ptr != '\0'){
    if (*cur_ptr == '/'){
      Serial.print("Found slash!\n");
      slash_ptr = cur_ptr;
    }
    cur_ptr++;
  }
  Serial.printf("get last section returns %p\n", slash_ptr);
  return (slash_ptr == NULL ? NULL : slash_ptr+1);
}

#define BLINK_TIME 500
#define MAX_PACKET_LEN 255

void loop() {

  if (do_reboot) {
    Serial.flush();
    ESP.restart();
  }

  // Check AP status, reconnect/connect if necessary
  WifiMulti.run();

  // blink the onboard leds if we're on a clock edge
  if ((counter % current_clock) == 0) {
    set_LEDs();
    digitalWrite(BLINK_PIN, onOff?HIGH:LOW);
    onOff = 1 - onOff;
  }

/*  // get board id. Not that it's going to change...
   if (counter % 10 == 0) {
    delay(1);
    get_board_id();
    char statusBuf[512];
    Serial.print("tick!\n");
    sprintf(statusBuf, "Reading %d on analog\n", analogRead(A0));
  }
*/

  // read and process UDP commands
  if (WiFi.status() == WL_CONNECTED) {
    int packetSize;
//    WiFi.mode(WIFI_STA);
    while ((packetSize=Udp.parsePacket()) > 0) {
      
      char packet[MAX_PACKET_LEN + 1];
      char replyBuf[256];

      if (packetSize > MAX_PACKET_LEN) {
        // whatever the fuck this is, we don't want it.
        Udp.read(packet, MAX_PACKET_LEN);
        continue;      
      }
      int len = Udp.read(packet, MAX_PACKET_LEN);
    
      packet[len] = '\0';
      processPacket(packet, len);
    
      //sprintf(replyBuf, "Received packet %s\n", packet);
      //Serial.print(replyBuf);
    }
  }
/*
  // blink leds
  int curTime = millis();
  if (curTime > lasttime_ms + BLINK_TIME) {
    digitalWrite(BLINK_PIN, onOff?HIGH:LOW);
    //digitalWrite(DATA_PIN, onOff?HIGH:LOW);
    if (onOff) {
      leds[0] = CRGB::Black;
      leds[1] = CRGB::Black;
      leds[2] = CRGB::Black;
      leds[3] = CRGB::Black;
      leds[4] = CRGB::Black;
      leds[5] = CRGB::Black;
      leds[6] = CRGB::Black;
      leds[7] = CRGB::Black;
      leds[8] = CRGB::Black;
      leds[9] = CRGB::Black;
      Serial.println("Off");
    } else { 
 //     Serial.println("On");
 //     leds[0] = CRGB::Green;
 //     leds[1] = CRGB::Red;
 //     leds[2] = CRGB::Blue;
 //     leds[3] = CRGB::Green;
 //     leds[4] = CRGB::Red;
 //     leds[5] = CRGB::Blue;
 //     leds[6] = CRGB::Green;
 //     leds[7] = CRGB::Red;
 //     leds[8] = CRGB::Blue;
 //     leds[9] = CRGB::Green;       
      leds[0] = CRGB::White;
      leds[1] = CRGB::White;
      leds[2] = CRGB::White;
      leds[3] = CRGB::White;
      leds[4] = CRGB::White;
      leds[5] = CRGB::White;
      leds[6] = CRGB::White;
      leds[7] = CRGB::White;
      leds[8] = CRGB::White;
      leds[9] = CRGB::White;      
    }
    FastLED.show();
    lasttime_ms = curTime;
    onOff = 1 - onOff;
    
  }*/
  counter++;
  if (counter > 10000) {
    counter = 0;
  }
  delay(CLOCK_TIME);
}
