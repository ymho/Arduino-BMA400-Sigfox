#include <Arduino.h>
#include <SoftwareSerial.h>
#include "BMA400.h"   // BMA400(加速度センサ用)

#define rxPin 10
#define txPin 11

uint32_t cnt = 0;

unsigned long s_time;   // millsで使用する起動時間を格納する変数  50日でオーバーフローする！
unsigned long n_time;

SoftwareSerial Sigfox =  SoftwareSerial(rxPin, txPin);

#define DEBUG 1 // デバッグモード

//uint8_t msg[12];  1byteの符号なし整数値の12byte分のバッファ
uint32_t msg[3];    // 4byteの符号なし整数値の12byte分のバッファ

//Get Sigfox ID
String getID(){
  String id = "";
  char output;

  Sigfox.print("AT$I=10\r");
  while (!Sigfox.available())

  while(Sigfox.available()){
    output = Sigfox.read();
    id += output;
    delay(10);
  }

  if(DEBUG){
    Serial.println("Sigfox Device ID: ");
    Serial.println(id);
  }

  return id;
}

//Get PAC number
String getPAC(){
  String pac = "";
  char output;

  Sigfox.print("AT$I=11\r");
  while (!Sigfox.available())

  while(Sigfox.available()){
    output = Sigfox.read();
    pac += output;
    delay(10);
  }

  if(DEBUG){
    Serial.println("PAC number: ");
    Serial.println(pac);
  }

  return pac;
}


//Send Sigfox Message
void sendMessage(uint32_t msg[], int size){

  String status = "";
  char output;

  Sigfox.print("AT$SF=");
  for(int i= 0;i<size;i++){
    Sigfox.print(String(msg[i], HEX));  // 16進数で
    if(DEBUG){
      Serial.print("Byte:");
      Serial.println(msg[i], HEX);
    }
  }

  Sigfox.print("\r");

  while (!Sigfox.available())

  while(Sigfox.available()){
    output = (char)Sigfox.read();
    status += output;
    delay(10);
  }
  if(DEBUG){
    Serial.println();
    Serial.print("Status \t");
    Serial.println(status);
  }
}

void setup() {
  Wire.begin();

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  while(!Serial);
  Serial.println("BMA400 Raw Data and Send Sigfox");

  if(DEBUG){
    Serial.println("DebugMode: On");
  }

  if(bma400.isConnection()){  // BMA400に接続可能かどうか
    bma400.initialize();      // 可能なら初期化
    Serial.println("bma400 is connected");
  }
  else Serial.println("bma400 is not connected");

  // open Wisol communication
  // define pin modes for tx, rx:
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  Sigfox.begin(9600);
  delay(100);
  getID();
  delay(100);
  getPAC();

  // s_time = millis();
}


void loop() {

  bma400.EnableStepCounter(); // 歩数カウントを有効化する

  delay(1000);

  cnt = bma400.getStepCounter();  // 歩数レジスタにアクセスして歩数を取得

  if(DEBUG){
    Serial.print("Step: ");
    Serial.println(cnt);
  }

  msg[0]=cnt;   // 歩数を格納
  msg[1]=0x00;  // 空きは0で埋める
  msg[2]=0x00;  // 空きは0で埋める

  // n_time = millis();

  sendMessage(msg, 3);

  // delay(300000);   // 300000ms = 5m , Sigfoxの送信に10sはかかるので注意！
}
