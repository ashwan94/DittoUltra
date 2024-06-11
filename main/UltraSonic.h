#include "HardwareSerial.h"
bool goOnUltraSonic();

// 착석하면 true, 아니면 false 를 return
bool goOnUltraSonic() {
  bool seatStatus = false;
  long duration, distance;
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);
  distance = duration * 17 / 1000;

  // Serial.println(duration ); // 초음파가 반사되어 돌아오는 시간
  // Serial.print("\nDistance : ");
  // Serial.print(distance);  // 측정된 물체로부터 거리값(cm값)
  // Serial.println(" cm");   // 측정된 물체로부터 거리값(cm값)

  // 거리가 10cm 이하일 때 LED를 켜고, 그 외에는 끄기
  Serial.println(distance);
  if (distance <= 70) {
    // 착석했을 경우
    digitalWrite(LED, HIGH);
    seatStatus = true;
  } else {
    // 자리에서 이탈했을 경우
    digitalWrite(LED, LOW);
    seatStatus = false;
  }

  return seatStatus;
}