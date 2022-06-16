# LibGpioDC-
An example for LibGpiod in c++ using threads for listening to events on gpio 26 using a raspberry PI 4.
  Also, I added boost libraries for making http request when the event happens. I will include an expample in the near future.
how to compile:
g++ -std=c++1z -lgpiodcxx -lpthread -D_GLIBCXX_USE_NANOSLEEP -lboost_system  -c /home/pi/interrupts/main.cpp -o /home/pi/interrupts/main.o
g++  -o /home/pi/interrupts/Main /home/pi/interrupts/main.o   -lgpiodcxx -lpthread -lboost_system
