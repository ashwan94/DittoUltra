#define TRIG 12  // TRIG 핀 설정 (초음파 송신)
#define ECHO 13  // ECHO 핀 설정 (초음파 수신)
#define LED 14   // 초음파 확인용 LED

void setup() {

  Serial.begin(115200);

  // 초음파 센서 초기화
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(LED, OUTPUT);
}
void loop() {

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
  Serial.print(distance);  // 측정된 물체로부터 거리값(cm값)
  Serial.println(" cm");   // 측정된 물체로부터 거리값(cm값)

    // 거리가 10cm 이하일 때 LED를 켜고, 그 외에는 끄기
  if (distance <= 10) {
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
  }

  delay(1000);

}