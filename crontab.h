void cron1() {
  //Your Cronjob action here
  
}

void cron2() {
  sensor.clearFields();
  sensor.addField("temperatur", bmp.readTemperature());
  sensor.addField("druck", bmp.readPressure()/100);
  Serial.print("Writing: ");
  Serial.println(influx_client.pointToLineProtocol(sensor));
  // Write point
  if (!influx_client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(influx_client.getLastErrorMessage());
  }
  
}

void cron3() {
  //Your Cronjob action here
  
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
