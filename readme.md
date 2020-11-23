# Pascallite Program
This program will catch all the syntax, sematic, lexical error in pascal file & convert from pascal file to assembly file\
This program is used for **educational purpose only**.

Take Home Project from class CS 4301 - Angelo State University - Professor Mark Molt

## **Special Function**
*Print out all the value in table entry, saved in ASM file*\
*Catch all types off error in pascal file*


![PascaliteReadMe](https://user-images.githubusercontent.com/37564253/99896231-44518400-2c54-11eb-9e39-f842c7ab1c97.gif)




### **Software require**: *[Visual Studio Community 2019](https://visualstudio.microsoft.com/vs/community/)* (free version) and *[Visual Studio Code](https://code.visualstudio.com/download)*

## **How to set up in Visual Studio Community 2019**

#### **1**.Go to Project Property -> Linker -> System -> change the SubSystem setting into Console(/SUBSYSTEM:CONSOLE) (since we are using main(int argc, char** argv)).
##### ⋅⋅**1.1** Go to Project Property -> Configuration Properties 
###### ⋅⋅⋅⋅**1.1.1** Go VC++ Directories -> Include Directories section: add a new path to the folder that contain header file *"stage1.h"*
###### ⋅⋅⋅⋅**1.1.2** Go to Debugging -> change the command arguments into 101.dat 101.lst 101.asm.
##### ⋅⋅**1.2** Go to Project Property -> Configuration Properties -> C/C++ Preprocessor Definitions -> add "_CRT_SECURE_NO_WARNINGS" to avoid unsafe warning -- ctime function
Change your 101.dat (raw pascal file) in the same folder that contain header file.\
After run on Visual Studio Community 2019, new converted files will be saved on **101.lst** and **101.asm** file \
*Note: Use Visual Studio Code to read .lst and .asm file*

**Author**: [Anh Minh Le](https://www.linkedin.com/in/minh-anh-le-20b85419a/)\
**Partner**: [Long Thanh Le](https://www.facebook.com/coldlwownwg/)
