#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>


#define largeurMENU 128
#define hauteurMENU 64

int boutonUP = A1;
int boutonDOWN = A2;
int boutonENTER = A3;

int alimmatrice = 0;

int compteurmod = 1;  // (1) pour format 24 (0) pour format 12 
int compteurflechemenu = 1;
int nbOPT = 3;

int verifreveil1 = 0;
int verifreveil2 = 0;
int verifreveil3 = 0;

int compteurreveilm = 0;
int compteurreveilh = 0;
int compteurreveilampm = 0; // 0 pour AM 1 pour PM
float etatampm;

// pin buzzer au pif
int pinBuzzer = 5;
/* int pinBuzzer = 5 

pinMode(pinBuzzer, OUTPUT)

if (compteurreveilm == affichageheurem)&&(compteurreveilh == affichagereveilh){
  
  //Fréquences des notes de la mélodie
  int melodie[] = {262, 196, 196, 220, 196, 247, 262};

  //Durée des notes (4 = dure un quart du temps, 8=dure un octave, etc.)//
  int dureeNote[] = {4,8,8,4,4,4,4,4 };
  int i;
  
  //on répete les 7 notes de la mélodie une après l'autre
  for (i = 0; i < 8; i++){
  
    //Pour calculer la durée de la note, on divise une seconde par la
    quantité signalée dans dureeNote[]. Exemple, un quart du temps
    est 1000/4 secondes, un octave est 1000/8 secondes, etc.//
    tone(9, melodie[i], 1000/dureeNote[i]);
  
    //Comme la fonction tone() ne bloque pas, le sketch continue à s'exécuter
    sans arrêt. Pour  éviter de revenir au début de la boucle for et pour pouvoir différencier les notes, j'établis un temps minimal
    entre elles//
    delay(1300/dureeNote[i]);
  
    // on arrête l'émission des sons//
    noTone(9);
  }
}
*/

Adafruit_SSD1306 display(largeurMENU, hauteurMENU, &Wire, -1);


void marche_arret();
void modeAMPM();
void reveil24();
void reveil12();
int conversion1224();
int conversion2412();

void setup() {
  pinMode(boutonDOWN, INPUT);
  pinMode(boutonENTER, INPUT);
  pinMode(boutonUP, INPUT);


  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.display();
  display.clearDisplay();
}

void loop() {

  display.clearDisplay();
  if (digitalRead(boutonUP) == HIGH) {
    compteurflechemenu++;
    Serial.println("up");
  }

  if (digitalRead(boutonDOWN) == HIGH) {
    compteurflechemenu--;
    Serial.println("down");
  }

  if (compteurflechemenu > nbOPT) {
    compteurflechemenu = 1;
  }

  if (compteurflechemenu < 1) {
    compteurflechemenu = nbOPT;
  }

  display.clearDisplay();
  display.setCursor(0, 10);

  if (compteurflechemenu == 1) {
    display.println("->allumer // eteindre ");
    display.println("  reveil");
    display.println("  mode");
  }

  if (compteurflechemenu == 2) {
    display.println("  allumer // eteindre ");
    display.println("->reveil");
    display.println("  mode");
  }

  if (compteurflechemenu == 3) {
    display.println("  allumer // eteindre ");
    display.println("  reveil");
    display.println("->mode");
  }

  display.display();
  delay(100);

  if (compteurflechemenu == 1 && digitalRead(boutonENTER) == HIGH) {
    marche_arret();
  }

  if (compteurflechemenu == 2 && digitalRead(boutonENTER) == HIGH) {
    if (compteurmod == 1) {
      reveil24();
    } else {
      reveil12();
    }
  }

  if (compteurflechemenu == 3 && digitalRead(boutonENTER) == HIGH) {
    modeAMPM();
  }
}

void marche_arret() {
  display.clearDisplay();
  display.setCursor(0, 10);

  if (alimmatrice == 1) {
    alimmatrice = 0;
    display.println("matrice eteinte");
    display.display();
    delay(5000);
  } else {
    alimmatrice = 1;
    display.println("matrice allume");
    display.display();
    delay(5000);
  }

  display.display();
  loop();
}

void modeAMPM() {
  display.clearDisplay();
  display.setCursor(0, 10);

  if (compteurmod == 0) {
    compteurmod = 1;
    display.println("Changement en format horaire de 24 h");
    if(compteurreveilm != 0 && compteurreveilh != 0){
      conversion1224();
    }

  } else {
    compteurmod = 0;
    display.println("Changement en format horaire de 12 h");
    conversion2412();
    if(compteurreveilm != 0 && compteurreveilh != 0){
      conversion2412();
    }
  }

  display.display();
  delay(5000);
  loop();
}

void reveil24() {
  display.clearDisplay();
  display.setCursor(0, 10);

  while (verifreveil1 == 0) {
    display.setCursor(0, 10);
    if (digitalRead(boutonUP) == HIGH) {
      compteurreveilh++;
    }

    if (digitalRead(boutonDOWN) == HIGH) {
      compteurreveilh--;
    }

    if (compteurreveilh > 23) {
      compteurreveilh = 0;
    }

    if (compteurreveilh < 0) {
      compteurreveilh = 23;
    }

    display.println("heure de la sonnerie  ");
    display.print(compteurreveilh);
    display.print(":");
    display.println("00");
    display.println("appuye sur le bouton central pour selectionner l'heure");
    display.display();
    delay(100);
    display.clearDisplay();

    if (digitalRead(boutonENTER) == HIGH) {
      verifreveil1 = 1;
    }
  }

  display.setCursor(0, 10);

  while (verifreveil2 == 0) {
    display.setCursor(0, 10);
    if (digitalRead(boutonUP) == HIGH) {
      compteurreveilm++;
    }

    if (digitalRead(boutonDOWN) == HIGH) {
      compteurreveilm--;
    }

    if (compteurreveilm > 59) {
      compteurreveilm = 0;
    }

    if (compteurreveilm < 0) {
      compteurreveilm = 59;
    }

    display.println("heure de la sonnerie : ");
    display.print(compteurreveilh);
    display.print(":");
    display.println(compteurreveilm);
    display.println("appuyer sur le bouton central pour selectionner l'heure");
    display.display();
    delay(100);
    display.clearDisplay();
    if(digitalRead(boutonENTER)==HIGH){
      verifreveil2=1;
    }
  }
  verifreveil1 = 0;
  verifreveil2 = 0;
  loop();
}

void reveil12() {
  display.clearDisplay();
  display.setCursor(0, 10);

  while (verifreveil1 == 0) {
    display.setCursor(0, 10);
    if (digitalRead(boutonUP) == HIGH) {
      compteurreveilh++;
    }

    if (digitalRead(boutonDOWN) == HIGH) {
      compteurreveilh--;
    }

    if (compteurreveilh > 12) {
      compteurreveilh = 0;
    }

    if (compteurreveilh < 0) {
      compteurreveilh = 12;
    }

    display.println("heure de la sonnerie  ");
    display.print(compteurreveilh);
    display.print(":");
    display.print(compteurreveilm);
    display.println(etatampm);
    display.println("appuye sur le bouton central pour selectionner l'heure");
    display.display();
    delay(100);
    display.clearDisplay();

    if (digitalRead(boutonENTER) == HIGH) {
      verifreveil1 = 1;
    }
  }

  display.setCursor(0, 10);

  while (verifreveil2 == 0) {
    display.setCursor(0, 10);
    if (digitalRead(boutonUP) == HIGH) {
      compteurreveilm++;
    }

    if (digitalRead(boutonDOWN) == HIGH) {
      compteurreveilm--;
    }

    if (compteurreveilm > 59) {
      compteurreveilm = 0;
    }

    if (compteurreveilm < 0) {
      compteurreveilm = 59;
    }
    display.println("heure de la sonnerie  ");
    display.print(compteurreveilh);
    display.print(":");
    display.print(compteurreveilm);
    display.println(etatampm);
    display.println("appuye sur le bouton central pour selectionner l'heure");
    display.display();
    delay(100);
    display.clearDisplay();
    if(digitalRead(boutonENTER)==HIGH){
      verifreveil2=1;
    }
  }
  while(verifreveil3==0){

    if (digitalRead(boutonUP) == HIGH) {
      compteurreveilampm++;
    }

    if (digitalRead(boutonDOWN) == HIGH) {
      compteurreveilampm--;
    }

    if (compteurreveilm >1) {
      compteurreveilampm =  0;
    }

    if (compteurreveilm < 0) {
      compteurreveilampm = 1;
    }
    if(compteurreveilampm == 0){
      etatampm= 'am';
    }
    if(compteurreveilampm == 1){
      etatampm ='pm';
    }
    display.println("heure de la sonnerie  ");
    display.print(compteurreveilh);
    display.print(":");
    display.print(compteurreveilm  );
    display.println(etatampm);
    display.println("appuye sur le bouton central pour selectionner l'heure");
    display.display();
    delay(100);
    display.clearDisplay();
    if(digitalRead(boutonENTER)==HIGH){
      verifreveil3 = 1;
    }

  }
  verifreveil1 = 0;
  verifreveil2 = 0;
  verifreveil3 = 0;
  loop();
}
void conversion1224(){
  if(compteurreveilampm == 1){
    compteurreveilh += 12;
  }
}
void conversion2412(){
  if(compteurreveilh > 12){
    compteurreveilh=-12;
    compteurreveilampm = 1; 
  }

}
  