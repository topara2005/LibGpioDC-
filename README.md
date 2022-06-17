# LibGpioDC-
An example for LibGpiod in c++ using threads for listening to events on gpio 26 using a raspberry PI 4.
 Listens for falling edge events. A Pullup resistor must be connected to 3.3v on PIN 26. A led with a resistor on PIN 13 of the PI
 
  Also, I added boost libraries for making http request when the event happens. I will include an example in the near future.
how to build:
g++ -std=c++1z -lgpiodcxx -lpthread -D_GLIBCXX_USE_NANOSLEEP -lboost_system  -c /home/pi/interrupts/main.cpp -o /home/pi/interrupts/main.o
g++  -o /home/pi/interrupts/Main /home/pi/interrupts/main.o   -lgpiodcxx -lpthread -lboost_system
