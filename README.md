# BMX2WAV

BMX2WAV is a Windows program that converts BMS files to WAV or OGG files.

## Features

- Supports a wide range of standard BMS descriptions
- #RANDOM syntax
- 00 - ZZ extended definitions
- Extended BPM
- STOP sequence
- Long notes
- OGG input/output support
- Volume adjustment (normalization) function
- BMX2WAV Searcher for finding the BMS files you want to convert
- Batch conversion functionality
- Allows collaboration with other programs (e.g., MP3 conversion) after conversion
- Equipped with extension features using Squirrel scripting
- Localization support, currently only supporting Japanese, more language options will be implemented in the future.

## Directory Structure

```
├─ttl
├─tt_squirrel
│ └─squirrel3 (http://www.squirrel-lang.org/)
└─bmx2wav
└─ogg
├─libogg(https://xiph.org/)
└─libvorbis(https://xiph.org/)
```

## Usage

Run bmx2wav.exe and register BMS files by dragging and dropping them into the list. Select the registered BMS and press the conversion button to convert. The converted files will be saved in WAV or OGG format.

To change the language, use the implemented language selection function in the application.

## Localization Instructions

Translations are welcomed, to create a new language DLL, you will need the following tools:

- Microsoft Visual Studio
- Resource Compiler (rc.exe)
- Linker (link.exe)

And the following files:

- lang/japanese.rc
- string_table_id.h

Execute the following commands:

```
rc.exe japanese.rc
link.exe /DLL /NOENTRY /MACHINE:X64 japanese.res
```

Replace "japanese.rc" and "japanese.res" with the appropriate file names for the language you are adding.

## Links

- [BMX2WAV Official Website](http://childs.squares.net/program/bmx2wav/index.html)

- [Hatena DevBlog](https://bmx2wav.hatenadiary.jp/)

## Contributing

If you find any issues or have suggestions for improvement, please open an issue or submit a pull request.