/* DIY MIDI Controller

Esta sketch permite transformar um Arduino Micro, Pro Micro, ou Leonardo (nao testado),
em um controlador MIDI class compliante. Ou seja, nao ha necessidade de nenhum conversor serial,
como o Hairless Midi.

/// Licensa ///

Este codigo esta licensisado sob forma de "pagamento de cerveja" (se possivel) e 
"dar os devidos creditos" (sempre).

Feito por Gustavo Silveira, 2017.

http://www.musiconerd.com
http://www.bitcontrollers.com
gustavosilveira@musiconerd.com

*/
 
#include <Multiplexer4067.h> // Multiplexer CD4067 library >> https://github.com/sumotoy/Multiplexer4067
#include <Thread.h> // Threads library >> https://github.com/ivanseidel/ArduinoThread
#include <ThreadController.h> // Mesma lib de cima
//#include "digitalIOPerformance.h" 

/////////////////////////////////////////////
// buttons
const byte muxNButtons = 5; // *coloque aqui o numero de entradas digitais utilizadas no multiplexer
const byte NButtons = 11; // *coloque aqui o numero de entradas digitais utilizadas
const byte totalButtons = muxNButtons + NButtons;
const byte muxButtonPin[muxNButtons] = {12, 13, 14, 15, 11}; // *neste array coloque na ordem desejada os pinos das portas digitais utilizadas
const byte buttonPin[NButtons] = {18, 19, 20, 10, 16, 14, 15, 6, 7, 8, 9}; // *neste array coloque na ordem desejada os pinos das portas digitais utilizadas
int buttonCState[totalButtons] = {0}; // estado atual da porta digital
int buttonPState[totalButtons] = {0}; // estado previo da porta digital

/////////////////////////////////////////////
// debounce
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 5;    // the debounce time; increase if the output flickers

/////////////////////////////////////////////
// potentiometers
const byte NPots = 10; // *coloque aqui o numero de entradas analogicas utilizadas
const byte muxPotPin[NPots] = {8, 9, 10, 1, 0, 3, 2, 5, 4, 6}; // *neste array coloque na ordem desejada os pinos das portas analogicas, ou mux channel, utilizadas
int potCState[NPots] = {0}; // estado atual da porta analogica
int potPState[NPots] = {0}; // estado previo da porta analogica
int potVar = 0; // variacao entre o valor do estado previo e o atual da porta analogica
int lastCcValue[NPots] = {0};

/////////////////////////////////////////////
// pot reading
int TIMEOUT = 50; //quantidade de tempo em que o potenciometro sera lido apos ultrapassar o varThreshold
byte varThreshold = 4; //threshold para a variacao no sinal do potenciometro
boolean potMoving = true; // se o potenciometro esta se movendo
unsigned long pTime[NPots] = {0}; // tempo armazenado anteriormente
unsigned long timer[NPots] = {0}; // armazena o tempo que passou desde que o timer foi zerado

/////////////////////////////////////////////
// midi
byte midiCh = 0; // *Canal midi a ser utilizado
byte note = 36; // *Nota mais grave que sera utilizada
byte cc = 1; // *CC mais baixo que sera utilizado

/////////////////////////////////////////////
// Multiplexer
Multiplexer4067 mplex = Multiplexer4067(2, 3, 4, 5, A3);

/////////////////////////////////////////////
// threads - programa cada atividade do Arduino para acontecer em um determinado tempo
ThreadController cpu; //thread master, onde as outras vao ser adicionadas
Thread threadReadPots; // thread para controlar os pots

/////////////////////////////////////////////

void setup() {

  mplex.begin(); // inicializa o multiplexer

  for (int i = 0; i < NButtons; i++) { // inicializa os botoes como input utilizando o pull up resistor
    pinMode(buttonPin[i], INPUT_PULLUP);
  }

  pinMode(A3, INPUT_PULLUP);

  /////////////////////////////////////////////
  // threads
  threadReadPots.setInterval(10);
  threadReadPots.onRun(readPots);
  cpu.add(&threadReadPots);
  /////////////////////////////////////////////

}

void loop() {

  cpu.run();
  readButtons();

}


/////////////////////////////////////////////
// read buttons

void readButtons() {

  for (int i = 0; i < muxNButtons; i++) { //reads buttons on mux
    int buttonReading = mplex.readChannel(muxButtonPin[i]);
    //buttonCState[i] = map(mplex.readChannel(muxButtonPin[i]), 22, 1023, 0, 2); // stores on buttonCState
    if (buttonReading > 1000) {
      buttonCState[i] = HIGH;
    }
    else {
      buttonCState[i] = LOW;
    }
    //Serial.print(buttonCState[i]); Serial.print("   ");
  }
  //Serial.println();

  for (int i = 0; i < NButtons; i++) { //read buttons on Arduino
    buttonCState[i + muxNButtons] = digitalRead(buttonPin[i]); // stores in the rest of buttonCState
  }

  for (int i = 0; i < totalButtons; i++) {

    if ((millis() - lastDebounceTime) > debounceDelay) {

      if (buttonCState[i] != buttonPState[i]) {
        lastDebounceTime = millis();

        if (buttonCState[i] == LOW) {
          noteOn(potMidiCh(), note + i, 127);  // Channel 0, middle C, normal velocity
          MIDIUSB.flush();
          //MIDI.sendNoteOn(note + i, 127, potMidiCh()); // envia NoteOn(nota, velocity, canal midi)
          //Serial.print("Note: "); Serial.print(note + i); Serial.println(" On");
          buttonPState[i] = buttonCState[i];
        }
        else {
          noteOn(potMidiCh(), note + i, 0);  // Channel 0, middle C, normal velocity
          MIDIUSB.flush();
          //MIDI.sendNoteOn(note + i, 0, potMidiCh());
          //Serial.print("Note: "); Serial.print(note + i); Serial.println(" Off");
          buttonPState[i] = buttonCState[i];
        }
      }
    }

  }

}


/////////////////////////////////////////////
//read potentiometers

void readPots() {

  for (int i = 0; i < NPots - 1; i++) { // le todas entradas analogicas utilizadas, menos a dedicada a troca do canal midi
    potCState[i] = mplex.readChannel(muxPotPin[i]);
  }

  for (int i = 0; i < NPots; i++) {

    potVar = abs(potCState[i] - potPState[i]); // calcula a variacao da porta analogica

    if (potVar >= varThreshold) {  //sets a threshold for the variance in the pot state, if it varies more than x it sends the cc message
      pTime[i] = millis(); // armazena o tempo previo
    }
    timer[i] = millis() - pTime[i]; // reseta o timer
    if (timer[i] < TIMEOUT) { // se o timer for menor que o tempo maximo permitido significa que o potenciometro ainda esta se movendo
      potMoving = true;
    }
    else {
      potMoving = false;
    }

    if (potMoving == true) { // se o potenciometro ainda esta se movendo, mande o control change
      int ccValue = map(potCState[i], 22, 1022, 0, 127);
      if (lastCcValue[i] != ccValue) {
        controlChange(11, cc + i, ccValue); // manda control change (channel, CC, value)
        MIDIUSB.flush();
        //MIDI.sendControlChange(cc + i, map(potCState[i], 0, 1023, 0, 127), 11); // envia Control Change (numero do CC, valor do CC, canal midi)
        //Serial.print("CC: "); Serial.print(cc + i); Serial.print(" value:"); Serial.println(map(potCState[i], 22, 1023, 0, 127));
        potPState[i] = potCState[i]; // armazena a leitura atual do potenciometro para comparar com a proxima
        lastCcValue[i] = ccValue;
      }
    }
  }

}

/////////////////////////////////////////////
// calculates midi channel based on pot position
int potMidiCh () {
  int potCh =  map(mplex.readChannel(muxPotPin[9]), 22, 1023, 0, 4);

  if (potCh == 4) {
    potCh = 3;
  }
  
  return potCh + midiCh;
}

/////////////////////////////////////////////
// Arduino (pro)micro midi functions
void noteOn(byte channel, byte pitch, byte velocity) {
  MIDIEvent noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MIDIUSB.write(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  MIDIEvent noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MIDIUSB.write(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  MIDIEvent event = {0x0B, 0xB0 | channel, control, value};
  MIDIUSB.write(event);
}
