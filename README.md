# SIO2IDE for ATARI

## Project package (25.03.2002)

---

Package contents (see detailed filelist below):

Root files:

- \License.TXT - license for use and distribution
- \Config.MAK - please edit this file to adjust this project to your environment, note that project uses the
  following utilities
- Compilers:
  - for PC software
    - Borland C 3.0 or 3.1 (must be updated)
  - for Atari software
    - CC65 2.6.1 or higher
  - for AVR software
    - IAR A90 1.40 or higher (command line only)
    - or
    - GNU AVR 3.0.2 or higher
  - Make utility
    - Opus make 6.11 for DOS

## Utilities

- \UTILS\HEX2BIN - Hex to Bin converter
- \UTILS\AVRISP - AVR ISP programmer software

## SIO2IDE hardware

\HARDWARE - interface hardware sources for all versions

## Versions

### Software version 1.6

- \VER.160 - interface software version 1.60 for AT90S8515
  and related utilities for Atari

### Software version 3.0

- \VER.300 - interface software version 3.00 for ATmega161
  and related utilities (for Atari and PC)

### Software version 4.0

- \VER.400 - interface software version 4.00 for ATmega161
  and related utilities (for Atari and PC)

## NOTES

- All software components are compiled (package includes all object, list,
  map and other non source files). To clean up please use 'make clean'
  command for each source directory.
- To rebuild selected application please use 'make all' command.
- If you want to clean or rebuild selected application please first
  set your environment paths in the Config.MAK file.

## License

Unless stated otherwise, the source code of this projects is
licensed under the Apache License, Version 2.0 (the "License");
you may not use any of the licensed files within this project
except in compliance with the License.

The full document can be found in the [LICENSE][1] file.

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

[1]: https://github.com/MM-Soft-Atari/sio2ide/blob/master/LICENSE
