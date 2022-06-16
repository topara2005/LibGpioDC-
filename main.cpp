
#include <gpiod.hpp>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <string.h>
 #include <mutex>

using namespace std::literals::chrono_literals;
std::mutex _mutex;
 const std::string chipname = "gpiochip0";
 gpiod::chip chip;
 gpiod::line ln_led;
 gpiod::line ln_pushButton;
 const int led_pin =  13;
 const int pushbutton_pin =  26;
 static bool finished=false;


void initChipAndLines(){
    try{
        //It initializes chip 0
        chip = gpiod::chip(chipname, gpiod::chip::OPEN_BY_NAME);
        std::cout<<"chip opened.."<<::std::endl;
        //it gets the line for the LED
        ln_led=chip.get_line(led_pin);
        ln_led.request({"GPIO13", gpiod::line_request::DIRECTION_OUTPUT, 0},0);
         std::cout<<"Led's line aquired.."<<::std::endl;
         //it gets the line for the Push Button
        ln_pushButton=chip.get_line(pushbutton_pin);
        // Listen for falling edge events. A Pullup resistor must be connected to 3.3v
        ln_pushButton.request({"GPIO26", gpiod::line_request::EVENT_FALLING_EDGE, 0},0);
        std::cout<<"Push button line aquired and ready for falling edge events.."<<std::endl;
    } catch(std::exception& e){
        std::cout<<"An exception has occured: "<<e.what()<<::std::endl;
    }

}
void eventHandler(){
    auto res=1;
    std::cout<<"Entering thread.."<<std::endl;
    while(!finished){
        std::lock_guard<std::mutex> lock(_mutex);
        //waits for an event
        auto irq=ln_pushButton.event_wait(::std::chrono::seconds(1));
        if(irq){
          std::cout<<"Event read.."<<std::endl;
          auto event=ln_pushButton.event_read();
          if (event.event_type == ::gpiod::line_event::FALLING_EDGE)
            std::cout << " FALLING EDGE";
            std::cout << std::chrono::duration_cast<::std::chrono::seconds>(event.timestamp).count();
            std::cout << ".";
            std::cout << event.timestamp.count() % 1000000000;
            std::cout << " line: " << event.source.offset();
            std::cout << std::endl;
            //it blinks the led twice
            ln_led.set_value(1);
            std::this_thread::sleep_for(0.5s);
            ln_led.set_value(0);
            std::this_thread::sleep_for(0.5s);
            ln_led.set_value(1);
            std::this_thread::sleep_for(0.5s);
            ln_led.set_value(0);
            res=!res;
        }
   }
}
int main(int argc, char **argv)
{

    initChipAndLines();

    std::cout<<"Starting waiting for event.."<<std::endl;
     std::thread worker(eventHandler);
       worker.join();
      std::cin.get();

      finished=true;

	std::cout<<"exit..";
    return EXIT_SUCCESS;
}
