#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <IRremote.h>

#define largeurMENU 128
#define hauteurMENU 64

int boutonUP;
int boutonDOWN;
int boutonENTER;
int potentio;
const int RecepteurIR;

int compteurflechemenu=0;
int nbOPT=4;

//declaration de l'écran connecté en I2C
Adafruit_SSD1306 display(largeurMENU, hauteurMENU, &Wire,-1);

//init de la transmission par telecomande
IRrecv irrecv(RecepteurIR);
decode_results DonneRecue;

void setup(){
  Serial.begin(115200);                               //debut de l'afficheur serie
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){     
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  irrecv.enableIRIn();                                 //activation de la telecomande
  irrecv.blink13(true);                               

  display.setTextSize(1);                              //mode defaut du display
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("starting device."); delay(1000);display.print("starting device..");delay(1000);display.print("starting device..."); delay(1000);
  display.display();
  delay(4000);

}
void loop(){
  if(boutonUP==HIGH){
    compteurflechemenu++;
  }
  if(boutonDOWN==HIGH){
    compteurflechemenu--;
  }
  if(compteurflechemenu>nbOPT){
    compteurflechemenu=0;
  }
  if(compteurflechemenu<0){
    compteurflechemenu=nbOPT;
  }
  switch(compteurflechemenu){
    case 0:
      display.println("->alumer // eteindre l'horloge");
      display.println("  activer // desactvier un reveil");
      display.println("  AM/PM");
      break;
    case 1:
      display.println("  alumer // eteindre l'horloge");
      display.println("->activer // desactvier un reveil");
      display.println("  AM/PM");
      break;
    case 2:
      display.println("  alumer // eteindre l'horloge");
      display.println("  activer // desactvier un reveil");
      display.println("->AM/PM");
      break;
    case 3:
      display.print("     essaie");
      break;
    case 4:
      display.print("     essaie");
      break;
  }
}