void cron1() {
  //Your Cronjob action here
  Serial.println("Cronjob 1 active");
}

void cron2() {
  //Your Cronjob action here
  Serial.println("Cronjob 2 active");
}

void cron3() {
  //Your Cronjob action here
  Serial.println("Cronjob 3 active");
}

void do_cronjobs() {
  long tmp = millis();
  if ((cronjob.t1 + (cronjob.c1 * 1000)) <= tmp) {
    cron1();
    cronjob.t1 = millis();
  }
  if ((cronjob.t2 + (cronjob.c2 * 1000)) <= tmp) {
    cron2();
    cronjob.t2 = millis();
  }
  if ((cronjob.t3 + (cronjob.c3 * 1000)) <= tmp) {
    cron3();
    cronjob.t3 = millis();
  }
}
