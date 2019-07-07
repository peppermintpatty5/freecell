@ECHO ==Executing lazy "Makefile"==
@DEL *.exe
@TCC -c *.c
@TCC -efreecell.exe *.obj
