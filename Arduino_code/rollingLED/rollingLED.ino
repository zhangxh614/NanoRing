#include <LedControl.h>  
#include <SoftwareSerial.h>
#include <Servo.h>
//ble TX->D12 RX->D9 +5V->5V GND->GND
int DIN = 11;  
int CS =  10;  
int CLK = 13;  
const int TrigPin = 2; 
const int EchoPin = 3; 
float distance; 
int count_number = 0;
bool is_open = false;
Servo myservo;


byte ans[8][19];
byte num[10][8][4]={{{0,0,0,0},{1,1,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,1,1,0}},
                    {{0,0,0,0},{0,1,0,0},{1,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{1,1,1,0}},
                    {{0,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,1,0},{1,1,1,0},{1,0,0,0},{1,0,0,0},{1,1,1,0}},
                    {{0,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,1,0},{1,1,1,0},{0,0,1,0},{0,0,1,0},{1,1,1,0}},
                    {{0,0,0,0},{0,0,1,0},{0,1,1,0},{1,0,1,0},{1,1,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}},
                    {{0,0,0,0},{1,1,1,0},{1,0,0,0},{1,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,1,0},{1,1,1,0}},
                    {{0,0,0,0},{1,1,1,0},{1,0,0,0},{1,0,0,0},{1,1,1,0},{1,0,1,0},{1,0,1,0},{1,1,1,0}},
                    {{0,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,1,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}},
                    {{0,0,0,0},{1,1,1,0},{1,0,1,0},{1,0,1,0},{1,1,1,0},{1,0,1,0},{1,0,1,0},{1,1,1,0}},
                    {{0,0,0,0},{1,1,1,0},{1,0,1,0},{1,0,1,0},{1,1,1,0},{0,0,1,0},{0,0,1,0},{1,1,1,0}}
                    };
                    
byte ch[8][2]={{0,0},{0,0},{1,0},{1,0},{0,0},{1,0},{1,0},{0,0}};
char msg[15]={'0','0','0','0','0'};

SoftwareSerial BT(12, 9); // RX, TX
LedControl lc=LedControl(DIN,CLK,CS,4);  
  
void setup(){  
 Serial.begin(9600);
 BT.begin(9600);
 delay(100);
 pinMode(A3,OUTPUT);//R
 pinMode(A5,OUTPUT);//G
 pinMode(A6,OUTPUT);//B
 digitalWrite(A3,LOW);
 digitalWrite(A5,LOW);
 digitalWrite(A6,LOW);
 lc.shutdown(0,false);       //启动时，MAX72XX处于省电模式  
 lc.setIntensity(0,2);       //将亮度设置为合适的值(0-8）
 lc.clearDisplay(0);         //清除显示  
 pinMode(TrigPin, OUTPUT);   //超声波
 pinMode(EchoPin, INPUT); 

 myservo.attach(5);
}  

void loop(){
  int k=0;
  while(BT.available()>0) {
    msg[k++] = BT.read();
  }
  //Serial.println((int)msg[0]-48);
  //Serial.println((int)msg[1]-48);
  //Serial.println((int)msg[2]-48);
  //Serial.println((int)msg[3]-48);
  add((int)msg[0]-48,(int)msg[1]-48,(int)msg[2]-48,(int)msg[3]-48);
  if(msg[4]=='1') {
    digitalWrite(A3,HIGH);
    digitalWrite(A5,LOW);
    digitalWrite(A6,LOW);
    BT.println("red");
  }
  else if(msg[4]=='2') {
    digitalWrite(A3,LOW);
    digitalWrite(A5,HIGH);
    digitalWrite(A6,LOW);
    BT.println("gr");
  }
  else { 
    digitalWrite(A3,LOW);
    digitalWrite(A5,LOW);
    digitalWrite(A6,LOW);
    BT.println("an");
  }
  display(ans,19);
  //超声波
  con_trash();
}  

void con_trash(){
  get_distance();
  //计时器
  if(is_open){
    if(count_number == 12){
      count_number = 0;
      is_open = false;
      myservo.write(90);
    }
    else{
      count_number += 1;
    }
  }
  else{
      if(distance < 30){
      myservo.write(20);
      is_open = true;
      count_number = 0;
    }
  }
}

//点阵显示函数  
void printByte(byte character [])  
{  
  int i = 0;  
  for(i=0;i<8;i++)  
  {  
    lc.setRow(0,i,character[i]);  
  }  
} 
 
//组合数字
void add(int a,int b,int c,int d) {
  for(int i=0;i<8;i++) {
    for(int j=0;j<4;j++) {
      ans[i][j]=num[a][i][j];
    }
    for(int j=0;j<4;j++) {
      ans[i][j+4]=num[b][i][j];
    }
    for(int j=0;j<2;j++) {
      ans[i][j+8]=ch[i][j];
    }
    for(int j=0;j<4;j++) {
      ans[i][j+10]=num[c][i][j];
    }
    for(int j=0;j<4;j++) {
      ans[i][j+14]=num[d][i][j];
    }
  }
}

//二进制转十进制
byte getNum(byte row[]) {
  int sum=0;
  for(int j=7;j>=0;j--){
      sum+=(row[7-j]<<j);
      //Serial.println(sum);
  }
  return (byte)sum;
}

//滚动显示
void display(byte whole[][19], int n){
  byte img[8][8];
  for(int i=-8;i<n;i++){
    for(int j=0;j<8;j++) {
      if(i+j<0||i+j>=n) {
        for(int k=0;k<8;k++) {
          img[k][j]=0;
        }
      }
      else {
        for(int k=0;k<8;k++) {
          img[k][j]=whole[k][i+j];
        }
      }
    }
    byte ans[8];
    for(int k=0;k<8;k++) {
      ans[k]=getNum(img[k]);
    }
    printByte(ans);
    con_trash();
    delay(100);
  }
}

//超声波测距
void get_distance(){
  // 产生一个10us的高脉冲去触发TrigPin 
        digitalWrite(TrigPin, LOW); 
        delayMicroseconds(2); 
        digitalWrite(TrigPin, HIGH); 
        delayMicroseconds(10);
        digitalWrite(TrigPin, LOW); 
    // 检测脉冲宽度，并计算出距离
        distance = pulseIn(EchoPin, HIGH) / 58.00;
}
