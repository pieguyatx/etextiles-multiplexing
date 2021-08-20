// Pius Wong, 4/28/2016
// personal project
// This program is supposed to have lights softly falling down row by row
// Modified from "White on White" program.  Arrays "row" and "col" may not be
// intuitively defined, because the original "White on White" grid program
// may have swapped rows and columns.  Here row[] describes the grounded wires
// going more or less vertically, while [col] has the powered wires running
// more or less horizontally. 
boolean debug = false;

// Set the LED variables
// LED pins for columns, facing front, left to right
int row[] = {5, 6, 7};
const int sizeRow = 3;
// LED pins for rows, facing front, top to bottom
int col[] = {13, 12, 11, 10, 9, 8};
const int sizeCol = 6;
// LED pins for light sensor
int sensorPins[] = {A1, A2, A3};  // 0v, signal, 5v

// counters
int count = 0;
int count2 = 0;
int count3 = 0;
int count4 = 0;
int count5 = 0;
int count6 = 0;

// timing
// 21 ms PWM cycle time seems to work; 1/21 duty cycle still smooth
int cycletime = 8000; //15000;  15000 was good for 4 columns; 8000 seems to be OK for 3 columns
int timesegments = 100;  // should divide evenly into cycletime
int timestep = cycletime/timesegments; // = cycletime/timesegments..
                                  // directly defined because of variable type issues
int timeon;
int timeoff;
int timenow;
int timeshift;
int timeshiftstart;

// brightness
int brightness[sizeRow][sizeCol];

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

  // Turn on LEDs
  for (count = 0; count < sizeCol; count++){
    digitalWrite(col[count], HIGH);
  }

  randomSeed(analogRead(A0));
  Serial.begin(9600);

  // Initialize brightness array.  It is ZERO-INDEXED!
  for (count=0; count < sizeRow; count++){
    for (count2=0; count2 < sizeRow; count2++){
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
  Serial.println(ambientLight);    //debug
  int periodMax = map(ambientLight, 0, 1023, 8, 0);
    
  // set timing
  int period = random(1, periodMax); // seconds
  // Serial.println(period);  // debug
  int timecyclestart = millis();
  timeshiftstart = timecyclestart;
  int timeshift = random(50,1000); // milliseconds
  timeshift = timeshift*random(70,100)/100;  // skew distribution hack, instead of uniform distribution
  // Serial.println(timenow);  // debug
  boolean loopdone = false;

  // Set brightness coefficient
  float coeff = 2*3.14159/period;
  
  // Let LEDs shine through one full cycle
  while(loopdone == false)
  {

    // Get time
    timenow = millis();
    // Serial.print("timenow = "); Serial.println(timenow);  // debug

    // Set brightness according to time & phase shift
    for(count6 = 0; count6<sizeRow; count6++)  // go row by row
    {
      // If the time window is right...
      if( timenow - timecyclestart <= (period*1000 + count6*timeshift) ) // time < end of period
      {
        if( timenow - timecyclestart >= (count6*timeshift) ) // time > start of period
        {
          // ...set brightness according to the equation.
          brightness[count6][0] = round(timesegments*0.5*(
              1 - cos( coeff*(timenow-timecyclestart-count6*timeshift)/1000 )));                 
        }
      }
      else{  // Otherwise just set it to zero.
        brightness[count6][0] = 0;
      }
    }

//    // Copy brightness from Column 1 to all columns.
//    for (count2=1; count2 < sizeCol+1; count2++)
//    {
//      brightness[0][count2] = brightness[0][0];
//    }
    
    /*
    // Fix any memory errors; clean up brightness values
    for(count6 = 0; count6<sizeRow; count6++)
    {  
      if( brightness[count6][0] > timesegments )
      {
        Serial.print("weird data at end!!! Row# "); Serial.println(count6); // debug
        brightness[count6][0] = 0;
      }
    }
    */
    /*
    // debug lines below
    Serial.print("brightness = "); Serial.print(brightness[0][0]); Serial.print(" ");
    Serial.print(brightness[1][0]); Serial.print(" ");
    Serial.print(brightness[2][0]); Serial.print(" ");
    Serial.print(brightness[3][0]); Serial.print(" ");
    Serial.println(brightness[4][0]);
    // debug lines above
    */
    
    // Start one PWM cycle.
    // Default is LEDs are on. Turn off as needed for PWM. 
    for (count4=0; count4 < timesegments; count4++)  // for 0 to (max-1)...
    {
      // As time segments pass in PWM, turn LEDs off.
      for (count5=0; count5 < sizeRow+1; count5++) // check row by row
      {
        int rowOfInterest = count5;  // This is the row to adjust.
        // Serial.print(count4); Serial.print(" "); Serial.println(brightness[rowOfInterest][0]); // debug
        // If current time segment matches the intended brightness level...
        if (count4 == brightness[rowOfInterest][0]){
          // ...turn LEDs off.
          digitalWrite(row[rowOfInterest], HIGH);
          // as long as it's not the last time segment of the PWM cycle...
        }
      }
      delayMicroseconds(timestep);
      // Serial.println(micros()); // debug
    }
     
    // Turn on LEDs after one PWM cycle
    for (count = 0; count < sizeRow; count++){
      digitalWrite(row[count], LOW);
    }     
      
    // Check if the cycle is done
    if( timenow - timecyclestart > (period*1000 + timeshift*(sizeRow-1)) )
    {
      loopdone = true;
    }

  }
}
