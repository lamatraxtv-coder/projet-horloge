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
int RecepteurIR;

int compteurflechemenu=0;
int nbOPT=2;

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
  //logic pour fleche sur display
  if(boutonUP==HIGH){
    compteurflechemenu++;           //si on appuie sur le bouton up alors la fleche monte
  }
  if(boutonDOWN==HIGH){             //si on appuie sur le bouton down alors la fleche descend
    compteurflechemenu--;
  }
  if(compteurflechemenu>nbOPT){     //si la fleche depasse le nb d'option elle retourne en haut de la selection
    compteurflechemenu=0;
  }
  if(compteurflechemenu<0){         // si la fleche tente de depasser la premiere option alors elle va à la derniere option
    compteurflechemenu=nbOPT;
  }
  //affichage sur le display
  switch(compteurflechemenu){
    case 0:
      display.println("->alumer // eteindre l'horloge");
      display.println("  activer // desactvier un reveil");
      display.println("  AM/PM");
      display.display();
      break;
    case 1:
      display.println("  alumer // eteindre l'horloge");
      display.println("->activer // desactvier un reveil");
      display.println("  AM/PM");
      display.display();
      break;
    case 2:
      display.println("  alumer // eteindre l'horloge");
      display.println("  activer // desactvier un reveil");
      display.println("->AM/PM");
      display.display();
      break;
  }
}