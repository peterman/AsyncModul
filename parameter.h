struct Settings {
  char ssid[15] = "Weissig2011";
  char pass[15] = "VereinsHaus";
  char host[15] = "asm-connect";
  char edit_user[10] = "admin";
  char edit_pass[10] = "admin";  
};

struct Cronjob {
  int c1 = 10;  // 10 sek.
  int c2 = 300; // 300 sek.
  int c3 = 600; // 300 sek.
  long t1;
  long t2;
  long t3;
};

struct Influx {
  char url[40] = "http://iot.pfeiffer-privat.de:8086";
  char token[100] = "PBzjW8lC34KJS-JZZepZriH63TGNm1jH93wpKsDQuW-q7XgxCVY5nYcmYE3OVn5ZjK43B6ygvHppanMwXSSjmw==";
  char org[30] = "MyServer";
  char bucket[30] = "Sensor Vereinshaus";
};

#define ONBOARD_LED  2

           
  

const char *settingsfile = "/settings.json";
const char *wifinets = "/wifinets.json";

Influx influxdb;
Settings settings;
Cronjob cronjob;
Adafruit_BMP280 bmp;

InfluxDBClient influx_client(influxdb.url, influxdb.org, influxdb.bucket, influxdb.token);
Point sensor("Sensor 1");
