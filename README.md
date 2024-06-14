# Design_Pattern_Drawing_Project

To first compile the code from the command line (I am on windows specifically), do:

g++ ECGraphicViewImp.cpp ECMyObserver.cpp ECGraphicDocument.cpp ECShape.cpp ECCommand.cpp test-gv.cpp -lallegro -lallegro_font -lallegro_primitives -lallegro_image -lallegro_main

Then to run I ran the files through Visual Studio 2022 and it should work fine, I made sure in the settings my default file was tmp.dat and whenever I wanted to make a blank canvas I just made sure the file was completely empty.

Please make sure to include the test-gv.cpp file as that is how I make sure all the files are working with one another and the program actually works.
