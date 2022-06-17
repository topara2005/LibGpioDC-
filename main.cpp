
#include <gpiod.hpp>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <string.h>
 #include <mutex>
 #include <cstring>
  #include <atomic>
  #include <iostream>
#include <cctype>
#include <memory>
 #include "mqtt/async_client.h"


using namespace std::literals::chrono_literals;
using namespace std;
std::mutex _mutex;
 const std::string chipname = "gpiochip0";
 gpiod::chip chip;
 gpiod::line ln_led;
 gpiod::line ln_pushButton;
 const int led_pin =  13;
 const int pushbutton_pin =  26;
  static bool finished=false;


 const string DFLT_SERVER_ADDRESS	{ "ws://broker.hivemq.com:8000" };
const string CLIENT_ID				{ "paho_cpp_async_publish" };
const string PERSIST_DIR			{ "./persist" };




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
void eventHandler(mqtt::async_client_ptr cli){
    auto res=1;
    std::cout<<"Entering thread.."<<std::endl;
    while(!finished){
        std::lock_guard<std::mutex> lock(_mutex);
        auto irq=ln_pushButton.event_wait(::std::chrono::seconds(1));
        if(irq){
            std::cout<<"Event read.."<<std::endl;
          auto event=ln_pushButton.event_read();
         // ln_pushButton.reset();
          if (event.event_type == ::gpiod::line_event::FALLING_EDGE)
            std::cout << " FALLING EDGE";
            std::cout << std::chrono::duration_cast<::std::chrono::seconds>(event.timestamp).count();
            std::cout << ".";
            std::cout << event.timestamp.count() % 1000000000;
            std::cout << " line: " << event.source.offset();
            std::cout << std::endl;
            ln_led.set_value(1);
             std::this_thread::sleep_for(0.5s);
             ln_led.set_value(0);
             std::this_thread::sleep_for(0.5s);
              ln_led.set_value(1);
             std::this_thread::sleep_for(0.5s);
              ln_led.set_value(0);
               std::this_thread::sleep_for(0.5s);
            cli->publish("event_ebc", "clicked !!!")->wait();
             std::cout << "Message sent! "<< std::endl;

            res=!res;


        }
   }
}
int main(int argc, char **argv)
{

    initChipAndLines();

    // Create an MQTT client using a smart pointer to be shared among threads.
	auto cli = std::make_shared<mqtt::async_client>(DFLT_SERVER_ADDRESS, CLIENT_ID);
    // Connect options for a persistent session and automatic reconnects.
	auto connOpts = mqtt::connect_options_builder()
		.clean_session(false)
		.automatic_reconnect( std::chrono::seconds(2),  std::chrono::seconds(30))
		.finalize();
    auto TOPICS = mqtt::string_collection::create({ "data/#", "command" });
	const vector<int> QOS { 0, 1 };

	std::cout << "  ...OK" << endl;
     std::thread worker(eventHandler, cli);

	try {

        // Start consuming _before_ connecting, because we could get a flood
		// of stored messages as soon as the connection completes since
		// we're using a persistent (non-clean) session with the broker.
		cli->start_consuming();
        std::cout << "Connecting to the MQTT server at " << DFLT_SERVER_ADDRESS << "..." << flush;
		auto rsp = cli->connect(connOpts)->get_connect_response();
		std::cout << "OK\n" << endl;

		// Subscribe if this is a new session with the server
		if (!rsp.is_session_present())
			cli->subscribe(TOPICS, QOS);


        std::cout<<"Starting  listening for events thread.."<<std::endl;


         // Consume messages in this thread
		while (true) {
			auto msg = cli->consume_message();
			if (!msg)
				continue;

			if (msg->get_topic() == "command" &&
					msg->to_string() == "exit") {
				std::cout << "Exit command received" << endl;
				break;
			}

			//std::cout << msg->get_topic() << ": " << msg->to_string() << endl;

		}

        // Close the counter and wait for the publisher thread to complete
		cout << "\nShutting down..." << flush;



		// Disconnect

		cout << "OK\nDisconnecting..." << flush;
		cli->disconnect();
		cout << "OK" << endl;


	}catch (const mqtt::exception& exc) {
		cerr << exc.what() << endl;
		return 1;
	}

    finished=true;
     worker.join();

	std::cout<<"exit..";
    return EXIT_SUCCESS;
}
