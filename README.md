# freecell
This is a FreeCell solitaire card game for MS-DOS written in C.

## Prerequisites

This project requires a working version of MS-DOS. Unless you have a system that is old enough to support MS-DOS natively, I highly recommend using [DOSBox](https://www.dosbox.com/download.php?main=1).


Additionally, you will need a C compiler for MS-DOS. Use [Borland Turbo C 2.01](http://www.doshaven.eu/programming-tools/).

## Installation

1. Create a directory which contains Turbo C and this repository, and call it `root` or something like that. The directory structure of `root` should look like this:

    ```
    ├── TC
    │   ├── INCLUDE
    │   │   └── SYS
    │   └── LIB
    └── freecell
    ```

2. The next step is to configure DOSBox's startup script, which appears as a read-only file from inside DOSBox at the location `Z:\AUTOEXEC.BAT`. See [this resource](https://www.dosbox.com/wiki/Dosbox.conf) for help on modifying `autoexec`. Below is the sort of configuration you should use.

    ```
    [autoexec]
    # Lines in this section will be run at startup.
    # You can put your MOUNT lines here.

    MOUNT C root
    SET PATH=%PATH%;C:\TC\
    C:
    ```
## Building

Launch DOSBox and enter the following commands:

```bat
cd freecell
make
```

## Running

To run, simply run `freecell.exe`. For the time being, the controls are 'Q' to quit and 'N' to start a new game.