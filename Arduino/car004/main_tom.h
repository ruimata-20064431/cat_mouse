#ifndef main_tom_h
#define main_tom_h

void setup_tom(){
    // wait 10 seconds (allows time for physical car scenario setup)
    delay(10000);

    // @@@ DETERMINE "silence" baseline
    // ...
}
 
bool execute_tom(){
    // Returns TRUE if the game should end (when Tom captured Jerry).
    // Returns FALSE otherwise
    
    // @@@ Await to recognize a sound disruption (Jerry buzzer noise, hopefully) 
    // while (!disruption) { ... }
    
    // @@@ Calculate direction angle, based on sound localization from microphone samples

    // @@@ From the calculated angle, perform the rotation
    // PROBLEMA: qual a direçao? left(0)? right(1)?
    // rotate_car()
    
    // @@@ 

    // @@@ 

}

#endif
