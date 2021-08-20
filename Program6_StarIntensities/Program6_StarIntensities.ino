// Pius Wong, 4/28/2016
// personal project
// This program is supposed to have lights softly falling down row by row
// Modified from "White on White" program.  Arrays "row" and "col" may not be
// intuitively defined, because the original "White on White" grid program
// may have swapped rows and columns.  Here row[] describes the grounded wires
// going more or less vertically, while [col] has the powered wires running
// more or less horizontally. 
// ===========================================================================
boolean debug = false;  // Turning this to "true" turns on the Serial monitor

// Set the LED variables
// LED pins for power rows, top to bottom
int row[] = {13, 12, 11, 10, 9, 8};
const int sizeRow = 6;
// LED pins for ground columns, left to right
int col[] = {7, 6, 5};
const int sizeCol = 3;
// LED pins for light sensor
int sensorPins[] = {A1, A2, A3};  // 0v, signal, 5v

// counters
int count = 0;   // initializing brightness array
int count2 = 0;  // initializing brightness array
int count3 = 0;  // multiplexing counter for PWM
int count4 = 0;  // time segment for PWM
int count5 = 0;  // tracking rows in PWM
int count5b = 0; // tracking cols in PWM
int count6 = 0;

// timing
// 21 ms PWM cycle time seems to work; 1/21 duty cycle still smooth
long cycletime = 1000; //us; 500 OK for 3 columns
int timesegments = 100;  // should divide evenly into cycletime.  Makes timestep divisible by # of cols, too?
                         // This sets smallest PWM increment, and hence smallest brightness increment
int timestep = cycletime/timesegments; // = cycletime/timesegments..
                                  // Can be directly defined because of variable type issues
unsigned long timenow;
int timeshift;
int maxcycletime = 20; // seconds

// brightness matrix variables
int brightness[sizeRow][sizeCol];
int ledPeriod[sizeRow][sizeCol];
int ledShift[sizeRow][sizeCol];
int minBrightness = .1*timesegments;
int ledMinBrightness[sizeRow][sizeCol];

// sensing
boolean trigger = false;


void setup() 
{
  
  // Initialize and turn off all LEDs.
  for (int count = 0; count < sizeCol; count++){
    pinMode(col[count], OUTPUT);
    digitalWrite(col[count], LOW);
  }
  for (int count = 0; count < sizeRow; count++){
    pinMode(row[count], OUTPUT);
    digitalWrite(row[count], LOW);
  }

  randomSeed(analogRead(A0));
  if (debug==true){
    Serial.begin(115200);
  }
  
  // Initialize brightness array.  It is ZERO-INDEXED!
  for (count=0; count < sizeRow; count++){
    for (count2=0; count2 < sizeCol; count2++){
      brightness[count][count2] = 0;
    }
  }

  // Set LED Min Brightness array.  ***This is specific to the constellation.***
  for (count=0; count < sizeRow; count++){
    for (count2=0; count2 < sizeCol; count2++){
      ledMinBrightness[count][count2] = minBrightness;
      // Regulus (3,2) brightest
      if (count==3 && count2==2){
        ledMinBrightness[count][count2] = .7*timesegments;
      }
      // Denebola (0,0) 
      if (count==0 && count2==0){
        ledMinBrightness[count][count2] = .6*timesegments;
      }
      // Algieba (3,1)
      if (count==3 && count2==1){
        ledMinBrightness[count][count2] = .5*timesegments;
      }      
      // Zosma (1,0) and Algenubi (1,2)
      if ( count==1 && count2==0 ){
        ledMinBrightness[count][count2] = .4*timesegments;
      }
      if ( count==1 && count2==2 ){
        ledMinBrightness[count][count2] = .4*timesegments;
      }
    }
  }
  
  // Initialize light sensor pins
  pinMode(sensorPins[0], OUTPUT);
  pinMode(sensorPins[2], OUTPUT);
  digitalWrite(sensorPins[0], LOW);  // 0v
  digitalWrite(sensorPins[2], HIGH); // 5v
  
}


void loop() 
{

  // Sense environment
  int ambientLight = analogRead(sensorPins[1]); // 0 = darkest, 1023 = brightest
//  if (debug==true){
//    Serial.print("Ambient Light = ");
//    Serial.println(ambientLight);    //debug
//  }
  // Shorten the period when brighter ambient light.
  int periodMax = map(ambientLight, 0, 1023, maxcycletime, 1);  // last arguments set output range
//  if (debug==true){
//    Serial.print("PeriodMax = ");
//    Serial.println(periodMax);  // debug
//  }
//  // Increase minimum brightness when brighter out
//  minBrightness = 0.1*timesegments + 0.8*timesegments*map(ambientLight, 0, 1023, 0, 100)/100;
//  if (debug==true){
//    Serial.print("Min Brightness = ");
//    Serial.println(minBrightness);  // debug
//  }
    
  // set timing MAXIMUM, over all LEDs. Bias toward longer period when darker ambient light.
  int period = random(0.5*periodMax, periodMax); // seconds
//  if (debug==true){
//    Serial.print("Period = ");
//    Serial.println(period);  // debug
//  }
  unsigned long timecyclestart = millis();
//  if (debug==true){
//    Serial.print("Time Now = ");
//    Serial.println(timenow);  // debug
//  }
  boolean loopdone = false;

  // Set brightness matrix parameters according to max time. Each LED is individual.
  for(count6=0; count6<sizeRow; count6++){
    for(count3=0; count3<sizeCol; count3++){
      // Set periods for each LED.  
      ledPeriod[count6][count3] = random(100*period, 1000*period); // milliseconds
      // Set time shifts for each LED.
      ledShift[count6][count3] = random(0, 1000*period - ledPeriod[count6][count3]); // milliseconds
    }
  }
  
  // Let LEDs shine through one full cycle
  while(loopdone == false)
  {

    // Get time
    timenow = millis();
    // .print("timenow = "); Serial.println(timenow);  // debug

    // Find time passed.  Correct it if time resets.
    unsigned long timePassed = timenow - timecyclestart;
    if (timePassed < 0){
      timePassed = timenow + 4294967295;  // timenow + max value of unsigned long
    }

    // Set brightness matrix according to brightness matrix parameters above while() loop. 
    for(count6 = 0; count6<sizeRow; count6++)  // go row by row
    {
      for(count3 = 0; count3<sizeCol; count3++) // go col by col
      {
        // If the time window is right...
        if( timePassed <= (ledPeriod[count6][count3] + ledShift[count6][count3]) && // time < end of ledPeriod
            timePassed >= ledShift[count6][count3] ) // time > start of period
        {
          // ...set brightness according to a sinusoidal equation.  "timesegments" corresponds to brightness increments, too.
          float coeff = 2*3.14159*1000/ledPeriod[count6][count3];
          brightness[count6][count3] =  round( (timesegments-ledMinBrightness[count6][count3])*0.5*(
                                        1 - cos( coeff*(timePassed-ledShift[count6][count3])/1000 )) 
                                        + ledMinBrightness[count6][count3]);       
                                      // the conversion to an int is necessary or else memory problems occur w/ array          
          if (count3 == 2){ // Code added 4/30/16 as a correction for Column #3 (rightmost), which flickers when brightness = max...
            brightness[count6][count3] = brightness[count6][count3]-1;
          }
          // Correction if for whatever reason calculated brightness is lower than min allowed
          if (brightness[count6][count3] < ledMinBrightness[count6][count3]){
            brightness[count6][count3] = ledMinBrightness[count6][count3];
          }
        }
        else{  // Otherwise just set it to minimum.
          brightness[count6][count3] = ledMinBrightness[count6][count3];
        }
      }
    }
//    //DEBUG:
//    brightness[1][2] = 98;
//    brightness[2][2] = 99;
//    brightness[3][2] = 100;
//    brightness[4][2] = 101;
//    Serial.print(brightness[1][2]); Serial.print(" ");
//    //DEBUG^^ 
    
    // Execute PWM column by column.  Do a set of time segements for each column, then recheck brightness.
    for (count5b=0; count5b < sizeCol; count5b++) // check col by col
    {
      int colOfInterest = count5b; // This is the column to adjust.
      // Prepare a column for activation; turn off the others.
      for (count3=0; count3 < sizeCol; count3++){
        if (count3 == colOfInterest){
          digitalWrite(col[count3], LOW);  // Ground the column of interest.
        }
        else if (count3 != colOfInterest){
          digitalWrite(col[count3], HIGH);  // Reverse-bias the column of interest.
        }
      }

      // Start one PWM cycle.  Every column get a full number of timesegements. 
      // Default is LEDs are off. Turn on/off as needed for PWM. 
      for (count4=0; count4 < timesegments; count4++)  // for 0 to (max-1)...
      {      
 
        for (count5=0; count5 < sizeRow; count5++) // check row by row
        {
          int rowOfInterest = count5;  // This is the row to adjust.
          // Turn on LEDs at start of cycle if needed (if brightness > 0)
          if ( (count4 == 0) && (brightness[rowOfInterest][colOfInterest] > 0) ){
            digitalWrite(row[rowOfInterest], HIGH);
          }
          else if ( (count4 == 0) && brightness[rowOfInterest][colOfInterest] == 0 ){
            digitalWrite(row[rowOfInterest], LOW);
          }
          // If current time segment matches the intended brightness level...
          else if (count4 == brightness[rowOfInterest][colOfInterest]){
            // ...turn LED in that row off.
            digitalWrite(row[rowOfInterest], LOW);
          }
        }        
        // Leave some time for the LEDs to do their thing for one column.
        delayMicroseconds(timestep);
        // Serial.println(micros()); // debug
      }
      
    }
      
    // Check if the cycle is done
    if( timePassed > (period*1000) )
    {
      loopdone = true;
    }

  }
}
