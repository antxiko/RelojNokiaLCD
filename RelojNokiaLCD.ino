
#include <PCD8544.h>
#include <DHT.h>
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

//Definimos el pin y el tipo de sensor de temperatura 
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//Definimos el LCD
static PCD8544 lcd;


// Caracteres para iconos
static const byte reloj[] = 
{ B00011100,
  B00100010, 
  B00101110, 
  B00101010, 
  B00011100 
};

static const byte termo[] = 
{ B00110000,
  B01111110, 
  B01111001, 
  B01111110, 
  B00110000 
};

static const byte cal[] = 
{ B01111111,
  B01010111, 
  B01111111, 
  B01010111, 
  B01111111 
};

void setup() {
  
  // Inicializamos el LCD con la resolucion y el contraste
  lcd.begin(84, 48);
  lcd.setContrast(50);
  
  // Inicializamos el sensor de temperatura y humedad DHT11
  dht.begin();

  // creamos el caracter 0 con el reloj.
  lcd.createChar(0, reloj);
  // creamos el caracter 1 con el termometro
  lcd.createChar(1, termo);
  // creamos el caracter 2 con el calendario
  lcd.createChar(2, cal);
}

void loop() {

  //Metemos en t el valor de la temperatura del DHT11
  int t = dht.readTemperature();

  // elementos al Time
  tmElements_t tm;

  
  // Imprimimos:
  
  // temperatura
  lcd.setCursor(0, 0);
  lcd.write(1);
  lcd.setCursor(8, 0);
  lcd.print(t);
  lcd.setCursor(24, 0);
  lcd.print("C.");

  // hora
  RTC.read(tm);
  lcd.setCursor(0, 1);
  lcd.write(0);
  lcd.setCursor(8,1);
  print2digits(tm.Hour);
  lcd.setCursor(19, 1);
  lcd.print(":");
  lcd.setCursor(24, 1);
  print2digits(tm.Minute);
  
  // fecha
  lcd.setCursor(0, 2);
  lcd.write(2);
  lcd.setCursor(8,2);
  print2digits(tm.Day);
  lcd.setCursor(19, 2);
  lcd.print("/");
  lcd.setCursor(24, 2);
  print2digits(tm.Month);
  
  delay(2000); //esperamos 2 segundos y seguimos
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    lcd.print("0");
  }
    lcd.print(number);
}


