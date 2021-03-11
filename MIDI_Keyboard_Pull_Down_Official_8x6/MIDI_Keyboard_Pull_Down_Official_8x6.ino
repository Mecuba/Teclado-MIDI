// Código original de Evan Kale / Original code by Evan Kale

// El número de filas (rows) y columnas (cols) debe coincidir con
// las que hayas determinado en tu teclado -Mecuba
#define NUM_ROWS 8
#define NUM_COLS 6

#define NOTE_ON_CMD 0x90
#define NOTE_OFF_CMD 0x80
#define NOTE_VELOCITY 127

//MIDI baud rate -Evan Kale
#define SERIAL_RATE 31250

// Pin Definitions -Evan Kale

// Row input pins -Evan Kale

// Puedes modificar estos valores si deseas conectar tu teclado de
// una forma diferente -Mecuba
const int row1Pin = 5;
const int row2Pin = 6;
const int row3Pin = 7;
const int row4Pin = 8;
const int row5Pin = 9;
const int row6Pin = 10;
const int row7Pin = 11;
const int row8Pin = 12;

// 74HC595 pins -Evan Kale

// Estos pines también puede ser modificados, solo asegúrate de
// conectar correctamente el 74HC595 -Mecuba
const int dataPin = 2;
const int latchPin = 3;
const int clockPin = 4;

boolean keyPressed[NUM_ROWS][NUM_COLS];
uint8_t keyToMidiMap[NUM_ROWS][NUM_COLS];

// bitmasks for scanning columns -Evan Kale

// No implementamos el código versión Pull-Up como lo hizo el autor
// original, pero en su repositorio notarás que en esta sección,
// los 0's son intercambiados por 1's y viceversa -Mecuba 
int bits[] =
{ 
  B00000001,
  B00000010,
  B00000100,
  B00001000,
  B00010000,
  B00100000,
  B01000000,
  B10000000
};

void setup()
{
  int note = 25;

// Esta sección también fue modificada respecto al código original:
// en el primer 'for' se iteraba colCtr, pero lo intercambiamos porque
// al parecer conectamos algo mal xD y el código estaba interpretando
// columnas como filas, así que lo más fácil, en vez de cambiar las
// conexiones, fue cambiar esta parte del código. Si tienes un problema
// similar, entonces regrésalo comoe estaba, en el primer 'for' se itera
// colCtr y, en el 'for' anidado, se itera rowCtr. -Mecuba

  for(int rowCtr = 0; rowCtr < NUM_ROWS; ++rowCtr)
  {
    for(int colCtr = 0; colCtr < NUM_COLS; ++colCtr)
    {
      keyPressed[rowCtr][colCtr] = false;
      keyToMidiMap[rowCtr][colCtr] = note;
      note++;
    }
  }

  // setup pins output/input mode -Evan Kale
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);

  pinMode(row1Pin, INPUT);
  pinMode(row2Pin, INPUT);
  pinMode(row3Pin, INPUT);
  pinMode(row4Pin, INPUT);
  pinMode(row5Pin, INPUT);
  pinMode(row6Pin, INPUT);
  pinMode(row7Pin, INPUT);
  pinMode(row8Pin, INPUT);

  Serial.begin(SERIAL_RATE);
}

void loop()
{
  for (int colCtr = 0; colCtr < NUM_COLS; ++colCtr)
  {
    //scan next column -Evan Kale
    scanColumn(colCtr);

    //get row values at this column -Evan Kale
    int rowValue[NUM_ROWS];
    rowValue[0] = digitalRead(row1Pin);
    rowValue[1] = digitalRead(row2Pin);
    rowValue[2] = digitalRead(row3Pin);
    rowValue[3] = digitalRead(row4Pin);
    rowValue[4] = digitalRead(row5Pin);
    rowValue[5] = digitalRead(row6Pin);
    rowValue[6] = digitalRead(row7Pin);
    rowValue[7] = digitalRead(row8Pin);

    // process keys pressed -Evan Kale
    for(int rowCtr=0; rowCtr<NUM_ROWS; ++rowCtr)
    {
      if(rowValue[rowCtr] != 0 && !keyPressed[rowCtr][colCtr])
      {
        keyPressed[rowCtr][colCtr] = true;
        noteOn(rowCtr,colCtr);
      }
    }

    // process keys released-Evan Kale
    for(int rowCtr=0; rowCtr<NUM_ROWS; ++rowCtr)
    {
      if(rowValue[rowCtr] == 0 && keyPressed[rowCtr][colCtr])
      {
        keyPressed[rowCtr][colCtr] = false;
        noteOff(rowCtr,colCtr);
      }
    }
  }
}

void scanColumn(int colNum)
{
  digitalWrite(latchPin, LOW);

  if(0 <= colNum && colNum <= 7)
  {
    shiftOut(dataPin, clockPin, MSBFIRST, B00000000); //right sr -Evan Kale
    shiftOut(dataPin, clockPin, MSBFIRST, bits[colNum]); //left sr -Evan Kale
  }
  else
  {
    shiftOut(dataPin, clockPin, MSBFIRST, bits[colNum-8]); //right sr -Evan Kale
    shiftOut(dataPin, clockPin, MSBFIRST, B00000000); //left sr -Evan Kale
  }
  digitalWrite(latchPin, HIGH);
}

void noteOn(int row, int col)
{
  Serial.write(NOTE_ON_CMD);
  Serial.write(keyToMidiMap[row][col]);
  Serial.write(NOTE_VELOCITY);
}

void noteOff(int row, int col)
{
  Serial.write(NOTE_OFF_CMD);
  Serial.write(keyToMidiMap[row][col]);
  Serial.write(NOTE_VELOCITY);
}

