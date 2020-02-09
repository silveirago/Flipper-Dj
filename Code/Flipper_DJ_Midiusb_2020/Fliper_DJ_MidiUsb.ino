#include <Control_Surface.h>

USBMIDI_Interface midi;
// Instantiate a multiplexer
CD74HC4067 mux = {
  A3,              // analog pin
  {2, 3, 4, 5}, // Address pins S0, S1, S2, S3

};
// Create an array of potentiometers that send out MIDI Control Change messages 
// when you turn the potentiometers connected to the 16 input pins of the mux.
CCPotentiometer volumePotentiometers[] = {
// Start 10 Potentiomters
  { mux.pin(0), { MIDI_CC::Channel_Volume, CHANNEL_1 } },
  { mux.pin(1), { MIDI_CC::Channel_Volume, CHANNEL_2 } },
  { mux.pin(2), { MIDI_CC::Channel_Volume, CHANNEL_3 } },
  { mux.pin(3), { MIDI_CC::Channel_Volume, CHANNEL_4 } },
  { mux.pin(4), { MIDI_CC::Channel_Volume, CHANNEL_5 } },
  { mux.pin(5), { MIDI_CC::Channel_Volume, CHANNEL_6 } },
  { mux.pin(6), { MIDI_CC::Channel_Volume, CHANNEL_7 } },
  { mux.pin(7), { MIDI_CC::Channel_Volume, CHANNEL_8 } },
  { mux.pin(8), { MIDI_CC::Channel_Volume, CHANNEL_9 } },
  { mux.pin(9), { MIDI_CC::Channel_Volume, CHANNEL_10 } },
// End 10 Potentiometers
};
NoteButton button[]{  
// Start Button row 3
  { mux.pin(10), {47, CHANNEL_1 } },
  
// Start Buttons row 4 
  { mux.pin(11), {48, CHANNEL_1 } },
  { mux.pin(12), {49, CHANNEL_1 } },
  { mux.pin(13), {50, CHANNEL_1 } },
  { mux.pin(14), {51, CHANNEL_1 } },
// End Buttons row 4
  
//  { mux.pin(15), { MIDI_CC::Channel_Volume, CHANNEL_11 } },//  12 Potentiometers eq
};

using namespace MIDI_Notes;
///Row 1
NoteButton button1 = {6, {36, CHANNEL_1},}; // Pin 6 Note C2 on MIDI channel 1
NoteButton button2 = {7, {37, CHANNEL_1},};
NoteButton button3 = {8, {38, CHANNEL_1},};
NoteButton button4 = {9, {39, CHANNEL_1},};
///Row 2
NoteButton button5 = {10, {40, CHANNEL_1},}; // Pin 10 Note ... on MIDI channel 1
NoteButton button6 = {14, {41, CHANNEL_1},};
NoteButton button7 = {15, {42, CHANNEL_1},};
NoteButton button8 = {16, {43, CHANNEL_1},};
///Row 3
NoteButton button9 = {A0, {44, CHANNEL_1},}; // Pin A0 Note ... on MIDI channel 1
NoteButton button10 = {A1, {45, CHANNEL_1},};
NoteButton button11 = {A2, {46, CHANNEL_1},};
// This button pinout on mux
// Method 2 matrix buttons
/*
// The note numbers corresponding to the buttons in the matrix
const AddressMatrix<4, 4> addresses = {{
  {48, 49, 50, 51},// Ableton drumrack map
  {44, 45, 46, 47},
  {40, 41, 42, 43},
  {36, 37, 38, 39},
}};
NoteButtonMatrix<4, 4> buttonmatrix = {
  {2, 3, 4, 5}, // row pins
  {6, 7, 8, 9},    // column pins
  addresses,    // address matrix
  CHANNEL_0,    // channel and cable number
};
*/
void setup() {
  Control_Surface.begin();
}

void loop() {
  Control_Surface.loop();
}
