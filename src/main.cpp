#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>       //biblio utilisé
#include <EEPROM.h>
//#include <Ultrasonic.h>
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
int boutonDOWN = A2;      //bouton pour le controle
int boutonENTER = A3;

int alimmatrice = 0;      // alimentation de la matrice

int compteurmod = 1;  // (1) pour format 24 (0) pour format 12 
int compteurflechemenu = 1;   // compteur pour savoir ou est la fleche
int nbOPT = 5;            // nb option dans le menu

int verifreveil1 = 0;       // verification pour les boucle de reveil
int verifreveil2 = 0;
int verifreveil3 = 0;

int compteurreveilm = 0;     
int compteurreveilh = 0;      // pour le reveil
int compteurreveilampm = 0; // 0 pour AM 1 pour PM
float etatampm;           


void marche_arret();
void modeAMPM();
void reveil24();      // prototype des fonction
void reveil12();
void affichertemps();
void effacertemps();
void startStopTimer();

//Ultrasonic ultrasonic(trigPin, echoPin);
Adafruit_SSD1306 display(largeurMENU, hauteurMENU, &Wire, -1);

void setup() {

  pinMode(boutonDOWN, INPUT);
  pinMode(boutonENTER, INPUT);      // fonction des action 
  pinMode(boutonUP, INPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(interruptPin), startStopTimer, CHANGE);   // fonction d'interruption

  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));     // message qui capte un changement
    for (;;)
      ;
  }

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);// initialisation du display
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
  display.setTextSize(1);

  if (compteurflechemenu == 1) {
    display.println(F("->allumer // eteindre "));
    display.println(F("  reveil"));
    display.println(F("  mode //renit reveil"));
    display.println(F("  Afficher temps"));
    display.println(F("  Effacer temps"));
  }

  if (compteurflechemenu == 2) {
    display.println(F("  allumer // eteindre "));
    display.println(F("->reveil"));
    display.println(F("  mode //renit reveil"));
    display.println(F("  Afficher temps"));
    display.println(F("  Effacer temps"));
  }

  if (compteurflechemenu == 3) {
    display.println(F("  allumer // eteindre "));
    display.println(F("  reveil"));
    display.println(F("->mode //renit reveil"));
    display.println(F("  Afficher temps"));
    display.println(F("  Effacer temps"));
  }
  if (compteurflechemenu == 4) {
    display.println(F("  allumer // eteindre "));
    display.println(F("  reveil"));
    display.println(F("  mode //renit reveil"));
    display.println(F("->Afficher temps"));
    display.println(F("  Effacer temps"));
  }
  if (compteurflechemenu == 5) {
    display.println(F("  allumer // eteindre "));
    display.println(F("  reveil"));
    display.println(F("  mode //renit reveil"));
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
    delay(2000);
  } else {
    alimmatrice = 1;
    display.println(F("matrice allume"));
    display.display();
    delay(2000);
  }

  display.display();
}


void modeAMPM() {
  display.clearDisplay();
  display.setCursor(0, 10);

  compteurmod = 1 - compteurmod;  // Inversion de l'état

  display.print(F("Changement en format horaire de "));
  display.println(compteurmod ? F("24 h") : F("12 h"));
  display.println(" + réinitialisation de l'heure");
  compteurreveilm=-1;
  compteurreveilh=-1;

  display.display();
  delay(2000);
}




void controledutemps(int &value, int minValue, int maxValue) {
  if (digitalRead(boutonUP) == HIGH) {
    value = (value + 1) % (maxValue + 1);
  }

  if (digitalRead(boutonDOWN) == HIGH) {
    value = (value - 1 + maxValue + 1) % (maxValue + 1);
  }
}

void affichage() {
  display.setTextSize(2);
  display.println(F("reveil : "));
  display.print(compteurreveilh);
  display.print(F(":"));
  display.print(compteurreveilm);
  
  if (compteurreveilampm == 0) {
    display.println(F(" am"));
  } else {
    display.println(F(" pm"));
  }

  display.display();
  delay(100);
  display.clearDisplay();
}

void configurationAlarme(int &heures, int &minutes, int heureMax, int minuteMax, bool utiliserAmPm = false) {
  display.clearDisplay();
  display.setCursor(0, 10);

  while (!verifreveil1) {
    display.setCursor(0, 10);
    controledutemps(heures, 0, heureMax);
    affichage();

    if (digitalRead(boutonENTER) == HIGH) {
      verifreveil1 = 1;
    }
  }

  while (!verifreveil2) {
    display.setCursor(0, 10);
    controledutemps(minutes, 0, minuteMax);
    affichage();

    if (digitalRead(boutonENTER) == HIGH) {
      verifreveil2 = 1;
    }
  }

  if (utiliserAmPm) {
    while (!verifreveil3) {
      display.setCursor(0, 10);
      controledutemps(compteurreveilampm, 0, 1);
      affichage();

      if (digitalRead(boutonENTER) == HIGH) {
        verifreveil3 = 1;
      }

    }
  }

  verifreveil1 = 0;
  verifreveil2 = 0;
  verifreveil3 = 0;
}

void reveil24() {
  configurationAlarme(compteurreveilh, compteurreveilm, 23, 59);
}

void reveil12() {
  configurationAlarme(compteurreveilh, compteurreveilm, 12, 59, true);
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
  delay(1000);
}

void effacertemps(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Temps efface !");
  display.display();
  delay(1000);
  EEPROM.put(MEMORY_ADDRESS, 0); // Efface les données enregistrées dans la mémoire
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
