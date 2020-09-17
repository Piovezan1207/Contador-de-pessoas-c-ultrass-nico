#include <Arduino.h>
#include <EEPROM.h>

#define LED_verde A2
#define LED_VErdemlho A1
#define btn A0
#define btn2 A3

#define nmedia 3

int vet_media1[nmedia];
int vet_media2[nmedia];

long tempPisca, tempPisca2, temp, temp2, tempbtn, tempbtn2;
int cont = 0;
bool flag, flag2, flag3, flag4, flagbtn, flagbtn_2, flagbtn2, flagbtn2_2, flagmenu;
int nalunos;

byte in[] = {2, 8};                                                        //Pinos de entrada
byte in_pull[] = {btn, btn2};                                              //Pinos de entrada com pullup
byte out[] = {LED_verde, LED_VErdemlho, 3, 5, 6, 7, 4, 9, 10, 11, 12, 13}; //pinos de saída

int dist_1(void);
int dist_2(void);
void entrada(void);
void saida(void);
void logica(void);
void menu(void);

void num(byte n);

void setup()
{
  Serial.begin(9600);
  nalunos = EEPROM.read(0);

  for (byte i = 0; i < sizeof(in); i++)
    pinMode(in[i], INPUT);
  for (byte i = 0; i < sizeof(in_pull); i++)
    pinMode(in_pull[i], INPUT_PULLUP);
  for (byte i = 0; i < sizeof(out); i++)
    pinMode(out[i], OUTPUT);

  digitalWrite(LED_verde, HIGH);

  num(nalunos);
  delay(1500);
}

void loop()
{
  //Serial.println(dist_1());
  //Serial.println(dist_2());
  if (digitalRead(btn2)  && flagbtn2 == LOW && (millis() - tempbtn2) > 90)
  {
    tempbtn2 = millis();
    flagbtn2 = HIGH;
  }
  if (digitalRead(btn2) == LOW && flagbtn2)
  {
    flagbtn2 = LOW;
    flagbtn2_2 = LOW;
    if ((millis() - tempbtn2) < 1500 && flagmenu == LOW && cont != 0)
    {
      cont--;
      digitalWrite(LED_verde, HIGH);
      digitalWrite(LED_VErdemlho, LOW);
      flag = LOW;
      flag2 = LOW;
    }
    if (flagmenu)
    {
      if(nalunos != 0)nalunos--;
    }
  }

  if ((millis() - tempbtn2) > 2000 && flagbtn2 && flagbtn2_2 == LOW)
  {
    cont = 0;
    digitalWrite(LED_verde, HIGH);
    digitalWrite(LED_VErdemlho, LOW);
    flag = LOW;
    flag2 = LOW;
    for (byte i = 0; i < 4; i++)
    {
      digitalWrite(LED_verde, !digitalRead(LED_verde));
      digitalWrite(LED_VErdemlho, !digitalRead(LED_VErdemlho));
      delay(100);
    }
    flagbtn2_2 = HIGH;
  }
  /////////////////////////////////////////////////////////////////////////////
  if (digitalRead(btn)  && flagbtn == LOW && (millis() - tempbtn) > 90)
  {
    flagbtn = HIGH;
    tempbtn = millis();
    flagbtn_2 = LOW;
  }

  if (digitalRead(btn) == LOW && flagbtn)
  {
    flagbtn = LOW;

    if ((millis() - tempbtn) < 1500 && flagmenu == LOW )
    {
      if (cont < nalunos) cont++;
      flag = LOW;
      flag2 = LOW;
      if (cont < nalunos && flagmenu == LOW)
      {
        digitalWrite(LED_verde, HIGH);
        digitalWrite(LED_VErdemlho, LOW);
      }
    }
    if (flagmenu && flagbtn_2 == LOW && (millis() - tempbtn) < 2000)
    {
      nalunos++;
    }
  }

  if ((millis() - tempbtn) > 2000 && flagbtn && flagbtn_2 == LOW)
  {
    flagmenu = !flagmenu;
    flag = LOW;
    flag2 = LOW;
    if (cont < nalunos && flagmenu == LOW)
    {
      digitalWrite(LED_verde, HIGH);
      digitalWrite(LED_VErdemlho, LOW);
    }
    EEPROM.write(0, nalunos);
    flagbtn_2 = HIGH;
  }
  ////////////////////////////////////////////////////////////////////////////////
  if (flagmenu == LOW)
  {
    logica();
  }
  else
  {
    menu();
  }
}
////////////////////////////////////////////////////////////////////////////////
void menu(void)
{

  if ((millis() - tempPisca) > 100)
  {
    digitalWrite(LED_VErdemlho, !digitalRead(LED_VErdemlho));
    digitalWrite(LED_verde, !digitalRead(LED_verde));
    tempPisca = millis();
  }

  num(nalunos);
}
////////////////////////////////////////////////////////////////////////////////
void logica(void)
{
  num(cont);

  if (cont < nalunos)
  {
    entrada();
  }
  else if (flag2 == LOW)
  {
    digitalWrite(LED_verde, LOW);
    digitalWrite(LED_VErdemlho, HIGH);
  }

  saida();

  if (flag3 == HIGH && dist_2() > 15)
  {
    flag3 = LOW;
    Serial.println("Reset Flag 3");
    delay(1000);
  }

  if (flag4 == HIGH && dist_1() > 15)
  {
    flag4 = LOW;
    Serial.println("Reset Flag 4");
    delay(1000);
  }
}
////////////////////////////////////////////////////////////////////////////////
void entrada()
{
  if (dist_1() < 5 && flag2 == LOW && flag4 == LOW) //Verifica se o aluno entrou na sala
  {
    temp = millis();
    if (flag == LOW)
      tempPisca = millis();
    flag = HIGH;
    digitalWrite(LED_verde, LOW);
  }
  else if (dist_2() < 5 && (millis() - temp) < 10000 && flag == HIGH && flag2 == LOW) //Aguarda o aluno passar
  {                                                                                    //pelo segundo sensor para confirmar sua entrada
    cont++;
    Serial.println(cont);
    digitalWrite(LED_VErdemlho, LOW);
    if (cont != nalunos)
      digitalWrite(LED_verde, HIGH);
    flag = LOW;
    flag3 = HIGH;
  }
  else if ((millis() - temp) > 10000 && flag2 == LOW && flag) //Caso o aluno demora x segundos, para passar pelo segundo sensor, o sistema ignora
  {
    digitalWrite(LED_VErdemlho, LOW);
    digitalWrite(LED_verde, HIGH);
    flag = LOW;
    Serial.println("timeout entrada");
  }

  if (flag && (millis() - tempPisca) > 240 && flag2 == LOW)
  {
    digitalWrite(LED_VErdemlho, !digitalRead(LED_VErdemlho));
    tempPisca = millis();
  }
}
////////////////////////////////////////////////////////////////////////////////
void saida()
{
  if (dist_2() < 5 && flag == LOW && flag3 == LOW)
  {
    temp2 = millis();
    if (flag2 == LOW)
      tempPisca2 = millis();
    flag2 = HIGH;
    digitalWrite(LED_verde, LOW);
  }
  else if (dist_1() < 5 && (millis() - temp2) < 10000 && flag2 == HIGH && flag == LOW)
  {
    if (cont != 0)
      cont--;
    Serial.println(cont);
    digitalWrite(LED_VErdemlho, LOW);
    digitalWrite(LED_verde, HIGH);
    flag2 = LOW;
    flag4 = HIGH;
  }
  else if ((millis() - temp2) > 10000 && flag == LOW && flag2)
  {
    digitalWrite(LED_VErdemlho, LOW);
    digitalWrite(LED_verde, HIGH);
    flag2 = LOW;
    Serial.println("timeout saida");
  }

  if (flag2 && (millis() - tempPisca2) > 240 && flag == LOW)
  {
    digitalWrite(LED_VErdemlho, !digitalRead(LED_VErdemlho));
    tempPisca2 = millis();
  }
}
////////////////////////////////////////////////////////////////////////////////
void num(byte n)
{
  if (n < 10)
  {
    PORTD = n << 4;
    PORTB = 0 << 2;
  }
  else
  {
    String temp = String(n);
    temp = temp[0];
    PORTB = (temp.toInt()) << 2;
    temp = String(n);
    temp = temp[1];
    PORTD = (temp.toInt()) << 4;
  }
}
////////////////////////////////////////////////////////////////////////////////
int dist_1()
{
  int media = 0;
  digitalWrite(3, HIGH);
  delayMicroseconds(10);
  digitalWrite(3, LOW);
  delayMicroseconds(2);

  int val = pulseIn(2, HIGH);

  for (byte i = 0; i < nmedia - 1; i++)
    vet_media1[i] = vet_media1[i + 1];
  vet_media1[nmedia - 1] = val;
  for (byte i = 0; i < nmedia; i++)
    media += vet_media1[i];
  return (media / nmedia) / 58;
}
////////////////////////////////////////////////////////////////////////////////
int dist_2()
{
  int media = 0;
  digitalWrite(9, HIGH);
  delayMicroseconds(10);
  digitalWrite(9, LOW);
  delayMicroseconds(2);

  int val = pulseIn(8, HIGH);

  for (byte i = 0; i < nmedia - 1; i++)
    vet_media2[i] = vet_media2[i + 1];
  vet_media2[nmedia - 1] = val;
  for (byte i = 0; i < nmedia; i++)
    media += vet_media2[i];
  return (media / nmedia) / 58;
}