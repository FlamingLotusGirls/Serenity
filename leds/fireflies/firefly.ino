#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
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


/*
#define BUFFER_LENGTH 512
char incomingPacket[BUFFER_LENGTH]; 
*/
// NB - variables ssid and password are intentionally kept in 
// 'wifi_credentials.h' so that they don't need to be checked in

/*
extern const char* ssid;
extern const char *password;
*/

#define NUM_LEDS 10
#define DATA_PIN 4   // GPIO4. D2 on the board
#define BLINK_PIN 2  // GPI02. D4 on the board

CRGB leds[NUM_LEDS];
static int board_id = 1;



/*********************************************
 *  WIFI
 *********************************************/
static WiFiUDP Udp;
static IPAddress multicastAddress(224,3,29,71);
//IPAddress multicastAddress(224,0,0,1);
static unsigned int multicastPort = 5010;

static bool wifi_setup = false;

static void wifi_connect() {
  WiFi.begin(ssid, password);
  wifi_setup = false;
}

static bool wifi_is_connected() {
  return wifi_setup;
}

static void wifi_poke() {
  Serial.print("Wifi poke\n");
  int status = WiFi.status();
  if (status == WL_CONNECTED) {
    Serial.print("Wifi is connected!");
    if (!wifi_setup) {
      Serial.print("Connected, IP address: ");
      Serial.println(WiFi.localIP());
      WiFi.printDiag(Serial);

      WiFi.mode(WIFI_STA);
      wifi_set_sleep_type(NONE_SLEEP_T);

      Udp.beginMulticast(WiFi.localIP(), multicastAddress, multicastPort);
      wifi_setup = true;
    }
  } else {
    char printBuf[256];
    sprintf(printBuf, "Wifi status is %d\n", status);
    Serial.print(printBuf);
  }
}

static bool wifi_disconnect() {
  WiFi.disconnect();
  wifi_setup = false;
}


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
static uint8_t current_red = 0xe0;
static uint8_t current_blue = 0xe0;
static uint8_t current_green = 0xff;

static bool doLEDs = true;

static bool validate_color(const char *color) {
  char temp_buf[MAX_COLOR_STR_LEN];
  char *red, *green, *blue;
  long r, g, b;
  char *endptr;
  
  if (strlen(color) > MAX_COLOR_STR_LEN - 1) {
    return false;
  }
  strcpy(temp_buf, color);
  red = strtok(temp_buf, ",");
  green = strtok(NULL, ",");
  blue = strtok(NULL, ",");

  r = strtol(red, &endptr, 0);
  if (*endptr != '\0' 
       || r < 0 
       || r > 255 ) {
    Serial.print("red not correct\n");
    return false;
  }
  g = strtol(green, &endptr, 0);
  if (*endptr != '\0' 
       || g < 0 
       || g > 255 ) {
   Serial.print("green not correct\n");
   return false;
  }
  b = strtol(blue, &endptr, 0);
  if (*endptr != '\0' 
       || b < 0 
       || b > 255 ) {
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

  current_red = (uint8_t)strtol(red, &endptr, 0);
  current_green = (uint8_t)strtol(green, &endptr, 0);
  current_blue = (uint8_t)strtol(blue, &endptr, 0);

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
  if (hdr->recipient_id != 0 && hdr->recipient_id != board_id) {
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

  // Parse validated packet.
  cmd = strtok(payload_start, ":");
  if (!strcmp(cmd, "BL")) { // blink command
    color = strtok(NULL, ":");
    clock_time = strtok(NULL, ":");
    sequence = strtok(NULL, ":");
    if (color == NULL || clock_time == NULL || sequence == NULL) {
      Serial.print("Error parsing blink packet\n");
    }
    set_blink_pattern(color, clock_time, sequence);
  } else if (!strcmp(cmd, "HALT")) { 
    blink_halt();
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
    uint32_t color = ((current_red << 16) && (current_green << 8) && (current_blue));
    for (int i=0; i<NUM_LEDS; i++) {
      leds[i] = color;
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
   
   sprintf(statusBuf, "Reading %d on analog, board id %d\n", analogRead(A0), board_id);
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

    init_current_sequence(default_sequence_str);
      
    wifi_connect();

    delay(500);

    for (int i=0; i<100; i++) {
      wifi_poke();
      if (wifi_is_connected()) {
        break;
      }
      delay(500);
    }

    get_board_id();

    char serialBuf[256];

    sprintf(serialBuf, "...Board id is %d\n", board_id);

    
    
/*
    WiFi.begin(ssid, password);

    Serial.print("Connecting\n");
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println();

    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
    WiFi.printDiag(Serial);

    WiFi.mode(WIFI_STA);
    wifi_set_sleep_type(NONE_SLEEP_T);

    Udp.beginMulticast(WiFi.localIP(), multicastAddress, multicastPort);
*/
}

#define BLINK_TIME 500
#define WIFI_PING_INTERVAL 10
#define WIFI_CHECK_INTERVAL 5
#define MAX_PACKET_LEN 255

void loop() {
  
  // Attempt to connect to wifi if it's not up
  if (!wifi_is_connected() && (counter % WIFI_CHECK_INTERVAL) == 0) {
    wifi_poke();
  }

  // XXX - may want to have a poke counter and restart wifi...

  // Check for wifi up
  if (wifi_is_connected() && (counter % WIFI_PING_INTERVAL == 0)) {
    //ping_wifi(); 
  }

  // blink the onboard leds if we're on a clock edge
  if ((counter % current_clock) == 0) {
    set_LEDs();
    digitalWrite(BLINK_PIN, onOff?HIGH:LOW);
    onOff = 1 - onOff;
  }

  // get board id. Not that it's going to change...
   if (counter % 10 == 0) {
    delay(1);
    get_board_id();
    char statusBuf[512];
    Serial.print("tick!\n");
    sprintf(statusBuf, "Reading %d on analog\n", analogRead(A0));
  }

  // read UDP and process UDP commands
  if (wifi_is_connected()) {
    int packetSize;
    while ((packetSize=Udp.parsePacket()) > 0) {
      
      char packet[MAX_PACKET_LEN + 1];
      char replyBuf[256];

      if (packetSize > MAX_PACKET_LEN) {
        // whatever the fuck this is, we don't want it.
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
  delay(100);
}
