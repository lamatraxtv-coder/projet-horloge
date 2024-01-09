#include <Arduino.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <LedControl.h>
#include <Encoder.h>
#include <Adafruit_SSD1306.h>

#define encoderPinA 2  // Broche A de l'encodeur connectée à la broche 2 de l'Arduino
#define encoderPinB 3  // Broche B de l'encodeur connectée à la broche 3 de l'Arduino

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

Encoder myEncoder(encoderPinA, encoderPinB);


RTC_DS1307 rtc;
LedControl lc=LedControl(11,13,10,4); // (din, clk, cs)

void setup() {
  Serial.begin(9600);

  rtc.begin();
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
 
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);

    lc.shutdown(1,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(1,8);
  /* and clear the display */
  lc.clearDisplay(1);

  lc.shutdown(2,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(2,8);
  /* and clear the display */
  lc.clearDisplay(2);

  lc.shutdown(3,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(3,8);
  /* and clear the display */
  lc.clearDisplay(3);

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
  display.setCursor(0, 10);// initialisation du display
  display.display();
  display.clearDisplay();
}

void clignote(){
  static boolean oui = HIGH;
  byte deuxpoints[2] = {0, B00100100};
  lc.setColumn(2,6,deuxpoints[oui]);
  oui = !oui;
  
}

void printnombre(int heure, int minute) {
  byte tab[10][5] = {
    {255, 129, 129, 129, 255},
    {128, 128, 255, 130, 132},
    {134, 137, 145, 161, 194},
    {118, 137, 137, 129, 130},
    {255, 8, 8, 8, 15},
    {112, 137, 137, 137, 6},
    {241, 145, 145, 145, 255},
    {255, 17, 17, 17, 1},
    {118, 137, 137, 137, 118},
    {126, 137, 137, 137, 6}};


for (int i = 0; i < 5; i++)
{
  lc.setColumn(0,i,tab[heure/10][i]);
  lc.setColumn(1,i,tab[heure%10][i]);
  lc.setColumn(2,i,tab[minute/10][i]);
  lc.setColumn(3,i,tab[minute%10][i]);
}

}

void loop() {               

  static long oldPosition = -999;

  long newPosition = myEncoder.read();
  if (newPosition != oldPosition) {
      // Map la plage de l'encodeur (par exemple, 0 à 1000) à la plage de degrés souhaitée (par exemple, 0 à 360).
      int degrees = map(newPosition, 0, 1000, 0, 360);

      Serial.print("Rotation en degrés: ");
      Serial.println(degrees);
      oldPosition = newPosition;
  }


  DateTime now = rtc.now();
  printnombre(now.hour(), now.minute());



  /*Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());
  delay(10);*/


  if(now.second()%2 == 0){
  clignote();
  }



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
  delay(200);

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
  compteurreveilm=0;
  compteurreveilh=0;

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
  if(compteurmod==0){
    if (compteurreveilampm == 0) {
      display.println(F(" am"));
    } else {
      display.println(F(" pm"));
    }
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
