#include "MidiGen.h"

// Constructor
MidiGen::MidiGen(void) {
    mTrack = new UCHAR[TRACK_BUFFER_SIZE];
    mPPQN = 480;                  // Pulses Per Quarter Note
    mFileName = (char*)"FractalMusic.mid";  // MIDI file name
    mTempo = 120.f;               // Tempo in BPM
    mChannel = 0;
}

// Destructor
MidiGen::~MidiGen(void) {
    delete[] mTrack;
}

// Core function that calls the fractal generator
void MidiGen::GenerateMidi() {
    mN = 0;
    // Call the fractal generator with a depth of 4, starting pitch of C3 (MIDI 60)
    // and initial note duration of a quarter note.
    GenerateFractalMidi(4, 60, mPPQN);
}

// Fractal generator using a Koch-like approach
void MidiGen::GenerateFractalMidi(int depth, UCHAR basePitch, UINT duration) {
    if (depth == 0 || duration < (mPPQN / 8)) { // Ensure minimum duration (1/8th of a quarter note)
        // Base case: add the note with current pitch and duration
        InsertNote(0, basePitch, 100);      // Start note with velocity 100
        InsertNote(duration, basePitch, 0); // End note after 'duration'
        return;
    }

    // Recursive case: split the note into three parts (like Koch snowflake does)
    GenerateFractalMidi(depth - 1, basePitch, duration/1.3 );
    GenerateFractalMidi(depth - 1, basePitch + 7, duration / 1.3);
    GenerateFractalMidi(depth - 1, basePitch - 7, duration / 1.3);
    GenerateFractalMidi(depth - 1, basePitch, duration / 1.3);
}


// Function to insert variable length data (time between events)
void MidiGen::InsertVarLengthData(UINT value) {
    UINT buffer = value & 0x7F;
    while ((value >>= 7)) {
        buffer <<= 8;
        buffer |= ((value & 0x7F) | 0x80);
    }
    while (true) {
        mTrack[mN++] = buffer;
        if (buffer & 0x80) buffer >>= 8; else break;
    }
}

// Function to insert a MIDI note event (Note On/Off)
void MidiGen::InsertNote(UINT deltaTime, UCHAR pitch, UCHAR vel) {
    InsertVarLengthData(deltaTime);
    mTrack[mN++] = UCHAR(0x80 + (vel > 0) * 0x10 + mChannel);  // Note On/Off
    mTrack[mN++] = pitch;   // Pitch
    mTrack[mN++] = vel;     // Velocity
}

// Insert tempo information
void MidiGen::Insert_MicroTempo(int microTempo) {
    mTrack[mN++] = 0xFF;
    mTrack[mN++] = 0x51;
    mTrack[mN++] = 0x03;
    mTrack[mN++] = microTempo >> 16 & 0xFF;
    mTrack[mN++] = microTempo >> 8 & 0xFF;
    mTrack[mN++] = microTempo & 0xFF;
}

// Insert time signature
void MidiGen::Insert_TimeSignature(UCHAR num, UCHAR den) {
    mTrack[mN++] = 0xFF;
    mTrack[mN++] = 0x58;
    mTrack[mN++] = 0x04;
    mTrack[mN++] = num;  // Numerator
    mTrack[mN++] = den;  // Denominator
    mTrack[mN++] = 0x18;
    mTrack[mN++] = 0x08;
}

// Export MIDI file with two different instruments
void MidiGen::F_ExportMidi() {
    mOutFile.open(mFileName);
    if (!mOutFile.is_open()) {
        std::cout << "File Open Error\n";
        return;
    }

    F_WriteFileHeader(2);    // Number of tracks = 2

    // Write Track 1 (Tempo + Instrument Track)
    mN = 0;  // Reset track data index
    int microTempo = int(500000.0f / mTempo * 120.0f);

    // Insert tempo and time signature
    InsertVarLengthData(0);
    Insert_MicroTempo(microTempo);
    InsertVarLengthData(0);
    Insert_TimeSignature(4, 2);   // Time signature 4/4

    // Program Change for Track 1 (Piano, channel 0)
    InsertVarLengthData(0);
    mTrack[mN++] = 0xC0;  // Program Change for channel 0 (Track 1)
    mTrack[mN++] = 0x00;  // Piano (program number 1, but 0-based)

    // Set channel for Track 1
    mChannel = 0;
    GenerateMidi();  // Generate notes for Track 1 (Piano)
    InsertVarLengthData(0);
    mTrack[mN++] = 0xFF;  // End of Track meta event
    mTrack[mN++] = 0x2F;
    mTrack[mN++] = 0x00;
    F_WriteTrack();  // Write Track 1

    // Write Track 2 (Note Track + Different Instrument)
    mN = 0;  // Reset track data index for Track 2

    // Program Change for Track 2 (Violin, channel 1)
    InsertVarLengthData(0);
    mTrack[mN++] = 0xC1;  // Program Change for channel 1 (Track 2)
    mTrack[mN++] = 0x4A;  // Violin (program number 74, but 0-based)

    // Set channel for Track 2
    mChannel = 1;
    GenerateMidi();  // Generate notes for Track 2 (Violin)
    InsertVarLengthData(0);
    mTrack[mN++] = 0xFF;  // End of Track meta event
    mTrack[mN++] = 0x2F;
    mTrack[mN++] = 0x00;
    F_WriteTrack();  // Write Track 2

    mOutFile.close();
}



// Write file header
void MidiGen::F_WriteFileHeader(USHORT numTracks) {
    mOutFile.write("MThd", 4);
    UCHAR length[] = { 0, 0, 0, 6 };
    mOutFile.write((const char*)length, 4);
    UCHAR format[] = { 0, 1 };
    mOutFile.write((const char*)format, 2);
    numTracks = FlipEndian(numTracks);
    mOutFile.write((const char*)&numTracks, 2);
    USHORT division = FlipEndian(mPPQN);
    mOutFile.write((const char*)&division, 2);
}

// Write a MIDI track
void MidiGen::F_WriteTrack() {
    mOutFile.write("MTrk", 4);
    UINT length = FlipEndian((UINT)mN + 3);
    mOutFile.write((const char*)&length, 4);
    mOutFile.write((const char*)mTrack, mN);
    const UCHAR END_OF_TRACK[] = { 0xFF, 0x2F, 0x00 };
    mOutFile.write((const char*)END_OF_TRACK, 3);
}

// Flip the byte order (endianness)
USHORT MidiGen::FlipEndian(USHORT x) {
    return x << 8 | x >> 8;
}

UINT MidiGen::FlipEndian(UINT x) {
    return x << 24 | (x & 0xFF00) << 8 | (x >> 8 & 0xFF00) | x >> 24;
}
