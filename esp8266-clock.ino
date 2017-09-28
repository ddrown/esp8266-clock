#include <ESP8266WiFi.h>

#include <Adafruit_GFX.h>    // Core graphics library
// the ESP8266 version of the ST7735 library: https://github.com/ddrown/Adafruit-ST7735-Library
#include <Adafruit_ST7735.h> // Hardware-specific library
// millisecond precision clock library: https://github.com/ddrown/Arduino_Clock
#include <Clock.h>
// Timezone library ported to ESP8266: https://github.com/ddrown/Timezone
#include <Timezone.h>
// ESP8266 NTP client with millisecond precision: https://github.com/ddrown/Arduino_NTPClient
#include <NTPClient.h>
// ClockPID calculates the rate adjustment the local clock needs based on offset measurements. https://github.com/ddrown/Arduino_ClockPID
#include <ClockPID.h>

#include "settings.h" // see this file to change the settings

#include "icons/birthday.h"
#include "icons/haloween.h"
#include "icons/independance_day.h"
#include "icons/thanksgiving.h"
#include "icons/valentines.h"
#include "icons/candy_cane.h"
#include "icons/christmas_tree.h"
#include "icons/santa.h"
#include "icons/firework.h"
#include "icons/f0.h"
#include "icons/f1.h"
#include "icons/f2.h"
#include "icons/f3.h"
#include "icons/f4.h"
#include "icons/f5.h"
#include "icons/f6.h"
#include "icons/f7.h"
#include "icons/f8.h"
#include "icons/f9.h"
#include "icons/slash.h"
#include "icons/l0.h"
#include "icons/l1.h"
#include "icons/l2.h"
#include "icons/l3.h"
#include "icons/l4.h"
#include "icons/l5.h"
#include "icons/l6.h"
#include "icons/l7.h"
#include "icons/l8.h"
#include "icons/l9.h"
#include "icons/colon.h"

struct holiday_icon {
  const uint16_t *icon;
  uint8_t month;
  uint8_t day;
  boolean enabled;
};

// rules like "the fourth thursday" are encoded as WEEKDAY_RULE + DAY
// where DAY is the day of the month if the first day was Sunday
#define WEEKDAY_RULE 40
#define FIRST_SUNDAY WEEKDAY_RULE+1
#define FIRST_MONDAY WEEKDAY_RULE+2
#define FIRST_TUESDAY WEEKDAY_RULE+3
#define FIRST_WEDNESDAY WEEKDAY_RULE+4
#define FIRST_THURSDAY WEEKDAY_RULE+5
#define FIRST_FRIDAY WEEKDAY_RULE+6
#define FIRST_SATURDAY WEEKDAY_RULE+7
#define SECOND_THURSDAY WEEKDAY_RULE+5+7
#define THIRD_THURSDAY WEEKDAY_RULE+5+14
#define FOURTH_THURSDAY WEEKDAY_RULE+5+21
#define FIFTH_SUNDAY WEEKDAY_RULE+1+28

// TODO: settings to turn these off/on and configure day
struct holiday_icon icons[] = {
  { .icon = firework, .month = 1, .day = 1, .enabled = true},
  { .icon = valentines, .month = 2, .day = 14, .enabled = true},
  { .icon = birthday, .month = 9, .day = 28, .enabled = true},
  { .icon = independance_day, .month = 7, .day = 4, .enabled = true},
  { .icon = haloween, .month = 10, .day = 31, .enabled = true},
  { .icon = thanksgiving, .month = 11, .day = FOURTH_THURSDAY, .enabled = true},
  { .icon = christmas_tree, .month = 12, .day = 24, .enabled = true},
  { .icon = santa, .month = 12, .day = 25, .enabled = true},
  { .icon = candy_cane, .month = 12, .day = 24, .enabled = true},
};
uint8_t icons_index = 0;
#define NO_ICON 255

const uint16_t *small_digits[] = {
  f0, f1, f2, f3, f4, f5, f6, f7, f8, f9
};

const uint16_t *large_digits[] = {
  l0, l1, l2, l3, l4, l5, l6, l7, l8, l9
};

// local port to listen for UDP packets, TODO: randomize and switch
#define LOCAL_NTP_PORT 2390
IPAddress timeServerIP;
NTPClient ntp;

// full screen+text: ~72ms, full screen: ~11ms, tens seconds: ~2ms, ones seconds: ~1ms
#define SPI1_CLK   D5
#define SPI1_MISO  D6
#define SPI1_MOSI  D7
#define TFT_CS     D8
#define TFT_RST    D2
#define TFT_DC     D1
#define SERIALPORT Serial

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

uint32_t ntp_packets = 0;

void tft_setup() {
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(3);
  tft.setTextSize(1);
  tft.setTextWrap(false);
  tft.setTextColor(ST7735_WHITE);  
}

void wifi_setup() {
  tft.println("Connecting to SSID");
  tft.println(ssid);
  SERIALPORT.println("Connecting to SSID");
  SERIALPORT.println(ssid);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    tft.print(".");
    SERIALPORT.print(".");
  }
  tft.println();
  SERIALPORT.println();

  // TODO: show wifi strength
  tft.println("WiFi connected");
  SERIALPORT.println("WiFi connected");
  tft.println("IP address: ");
  SERIALPORT.println("IP address: ");
  tft.println(WiFi.localIP());
  SERIALPORT.println(WiFi.localIP());
}

void ntp_setup() {
  tft.println("Starting NTP/UDP");
  SERIALPORT.println("Starting NTP/UDP");
  ntp.begin(LOCAL_NTP_PORT);

  WiFi.hostByName(ntpServerName, timeServerIP); // TODO: lookup failures and re-lookup DNS pools

  tft.print("NTP IP: ");
  SERIALPORT.print("NTP IP: ");
  tft.println(timeServerIP.toString());
  SERIALPORT.println(timeServerIP.toString());
}

void setup() {
  tft_setup();

  SERIALPORT.begin(115200);
  SERIALPORT.println();

  tft.setTextWrap(true);
  tft.setCursor(0, 0);
  wifi_setup();
  ntp_setup();

  tft.println("Delayed start");
  SERIALPORT.println("Delayed start");
  tft.print(".");
  SERIALPORT.print(".");
  delay(1000);
  SERIALPORT.println();
  tft.fillScreen(ST7735_BLACK);
  tft.setTextWrap(false);
  tft.setCursor(0, 0);
}

int set_icons_index(time_t now) {
  for(uint8_t i = 0; i < sizeof(icons)/sizeof(icons[0]); i++) {
    if(icons[i].enabled && icons[i].month == month(now)) {
      if(icons[i].day > WEEKDAY_RULE) { // happens on a set day of the week
        uint8_t find_day = icons[i].day - WEEKDAY_RULE;  // find_day is a month starting on a sunday
        uint8_t want_wday = (find_day - 1) % 7; // sunday = 0, saturday = 6
        uint8_t want_week = (find_day - 1) / 7;
        uint8_t this_week = (day(now)-1) / 7;
        if(this_week == want_week && want_wday == weekday(now)-1) {
          icons_index = i;
          return 1;
        }
      } else if(icons[i].day == day(now)) { // happens on a set day
        icons_index = i;
        return 1;
      }
    }
  }
  icons_index = NO_ICON;
  return 0;
}

void digit(uint8_t x, uint8_t y, uint8_t value, const uint16_t **digit_array, uint8_t w, uint8_t h) {
  if(value > 9)
    return;
  tft.PixelArray(x, y, w, h, digit_array[value]);
}

void tens(uint16_t value, uint8_t x, uint8_t y, const uint16_t **digit_array, uint8_t w, uint8_t h) {
  value = (value / 10) % 10;
  digit(x, y, value, digit_array, w, h);
}

void ones(uint16_t value, uint8_t x, uint8_t y, const uint16_t **digit_array, uint8_t w, uint8_t h) {
  value = value % 10;
  digit(x, y, value, digit_array, w, h);
}

void show_next_ntp(int32_t next_ntp) {
  static char existing[4] = {' ',' ',' ',' '};
  char new_digits[4];
  uint32_t digit = 1000;

  for(uint32_t i = 0; i < 4; i++) {
    if(next_ntp < digit) {
      new_digits[i] = ' ';
    } else {
      new_digits[i] = ((next_ntp / digit) % 10) + '0';
    }
    if(i == 3 && next_ntp == 0) {
      new_digits[i] = '0';
    }

    if(new_digits[i] != existing[i]) {
      tft.fillRect(36+i*6, 120, 6, 8, ST7735_BLACK);
      if(new_digits[i] != ' ') {
        tft.setCursor(36+i*6, 120);
        tft.print(new_digits[i]);
      }
      existing[i] = new_digits[i];
    }
    
    digit = digit / 10;
  }
}

void time_print(time_t now, const char *tzname, int32_t next_ntp) {
  static uint8_t runonce = 1;
  if(runonce) {
    tft.fillScreen(ST7735_BLACK);
    tft.PixelArray(16, 0, 8, 16, slash);
    tft.PixelArray(40, 0, 8, 16, slash);
    
    tft.PixelArray(48, 40, 16, 24, colon);
    tft.PixelArray(96, 40, 16, 24, colon);

    tft.setCursor(0, 120);
    tft.print("next: ");
    tft.setCursor(66, 120);
    tft.print("s");
  }
  if(runonce || second(now) == 0) { // reprint everything at the top of the minute
    runonce = 0;
    
    // update seconds first because they're the most critical
    ones(second(now), 128, 40, large_digits, 16, 24);
    tens(second(now), 112, 40, large_digits, 16, 24);
    ones(minute(now), 80, 40, large_digits, 16, 24);
    tens(minute(now), 64, 40, large_digits, 16, 24);
    ones(hour(now), 32, 40, large_digits, 16, 24); // TODO: 12/24 hour modes
    tens(hour(now), 16, 40, large_digits, 16, 24);

    ones(year(now), 56, 0, small_digits, 8, 16);
    tens(year(now), 48, 0, small_digits, 8, 16);
    ones(day(now), 32, 0, small_digits, 8, 16);
    tens(day(now), 24, 0, small_digits, 8, 16);
    ones(month(now), 8, 0, small_digits, 8, 16);
    tens(month(now), 0, 0, small_digits, 8, 16);

    if(set_icons_index(now)) {
      tft.PixelArray(80, 0, 16, 16, icons[icons_index].icon);
    }
  } else {
    if(second(now) % 10) {
      // only the ones digit in seconds changed
    } else { 
      // both the ones and tens digit changed
      tens(second(now), 112, 40, large_digits, 16, 24);
    }
    ones(second(now), 128, 40, large_digits, 16, 24);
  }

  show_next_ntp((next_ntp > 0) ? next_ntp : 0);
}

void print_ntp_stats() {
  tft.fillRect(0, 88, 160, 32, ST7735_BLACK);
  tft.setCursor(0, 88);
  tft.print("offset: ");
  tft.print(ntp.getLastOffset_RTT());
  tft.println(" ms");
  tft.print("rtt: ");
  tft.print(ntp.getLastRTT());
  tft.println(" ms");
  tft.print(ClockPID.d() * 1000000);
  tft.print(" ppm-d ");
  tft.print(ClockPID.out() * 1000000);
  tft.println(" ppm-o");
  tft.print(ntp_packets);
  tft.println(" NTP packets");
}

int ntp_loop(bool ActuallySetTime) {
  PollStatus NTPstatus;
  static struct timems startLocalTS = {.tv_sec = 0};
  static struct timems startRemoteTS = {.tv_sec = 0};
  int retval = 0;

  ntp.sendNTPpacket(timeServerIP);

  while((NTPstatus = ntp.poll_reply(ActuallySetTime)) == NTP_NO_PACKET) { // wait until we get a response
    delay(1); // allow the background threads to run
  }
  
  if (NTPstatus == NTP_TIMEOUT) {
    adjustClockSpeed_ppm(ClockPID.d_out());
    SERIALPORT.println("NTP client timeout, using just the last D sample");
    retval = -1;
  } else if(NTPstatus == NTP_PACKET) {
    struct timems nowTS, remoteTS;
    uint32_t ms_delta;
    int32_t ppm_error;
    now_ms(&nowTS);

    ntp_packets++;

    ms_delta = nowTS.raw_millis - startLocalTS.raw_millis;
    ntp.getRemoteTS(&remoteTS);
    if((startLocalTS.tv_sec == 0) || (ms_delta > 2140000000)) { // reset clock on startup and when it gets close to wrapping at 2^31
      startLocalTS.tv_sec = nowTS.tv_sec;
      startLocalTS.tv_msec = nowTS.tv_msec;
      startLocalTS.raw_millis = nowTS.raw_millis;
      startRemoteTS.tv_sec = remoteTS.tv_sec;
      startRemoteTS.tv_msec = remoteTS.tv_msec;
      startRemoteTS.raw_millis = remoteTS.raw_millis;
      ClockPID.reset_clock();
      SERIALPORT.println("reset clock");
      ms_delta = 0;
    }

    int32_t offset = ntp.getLastOffset_RTT();
    int32_t raw_offset = ts_interval(&startRemoteTS, &remoteTS) - ms_delta;
    uint32_t rtt = ntp.getLastRTT();
    float clock_error = ClockPID.add_sample(ms_delta, raw_offset, offset);
    adjustClockSpeed_ppm(clock_error);

    SERIALPORT.print("now=");
    SERIALPORT.print(nowTS.tv_sec);
    SERIALPORT.print(" rtt=");
    SERIALPORT.print(rtt);
    SERIALPORT.print(" ppm=");
    SERIALPORT.print(clock_error * 1000000);
    SERIALPORT.print(" offset=");
    SERIALPORT.print(offset);
    SERIALPORT.print(" raw=");
    SERIALPORT.print(raw_offset);
    SERIALPORT.print(" delta=");
    SERIALPORT.println(ms_delta);

    SERIALPORT.print(ClockPID.p());
    SERIALPORT.print(",");
    SERIALPORT.print(ClockPID.i());
    SERIALPORT.print(",");
    SERIALPORT.print(ClockPID.d() * 1000000);
    SERIALPORT.print(",");
    SERIALPORT.print(ClockPID.d_chi() * 1000000);
    SERIALPORT.print(",");
    SERIALPORT.print(ClockPID.out() * 1000000);
    SERIALPORT.print(",");
    SERIALPORT.print(ClockPID.p_out() * 1000000);
    SERIALPORT.print(",");
    SERIALPORT.print(ClockPID.i_out() * 1000000);
    SERIALPORT.print(",");
    SERIALPORT.println(ClockPID.d_out() * 1000000);
  }

  return retval;
}

void loop() {
  struct timems startTS;
  TimeChangeRule *tcr;
  struct timems last_t;
  time_t local, next_ntp, fast_ntp_done;
  
  last_t.tv_sec = 0;
  last_t.tv_msec = 0;

  while(ntp_loop(true) < 0) { // set time
    SERIALPORT.println("NTP request failed, retrying");
    tft.println("NTP request failed, retrying");
    delay(64000);
  }
  next_ntp = now() + 64;
  fast_ntp_done = now() + 64*4; // first 4 samples at 64s each
  now_ms(&startTS);
  
  while(1) {
    struct timems nowTS;
    now_ms(&nowTS);
    if(nowTS.tv_sec == last_t.tv_sec) {
      struct timems afterSleepTS;
      uint32_t sleeptime = 1000 - nowTS.tv_msec;  // sleep till next second
      delay(sleeptime);
      now_ms(&afterSleepTS);
      if(
        ((afterSleepTS.tv_sec == nowTS.tv_sec) && (afterSleepTS.tv_msec < 990)) || 
        (afterSleepTS.tv_sec > nowTS.tv_sec+1) ||
        ((afterSleepTS.tv_sec != nowTS.tv_sec) && (afterSleepTS.tv_msec > 100))
        ) { // print a warning if we slept too long or too short
        SERIALPORT.print("unexpected return from sleep! before (");
        SERIALPORT.print(nowTS.tv_sec);
        SERIALPORT.print("s ");
        SERIALPORT.print(nowTS.tv_msec);
        SERIALPORT.print("ms) after (");
        SERIALPORT.print(afterSleepTS.tv_sec);
        SERIALPORT.print("s ");
        SERIALPORT.print(afterSleepTS.tv_msec);
        SERIALPORT.print("ms) sleeptime ");
        SERIALPORT.println(sleeptime);
      }
    }
    now_ms(&last_t);
    local = TIMEZONE.toLocal(last_t.tv_sec, &tcr);
    time_print(local, tcr->abbrev, next_ntp-last_t.tv_sec);

    if((last_t.tv_sec > next_ntp) && ((second(local) % 10) != 0)) { // repoll on seconds not ending in 0
      struct timems beforeNTP, afterNTP;
      now_ms(&beforeNTP);
      ntp_loop(false);
      if(last_t.tv_sec > fast_ntp_done) {
        next_ntp = last_t.tv_sec + NTP_INTERVAL;
      } else {
        next_ntp = last_t.tv_sec + 64;
      }
      now_ms(&afterNTP);
      SERIALPORT.print("ntp took ");
      SERIALPORT.print(ts_interval(&beforeNTP, &afterNTP));
      SERIALPORT.println("ms");

      print_ntp_stats();
    }
  }
}
