#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>

#define largeurMENU 128
#define hauteurMENU 64

/*à mettre sur la breadboard : un objet permettant de bloquer l'alim de la matrice 
*/
int boutonUP=A1;                 
int boutonDOWN=A2;
int boutonENTER=A0;
int LEDdecompteur;
int alimmatrice = HIGH;
int compteurmod=1; // si = 1 0 à 23h si = 0 mode AM PM.
int compteurflecheMOD;

int compteurflechemenu=1;
int nbOPT=3;

//declaration de l'écran connecté en I2C
Adafruit_SSD1306 display(largeurMENU, hauteurMENU, &Wire,-1);


                                        ////////SETUP////////
void setup(){
  pinMode(boutonDOWN, INPUT);
  pinMode(boutonENTER, INPUT);
  pinMode(boutonUP, INPUT);
  pinMode(LEDdecompteur,OUTPUT);
  Serial.begin(115200);                               //debut de l'afficheur serie
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){     
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
                               
  display.setTextSize(1);                              //mode defaut du display
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.display();
  display.clearDisplay();
}

                          ///fonction des parametre de l'horloge///


void marchearret(){                                         //fonction permettant l'allumage de l'horloge
  display.println("vous avez appuyé sur le bouton de marche arret");
  display.display();
  delay(1000);
  display.clearDisplay();
  

  switch(alimmatrice){

    case HIGH :
      alimmatrice = LOW;
    
    case LOW : 
      alimmatrice = HIGH;
  }

  loop();

}
void reveil(){ 
  int compteurheure=0;
  int compteurminute=0;
  int verif1=0;                                              //fonction permettant activation d'un reveil personalisé 
  int verif2=0;
  int verfi3=0;

  display.println("vous avez appuyé sur le bouton d'activation et de desactivation du reveil");
  display.display();
  delay(1000);
  display.clearDisplay();
  while(verif1==0){
    while(verif2==0){
      display.print("heure de reveil :");
      if(boutonUP==HIGH){
        compteurminute++;
      }
      if(boutonDOWN==HIGH){
        compteurminute--;
      }
      display.print(compteurminute);
      display.display();
      if(boutonENTER==HIGH){
        verif2=1;
      }  
    }

    while(verfi3=0){
      if(boutonUP==HIGH){
        compteurheure++;
      }
      if(boutonDOWN==HIGH){
        compteurheure--;
      }
      display.print(compteurheure);
      display.display();
      if(boutonENTER==HIGH){
        verfi3=0;
        verif1=0;
      }
    }
  }
  loop();
}
void modeAMPM(){                                              //fonction permettant de changer le mode d'afficahge de l'horloge. 
  display.println("vous avez appuyé sur le bouton du mode d'affichage du reveil");
  display.display();
  delay(1000);
  display.clearDisplay();
  if(boutonUP==HIGH){
    compteurflecheMOD++;           //si on appuie sur le bouton up alors la fleche monte
  }
  if(boutonDOWN==HIGH){             //si on appuie sur le bouton down alors la fleche descend
    compteurflecheMOD--;
  }
  if(compteurflecheMOD>1){     //si la fleche depasse le nb d'option elle retourne en haut de la selection
    compteurflecheMOD=0;
  }
  if(compteurflecheMOD<0){         // si la fleche tente de depasser la premiere option alors elle va à la derniere option
    compteurflecheMOD=1;
  }
  //affichage sur le display
  switch(compteurflecheMOD){
    case 0:
      display.clearDisplay();
      display.println("->MOD AM/PM");
      display.println("  MOD europe");
      display.display();
      break;
    case 1:
      display.clearDisplay();
      display.println("  MOD AM/PM");
      display.println("->MOD europe");
      display.display();
      break;
  }
  if(compteurflecheMOD==0 && boutonENTER==HIGH){
    compteurmod=0;
    display.println("mode AM/PM actif");
    delay(1000);
    display.clearDisplay();
    loop();
  }
  if(compteurflecheMOD==1 && boutonENTER==HIGH){
    compteurmod=1;
    display.println("mode europe actif");
    delay(1000);
    display.clearDisplay();
    loop();
  }


  loop();
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
    compteurflechemenu=1;
  }
  if(compteurflechemenu<1){         // si la fleche tente de depasser la premiere option alors elle va à la derniere option
    compteurflechemenu=nbOPT;
  }
  //affichage sur le display
  if(compteurflechemenu==1){
      display.println("->allumer // eteindre ");
      display.println("   reveil");   
      display.println("  mode");
      display.display();
      delay(1000);
      display.clearDisplay();
      display.setCursor(0,10);
      display.display();
  }
  if(compteurflechemenu==2){
      display.println("  allumer // eteindre ");
      display.println("->reveil");   
      display.println("  mode");
      display.display();
      delay(1000);
      display.clearDisplay();
      display.setCursor(0,10);
      display.display();
  }
  if(compteurflechemenu==3){
      display.println("  allumer // eteindre ");
      display.println("  reveil");   
      display.println("->mode");
      display.display();
      delay(1000);
      display.clearDisplay();
      display.setCursor(0,10);
      display.display();
  }

  if(compteurflechemenu==1 && boutonENTER==HIGH){
    marchearret();
  }
  if(compteurflechemenu==2 && boutonENTER==HIGH){         //selection de choix sur le menu
    reveil();
  }
  if(compteurflechemenu==3 && boutonENTER==HIGH){
    modeAMPM();
  }

}
