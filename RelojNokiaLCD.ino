
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

static const byte grados[] = 
{ B00000000,
  B00000110, 
  B00001001, 
  B00001001, 
  B00000110
};

static const byte gota[] = 
{ B00111000,
  B01000110, 
  B01000001, 
  B01000110, 
  B00111000
};


void setup() {
  
  // Inicializamos el LCD con la resolucion y el contraste
  lcd.begin(84, 48);
  lcd.setContrast(55);
  
  // Inicializamos el sensor de temperatura y humedad DHT11
  dht.begin();

  // creamos el caracter 0 con el reloj.
  lcd.createChar(0, reloj);
  // creamos el caracter 1 con el termometro
  lcd.createChar(1, termo);
  // creamos el caracter 2 con el calendario
  lcd.createChar(2, cal);
  // creamos el caracter 3 con simbolo "Grados"
  lcd.createChar(3, grados);
  // creamos el caracter 4 con la gota
  lcd.createChar(4, gota);

  printBordes();
}

void loop() {

  //Metemos en t el valor de la temperatura y en h la humedad
  int t = dht.readTemperature();
  int h = dht.readHumidity();
  // elementos al Time
  tmElements_t tm;

  
  // Imprimimos:
  
  // temperatura
  lcd.setCursor(8, 1);
  lcd.write(1);
  lcd.setCursor(16, 1);
  lcd.print(t);
  lcd.setCursor(32, 1);
  lcd.print("C");
  lcd.write(3);

  // Humedad relativa
  lcd.setCursor(50, 1);
  lcd.write(4);
  lcd.setCursor(58, 1);
  lcd.print(h);
  lcd.setCursor(70, 1);
  lcd.print("%");
  
  // hora
  RTC.read(tm);
  lcd.setCursor(12, 2);
  lcd.write(0);
  lcd.setCursor(20,2);
  print2digits(tm.Hour);
  lcd.setCursor(38, 2);
  print2digits(tm.Minute);
  lcd.setCursor(57, 2);
  print2digits(tm.Second);
  
  // fecha
  lcd.setCursor(8, 3);
  lcd.write(2);
  lcd.setCursor(16,3);
  print2digits(tm.Day);
  lcd.setCursor(28, 3);
  lcd.print("/");
  lcd.setCursor(33, 3);
  print2digits(tm.Month);
  lcd.setCursor(44, 3);
  lcd.print("/");
  lcd.setCursor(50, 3);
  print2digits(tmYearToCalendar(tm.Year));

  //Parpadeo de los separadores
  lcd.setCursor(32, 2);
  lcd.print(" ");
  lcd.setCursor(51, 2);
  lcd.print(" ");
  delay(500);
  lcd.setCursor(32, 2);
  lcd.print(":");
  lcd.setCursor(51, 2);
  lcd.print(":");
  delay(500);
}

//Funcion para imprimir los bordes de la pantalla
void printBordes() {
  lcd.setCursor(0,0);
  lcd.print("**************");
  lcd.print("*            *");
  lcd.print("*            *");
  lcd.print("*            *");
  lcd.print("* Ardu-Clock *");
  lcd.print("**************");
  
}
//Funcion para imprimir un 0 antes que un resultado menor de 10 (07:05)
void print2digits(int number) {
  if (number >= 0 && number < 10) {
    lcd.print("0");
  }
    lcd.print(number);
}



