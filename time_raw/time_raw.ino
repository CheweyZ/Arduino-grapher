#include <Time.h>
int time = 0;
time_t t;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:

}

void loop() {
  Serial.println(second());

}
