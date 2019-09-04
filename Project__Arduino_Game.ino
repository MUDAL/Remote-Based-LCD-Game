/**
 * \Description
 * An interactive LCD game
 * This program allows you to control a custom character 
 * on the LCD and avoid moving obstacles.
 * 
 * \Date
 * 03 - 09 - 2019
 * 
 * \Author
 * Raji Olaoluwa
 *
 * \Libraries used
 * IRremote.h by Ken Shirriff
 * LiquidCrystal.h
 * 
 * \Hardware
 * 16 x 2 LCD module
 * Ks0088 Infrared Wireless Remote
 * 10k Potentiometer
 * TL1838-Infrared Receiver
 * 
**/
#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include <LiquidCrystal.h>

enum constants {
  MAX_X = 15,
  MIN_X = 0,
  MAX_Y = 1,
  MIN_Y = 0,
  START_ENEMY1 = 4,
  START_ENEMY2 = 15
};

LiquidCrystal lcd(3,4,5,6,7,8);
const int RECV_PIN = A0;
const int BUZZ_PIN = A1;
/**Define IR receiver object and results object**/
IRrecv irrecv(RECV_PIN);
decode_results results;

/**Creating the custom characters**/
byte frowny[8] = {
  B00000,
  B10001,
  B00000,
  B00000,
  B00000,
  B01110,
  B10001,
  B00000,
};

byte smiley[8] = {
  B00000,
  B10001,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000,
  B00000,
};

byte enemy[8] = {
  B11011,
  B11011,
  B11011,
  B11111,
  B11111,
  B11011,
  B01110,
  B00100,
};

int flag = 0;//allows OK button of remote to toggle betweeen 5 and 7

struct GameCharacter {
  int mood = 5; //mood of custom character(5 for frowny and 7 for smiley)
  int x_pos = 0; //For horizontal movement
  int y_pos = 0; //For vertical movement
} Player, Enemy1, Enemy2;

/**Functions for character movement**/
void moveRight(void){
  (Player.x_pos)++;
  if (Player.x_pos > MAX_X){
    Player.x_pos = MAX_X;
  }
}

void moveLeft(void){
  (Player.x_pos)--;
  if (Player.x_pos < MIN_X){
    Player.x_pos = MIN_X;
  }
}

void moveDown(void){
  (Player.y_pos)++;
  if (Player.y_pos > MAX_Y){
    Player.y_pos = MAX_Y;
  }
}

void moveUp(void){
  (Player.y_pos)--;
  if (Player.y_pos < MIN_Y){
    Player.y_pos = MIN_Y;
  }
}
/**Functions for moving enemy**/
void enemyRight(void){
  lcd.setCursor((Enemy1.x_pos) + START_ENEMY1 ,MIN_Y);
  lcd.write(6);
  if (Enemy1.x_pos == MAX_X - START_ENEMY1){
   Enemy1.x_pos = MIN_X - 1;
  }
}

void enemyLeft(void){
  lcd.setCursor((Enemy2.x_pos) + START_ENEMY2 ,MAX_Y);
  lcd.write(6);
  if (Enemy2.x_pos == MIN_X - START_ENEMY2){
   Enemy2.x_pos = MIN_X;
  }
}

/*Game over function*/
void GameOver(void){
  digitalWrite(BUZZ_PIN,HIGH);
  delay(500);
  digitalWrite(BUZZ_PIN,LOW);
  lcd.print("Game Over");
  delay(1500);
  lcd.clear();
  Enemy1.x_pos = 0;
  Enemy2.x_pos = 0;
}

/*Function for changing player's mood*/
void ChangeMood(void){
  if (!flag) Player.mood = 7;
  if (flag) Player.mood = 5;
  if (flag > 1) flag = 0;
}

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);
  Serial.begin(9600);
  lcd.createChar(5,frowny);
  lcd.createChar(6,enemy);
  lcd.createChar(7,smiley);
  irrecv.enableIRIn();//Enable IR receiver
  pinMode(BUZZ_PIN,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (irrecv.decode(&results)){
    lcd.setCursor(Player.x_pos , Player.y_pos);
    switch(results.value){
      case 0xFF629D:
        moveUp();
        break;
      case 0xFF22DD:
        moveLeft();
        break;
      case 0xFFA857:
        moveDown();
        break;
      case 0xFFC23D:
        moveRight();
        break;
      case 0xFF02FD:
        flag++;
        break;
    }
    irrecv.resume();
  }
  
  ChangeMood();
  
  lcd.setCursor(Player.x_pos,Player.y_pos);
  lcd.write(Player.mood);
  enemyRight();
  enemyLeft();
  (Enemy1.x_pos)++;
  (Enemy2.x_pos)--;
  delay(200);//Affects Game's difficulty(enemy's speed)
  lcd.clear();

  //Game Over
  if ((Player.y_pos == MIN_Y) && Player.x_pos - Enemy1.x_pos == START_ENEMY1 - 1){
    GameOver();
  }
  else if ((Player.y_pos == MAX_Y) && Player.x_pos - Enemy2.x_pos == START_ENEMY2 + 1){
    GameOver();
  }  
}
