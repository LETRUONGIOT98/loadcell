 
#include "HX711.h"  
 
const int LOADCELL_DOUT_PIN = 19;
const int LOADCELL_SCK_PIN = 21;
 
HX711 scale;

float calibration_factor = -1040835.00
;//Thông số này đã đc cân chỉnh không được thay đổi; 
 
//=============================================================================================
//                         SETUP
//=============================================================================================

float NEWTON;
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
#define TFT_CS 15
#define TFT_DC 4
#define TFT_MOSI 16
#define TFT_CLK 17
#define TFT_RST 2
#define TFT_MISO 18
#define led 5
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
#define can 25
#define rst 26
#define doam 27
int old1,old2,old3;
long times;
void setup() {
  Serial.begin(9600);
   pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  pinMode(can, INPUT_PULLUP);
  pinMode(rst, INPUT_PULLUP);
  pinMode(doam, INPUT_PULLUP);
  tft.begin();
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.println("HX711 Calibration");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press a,s,d,f to increase calibration factor by 10,100,1000,10000 respectively");
  Serial.println("Press z,x,c,v to decrease calibration factor by 10,100,1000,10000 respectively");
  Serial.println("Press t for tare");
  scale.set_scale();
  scale.tare(); //Reset giá trị về 0
 
  long zero_factor = scale.read_average(); //đọc thông tin
  Serial.print("Zero factor: "); 
  Serial.println(zero_factor);

  scale.tare();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLUE);
tft.setCursor(15,30);
tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(4);
  tft.println("CAN HAT DIEU");
}
 
 
//=============================================================================================
//                         LOOP
//=============================================================================================
void loop() {
  if(millis() - times >= 10000 && millis() - times < 11000){
    tft.fillScreen(ILI9341_BLUE);
    tft.setCursor(15,30);
tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(4);
  tft.println("CAN HAT DIEU");
  }
  float sum,am,val;
  /////////Hàm đọc loadcell//////////////
  for(int i =0; i <=50; i++){
    val = analogRead(35);
    sum += val;
    delay(1);
  }
  am = map(sum/50,0,4096,100,0);
  if(am >= 100) am=100;
  if(am<=0)am=0;
 Serial.print("Reading: ");
  Serial.print(scale.get_units(), 3);
  Serial.print(" kg"); //Thay đổi giá trị này thành kg và điều chỉnh lại hệ số hiệu chuẩn
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();
  scale.set_scale(calibration_factor); //điều chỉnh theo hệ số hiệu chỉnh
NEWTON = scale.get_units();
////////////Hàm hiển thị giá trị đọc được lên màn hình LCD////////////
int val1 = digitalRead(can);
int val2 = digitalRead(rst);
int val3 = digitalRead(doam);
if(old1 != val1){
delay(100);
if(val1 == 0){
  times = millis();
tft.fillScreen(ILI9341_BLUE);
tft.setCursor(100, 30);
tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(4);
  tft.println("weight");
  tft.setCursor(100, 100);
  tft.setTextSize(5);
  tft.setTextColor(ILI9341_RED);
  tft.println((NEWTON*1000),0);
  tft.setCursor(100, 160);
  tft.setTextColor(ILI9341_GREEN);
  tft.println("Gram");
}
}
old1 = val1;

if(old2 != val2){
delay(100);
if(val2 == 0){
  times = millis();
  scale.tare();
tft.fillScreen(ILI9341_BLUE);
tft.setCursor(30, 100);
tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(4);
  tft.println("Zero factor");
}
}
old2 = val2;

if(old3 != val3){
  times = millis();
delay(100);
if(val3 == 0){
tft.fillScreen(ILI9341_BLUE);
tft.setCursor(60, 30);
tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(4);
  tft.println("humidity");
  tft.setCursor(80 , 110);
  tft.setTextSize(5);
  tft.setTextColor(ILI9341_RED);
  tft.println(String(am,0)+ "  %");
  
}
}
old3 = val3;
  //////////Hàm cân chỉnh loadcell/////////
  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 10;
    else if(temp == 's')
      calibration_factor += 100;  
    else if(temp == 'x')
      calibration_factor -= 100;  
    else if(temp == 'd')
      calibration_factor += 1000;  
    else if(temp == 'c')
      calibration_factor -= 1000;
    else if(temp == 'f')
      calibration_factor += 10000;  
    else if(temp == 'v')
      calibration_factor -= 10000;  
    else if(temp == 't')
      scale.tare();  //Reset giá trị về 0
  }
}
