# LibGpioDC
An example for LibGpiod in c++ using threads for listening to events on gpio 26 using a raspberry PI 4.
 Listens for falling edge events. A Pullup resistor must be connected to 3.3v on PIN 26. A led with a resistor on PIN 13 of the PI.
  Also, It uses pahocpp mqtt library for sending messages to the broker broker.mqttdashboard.com, using web sockets.
  Two threads are used: one for listening to incoming messages to topics and another for listenings to the events.
 
  Also, I added boost libraries for making http request when the event happens. I will include an example in the near future.
  
  PAHO for c++:   
    https://github.com/eclipse/paho.mqtt.cpp
    
how to build:

g++ -std=c++1z -lgpiodcxx -lpthread -D_GLIBCXX_USE_NANOSLEEP -lboost_system -lpaho-mqttpp3 -lpaho-mqtt3as -lcrypto -lssl  -c /home/pi/interrupts/Main.cpp -o /home/pi/interrupts/Main.o

g++  -o /home/pi/interrupts/Main /home/pi/interrupts/Main.o   -lgpiodcxx -lpthread -lboost_system -lpaho-mqttpp3 -lpaho-mqtt3as
