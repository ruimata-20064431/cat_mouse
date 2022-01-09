#include <EEPROM.h>

/*
* * * * * * * * * * * * * * *
* EEPROM inventory "stampimg"
* v0.0.1 RMata
*/



/*    *******  *******  **   **  *******  *******  *******  **   **    ***
 *    *******  *******  **   **  *******  *******  *******  ***  **    ***
 *    **       **   **  **   **    **        **    **   **  ***  **    ***
 *    **       **   **  **   **    **        **    **   **  ***  **    ***
 *    **       *******  **   **    **        **    **   **  ** ****    ***
 *    **       *******  **   **    **        **    **   **  **  ***   
 *    *******  **   **  *******    **     *******  **   **  **  ***    ***
 *    *******  **   **  *******    **     *******  *******  **   **    ***
 */

struct InventoryStruct{
  char owner[5];
  char yearMonth[4];
  char modelVersion[3];
  char inventoryNum[3];
};

const int   INVENTORY_TAG_SIZE  =  15 ;
      int   eepromSize          =   0 ;                         // EEPROM size
      int   eepromWriteByte     =   0 ;                         // EEPROM byte where inventory tag will be written (start)
      
      char  out[64]                   ;

                     //   1                          1
                     //   0                          0
                     //   0                          2  // example for a 1024 (1Kb) EEPROM
                     //   901234   4567    8901   1234  // bytes where to stamp inventory info
InventoryStruct stamp = {"RMATA", "2112", "N03", "001"};
// Ownership      RMATA
// Year/month     2112
// Model/version  N03 (NANOv03)
// inventory #    000
InventoryStruct currentStamp = {"", "", "", ""};
InventoryStruct newStamp     = {"", "", "", ""};



void setup() {
  Serial.begin(9600);
  
  Serial.println("\n\n\n\n\n");
  Serial.println("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
  
  // get current inventory tag info
  EEPROM.get(1009, currentStamp);
  Serial.print("  Inventory item: ");
  sprintf(out, "%s%s%s%s", currentStamp.owner, currentStamp.yearMonth, currentStamp.modelVersion, currentStamp.inventoryNum);
  out[INVENTORY_TAG_SIZE] = '\0';
  Serial.println(out);

  // display EEPROM size
  eepromSize = EEPROM.length();
  eepromWriteByte = eepromSize - INVENTORY_TAG_SIZE;  // establish start byte to write inventory tag (last bytes of EEPROM)
  sprintf(out,   "  EEPROM length = %d, inventory tag will be writen at byte %d", eepromSize, eepromWriteByte);
  Serial.println(out);
  

  // request user confirmation
  Serial.print("\n  ==>> YOU WILL NOW WRITE TO YOUR EEPROM!! <<==");
  Serial.print("  Do you confirm? (Y, [N]) ");
  
  while (!Serial.available()) {}
  char  confirmation        = '\0' ;
  confirmation = Serial.read();
  Serial.print(confirmation);
  
  if (confirmation == 'Y') {
    
    // print previous tag info
    Serial.print("Previous inventory item = ");
    //EEPROM.get(1009, currentStamp);
    sprintf(out, "%s%s%s%s", currentStamp.owner, currentStamp.yearMonth, currentStamp.modelVersion, currentStamp.inventoryNum);
    out[INVENTORY_TAG_SIZE] = '\0';
    Serial.println(out);




    // write inventory tag to EEPROM
    EEPROM.put(1009, stamp);



    
  } else {
    Serial.println("Canceled");
  }


  // retrieve and display current inventory tag
  EEPROM.get(1009, newStamp);
  Serial.print("  Inventory item: ");
  out[0] = '\0';
  sprintf(out, "%s%s%s%s", newStamp.owner, newStamp.yearMonth, newStamp.modelVersion, newStamp.inventoryNum);
  out[INVENTORY_TAG_SIZE] = '\0';
  Serial.println(out);


  Serial.println("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
}

void loop() {
  // do nothing!!
}
