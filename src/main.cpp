#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <Ultrasonic.h>
#include <Arduino.h>


#define OLED_RESET 4
#define largeurMENU 128
#define hauteurMENU 64
#define trigPin 11 // Broche de déclenchement du capteur à ultrasons
#define echoPin 12 // Broche de réception du capteur à ultrasons
#define interruptPin 2 // Broche d'interruption externe pour le bouton poussoir
#define MEMORY_ADDRESS 0 // Adresse de départ dans la mémoire EEPROM pour enregistrer le temps


volatile bool started = false;
volatile unsigned long startTime = 0;
volatile unsigned long stopTime = 0;
volatile unsigned long elapsedTime = 0;

int boutonUP = A1;
int boutonDOWN = A2;
int boutonENTER = A3;

int alimmatrice = 0;

int compteurmod = 1;  // (1) pour format 24 (0) pour format 12 
int compteurflechemenu = 1;
int nbOPT = 5;

int verifreveil1 = 0;
int verifreveil2 = 0;
int verifreveil3 = 0;

int compteurreveilm = 0;
int compteurreveilh = 0;
int compteurreveilampm = 0; // 0 pour AM 1 pour PM
float etatampm;

void marche_arret();
void modeAMPM();
void reveil24();
void reveil12();
void conversion1224();
void conversion2412();
void affichertemps();
void effacertemps();
void startStopTimer(); 

Ultrasonic ultrasonic(trigPin, echoPin);
Adafruit_SSD1306 display(largeurMENU, hauteurMENU, &Wire, -1);

void setup() {

  pinMode(boutonDOWN, INPUT);
  pinMode(boutonENTER, INPUT);
  pinMode(boutonUP, INPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(interruptPin), startStopTimer, CHANGE);

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
    display.println(F("->allumer // eteindre "));
    display.println(F("  reveil"));
    display.println(F("  mode"));
    display.println(F("  Afficher temps"));
    display.println(F("  Effacer temps"));
  }

  if (compteurflechemenu == 2) {
    display.println(F("  allumer // eteindre "));
    display.println(F("->reveil"));
    display.println(F("  mode"));
    display.println(F("  Afficher temps"));
    display.println(F("  Effacer temps"));
  }

  if (compteurflechemenu == 3) {
    display.println(F("  allumer // eteindre "));
    display.println(F("  reveil"));
    display.println(F("->mode"));
    display.println(F("  Afficher temps"));
    display.println(F("  Effacer temps"));
  }
  if (compteurflechemenu == 4) {
    display.println(F("  allumer // eteindre "));
    display.println(F("  reveil"));
    display.println(F("  mode"));
    display.println(F("->Afficher temps"));
    display.println(F("  Effacer temps"));
  }
  if (compteurflechemenu == 5) {
    display.println(F("  allumer // eteindre "));
    display.println(F("  reveil"));
    display.println(F("  mode"));
    display.println(F("  Afficher temps"));
    display.println(F("->Effacer temps"));
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
  if(compteurflechemenu == 4 && digitalRead(boutonENTER) == HIGH){
    affichertemps();
  }
  if(compteurflechemenu == 5 && digitalRead(boutonENTER) == HIGH){
    effacertemps();
  }
}

void marche_arret() {
  display.clearDisplay();
  display.setCursor(0, 10);

  if (alimmatrice == 1) {
    alimmatrice = 0;
    display.println(F("matrice eteinte"));
    display.display();
    delay(5000);
  } else {
    alimmatrice = 1;
    display.println(F("matrice allume"));
    display.display();
    delay(5000);
  }

  display.display();
}

void modeAMPM() {
  display.clearDisplay();
  display.setCursor(0, 10);

  if (compteurmod == 0) {
    compteurmod = 1;
    display.println(F("Changement en format horaire de 24 h"));
    if(compteurreveilm != 0 && compteurreveilh != 0){
      conversion1224();
    }

  } else {
    compteurmod = 0;
    display.println(F("Changement en format horaire de 12 h"));
    conversion2412();
    if(compteurreveilm != 0 && compteurreveilh != 0){
      conversion2412();
    }
  }

  display.display();
  delay(5000);
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

    display.println(F("heure de la sonnerie  "));
    display.print(compteurreveilh);
    display.print(F(":"));
    display.println(compteurreveilm);
    display.println(F("appuye sur le bouton central pour selectionner l'heure"));
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

        display.println(F("heure de la sonnerie  "));
    display.print(compteurreveilh);
    display.print(F(":"));
    display.println(compteurreveilm);
    display.println(F("appuye sur le bouton central pour selectionner l'heure"));
    display.display();
    delay(100);
    display.clearDisplay();
    if(digitalRead(boutonENTER)==HIGH){
      verifreveil2=1;
    }
  }
  verifreveil1 = 0;
  verifreveil2 = 0;
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

    display.println(F("heure de la sonnerie  "));
    display.print(compteurreveilh);
    display.print(F(":"));
    display.print(compteurreveilm);
    display.println(etatampm);
    display.println(F("appuye sur le bouton central pour selectionner l'heure"));
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
    display.println(F("heure de la sonnerie  "));
    display.print(compteurreveilh);
    display.print(":");
    display.print(compteurreveilm);
    display.println(etatampm);
    display.println(F("appuye sur le bouton central pour selectionner l'heure"));
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
    display.println(F("heure de la sonnerie  "));
    display.print(compteurreveilh);
    display.print(":");
    display.print(compteurreveilm  );
    display.println(etatampm);
    display.println(F("appuye sur le bouton central pour selectionner l'heure"));
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
void affichertemps(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Temps d'extinction:");
  display.print("   ");
  display.print(elapsedTime);
  display.println(" ms");
  display.display();
  delay(5000);
}

void effacertemps(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Temps effacé !");
  display.display();
  EEPROM.put(MEMORY_ADDRESS, 0); // Efface les données enregistrées dans la mémoire
  delay(1000);
}
void startStopTimer() {
  if (!started) {
    started = true;
    startTime = millis(); // Démarre le chrono
  } else {
    stopTime = millis(); // Arrête le chrono
    started = false;
    elapsedTime = stopTime - startTime;

    // Enregistrement dans la mémoire EEPROM
    EEPROM.put(MEMORY_ADDRESS, elapsedTime);
  }
}