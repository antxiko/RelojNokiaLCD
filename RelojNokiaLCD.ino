#include <PCD8544.h>
#include <DHT.h>
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <Rtc_Pcf8563.h>
#include "pitches.h"

//Definimos el pin y el tipo de sensor de temperatura 
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//init the real time clock
Rtc_Pcf8563 rtc;

const int boton1 = 9;
const int boton2 = 10;
const int boton3 = 11;


int estadoBoton1= 0;
int estadoBoton2= 0;
int estadoBoton3= 0;

int casilla = 0;
int alarm = 0;
int dia = 0;
int hoy = 0;
int ho = 0;
int mi = 0;
int se = 0;
int di = 0;
int me = 0;
int an = 0;
int hoAL = 0;
int miAL = 0;
int seAL = 0;
int l = 0;
int m = 0;
int mx = 0;
int j = 0;
int v = 0;
int s = 0;
int d = 0;
int boton = 6;

String dias[] = { "", "Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado" };
int alDias[] = {0,0,0,0,0,0,0,0};
int diasMes[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
bool bisiesto = false;
bool alarmaSonando = false;

int intervalo = 500;
int puntos = 0;
int confTime = 0;
unsigned long preMillis=0;
unsigned long sonMillis=0;
 
//Definimos el LCD
#define backlight 12
static PCD8544 lcd;

// Caracteres para iconos
static const byte reloj[] = 
{ 0x1c, 0x22, 0x2e, 0x2a, 0x1c };
static const byte termo[] = 
{ 0x70, 0xfe, 0xf9, 0xfe, 0x70 };
static const byte cal[] = 
{ 0x7f, 0x57, 0x7f, 0x57, 0x7f };
static const byte grados[] = 
{ 0x00, 0x06, 0x09, 0x09, 0x06 };
static const byte gota[] = 
{ 0x38, 0x46, 0x41, 0x46, 0x38 };
static const byte alarma[] = 
{ 0x42, 0x76, 0x6a, 0x76, 0x42 };
static const byte botonOff[] = 
{ 0x1c, 0x22, 0x22, 0x22, 0x1c };
static const byte botonOn[] = 
{ 0x1c, 0x3e, 0x3e, 0x3e, 0x1c };

void setup() {

  Serial.begin(9600);

  pinMode(boton1, INPUT);
  pinMode(boton2, INPUT);
  pinMode(boton3, INPUT);
  
  // Inicializamos el LCD con la resolucion y el contraste
  lcd.begin(84, 48);
  lcd.setContrast(50);
  analogWrite(backlight, 1);
  
  // Inicializamos el sensor de temperatura y humedad DHT11
  dht.begin();

  //RTC clear out all the registers
  //rtc.initClock();
  

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
  // creamos el caracter 5 con la alarma
  lcd.createChar(5, alarma);
  // creamos el caracter 6 con la botonOn
  lcd.createChar(6, botonOff);
  // creamos el caracter 7 con la botonOff
  lcd.createChar(7, botonOn);
   printBordes();
  
}

void loop() {

//   tmElements_t tm;
//   RTC.read(tm);
   ho = rtc.getHour();
   mi = rtc.getMinute();
   se = rtc.getSecond();
   di = rtc.getDay();
   me = rtc.getMonth();
   an = rtc.getYear();
   anoBisiesto(an);
   if ( bisiesto == true) {
    diasMes[2] = 29;
    } else {
      diasMes[2] = 28; 
    } 
   setTime(ho,mi,se,di,me,an);
   printDatos();
   dosPuntos();

   int alarmCheck = alDias[1] + alDias[2] + alDias[3] + alDias[4] + alDias[5] + alDias[6] + alDias[7];
   if (alarmCheck == 0) {
   alarm = 0;
   }
   if (alarm == 1) {
    lcd.setCursor(70,2);
    lcd.write(5);
    } else {
      lcd.setCursor(70,2);
      lcd.print(" ");
      alarm = 0;
    }
   
  estadoBoton2 = digitalRead(boton2);
  delay(150);
  if (estadoBoton2 == LOW) {
    alarm = !alarm;
    }   
  hoy = weekday(now());      
  if ( hoAL == rtc.getHour() && miAL == rtc.getMinute() && seAL == rtc.getSecond() && alDias[hoy] == 1 && alarm == 1) {
    alarmaSonando = true;
  }
  if (alarmaSonando == true) {
    sonarAlarma();
      estadoBoton2 = digitalRead(boton2);
      delay(150);
        if (estadoBoton2 == LOW) {
          alarmaSonando = false;
          }
    }
  
  //Leemos el estado de los botones
  estadoBoton1 = digitalRead(boton1);
  if (estadoBoton1 == LOW) {
      lcd.clear();
      printSetTime();
      printSetDatos();
      dosPuntos();
      confTime = 1;    
        while (confTime == 1) {
          estadoBoton1 = digitalRead(boton1);
            if (estadoBoton1 == LOW) {
              printSetDatos();
              delay(150);
              casilla++;
              if (casilla > 5) {
                casilla = 0;
                }
              }
          switch (casilla) {
            case 0:
            setHoras();
            break;
            case 1:
            setMinutos();
            break;
            case 2:
            setSegundos();
            break;
            case 3:
            setDias();
            break;
            case 4:
            setMes();
            break;
            case 5:
            setAno();
            break;
            }  
        estadoBoton3 = digitalRead(boton3);
        if (estadoBoton3 == LOW) {
         confTime = 0;
         setTime(ho,mi,se,di,me,an);
         rtc.setTime(ho, mi, se);
         rtc.setDate(di,dia,me,0,an);
         lcd.clear();
         lcd.setCursor(0,1);
         lcd.print("time SET");
         delay(500);
         lcd.clear();
         printBordes();
        }
       }
     }

  //Leemos el estado del boton 3 para entrar en configurar Alarma
  estadoBoton3 = digitalRead(boton3);
    if (estadoBoton3 == LOW) {
      lcd.clear();
      tmElements_t tm;
      RTC.read(tm);
      printSetAlarm();
      printAlDatos();
      dosPuntos();
      printLDB();
      confTime = 2; 
    }
      while (confTime == 2) {
          estadoBoton3 = digitalRead(boton3);
            if (estadoBoton3 == LOW) {
              printAlDatos();
              printLDB();
              delay(150);
              casilla++;
              
              if (casilla > 9) {
                casilla = 0;
                }
              }
              switch (casilla) {
              case 0:
              setAlHoras();
              break;
              case 1:
              setAlMinutos();
              break;
              case 2:
              setAlSegundos();
              break;
              case 3:
              setL();
              break;
              case 4:
              setM();
              break;
              case 5:
              setMI();
              break;
              case 6:
              setJ();
              break;
              case 7:
              setV();
              break;
              case 8:
              setS();
              break;
              case 9:
              setD();
              break;
              }
        estadoBoton1 = digitalRead(boton1);
        if (estadoBoton1 == LOW) {
         confTime = 0;
         alarm = 1;
         lcd.clear();
         lcd.setCursor(0,1);
         lcd.print("alarm SET");
         delay(500);
         lcd.clear();
         printBordes();
        }
       }
     }  

//Funcion para que suene la alarma
void sonarAlarma() {
  
  sonMillis = millis();
  if ((unsigned long)(sonMillis - preMillis) >= intervalo) {
    
    tone(2, NOTE_A6, 150);
    delay(150);
    tone(2, NOTE_A7, 150);
    delay(150);
    tone(2, NOTE_A6, 150);
    delay(150);
    tone(2, NOTE_A7, 150);
    delay(150);
    }
    preMillis = sonMillis;  
}

//Funcion para imprimir los bordes de la pantalla
void printBordes() {
  lcd.setCursor(0,0);
  lcd.print("**************");
  lcd.print("*            *");
  lcd.print("*            *");
  lcd.print("*            *");
  lcd.print("*            *");
  lcd.print("* AntxiClock *");
}

//Funcion para el menu de configurar hora
void printSetTime() {
  lcd.setCursor(0,0);
  lcd.print("**************");
  lcd.print("*            *");
  lcd.print("*            *");
  lcd.print("*            *");
  lcd.print("*            *");
  lcd.print("** Set Time **");
}
//Funcion para el menu de configurar hora
void printSetAlarm() {
  lcd.setCursor(0,0);
  lcd.print("**************");
  lcd.print("*            *");
  lcd.print("*            *");
  lcd.print("*            *");
  lcd.print("*            *");
  lcd.print("* Set  Alarm *");
}
void printSetDatos() {

  // hora
  lcd.setCursor(12, 2);
  lcd.write(0);
  lcd.setCursor(20,2);
  print2digits(ho);
  lcd.setCursor(38, 2);
  print2digits(mi);
  lcd.setCursor(57, 2);
  print2digits(se);
  
  // fecha
  lcd.setCursor(8, 3);
  lcd.write(2);
  lcd.setCursor(16,3);
  print2digits(di);
  lcd.print("/");
  print2digits(me);
  lcd.print("/");
  lcd.print(an);
  }

void printAlDatos() {
  
  // hora
  lcd.setCursor(12, 2);
  lcd.write(0);
  lcd.setCursor(20,2);
  print2digits(hoAL);
  lcd.setCursor(38, 2);
  print2digits(miAL);
  lcd.setCursor(57, 2);
  print2digits(seAL);
  }

void printLDB() {      
      lcd.setCursor(9,3);
      lcd.print("L");
      lcd.setCursor(19,3);
      lcd.print("M");
      lcd.setCursor(29,3);
      lcd.print("M");
      lcd.setCursor(39,3);
      lcd.print("J");
      lcd.setCursor(49,3);
      lcd.print("V");
      lcd.setCursor(59,3);
      lcd.print("S");
      lcd.setCursor(69,3);
      lcd.print("D");
      lcd.setCursor(9,4);
      printBoton(alDias[2]);
      lcd.setCursor(19,4);
      printBoton(alDias[3]);
      lcd.setCursor(29,4);
      printBoton(alDias[4]);
      lcd.setCursor(39,4);
      printBoton(alDias[5]);
      lcd.setCursor(49,4);
      printBoton(alDias[6]);
      lcd.setCursor(59,4);
      printBoton(alDias[7]);
      lcd.setCursor(69,4);
      printBoton(alDias[1]);
}

void printDatos() {

  // copiamos a t y h la temperatura y la humedad
  int t = dht.readTemperature();
  int h = dht.readHumidity();

  // elementos al Time
//  tmElements_t tm;
//  RTC.read(tm);
  dia = weekday(now());
  
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
  lcd.setCursor(12, 2);
  lcd.write(0);
  lcd.setCursor(20,2);
  print2digits(ho);
  lcd.setCursor(38, 2);
  print2digits(mi);
  lcd.setCursor(57, 2);
  print2digits(se);
  
  // fecha
  //Dia de la semana
  int ca = dias[dia].length();
  int cu = (74/ca)*2;
  lcd.setCursor(cu,3);
  lcd.print(dias[dia]);
  lcd.setCursor(8, 4);
  lcd.write(2);
  lcd.setCursor(16,4);
  print2digits(di);
  lcd.print("/");
  print2digits(me);
  lcd.print("/");
  lcd.print("20");
  lcd.print(an);
  }

void setHoras() {
        //funcion para poner en hora HORAS
        sonMillis = millis();
        estadoBoton2 = digitalRead(boton2);
        delay(150);
            if (estadoBoton2 == LOW) {
            ho++;
            if (ho > 23) {
              ho = 0;
              }
            lcd.setCursor(20,2);
            print2digits(ho);    
            }
  
        if ((unsigned long)(sonMillis - preMillis) >= intervalo) {
          switch (puntos) {
          //encendemos Hora
          case 0:
          lcd.setCursor(20,2);
          print2digits(ho);
          puntos = 1;
          break;
          case 1:
          //apagamos Hora
          lcd.setCursor(20,2);
          lcd.print("  ");
          puntos = 0;
          break;
          }
          preMillis = sonMillis;
          }
        }
        
void setAlHoras() {
        //funcion para poner en hora HORAS
        sonMillis = millis();
        estadoBoton2 = digitalRead(boton2);
        delay(150);
            if (estadoBoton2 == LOW) {
            hoAL++;
            if (hoAL > 23) {
              hoAL = 0;
              }
            lcd.setCursor(20,2);
            print2digits(hoAL);    
            }
  
        if ((unsigned long)(sonMillis - preMillis) >= intervalo) {
          switch (puntos) {
          //encendemos Hora
          case 0:
          lcd.setCursor(20,2);
          print2digits(hoAL);
          puntos = 1;
          break;
          case 1:
          //apagamos Hora
          lcd.setCursor(20,2);
          lcd.print("  ");
          puntos = 0;
          break;
          }
          preMillis = sonMillis;
          }
        }
        
void setMinutos() {
        //funcion para poner en hora Minutos
        sonMillis = millis();
        estadoBoton2 = digitalRead(boton2);
        delay(150);
            if (estadoBoton2 == LOW) {
            mi++;
            if (mi > 59) {
              mi = 0;
              }
            lcd.setCursor(38, 2);
            print2digits(mi);    
            }
  
        if ((unsigned long)(sonMillis - preMillis) >= intervalo) {
          switch (puntos) {
          //encendemos Hora
          case 0:
          lcd.setCursor(38, 2);
          print2digits(mi);
          puntos = 1;
          break;
          case 1:
          //apagamos Hora
          lcd.setCursor(38, 2);
          lcd.print("  ");
          puntos = 0;
          break;
          }
          preMillis = sonMillis;
          }
        }
        
void setAlMinutos() {
        //funcion para poner en hora Minutos
        sonMillis = millis();
        estadoBoton2 = digitalRead(boton2);
        delay(150);
            if (estadoBoton2 == LOW) {
            miAL++;
            if (miAL > 59) {
              miAL = 0;
              }
            lcd.setCursor(38, 2);
            print2digits(miAL);    
            }
  
        if ((unsigned long)(sonMillis - preMillis) >= intervalo) {
          switch (puntos) {
          //encendemos Hora
          case 0:
          lcd.setCursor(38, 2);
          print2digits(miAL);
          puntos = 1;
          break;
          case 1:
          //apagamos Hora
          lcd.setCursor(38, 2);
          lcd.print("  ");
          puntos = 0;
          break;
          }
          preMillis = sonMillis;
          }
        }

void setSegundos() {
        //funcion para poner en hora Segundos
        sonMillis = millis();
        estadoBoton2 = digitalRead(boton2);
        delay(150);
            if (estadoBoton2 == LOW) {
            se++;
            if (se > 59) {
              se = 0;
              }
            lcd.setCursor(57,2);
            print2digits(se);    
            }
  
        if ((unsigned long)(sonMillis - preMillis) >= intervalo) {
          switch (puntos) {
          //encendemos Hora
          case 0:
          lcd.setCursor(57,2);
          print2digits(se);
          puntos = 1;
          break;
          case 1:
          //apagamos Hora
          lcd.setCursor(57,2);
          lcd.print("  ");
          puntos = 0;
          break;
          }
          preMillis = sonMillis;
          }
        }
void setAlSegundos() {
        //funcion para poner en hora Segundos
        sonMillis = millis();
        estadoBoton2 = digitalRead(boton2);
        delay(150);
            if (estadoBoton2 == LOW) {
            seAL++;
            if (seAL > 59) {
              seAL = 0;
              }
            lcd.setCursor(57,2);
            print2digits(seAL);    
            }
  
        if ((unsigned long)(sonMillis - preMillis) >= intervalo) {
          switch (puntos) {
          //encendemos Hora
          case 0:
          lcd.setCursor(57,2);
          print2digits(seAL);
          puntos = 1;
          break;
          case 1:
          //apagamos Hora
          lcd.setCursor(57,2);
          lcd.print("  ");
          puntos = 0;
          break;
          }
          preMillis = sonMillis;
          }
        }
        
void setDias() {
        //funcion para poner fecha DIAS
        sonMillis = millis();
        estadoBoton2 = digitalRead(boton2);
        delay(150);
            if (estadoBoton2 == LOW) {
            di++;
            if (di > diasMes[me]) {
              di = 1;
              }
            lcd.setCursor(16,3);
            print2digits(di);    
            }
  
        if ((unsigned long)(sonMillis - preMillis) >= intervalo) {
          switch (puntos) {
          //encendemos Hora
          case 0:
          lcd.setCursor(16,3);
          print2digits(di);
          puntos = 1;
          break;
          case 1:
          //apagamos Hora
          lcd.setCursor(16,3);
          lcd.print("  ");
          puntos = 0;
          break;
          }
          preMillis = sonMillis;
          }
        }
void setMes() {
        //funcion para poner fecha MES
        sonMillis = millis();
        estadoBoton2 = digitalRead(boton2);
        delay(150);
            if (estadoBoton2 == LOW) {
            me++;
            if (me > 12) {
              me = 1;
              }
            lcd.setCursor(33, 3);
            print2digits(me);    
            }
  
        if ((unsigned long)(sonMillis - preMillis) >= intervalo) {
          switch (puntos) {
          //encendemos Hora
          case 0:
          lcd.setCursor(33, 3);
          print2digits(me);
          puntos = 1;
          break;
          case 1:
          //apagamos Hora
          lcd.setCursor(33, 3);
          lcd.print("  ");
          puntos = 0;
          break;
          }
          preMillis = sonMillis;
          }
        }
void setAno() {
        //funcion para poner fecha Año
        sonMillis = millis();
        estadoBoton2 = digitalRead(boton2);
        delay(150);
            if (estadoBoton2 == LOW) {
            an++;
            if (an > 99) {
              an = 16;
              }
            lcd.setCursor(50, 3);
            lcd.print("20");
            lcd.print(an);
            }
  
        if ((unsigned long)(sonMillis - preMillis) >= intervalo) {
          switch (puntos) {
          //encendemos Hora
          case 0:
          lcd.setCursor(50, 3);
          lcd.print("20");
          lcd.print(an);
          puntos = 1;
          break;
          case 1:
          //apagamos Hora
          lcd.setCursor(50, 3);
          lcd.print("    ");
          puntos = 0;
          break;
          }
          preMillis = sonMillis;
          }
        }

//Funcion para activar o desactivar la alarma los LUNES
void setL() {
        sonMillis = millis();
        estadoBoton2 = digitalRead(boton2);
        delay(150);
            if (estadoBoton2 == LOW) {
             l = !l;   
             if (l == 0) {
              boton = 6;
              } else {
                boton = 7;
                }
              if (l == 1) {
                alDias[2] = 1;
                } else {
                  alDias[2] = 0;
                  }
                  lcd.setCursor(9, 4);
                  lcd.write(boton);
            }
  
        if ((unsigned long)(sonMillis - preMillis) >= intervalo) {
          switch (puntos) {
          //encendemos Hora
          case 0:
          lcd.setCursor(9, 4);
          lcd.write(boton);
          puntos = 1;
          break;
          case 1:
          //apagamos Hora
          lcd.setCursor(9, 4);
          lcd.print(" ");
          puntos = 0;
          break;
          }
          preMillis = sonMillis;
          }
        }
//Funcion para activar o desactivar la alarma los MARTES
void setM() {
        sonMillis = millis();
        estadoBoton2 = digitalRead(boton2);
        delay(150);
            if (estadoBoton2 == LOW) {
             m = !m;   
             if (m == 0) {
              boton = 6;
              } else {
                boton = 7;
                }
              if (m == 1) {
                alDias[3] = 1;
                } else {
                  alDias[3] = 0;
                  }
                  lcd.setCursor(19, 4);
                  lcd.write(boton);
            }
  
        if ((unsigned long)(sonMillis - preMillis) >= intervalo) {
          switch (puntos) {
          //encendemos Hora
          case 0:
          lcd.setCursor(19, 4);
          lcd.write(boton);
          puntos = 1;
          break;
          case 1:
          //apagamos Hora
          lcd.setCursor(19, 4);
          lcd.print(" ");
          puntos = 0;
          break;
          }
          preMillis = sonMillis;
          }
        }
//Funcion para activar o desactivar la alarma los MIERCOLES
void setMI() {
        sonMillis = millis();
        estadoBoton2 = digitalRead(boton2);
        delay(150);
            if (estadoBoton2 == LOW) {
             mx = !mx;   
             if (mx == 0) {
              boton = 6;
              } else {
                boton = 7;
                }
              if (mx == 1) {
                alDias[4] = 1;
                } else {
                  alDias[4] = 0;
                  }
                  lcd.setCursor(29, 4);
                  lcd.write(boton);
            }
  
        if ((unsigned long)(sonMillis - preMillis) >= intervalo) {
          switch (puntos) {
          //encendemos Hora
          case 0:
          lcd.setCursor(29, 4);
          lcd.write(boton);
          puntos = 1;
          break;
          case 1:
          //apagamos Hora
          lcd.setCursor(29, 4);
          lcd.print(" ");
          puntos = 0;
          break;
          }
          preMillis = sonMillis;
          }
        }
//Funcion para activar o desactivar la alarma los JUEVES
void setJ() {
        sonMillis = millis();
        estadoBoton2 = digitalRead(boton2);
        delay(150);
            if (estadoBoton2 == LOW) {
             j = !j;   
             if (j == 0) {
              boton = 6;
              } else {
                boton = 7;
                }
              if (j == 1) {
                alDias[5] = 1;
                } else {
                  alDias[5] = 0;
                  }
                  lcd.setCursor(39, 4);
                  lcd.write(boton);
            }
  
        if ((unsigned long)(sonMillis - preMillis) >= intervalo) {
          switch (puntos) {
          //encendemos Hora
          case 0:
          lcd.setCursor(39, 4);
          lcd.write(boton);
          puntos = 1;
          break;
          case 1:
          //apagamos Hora
          lcd.setCursor(39, 4);
          lcd.print(" ");
          puntos = 0;
          break;
          }
          preMillis = sonMillis;
          }
        }
//Funcion para activar o desactivar la alarma los VIERNES
void setV() {
        sonMillis = millis();
        estadoBoton2 = digitalRead(boton2);
        delay(150);
            if (estadoBoton2 == LOW) {
             v = !v;   
             if (v == 0) {
              boton = 6;
              } else {
                boton = 7;
                }
              if (v == 1) {
                alDias[6] = 1;
                } else {
                  alDias[6] = 0;
                  }
                  lcd.setCursor(49, 4);
                  lcd.write(boton);
            }
  
        if ((unsigned long)(sonMillis - preMillis) >= intervalo) {
          switch (puntos) {
          //encendemos Hora
          case 0:
          lcd.setCursor(49, 4);
          lcd.write(boton);
          puntos = 1;
          break;
          case 1:
          //apagamos Hora
          lcd.setCursor(49, 4);
          lcd.print(" ");
          puntos = 0;
          break;
          }
          preMillis = sonMillis;
          }
        }
//Funcion para activar o desactivar la alarma los SABADO
void setS() {
        sonMillis = millis();
        estadoBoton2 = digitalRead(boton2);
        delay(150);
            if (estadoBoton2 == LOW) {
             s = !s;   
             if (s == 0) {
              boton = 6;
              } else {
                boton = 7;
                }
              if (s == 1) {
                alDias[7] = 1;
                } else {
                  alDias[7] = 0;
                  }
                  lcd.setCursor(59, 4);
                  lcd.write(boton);
            }
  
        if ((unsigned long)(sonMillis - preMillis) >= intervalo) {
          switch (puntos) {
          //encendemos Hora
          case 0:
          lcd.setCursor(59, 4);
          lcd.write(boton);
          puntos = 1;
          break;
          case 1:
          //apagamos Hora
          lcd.setCursor(59, 4);
          lcd.print(" ");
          puntos = 0;
          break;
          }
          preMillis = sonMillis;
          }
        }
//Funcion para activar o desactivar la alarma los DOMINGOS
void setD() {
        sonMillis = millis();
        estadoBoton2 = digitalRead(boton2);
        delay(150);
            if (estadoBoton2 == LOW) {
             d = !d;   
             if (d == 0) {
              boton = 6;
              } else {
                boton = 7;
                }
              if (d == 1) {
                alDias[1] = 1;
                } else {
                  alDias[1] = 0;
                  }
                  lcd.setCursor(69, 4);
                  lcd.write(boton);
            }
  
        if ((unsigned long)(sonMillis - preMillis) >= intervalo) {
          switch (puntos) {
          //encendemos Hora
          case 0:
          lcd.setCursor(69, 4);
          lcd.write(boton);
          puntos = 1;
          break;
          case 1:
          //apagamos Hora
          lcd.setCursor(69, 4);
          lcd.print(" ");
          puntos = 0;
          break;
          }
          preMillis = sonMillis;
          }
        }
        
//Funcion para apagar y encender los separadores de la hora (:)
void dosPuntos() {
  sonMillis = millis();
  
  if ((unsigned long)(sonMillis - preMillis) >= intervalo) {
    switch (puntos) {
      //encendemos puntos
      case 0:
      lcd.setCursor(32, 2);
      lcd.print(":");
      lcd.setCursor(51, 2);
      lcd.print(":");
      puntos = 1;
      break;
      case 1:
      //apagamos puntos
       lcd.setCursor(32, 2);
       lcd.print(" ");
       lcd.setCursor(51, 2);
       lcd.print(" ");
       puntos = 0;
      break;
    }
      preMillis = sonMillis;
  }
}

void printBoton(int number) {
  if (number == 1) {
    lcd.write(7);
    }
  if (number == 0) {
    lcd.write(6);
    }
  }
  
//Funcion para imprimir un 0 antes que un resultado menor de 10 (07:05)
void print2digits(int number) {
  if (number >= 0 && number < 10) {
    lcd.print("0");
  }
    lcd.print(number);
}

void anoBisiesto (int number) { 
    if (number/400){
      bisiesto = true;
    }
    else if(number/100){
      bisiesto = false;
    }
    else if(number/4){
      bisiesto = true;
    }
    else{
      bisiesto = false;
    }
  }

