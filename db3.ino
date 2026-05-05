const int inputPin { A0 };               
const int red { 2 };                      //
const int yellow { 1 };                   // Punainen, keltainen ja vihreä led.
const int green { 0 };                    //
const int sampleWindow { 50 };            // näyteikkunan aikaraja millisekuntteina.
const int offset { 512 };                 // DC-offsetin määrä.
const int ledPins[] = {A1, A2, A3};       // ATMEGA328P pinnit, joihin ledit on yhdistetty.
const int lowerLimit { 70 };              // Raja, jolloin keltainen ledi syttyy.
const int upperLimit { 90 };              // Raja, jolloin punainen ledi syttyy.
const float vref{0.05};                   // Referenssi, jota käytetään kalibrointiin.

unsigned long interval { 100 };           // Ilmaisee millisekuntteina, kuinka kauan ledi palaa.
unsigned long previousMillis { 0 };       // Tallennetaan edellisen millis-funktion aika.

enum Level {
  OK,
  MEDIUM,                                
  BIG
};
enum Level ledState { OK };              

int counter;                              // Laskee näytteiden määrän.
int sample;                               // Raakadata analogi pinnistä.
int signal;                               // Data, johon on tehty offsetin poisto ja abs.
float mean;                               // keskiarvo.

void setup()
{
  analogReference(EXTERNAL);              // Määritetään käyttämään 5V referenssiä.
  pinMode(ledPins[red], OUTPUT);          //
  pinMode(ledPins[yellow], OUTPUT);       // Alustetaan pinnit, jossa ledit on yhdistetty outputiksi.
  pinMode(ledPins[green], OUTPUT);        //
}

void loop()
{
  int finalDecibel = getDecibel();  
  compareDecibel(finalDecibel);
  ledControl(finalDecibel);

  counter = 0;
}


int getDecibel() {                         // Funktio lukee dataa analogi pinnistä ja muuttaa sen desibeleiksi.
  unsigned long timeStart = millis();

  while (millis() - timeStart < sampleWindow)
  {
    sample = analogRead(inputPin);
    signal = abs(sample - offset);
    mean += ((long) signal * signal);

    counter ++;
  }

  mean /= counter;

  float RMS = sqrt(mean);
  float dB = 20 * log10(RMS / vref);
  int rounded = round(dB);
  
  return rounded;
}

void compareDecibel(int dB) {                 // Vertaillaan laskettua Desibeli arvoa asetettuja rajoja vastaan ja muutetaan ledState sen mukaan.
  if (dB >= lowerLimit && dB < upperLimit) {
    ledState = MEDIUM;
  } else if (dB >= upperLimit) {
    ledState = BIG;
  } else {
    ledState = OK;
  }
}

void ledControl() {               			  // Sytytetään ja sammutetaan ledejä ledStaten mukaan.
  unsigned long currentMillis { millis() };
  if (currentMillis - previousMillis > interval) {

    previousMillis = currentMillis;

    switch (ledState) {
      default:
      case  OK:
        turnOtherLedsOff(green);
        turnLedOn(green);
        break;
      case MEDIUM:
        turnOtherLedsOff(yellow);
        turnLedOn(yellow);
        break;
      case BIG:
        turnOtherLedsOff(red);
        turnLedOn(red);
        break;
    }
  }
}

void turnOtherLedsOff(byte light) {          // Sammuttaa kaikki ledit paitsi sille parametrina annettua.
  for (int i = 0; i < 3; i++)  {
    if (i != light) {
      digitalWrite(ledPins[i], LOW);
    }
  }
}

void turnLedOn(byte light) {                // Sytyttää sille parametrina annetun ledin palamaan.
  digitalWrite(ledPins[light], HIGH);
}
