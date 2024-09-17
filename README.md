# Fractal Music Generator

This project implements a fractal music generation system using the `MidiGen` class. The system recursively generates a musical sequence similar to the process of creating fractal images (like the Koch Snowflake). It then writes the generated sequence to a MIDI file, playable in most media players.

## Features

- **Fractal Music Generation**: Generates music using a recursive approach, creating complex musical patterns by subdividing and modifying sequences.
- **MIDI File Creation**: Outputs a standard MIDI file (`FractalMusic.mid`), which can be played on any MIDI-compatible media player.
- **Variable Instrumentation**: Generates music with two different instruments (Piano on Track 1 and Violin on Track 2).
- **Tempo and Time Signature Control**: Inserts tempo and time signature meta-events into the MIDI file for a structured musical piece.
- **Customizable**: Adjust depth, pitch, duration, tempo, and other parameters to explore different fractal musical patterns.

## File Structure

- `MidiGen.h`: Header file for the `MidiGen` class. Defines methods and variables for generating and exporting MIDI files.
- `MidiGen.cpp`: Implementation file for the `MidiGen` class. Contains the logic for fractal music generation and MIDI file writing.

## How It Works

1. **Fractal Generation**: The `GenerateFractalMidi` method recursively generates a sequence of MIDI notes, modifying the pitch and duration at each recursive step. The algorithm follows a Koch-like approach, splitting notes into smaller parts.
2. **MIDI File Creation**: The `F_ExportMidi` method writes the generated fractal music to a standard MIDI file. It also inserts tempo and time signature events and supports multiple instruments.
3. **Instrumentation**: By default, the system uses Piano (channel 0) and Violin (channel 1) for its two tracks. You can modify the instruments by changing the program change events in the `F_ExportMidi` method.

## Getting Started

### Prerequisites

- A C++ compiler (e.g., GCC, MSVC).
- Basic knowledge of C++ and MIDI file format.
- A media player capable of playing MIDI files (e.g., VLC, Windows Media Player).

### Building the Project

1. Clone the repository to your local machine:
    ```bash
    git clone https://github.com/your-username/fractal-music-generator.git
    ```
2. Navigate to the project directory:
    ```bash
    cd fractal-music-generator
    ```
3. Compile the project:
    ```bash
    g++ MidiGen.cpp -o fractal_music_generator
    ```
4. Run the executable:
    ```bash
    ./fractal_music_generator
    ```
5. A MIDI file named `FractalMusic.mid` will be generated in the project directory.

### Usage

The main parameters for the fractal music generation are defined in the `MidiGen.cpp` file:

- **Fractal Depth**: Controls the complexity of the generated music. Modify the depth parameter in the `GenerateMidi()` method:
    ```cpp
    GenerateFractalMidi(4, 60, mPPQN);
    ```
    Adjust the first argument to change the depth of recursion.

- **Base Pitch**: Sets the starting pitch of the sequence. The default is C3 (MIDI number 60). Change this in the `GenerateMidi()` method.

- **Duration**: Sets the duration of the notes. Adjust the third argument in `GenerateFractalMidi()`.

- **Tempo**: Modify the `mTempo` variable in the `MidiGen` constructor to change the tempo (in BPM).

## Customization

- **Instrument Change**: The `F_ExportMidi` method sets the instruments for each track using the Program Change message. You can change the instrument by modifying these lines:
    ```cpp
    mTrack[mN++] = 0xC0;  // Channel 0 (Piano)
    mTrack[mN++] = 0x00;  // Instrument number (Piano)

    mTrack[mN++] = 0xC1;  // Channel 1 (Violin)
    mTrack[mN++] = 0x4A;  // Instrument number (Violin)
    ```
    Refer to the General MIDI instrument list to choose different instruments.

## Known Issues and Limitations

- **Atonality**: The fractal music generated may sound atonal due to unrestricted pitch modifications. You can implement scale filtering within the `GenerateFractalMidi` method to introduce more musicality.
- **Duration Handling**: The current implementation limits note durations to prevent excessively short notes. You can adjust this behavior in the base case of the `GenerateFractalMidi` method.

## Further Enhancements

- Implement scale or key restrictions to constrain pitch modifications and make the output sound more musical.
- Introduce more rhythmic variety by adding randomization or using a transformation table for note durations.

## License

This project is licensed under the MIT License. See the `LICENSE` file for more information.

## Acknowledgments

- MIDI file handling inspired by various open-source MIDI libraries.
- Project template provided in `A3/MidiGen` for generating fractal music.
