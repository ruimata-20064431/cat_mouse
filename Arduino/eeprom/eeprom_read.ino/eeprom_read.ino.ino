#include <EEPROM.h>

struct InventoryStruct{
  char owner[5];
  char yearMonth[4];
  char modelVersion[3];
  char inventoryNum[3];
};

InventoryStruct inventoryStamp;

int eepromSize = 1024;

char mc = ' ';
int  mi = 0;
char out[32];
char confirmation = ' ';

void setup() {

  Serial.begin(9600);
  
  Serial.println(    "\n");
  Serial.println(    "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");

  // get current inventory info
  EEPROM.get(1009, inventoryStamp);
  Serial.print("  Inventory item: ");
  sprintf(out, "%s%s%s%s", inventoryStamp.owner, inventoryStamp.yearMonth, inventoryStamp.modelVersion, inventoryStamp.inventoryNum);
  out[15] = '\0';
  Serial.println(out);
  // EEPROM info
  sprintf       (out,"  EEPROM length = %d\n", EEPROM.length());
  Serial.println(out);
  Serial.println(    "Printing EEPROM content: ");
  

  
  /*                255 ff |255 ff |255 ff |255 ff |255 ff |255 ff |255 ff |255 ff |255 ff |255 ff |255 ff |255 ff |255 ff |255 ff |255 ff |255 ff |
                    01234567012345670123456701234567012345670123456701234567012345670123456701234567012345670123456701234567012345670123456701234567 */
  Serial.println("    0                                                                               1");                  
  Serial.print(  "    0         1         2         3         4         5         6         7         8         9         0         1         2         3         4         5         ");

  for (int i = 0; i < eepromSize; i++){
    if ( i % 16 == 0) {
      sprintf(out, "\n%03d ", i/16);
      Serial.print(out);
    }
    mi = EEPROM.read(i);
    mc = mi;
    sprintf(out, "%03d %02x %c  ", mi, mi, mc);
    Serial.print(out);
  }
  Serial.println("\n");
  Serial.println("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
  Serial.println("");

  



}

void loop() {
  // put your main code here, to run repeatedly:

}
