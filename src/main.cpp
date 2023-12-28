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
int compteurflecheMOD=1;
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
  Serial.begin(9600);                               //debut de l'afficheur serie
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

                                ///////LOOP///////  
void modeAMPM(){
  display.clearDisplay();
  if (compteurmod == 0){
    compteurmod = 1;
    display.println("Changement en format horaire de 12 h");
  }
  else {
    compteurmod = 0;
    display.println("Changement en format horaire de 24 h");
  }
  display.display();
  delay(10000);
}


void reveil(){
  display.clearDisplay();
  display.println();
  return 0;
}

void marchearret(){
  return 0;
}
void loop(){
  //logic pour fleche sur display
  if(digitalRead(boutonUP) == HIGH){
    compteurflechemenu++;           //si on appuie sur le bouton up alors la fleche monte
    Serial.println("up");
  }
  if(digitalRead(boutonDOWN) == HIGH){             //si on appuie sur le bouton down alors la fleche descend
    compteurflechemenu--;
    Serial.println("down");
  }
  if(compteurflechemenu > nbOPT){     //si la fleche depasse le nb d'option elle retourne en haut de la selection
    compteurflechemenu = 1;
  }
  if(compteurflechemenu < 1){         // si la fleche tente de depasser la premiere option alors elle va à la derniere option
    compteurflechemenu = nbOPT;
  }
  //affichage sur le display
  display.clearDisplay(); 
  display.setCursor(0,10);

  if(compteurflechemenu==1){
      display.println("->allumer // eteindre ");
      display.println("  reveil");   
      display.println("  mode");
  }
  if(compteurflechemenu==2){
      display.println("  allumer // eteindre ");
      display.println("->reveil");   
      display.println("  mode");
  }
  if(compteurflechemenu==3){
      display.println("  allumer // eteindre ");
      display.println("  reveil");   
      display.println("->mode");
  }

  display.display();
  delay(100); 

  if(compteurflechemenu==1 && digitalRead(boutonENTER) == HIGH){
    marchearret();
  }
  if(compteurflechemenu==2 && digitalRead(boutonENTER) == HIGH){         //selection de choix sur le menu
    reveil();
  }
  if(compteurflechemenu==3 && digitalRead(boutonENTER) == HIGH){
    modeAMPM();
  }
}
