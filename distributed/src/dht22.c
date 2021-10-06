#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>

#include "../inc/dht22.h"

#define MAX_TIMINGS	85
#define DEBUG 1
#define WAIT_TIME 2000

uint8_t dht_pin = 29;

float temp_cels = -1;
float temp_fahr = -1;
float humidity  = -1;

uint8_t sizecvt(const int read);

float get_temp() {
    return temp_cels;
}

float get_hum() {
    return humidity;
}

int read_dht22_dat() {
  uint8_t laststate = HIGH;
  uint8_t counter = 0;
  uint8_t j = 0, i;
  int dht22_dat[5] = {0,0,0,0,0};

  dht22_dat[0] = dht22_dat[1] = dht22_dat[2] = dht22_dat[3] = dht22_dat[4] = 0;

  // pull pin down for 18 milliseconds
  pinMode(dht_pin, OUTPUT);
  digitalWrite(dht_pin, HIGH);
  delay(500);
  digitalWrite(dht_pin, LOW);
  delay(20);
  // prepare to read the pin
  pinMode(dht_pin, INPUT);

  // detect change and read data
  for ( i=0; i< MAX_TIMINGS; i++) {
    counter = 0;
    while (sizecvt(digitalRead(dht_pin)) == laststate) {
      counter++;
      delayMicroseconds(2);
      if (counter == 255) {
        break;
      }
    }
    laststate = sizecvt(digitalRead(dht_pin));

    if (counter == 255) break;

    // ignore first 3 transitions
    if ((i >= 4) && (i%2 == 0)) {
      // shove each bit into the storage bytes
      dht22_dat[j/8] <<= 1;
      if (counter > 16)
        dht22_dat[j/8] |= 1;
      j++;
    }
  }

  // check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
  // print it out if data is good
  if ((j >= 40) && 
      (dht22_dat[4] == ((dht22_dat[0] + dht22_dat[1] + dht22_dat[2] + dht22_dat[3]) & 0xFF)) ) {
        float t, h;
        h = (float)dht22_dat[0] * 256 + (float)dht22_dat[1];
        h /= 10;
        t = (float)(dht22_dat[2] & 0x7F)* 256 + (float)dht22_dat[3];
        t /= 10.0;
        if ((dht22_dat[2] & 0x80) != 0)  t *= -1;

    temp_cels = t;
    humidity = h;

    if (DEBUG) printf("Humidity = %.1f %% Temperature = %.1f *C \n", humidity, temp_cels );
    return 1;
  }
  else
  {
    if (DEBUG) printf("Data not good, skip\n");
    return 0;
  }
}

uint8_t sizecvt(const int read) {
  /* digitalRead() and friends from wiringpi are defined as returning a value
  < 256. However, they are returned as int() types. This is a safety function */

  if (read > 255 || read < 0)
  {
    if (DEBUG) printf("Invalid data from wiringPi library\n");
    exit(EXIT_FAILURE);
  }
  return (uint8_t)read;
}
