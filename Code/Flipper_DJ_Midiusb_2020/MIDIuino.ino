// 16 buttons
// 1 rotary encoder select track
// 2 rotary encoders loop
// 8 EQ Potentiometer     \
// 2 Potentiometers Filter \  14 Potentiometers
// 2 faders                 \
// 1 crossfader              \
// 12 leds
// led per buton cue, play
#include <Encoder.h> // Include the Encoder library.
#include <Control_Surface.h>// Include the Control Surface library

USBMIDI_Interface midi;

SPIShiftRegisterOut<16> sreg = {SS, MSBFIRST};

// Instantiate a VULEDs object with 12 LEDs.
MCU::VULEDs<12> vu = {
  sreg.pins().slice<0, 11>(), // first 12 pins of the shift registers
  1,                          // track number [1, 8]
  MCU::VUDecay::Default,      // how long does it take for the meter to decay
};



// 8 potentiometer to analog pinout
// Deck 1
CCPotentiometer potentiometer1(A0, {MIDI_CC::Channel_Volume, CHANNEL_2});

CCPotentiometer potentiometer2(A1, {MIDI_CC::Channel_Volume, CHANNEL_3});

CCPotentiometer potentiometer3(A2, {MIDI_CC::Channel_Volume, CHANNEL_4});

CCPotentiometer potentiometer4(A3, {MIDI_CC::Channel_Volume, CHANNEL_5});

// Deck 2
 
CCPotentiometer potentiometer5(A4, {MIDI_CC::Channel_Volume, CHANNEL_6});

CCPotentiometer potentiometer6(A5, {MIDI_CC::Channel_Volume, CHANNEL_7});

CCPotentiometer potentiometer7(A6, {MIDI_CC::Channel_Volume, CHANNEL_8});

CCPotentiometer potentiometer8(A7, {MIDI_CC::Channel_Volume, CHANNEL_9});

// 2 faders 

CCPotentiometer fader1(A6, {MIDI_CC::Channel_Volume, CHANNEL_10});

CCPotentiometer fader2(A7, {MIDI_CC::Channel_Volume, CHANNEL_11});

// 2 filters

CCPotentiometer filter1(A6, {MIDI_CC::Channel_Volume, CHANNEL_12});

CCPotentiometer filter2(A7, {MIDI_CC::Channel_Volume, CHANNEL_13});

// 1 crossfader

CCPotentiometer scrossfader1(A9, {MIDI_CC::Channel_Volume, CHANNEL_11});

// Instantiate a CCRotaryEncoder object

CCRotaryEncoder encBrowser = {
  {14, 15},       // pins
  MCU::V_POT_1, // MIDI address (CC number + optional channel)
  1,            // optional multiplier if the control isn't fast enough
};

CCRotaryEncoder encL = {
  {16, 17},       // pins
  MCU::V_POT_1, // MIDI address (CC number + optional channel)
  1,            // optional multiplier if the control isn't fast enough
};

CCRotaryEncoder encR = {
  {18, 19},       // pins
  MCU::V_POT_1, // MIDI address (CC number + optional channel)
  1,            // optional multiplier if the control isn't fast enough
};

// The note numbers corresponding to the buttons in the matrix

const AddressMatrix<4, 4> addresses = {{
  {51, 50, 49, 48},// 16 buttons
  {47, 46, 45, 44},
  {43, 42, 41, 40},
  {39, 38, 37, 36},
}};

NoteButtonMatrix<4, 4> buttonmatrix = {
  {2, 3, 4, 5}, // row pins
  {6, 7, 8, 9},    // column pins
  addresses,    // address matrix
  CHANNEL_1,    // channel and cable number
};

Button dotBarModeButton = A10;

void setup() {
  dotBarModeButton.begin();
  Control_Surface.begin(); // Initialize Control Surface
}

void loop() {
  Control_Surface.loop(); // Update the Control Surface
  if (dotBarModeButton.update() == Button::Falling) // when button is pressed
  vu.toggleMode(); // toggle between dot and bar mode
}

