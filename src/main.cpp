#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <RTClib.h>

#define largeurMENU 128
#define hauteurMENU 64

int boutonUP = A1;
int boutonDOWN = A2;
int boutonENTER = A3;
int LEDdecompteur;
int alimmatrice = 0;
int compteurmod = 1;
int compteurflechemenu = 1;
int nbOPT = 3;
int verifreveil1 = 0;
int verifreveil2 = 0;
int compteurreveilm = 0;
int compteurreveilh = 0;
int compteurreveilampm = 0;

// pin buzzer au pif
int pinBuzzer = 5 

Adafruit_SSD1306 display(largeurMENU, hauteurMENU, &Wire, -1);

// Function prototypes
void marche_arret();
void modeAMPM();
void reveil24();
void reveil12();

void setup() {
  pinMode(boutonDOWN, INPUT);
  pinMode(boutonENTER, INPUT);
  pinMode(boutonUP, INPUT);
  pinMode(LEDdecompteur, OUTPUT);

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
  verifreveil1 = 0;
  verifreveil2 = 0;

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
    marchearret();
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
    display.println("Changement en format horaire de 12 h");
  } else {
    compteurmod = 0;
    display.println("Changement en format horaire de 24 h");
  }

  display.display();
  delay(5000);
  loop();
}

void reveil24() {
  display.clearDisplay();
  display.setCursor(0, 10);

  while (verifreveil1 == 0) {
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

    display.println("heure de la sonnerie : ");
    display.println(compteurreveilh, ":", "00");
    display.println("appuyé sur le bouton central pour selectionner l'heure");
    display.display();
    delay(100);
    display.clearDisplay();

    if (digitalRead(boutonENTER) == HIGH) {
      verifreveil1 = 1;
    }
  }

  display.setCursor(0, 10);

  while (verifreveil2 == 0) {
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
    display.println(compteurreveilh, ":", compteurreveilm);
    display.println("appuyé sur le bouton central pour selectionner l'heure");
    display.display();
    delay(100);
    display.clearDisplay();
  }
}

void reveil12() {
  // Add your code for 12-hour format alarm here
}
