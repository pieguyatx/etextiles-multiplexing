// Pius Wong, 4/27/2016
// Project B: Leo Constellation

boolean debug = false;

// Set the LED variables
// LED pins for power rows, facing front, top to bottom
int row[] = {13, 12, 11, 10, 9, 8};
int sizeRow = 6;
// LED pins for ground columns, facing front, left to right
int col[] = {5, 6, 7};
int sizeCol = 3;

// counters
int count = 0;

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

  // Turn on LEDs (power rows high, ground cols low)
  for (count = 0; count < sizeRow; count++){
    digitalWrite(row[count], HIGH);
  }
//  digitalWrite(8, HIGH);
}

void loop() {

//  // Turn on LEDs
//  for (count = 0; count < sizeCol; count++){
//    digitalWrite(col[count], HIGH);
//  }
//  delay(1000);
//  for (count = 0; count < sizeCol; count++){
//    digitalWrite(col[count], LOW);
//  }
  delay(500);
}
