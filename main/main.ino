#define TRIG 12          // TRIG 핀 설정 (초음파 송신)
#define ECHO 13          // ECHO 핀 설정 (초음파 수신)
#define LED 14           // 초음파 확인용 LED
#define operatingPin 16  // WiFi Mode 전환

#include "MQTTManager.h"
#include "WIFIManager.h"
#include "UltraSonic.h"

// WiFi 모드 전환
int operatingMode = 1;  // 1 - AP, 2 - STA 를 의미
int buttonState;

// 시분할 처리
unsigned long currentMillis = 0;   // 현재 시간
unsigned long previousMillis = 0;  // 이전 시간
const int interval = 500;          // 간격 0.5초

void setup() {

  Serial.begin(115200);

  // // WiFi Mode 전환 button
  pinMode(operatingPin, INPUT);

  // // 초음파 센서 초기화
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(LED, OUTPUT);

  EEPROM.begin(255);
  Serial.println("Start WiFi");
  Serial.println("Start AP");

  // // 보드 자체 WiFi 구축
  long startPoint = micros();
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);
  long endPoint = micros();
  long delta = endPoint - startPoint;

  Serial.print("WiFI 켜는데 소요된 시간 ");
  Serial.println(delta);

  delay(1000);

  IPAddress myIP = WiFi.softAPIP();
  Serial.println("AP IP address : ");
  Serial.println(myIP);

  // 주소 GetMapping
  server.on("/", handleRoot);
  server.on("/action_page", handleForm);
  Serial.println("Start Server");
  server.begin();
  delay(1000);

  String SSID_temp = getSSIDEEPROM();
  String Password = getPWEEPROM();

  // MQTT 세팅
  setupMQTT();
}


void loop() {

  currentMillis = millis();  // 현재 시간
  int buttonState = digitalRead(operatingPin);

  if (operatingMode == 2 && buttonState) {
    operatingMode = 1;
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid, ap_password);
    server.begin();
    delay(1000);
    Serial.println("Switching STA >>>>> AP");
  } else if (operatingMode == 1 && buttonState) {
    String SSID_temp = getSSIDEEPROM();
    String Passowrd = getPWEEPROM();

    Serial.println("ROM Load");
    Serial.println("SSID_temp");
    Serial.println("Password_temp");

    if (isValidate(SSID_temp) && isValidate(Passowrd)) {
      connectedSSID = SSID_temp;
      connectedPassword = Passowrd;
      operatingMode = 2;

      Serial.println();
      Serial.print("Connecting to ");
      Serial.println(connectedSSID);

      WiFi.mode(WIFI_STA);                           // Board 자체 WiFi 끄기
      WiFi.begin(connectedSSID, connectedPassword);  // 숙주 WiFi 에 기생

      // 숙주 WiFi 에 연결될때까지 대기
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }

      // 숙주 WiFi 로부터 연결 허가
      Serial.println("WiFi connected OK");
      Serial.print("IP address : ");
      Serial.println(WiFi.localIP());
      Serial.println("Switching AP >>>>> STA");

      // MQTT 연결
      reconnect();
    } else {
      Serial.println("WiFi setting not yet");
    }
  }

  switch (operatingMode) {
    // 숙주 WiFi 탐색
    case 1:
      networking();  // 숙주 WiFi 찾기
      break;

    case 2:
      sensing();
      break;
  }
}

void networking() {
  server.handleClient();
}

void sensing() {
  if (currentMillis - previousMillis > interval) {
    bool checkSeatStatus = goOnUltraSonic();
    // Serial.print("착석 여부 : ");
    // Serial.println(checkSeatStatus);

    if(WiFi.status() == WL_CONNECTED && pubClient.connected()){
      // Serialization
      String sensorID = "Ditto";
      String seatStatus = String(checkSeatStatus);

      String data = String("{\"sensorID\" : \"" + sensorID
                            + "\", \"seatStatus\" : \"" + seatStatus
                            + "\"}");
      // MQTT 경로 설정 -> IoT/Sensor
      String rootTopic = "/IoT/Sensor/" + sensorID;

      publish(rootTopic, data);
    }

    previousMillis = currentMillis;
  }
}