# freecell
This is a FreeCell solitaire card game for MS-DOS written in C.

## Prerequisites

Windows 9x (95, 98, ME) and 32-bit NT (2000, XP, Vista, 7, 8, 8.1, 10) operating systems will support most MS-DOS applications natively. For users with modern systems, I highly recommend using [DOSBox](https://www.dosbox.com/download.php?main=1).

Additionally, you will need [Borland Turbo C 2.01](https://archive.org/details/msdos_borland_turbo_c_2.01), which is a C compiler for MS-DOS.

## Installation

_The following instructions assume a fresh install of DOSBox._

1. Create an enclosing directory (folder) for Turbo C and this repository. Give it the name `root`, or something similar. The directory structure should look like this:

    ```
    root
    ├── freecell
    └── TC
        ├── INCLUDE
        │   └── SYS
        └── LIB
    ```

2. Start DOSBox and enter the following commands:

    ```bat
    MOUNT C your/path/to/root
    SET PATH=%PATH%;C:\TC\
    C:
    CD C:\FREECELL\
    MAKE
    FREECELL.EXE
    ```

## Controls

To move cards around, press the keys indicated on the top row of the screen.

* q: quit
* n: new game
