// Pius Wong, 4/28/2016
// personal project
// This program is supposed to have lights softly falling down row by row
// Modified from "White on White" program.  Arrays "row" and "col" may not be
// intuitively defined, because the original "White on White" grid program
// may have swapped rows and columns.  Here row[] describes the grounded wires
// going more or less vertically, while [col] has the powered wires running
// more or less horizontally. 
// ===========================================================================
boolean debug = true;

// Set the LED variables
// LED pins for power rows, top to bottom
int row[] = {13, 12, 11, 10, 9, 8};
const int sizeRow = 6;
// LED pins for ground columns, left to right
int col[] = {5, 6, 7};
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
long cycletime = 600; //us; 500 OK for 3 columns
int timesegments = 50;  // should divide evenly into cycletime.  Makes timestep divisible by # of cols, too?
                         // This sets smallest PWM increment, and hence smallest brightness increment
int timestep = cycletime/timesegments; // = cycletime/timesegments..
                                  // Can be directly defined because of variable type issues
unsigned long timenow;
int timeshift;
int maxcycletime = 10;

// brightness matrix variables
int brightness[sizeRow][sizeCol];
float ledPeriod[sizeRow][sizeCol];
int ledShift[sizeRow][sizeCol];
int minBrightness = .05*timesegments;

// sensing
boolean trigger = false;


void setup() {
  
  // Initialize and turn off all LEDs.
  for (int count = 0; count < sizeCol; count++){
    pinMode(col[count], OUTPUT);
    digitalWrite(col[count], LOW);
  }
  for (int count = 0; count < sizeRow; count++){
    pinMode(row[count], OUTPUT);
    digitalWrite(row[count], LOW);
  }

//  // Turn on LEDs
//  for (count = 0; count < sizeCol; count++){
//    digitalWrite(row[count], HIGH);
//  }

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

  // Initialize light sensor pins
  pinMode(sensorPins[0], OUTPUT);
  pinMode(sensorPins[2], OUTPUT);
  digitalWrite(sensorPins[0], LOW);  // 0v
  digitalWrite(sensorPins[2], HIGH); // 5v
  
}


void loop() {

  // Sense environment
  int ambientLight = analogRead(sensorPins[1]); // 0 = darkest, 1023 = brightest
  if (debug==true){
    Serial.print("Ambient Light = ");
    Serial.println(ambientLight);    //debug
  }
  int periodMax = map(ambientLight, 0, 1023, maxcycletime, 0);  // last arguments set output range
  if (debug==true){
    Serial.print("PeriodMax = ");
    Serial.println(periodMax);  // debug
  }
    
  // set timing MAXIMUM, over all LEDs. Bias toward longer period when darker ambient light.
  int period = random(0.5*periodMax, periodMax); // seconds
  if (debug==true){
    Serial.print("Period = ");
    Serial.println(period);  // debug
  }
  unsigned long timecyclestart = millis();
//  timeshiftstart = timecyclestart;
//  int timeshift = random(50,1000); // milliseconds
//  timeshift = timeshift*random(70,100)/100;  // skew distribution hack, instead of uniform distribution
  if (debug==true){
    Serial.print("Time Now = ");
    Serial.println(timenow);  // debug
  }
  boolean loopdone = false;

  // Set brightness matrix parameters according to max time. Each LED is individual.
  for(count6=0; count6<sizeRow; count6++){
    for(count3=0; count3<sizeCol; count3++){
      // Set periods for each LED.  
      ledPeriod[count6][count3] = (float) random(0, 1000*period)/1000; // seconds
      // Set time shifts for each LED.
      ledShift[count6][count3] = random(0, 1000*(period - ledPeriod[count6][count3])); // milliseconds
      if (debug==true){
        Serial.print(ledShift[count6][count3]); Serial.print(" "); // debug
      }
    }
    Serial.println("");
  }
  if (debug==true){
    Serial.println(" "); // debug
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
        if( timePassed <= (ledPeriod[count6][count3]*1000 + ledShift[count6][count3]) && // time < end of ledPeriod
            timePassed >= ledShift[count6][count3] ) // time > start of period
        {
          // ...set brightness according to a sinusoidal equation.  "timesegments" corresponds to brightness increments, too.
          float coeff = 2*3.14159/ledPeriod[count6][count3];
          brightness[count6][count3] = (int) round( (timesegments-minBrightness)*0.5*(
                                        1 - cos( coeff*(timePassed-ledShift[count6][count3])/1000 )) 
                                        + minBrightness);       
                                      // the conversion to an int is necessary or else memory problems occur w/ array          
          if (brightness[count6][count3] < minBrightness){
            brightness[count6][count3] = minBrightness;
          }
        }
        else{  // Otherwise just set it to minimum.
          brightness[count6][count3] = minBrightness;
        }
        // DEBUG===============
        if (count3==0){
          if (count6==0){
            brightness[count6][count3] = 10;
          }
          if (count6==1){
            brightness[count6][count3] = 2;
          }
          if (count6==2){
            brightness[count6][count3] = 30;
          }
          if (count6==3){
            brightness[count6][count3] = 20;
          }
          if (count6==4){
            brightness[count6][count3] = 10;
          }
          if (count6==5){
            brightness[count6][count3] = 2;
          }
        }
        else if (count3==1){
          if (count6==0){
            brightness[count6][count3] = 30;
          }
          if (count6==1){
            brightness[count6][count3] = 40;
          }
          if (count6==2){
            brightness[count6][count3] = 30;
          }
          if (count6==3){
            brightness[count6][count3] = 20;
          }
          if (count6==4){
            brightness[count6][count3] = 10;
          }
          if (count6==5){
            brightness[count6][count3] = 2;
          }
        }
        else if (count3==2){
          if (count6==0){
            brightness[count6][count3] = 49;
          }
          if (count6==1){
            brightness[count6][count3] = 50;
          }
          if (count6==2){
            brightness[count6][count3] = 30;
          }
          if (count6==3){
            brightness[count6][count3] = 20;
          }
          if (count6==4){
            brightness[count6][count3] = 10;
          }
          if (count6==5){
            brightness[count6][count3] = 2;
          }
        }        
        // DEBUG=================
//        if (debug==true){
//          Serial.print(brightness[count6][count3]); Serial.print(" ");// debug
//        }
      }
//      if (debug==true){
//        Serial.println("");
//      }
    }
//    if (debug==true){
//      Serial.println("");
//    }
    
    // Start one PWM cycle.
    // Default is LEDs are off. Turn on/off as needed for PWM. 
    for (count4=0; count4 < timesegments; count4++)  // for 0 to (max-1)...
    {      
      // As time segments pass in PWM, turn LEDs on and off as needed.
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

        for (count5=0; count5 < sizeRow; count5++) // check row by row
        {
          int rowOfInterest = count5;  // This is the row to adjust.
          // Turn on LEDs at start of cycle if needed (if brightness > 0)
          if ( (brightness[rowOfInterest][colOfInterest] > 0) && (count4 < brightness[rowOfInterest][colOfInterest])){
            digitalWrite(row[rowOfInterest], HIGH);
          }
          else if (brightness[rowOfInterest][colOfInterest] == 0 ){
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
    if( timenow - timecyclestart > (period*1000) )
    {
      loopdone = true;
    }

  }
}
