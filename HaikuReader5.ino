// Haiku Reader v2
// Roni Bandini
// Buenos Aires, Argentina 
// September 2021
// MIT License

// Arduino MKR WiFi and eInk Waveshare 2.9 display
// GxEPD2 library must be installed
// eInk display to Arduino MKR connections BUSY -> 5, RST -> 6, DC -> 7, CS-> 4, CLK -> 9, DIN -> 8
// Buttons to D11, D12 and GND
// Image BMP 1 bit, LCD Assistant, Byte orientation Horizontal
// Select u8g2 Fonts from here: https://github.com/olikraus/u8g2/wiki/fntlistall

#define ENABLE_GxEPD2_GFX 0

#include <SPI.h>
#include <SD.h>

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_7C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "GxEPD2_display_selection_new_style.h"
#include "screens.h"

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

int buttonPin1=12;
int buttonPin2=11;
int buttonReading1 = 1;
int buttonReading2 = 1;
File myFile;
String myHaiku="";
const int chipSelect = SDCARD_SS_PIN; 
int next=0;
int size_of_myArray=150;
int currentFile=1;
int maxFiles=0;

void setup()
{

  // button pins as input
  pinMode(buttonPin1, INPUT_PULLUP);  
  pinMode(buttonPin2, INPUT_PULLUP); 
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);
  delay(3000);

  Serial.println();
  Serial.println("Haiku Reader started...");

  display.init();
  u8g2Fonts.begin(display); 

 // turn the LED on 
 digitalWrite(LED_BUILTIN, HIGH);  

 if (!SD.begin(chipSelect)) {
    Serial.println("Card failed or not present");
    printScreen("Card not present");
    return;    
  }  
  Serial.println("Card initialized");

  maxFiles=countFiles();

  Serial.println("Files counted: ");
  Serial.println(maxFiles);
  
  // turn the LED off
  digitalWrite(LED_BUILTIN, LOW);   
  
  // print welcome screen
  display.setRotation(0);
  display.fillScreen(GxEPD_WHITE);  
  display.drawBitmap(0, 0, cover, 128, 296, GxEPD_BLACK);
  while (display.nextPage());
  delay(3000);

  // print menu
  display.setRotation(0);
  display.fillScreen(GxEPD_WHITE);  
  display.drawBitmap(0, 0, menu, 128, 296, GxEPD_BLACK);
  u8g2Fonts.setCursor(1, 140);
  u8g2Fonts.setFont(u8g2_font_ImpactBits_tr);
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);  
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE); 
  u8g2Fonts.println("       1. About"); 
  u8g2Fonts.println(""); 
  u8g2Fonts.println("       2. Read"); 
    
  while (display.nextPage());

  // wait for button
  while (buttonReading1==1 and buttonReading2==1){
     delay(250);
     buttonReading1 = digitalRead(buttonPin1);
     buttonReading2 = digitalRead(buttonPin2);
  }

  if (buttonReading1==0){
    // about screen    
    display.fillScreen(GxEPD_WHITE);
    display.drawBitmap(0, 0, about, 128, 296, GxEPD_BLACK);
    display.setRotation(0);
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);  
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);          
    u8g2Fonts.setCursor(1, 120);
    u8g2Fonts.setFont(u8g2_font_t0_14b_tf);
    u8g2Fonts.println("About Haiku Reader"); 
    u8g2Fonts.setFont(u8g2_font_t0_12_tr);
    u8g2Fonts.println("");
    u8g2Fonts.println("Haiku Reader was made");  
    u8g2Fonts.println("by Roni Bandini with");  
    u8g2Fonts.println("Arduino MKR, 2.9 eInk"); 
    u8g2Fonts.println("display and a custom"); 
    u8g2Fonts.println("case. All as part of a"); 
    u8g2Fonts.println("counter culture maker"); 
    u8g2Fonts.println("initiative."); 
    u8g2Fonts.println("");
    u8g2Fonts.println("CABA, Argentina");     
    u8g2Fonts.println("September 30th, 2021");     
    u8g2Fonts.println("bandini.medium.com"); 
    u8g2Fonts.println("@RoniBandini"); 
    u8g2Fonts.println("");
    u8g2Fonts.println("");
    u8g2Fonts.println(">>Press a key to exit");
    display.display(false); 
     
    buttonReading1=1;
    buttonReading2=1;
    
     // wait for button
    while (buttonReading1==1 and buttonReading2==1){
     delay(250);
     buttonReading1 = digitalRead(buttonPin1);
     buttonReading2 = digitalRead(buttonPin2);
    }
    
    }// button2
  
}

void loop()
{

 Serial.println("Current file:");
 Serial.println(currentFile); 
 
  // open file
  myFile = SD.open(String(currentFile)+".txt");
  
  if (myFile) {

    int index = 0;
    char stringArray[size_of_myArray];
    myHaiku="";
    
    while ((next = myFile.read()) != -1)
    {
        char nextChar = (char) next;
        
        if (nextChar == '\n')
        {
            // end of readed line
            stringArray[index] = '\0';            
            for(int i = 0; i < index; i++) {myHaiku=myHaiku+stringArray[i];}            
            myHaiku=myHaiku+"\n"; // insert end of line
            index = 0;
        }
        else
        {
            stringArray[index] = nextChar;
            index += 1;
        }
    }

    // close the file:
    myFile.close();
    
    //Serial.write("Haiku readed:");
    //writeString(myHaiku);
    
  } else {
    // if the file didn't open, print an error:
    Serial.println("Error opening haiku");
    printScreen("Error opening haiku file");
  }
      
  display.fillScreen(GxEPD_WHITE);
  display.drawBitmap(0, 0, frame, 128, 296, GxEPD_BLACK);
  display.setRotation(0);
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);  
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE); 
  
  u8g2Fonts.setCursor(1, 25);
  u8g2Fonts.setFont(u8g2_font_t0_14b_tf);
  u8g2Fonts.println("Haiku #"+String(currentFile)); 
     
  u8g2Fonts.setFont(u8g2_font_t0_12_tr);
  u8g2Fonts.setCursor(1, 60);
  u8g2Fonts.println(myHaiku); 
  display.display(false); // full update  
       
   // wait for button
  buttonReading1=1;  
  buttonReading2=1;  
  while (buttonReading1==1 and buttonReading2==1){
     delay(250);
     buttonReading1 = digitalRead(buttonPin1);
     buttonReading2 = digitalRead(buttonPin2);
  }        

 if (buttonReading1==0){
    // prev
    currentFile=currentFile-1;
    if (currentFile==0){currentFile=1;}
 }

  if (buttonReading2==0){
    // fwd
    currentFile=currentFile+1;
    if (currentFile>maxFiles){currentFile=maxFiles;}
 }

}// loop

void writeString(String stringData) { 

  for (int i = 0; i < stringData.length(); i++)
  {
    Serial.write(stringData[i]);   
  }

}// end writeString

void printScreen(String myMessage){

  display.fillScreen(GxEPD_WHITE);
  display.setRotation(0);
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);  
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE);  
  u8g2Fonts.setFont(u8g2_font_t0_12_tr);
  u8g2Fonts.setCursor(1, 8);
  u8g2Fonts.println(myMessage);  
  display.display(false); // full update  
  delay(3000);
  
}


int countFiles()
{
  File d = SD.open("/");
  int count_files = 0;
  
  while( true )
  {
    File entry =  d.openNextFile();
    if( !entry )
    {
      // no more files. Let's return the number of files.
      return count_files;
    }
    String file_name = entry.name();   
                 
    if( file_name.indexOf('.TXT') > 0 and file_name.indexOf('~') == -1 )    
    {              
      count_files++;
    }
    
  }
}
