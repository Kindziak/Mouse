char x,y,opcje;
int XX,YY;

#define CLOCK  6
#define DATA   5

void SetHigh(int pin){
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH);
}

void SetLow(int pin){
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void Write(unsigned char data){
  unsigned char parity=1;
  SetLow(CLOCK); //1
  delayMicroseconds(100); //1
  SetLow(DATA); //2
  SetHigh(CLOCK); //3
  while(digitalRead(CLOCK)==HIGH); //4
  for(int i=0; i<8; i++){
    if(data&0x01) SetHigh(DATA); //5
    else SetLow(DATA);
    while(digitalRead(CLOCK)==LOW); //6
    while(digitalRead(CLOCK)==HIGH); //7
    parity^=(data&0x01);
    data=data>>1;
  } //8
  if(parity) SetHigh(DATA); //9
  else SetLow(DATA);
  while(digitalRead(DATA)==HIGH); //10
  while(digitalRead(CLOCK)==HIGH); //11
  while((digitalRead(CLOCK)==LOW)&&(digitalRead(DATA)==LOW)); //12
}

unsigned char Read(void){
  unsigned char data=0, bit=1;
  SetHigh(CLOCK);
  SetHigh(DATA);
  delayMicroseconds(50);
  while(digitalRead(CLOCK)==HIGH);
  delayMicroseconds(5);
  while(digitalRead(CLOCK)==LOW);
  for(int i=0; i<8; i++){
    while(digitalRead(CLOCK)==HIGH);
    if(digitalRead(DATA)==HIGH) data|=bit;
    while(digitalRead(CLOCK)==LOW);
    bit=bit<<1;
  }
  while(digitalRead(CLOCK)==HIGH);
  while(digitalRead(CLOCK)==LOW);
  while(digitalRead(CLOCK)==HIGH);
  while(digitalRead(CLOCK)==LOW);
  SetLow(CLOCK);
  delayMicroseconds(100);
  return data;
}

void MousePosition(char &x, char &y){
  Write(0xEB); //Read Data
  Read(); // potwierdzenie FA
  //Read(); // 1Byte
  opcje = Read();
  x = Read();
  y = Read();
}

void InitializeMouse(void){
  Write(0xFF);
  for(int i=0; i<3; i++) Read();
  Write(0xF0); //a moze 0xFF
  Read();
  delayMicroseconds(100);
  XX = 0;
  YY = 0;
}

void setup() {
  SetHigh(CLOCK);
  SetHigh(DATA);
  Serial.begin(115200);
  while(!Serial); 
  Serial.println("Do biegu");
  InitializeMouse();
  Serial.println("Gotowi!");
  Serial.println("Start!");
}

void loop() {
  int yInt = 0;
  int xInt = 0;
  MousePosition(x,y);
  //Serial.print("Opcje = ");
  //Serial.print(opcje, BIN);
  if(opcje&0x07 == 1) { //jezeli jest overflow w y
    yInt = int(y) + 256;
  }
  if(bitRead(opcje,6) == 1) { //jezeli jest overflow w x
    xInt = int(x) + 256;
  }

  XX = XX + x + xInt;
  YY = YY + y + yInt;
  
  Serial.print("\tX = ");
  Serial.print(x, DEC);
  Serial.print("\tXX = ");
  Serial.print(XX);
  Serial.print("\tY = ");
  Serial.print(y, DEC);
  Serial.print("\tYY = ");
  Serial.println(YY);
  
  delay(1000);
}
