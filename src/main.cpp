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
int LEDdecompteur;

int compteurflechemenu=0;
int nbOPT=2;

//declaration de l'écran connecté en I2C
Adafruit_SSD1306 display(largeurMENU, hauteurMENU, &Wire,-1);

//init de la transmission par telecomande
IRrecv irrecv(RecepteurIR);
decode_results DonneRecue;
                                        ////////SETUP////////
void setup(){
  pinMode(boutonDOWN, INPUT);
  pinMode(boutonENTER, INPUT);
  pinMode(boutonUP, INPUT);
  pinMode(potentio, INPUT);
  pinMode(LEDdecompteur,OUTPUT);
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

                          ///fonction des parametre de l'horloge///


void marchearret(){                                         //fonction permettant l'allumage de l'horloge
  display.println("vous avez appuyé sur le bouton de marche arret");
}
void reveil(){                                               //fonction permettant activation d'un reveil personalisé      
  display.println("vous avez appuyé sur le bouton d'activation et de desactivation du reveil");
}
void modeAMPM(){                                              //fonction permettant de changer le mode d'afficahge de l'horloge. 
  display.println("vous avez appuyé sur le bouton du mode d'affichage du reveil");
}



                                ///////LOOP///////  

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
      display.clearDisplay();
      display.println("->alumer // eteindre l'horloge");
      display.println("  activer // desactvier un reveil");   
      display.println("  AM/PM");
      display.display();
      break;
    case 1:
      display.clearDisplay();
      display.println("  alumer // eteindre l'horloge");
      display.println("->activer // desactvier un reveil");
      display.println("  AM/PM");
      display.display();
      break;
    case 2:
      display.clearDisplay();
      display.println("  alumer // eteindre l'horloge");
      display.println("  activer // desactvier un reveil");
      display.println("->AM/PM");
      display.display();
      break;
  }
  display.clearDisplay();

  if(compteurflechemenu==0 && boutonENTER==HIGH){
    marchearret();
  }
  if(compteurflechemenu==1 && boutonENTER==HIGH){         //selection de choix sur le menu
    reveil();
  }
  if(compteurflechemenu==2 && boutonENTER==HIGH){
    modeAMPM();
  }

}
