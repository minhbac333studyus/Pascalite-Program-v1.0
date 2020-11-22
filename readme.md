# Pascallite Program
This program will catch all the syntax, sematic, lexical error in pascal file & convert to assembly file\
This program is used for **educational purpose only**.

Inspired from class CS 4301 - Angelo State University - Professor Mark Molt

## **Special Function**
*Print out all the value in table entry, saved in ASM file*\
*Catch all types off error in pascal file*


[DEMO](https://github.com/minhbac333studyus/Pascalite-Program-v1.0/blob/master/PascaliteReadMe.gif)


### **Software require**: *[Visual Studio Community 2019](https://visualstudio.microsoft.com/vs/community/)* (free version) and *[Visual Studio Code](https://code.visualstudio.com/download)*

## **How to set up in Visual Studio Community 2019**

#### **1**.Go to Project Property -> Linker -> System -> change the SubSystem setting into Console(/SUBSYSTEM:CONSOLE) (since we are using main(int argc, char** argv)).
##### ⋅⋅**1.1** Go to Project Property -> Configuration Properties 
###### ⋅⋅⋅⋅**1.1.1** Go VC++ Directories -> Include Directories section: add a new path to the folder that contain header file *"stage1.h"*
###### ⋅⋅⋅⋅**1.1.2** Go to Debugging -> change the command arguments into my.dat my.lst my.asm.
##### ⋅⋅**1.2** Go to Project Property -> Configuration Properties -> C/C++ Preprocessor Definitions -> add "_CRT_SECURE_NO_WARNINGS" to avoid unsafe warning -- ctime function
Add all of your .dat file  in the same folder that contain header file.\
After run on visual, all of the work will be saved on my.lst and my.asm file \
*Note: Use Visual Studio Code to read .lst and .asm file*

**Author**: Anh Minh Le\
**Partner**: Long Thanh Le
