Pascallite Program
This program will catch all the syntax, sematic, lexical error in pascal file & convert to assembly file
This program is used for educational purpose only.

Inspired from class CS 4301 - Angelo State University - Professor Mark Molt\

**Special Function**\
*Print out all the value in table entry, saved in ASM file*\
*Catch all types off error in pascal file*\

**Software require**: *Visual Studio Community 2019* (free version) and *Visual Studio Code*\

**How to set up in Visual**\

1.Go to Project Property -> Linker -> System -> change the SubSystem setting into Console(/SUBSYSTEM:CONSOLE) (since we are using main(int argc, char** argv)).\
\t1.1 Go to Project Property -> Configuration Properties \
\t1.1.1 Go VC++ Directories -> Include Directories section: add a new path to the folder that contain header file *"stage1.h"*\
\t1.1.2 Go to Debugging -> change the command arguments into my.dat my.lst my.asm.\
\t1.2 Go to Project Property -> Configuration Properties -> C/C++ Preprocessor Definitions -> add "_CRT_SECURE_NO_WARNINGS" to avoid unsafe warning -- ctime function / or you can use ctime_s instead.\


Add all of your .dat file  in the same folder that contain header file.\
After run on visual, all of the work will be saved on my.lst and my.asm file \
*Note: Use Visual Studio Code to read .lst and .asm file*\

**Author**: Anh Minh Le
**Partner**: Long Thanh Le