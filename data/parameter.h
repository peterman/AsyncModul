struct Settings {
  char ssid[15] = "devwifi";
  char pass[15] = "devwifi1";
  char host[15] = "asm-connect";
  char edit_user[10] = "admin";
  char edit_pass[10] = "admin";
};

struct Cronjob {
  int c1 = 10;  // 10 sek.
  int c2 = 30;  // 30 sek.
  int c3 = 300; // 300 sek.
  long t1;
  long t2;
  long t3;
};

const char *settingsfile = "/settings.json";
const char *wifinets = "/wifinets.json";

Settings settings;
Cronjob cronjob;
