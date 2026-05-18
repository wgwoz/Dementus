// Dementus v1.0
//
// You need to modify Adafruit ST7735 library. to do that copy offset lines from init greentab and paste them to init blacktab
// analogRead goes from 0 to 1023
// digitalRead(BUTTON_1)==1 means button not pressed.
//


//general defining of pins

 //Uart used to communicate with dfplayer mini
  #define UART_RX 0
  #define UART_TX 1
 //
 //backup button control
  #define  BUTTON_1 2
  #define BUTTON_2 3
  #define BUTTON_3 4
  #define BUTTON_4 5
 //
 //backup light control
  #define LIGHT_1 6
  #define LIGHT_2 7
  #define LIGHT_3 8
  #define LIGHT_4 9
 // 
 //backup motor control
  #define MOTOR_1 10
  #define MOTOR_2 11
  #define MOTOR_3 12
  #define MOTOR_4 13
 //
 //direction of movement of chosen motors
  #define UP 15
  #define DOWN 14
 //
 //analog pins
  #define POTENTIOMETER 26 // analog pin, checks position of potentiometer 
  #define VOLTAGE 28 // analog pin, checks battery voltage 
  #define KEEP_ON 27 // keeps mosftet on, if turned to input mode, will turn off the device
 //
 //pins needed to operate LCD screen
  #define LCD_BACKLIGHT 22
  #define LCD_DC 21
  #define LCD_CS 20
  #define TFT_CS        20 // Hallowing display control pins: chip select
  #define TFT_RST       37 // Display reset
  #define TFT_DC        21 // Display data/command select
  #define TFT_BACKLIGHT  22 // Display backlight pin
  #define SerialDebugging true
 //
 //pins needed for SD card
  #define SD_CS 17
 //
 //SPI pins
  #define MOSI 19
  #define MISO 16
  #define CLK  18
 
//
// constants
 #define maxGameLenght 100 // limits lenght of memory game
//
//stuff for lcd text display

 #include <Adafruit_GFX.h>    // Core graphics library
 #include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
 #include <SPI.h>

 Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);


 // color definitions
  const uint16_t  Display_Color_Black        = 0x0000;
  const uint16_t  Display_Color_Blue         = 0x001F;
  const uint16_t  Display_Color_Red          = 0xF800;
  const uint16_t  Display_Color_Green        = 0x07E0;
  const uint16_t  Display_Color_Cyan         = 0x07FF;
  const uint16_t  Display_Color_Magenta      = 0xF81F;
  const uint16_t  Display_Color_Yellow       = 0xFFE0;
  const uint16_t  Display_Color_White        = 0xFFFF;
 //
 // The colors we actually want to use
  uint16_t        Display_Text_Color         = Display_Color_Black;
  uint16_t        Display_Backround_Color    = Display_Color_Blue;
 //
 // assume the display is off until configured in setup()
  bool            isDisplayVisible        = false;
 //
 // declare size of working string buffers. Basic strlen("d hh:mm:ss") = 10
  const size_t    MaxString               = 16;
 //
//
// stuff for DFPlayer mini
  #include <DFMiniMp3.h>
 
 // forward declare the notify class, just the name
 //
 class Mp3Notify; 

 // define a handy type using serial and our notify class
 //
 typedef DFMiniMp3<HardwareSerial, Mp3Notify> DfMp3; 

 // instance a DfMp3 object, 
 //
 DfMp3 dfmp3(Serial1);
 class Mp3Notify
 {
 public:
   static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
   {
     if (source & DfMp3_PlaySources_Sd) 
     {
         Serial.print("SD Card, ");
     }
     if (source & DfMp3_PlaySources_Usb) 
     {
         Serial.print("USB Disk, ");
     }
     if (source & DfMp3_PlaySources_Flash) 
     {
         Serial.print("Flash, ");
     }
     Serial.println(action);
   }
   static void OnError([[maybe_unused]] DfMp3& mp3, uint16_t errorCode)
   {
     // see DfMp3_Error for code meaning
     Serial.println();
     Serial.print("Com Error ");
     Serial.println(errorCode);
   }
   static void OnPlayFinished([[maybe_unused]] DfMp3& mp3, [[maybe_unused]] DfMp3_PlaySources source, uint16_t track)
   {
     Serial.print("Play finished for #");
     Serial.println(track);  
   }
   static void OnPlaySourceOnline([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
   {
     PrintlnSourceAction(source, "online");
   }
   static void OnPlaySourceInserted([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
   {
     PrintlnSourceAction(source, "inserted");
   }
   static void OnPlaySourceRemoved([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
   {
     PrintlnSourceAction(source, "removed");
   }
 };
//
//global variables
 int score;

 int light[4] = {6,7,8,9};
 int button[4] = {2,3,4,5};
 int motor[4] = {10,11,12,13};
//
//functions
 int whichButtonPressed(){
   while(true){
     for(int i=0; i<4; i++){
       if(digitalRead(button[i])==0){
         delay(50);
         if(digitalRead(button[i])==0){
           return i;
         }
       }
     }
   }
 }
 int memoryGame(){
   int sequence[maxGameLenght];
   for(int i=0; i<maxGameLenght; i++){
     sequence[i]=random(0,4);
     delay (500);
     //show sequence of lights
     for(int k=0; k<=i; k++){
       Serial.print(sequence[k]);
       digitalWrite(light[sequence[k]], HIGH);
       dfmp3.playFolderTrack(1, sequence[k]+3);
       delay(500);
       digitalWrite(light[sequence[k]], LOW);
       delay(100);
     }

     //check if sequence was memorised properly
     for(int k=0; k<=i; k++){
       if(whichButtonPressed()!=sequence[k]){
         dfmp3.playFolderTrack(1, 2);
         return i ;
       }
       digitalWrite(light[sequence[k]], HIGH);
       dfmp3.playFolderTrack(1, sequence[k]+3);
       delay(200);
       digitalWrite(light[sequence[k]], LOW);
     }
     delay(1000);
     dfmp3.playFolderTrack(1, 1);
     delay(2000);

   }
   return 0 ;
 }
//
//
void setup() {

 
 Serial.begin(9600); 

 //initialise lcd screen
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
   tft.setFont();
   tft.fillScreen(Display_Backround_Color);
   tft.setTextColor(Display_Text_Color);
   tft.setTextSize(12);
  // the display is now on
  isDisplayVisible = true;
 //
 //DFPLayer mini
  dfmp3.begin();
  dfmp3.reset();

 //
 //pinModes
  pinMode(light[0],OUTPUT);
  pinMode(light[1],OUTPUT);
  pinMode(light[2],OUTPUT);
  pinMode(light[3],OUTPUT);

  pinMode(button[0],INPUT_PULLUP);
  pinMode(button[1],INPUT_PULLUP);
  pinMode(button[2],INPUT_PULLUP);
  pinMode(button[3],INPUT_PULLUP);

  pinMode(motor[0],OUTPUT);
  pinMode(motor[1],OUTPUT);
  pinMode(motor[2],OUTPUT);
  pinMode(motor[3],OUTPUT);

  pinMode(UP,OUTPUT);
  pinMode(DOWN,OUTPUT);

  pinMode(KEEP_ON,OUTPUT);
 
 randomSeed(analogRead(POTENTIOMETER));

 digitalWrite(KEEP_ON,1); // keeps ground connected when using battery
 // pinMode(KEEP_ON, INPUT);
}

void loop() {
 Serial.println(analogRead(POTENTIOMETER));
 delay(500)
;
 }
 



