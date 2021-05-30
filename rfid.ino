#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>

#define RST_PIN     9           
#define SS_PIN      10   
#define GREEN       4           
#define RED         3 
#define YELLOW      2  

 void ReaData(byte* uid, MFRC522 rfid){
   for (byte i = 0; i < rfid.uid.size; i++) {
    uid[i] = rfid.uid.uidByte[i];
    Serial.print(uid[i]);
    Serial.print (" ");
  }
 }

  bool isKey(byte* uid,byte s){
    for(int i=0;i<s;i++){
      byte n = EEPROM.read(i+2);
      if(n!=uid[i])return false;
    }
    return true;
  }

   bool SearchData(byte* uid, byte s){
    byte size = EEPROM.read(0);
    byte n;
    byte count = 0;
    byte i;
    for(i=1; i<=size; i++){
      for (int j=0; j<s;j++){
        n = EEPROM.read(2+i*s+j);
        if (n!=uid[j]){
          count = 0;
          break;
          } else count++;
        }
        if(count==s)return true;
      }
      if(i>size) return false;
    }

   void RemoveData(byte* uid, byte s){
     byte size = EEPROM.read(0);
     bool f=0;
    byte n;
    byte count = 0;
    for(byte i=1; i<=size; i++){
      for (int j=0; j<s;j++){
        n = EEPROM.read(2+i*s+j);
        if (n!=uid[j]){count = 0;break;} else count++;
       }
        if(count==s){
          for(int k=2+i*s;k<=2+s*size; k++){
            byte n = EEPROM.read(k+s);
            EEPROM.write(k,n);
            }
            f=1;
            size--;
            EEPROM.write(0,size);
          }
          if (f) return;
      }
 }

   void RecorData(byte* uid, byte s){
    byte size = EEPROM.read(0);
    for(byte i = 0; i<s; i++){
      EEPROM.write(2+s*size+i, uid[i]);
      }
      size++;
      EEPROM.write(0,size);
    }

   void led(bool f){
      if(f){
        digitalWrite(GREEN, HIGH);
        delay(1000);                      
        digitalWrite(GREEN, LOW);   
      }
      else {   
        digitalWrite(RED, HIGH);
        delay(1000);                      
        digitalWrite(RED, LOW);  
      }
    }

    
bool prev = 0;
MFRC522 rfid (SS_PIN, RST_PIN); 

void setup() {
  Serial.begin(9600);
  SPI.begin(); 
  rfid.PCD_Init();
  if(EEPROM.read(1) == 255){
      EEPROM.write(0,0);
      EEPROM.write(1,0);
  }
  pinMode (RED,OUTPUT);
  pinMode (GREEN,OUTPUT);
  pinMode (YELLOW,OUTPUT);

}

void loop() {
 byte uid[4];
 if (rfid.PICC_IsNewCardPresent()){
     if ( rfid.PICC_ReadCardSerial()) { 
           ReaData(uid,rfid);
           bool n = EEPROM.read(1);
           if(!n){ 
                  EEPROM.write(1,1);
                  prev = 1;
                  digitalWrite(YELLOW, LOW);
                  RecorData(uid,rfid.uid.size);
           
             }
           else {
                   if(isKey(uid,rfid.uid.size)){
                         if (prev == 0){
                               prev = 1;
                               digitalWrite(YELLOW, HIGH);
                          } 
                         else {
                                prev = 0; 
                                digitalWrite(YELLOW, LOW);}
                          }
                  else { 
                     if (!prev){
                           bool f = SearchData(uid,rfid.uid.size);
                           led(f);
                      }
                     else if(prev){
                           bool f = SearchData(uid,rfid.uid.size);
                           if(f)RemoveData(uid,rfid.uid.size);
                           else RecorData(uid,rfid.uid.size );
                           led(!f);
                  } 
           }        
      }
 }
 }
 delay(300);
}
