
#include "Gpio.H"
#include "log.h"

#include <zmq.hpp>
//#include <zmq.h>

#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <thread>
#include <signal.h>
#include <unistd.h>
#include <functional>

#include <poll.h>



using std::string;
using std::vector;
using std::map;

//configuration
//
//this may be read in future versions from some kind
//of configuration file.
//
struct me_config {
	vector<int> gpionums = {};
	unsigned uchan_port = 12345;
	string uchanprfx = "tcp://*:";
} cfg;

void gpiopolllop(vector<Gpio> & gpios, zmq::context_t & ctx){
	zmq::socket_t backnot(ctx, ZMQ_PAIR);
	backnot.connect("inproc://notify");

        vector<struct pollfd> ppack;
	map<int,Gpio*> transfdgpio;
	for (auto &x : gpios){
		ppack.emplace_back();
		ppack.back().fd = x.get_fd();
		transfdgpio[x.get_fd()] = &x;
		ppack.back().events = POLLPRI|POLLERR ;
	}
	while (1){
		poll(&(ppack[0]),ppack.size(),8000);
		L::log << "♥" << std::endl;
		for (auto &i : ppack){
			if(i.revents & (POLLPRI) ){
				Gpio &gpio = *(transfdgpio[i.fd]);
				gpio.read_update();
				L::log << gpio.get_gpionum() << " said " << gpio.get() <<
					std::endl;
			}
		}
		usleep(500);
	}
	return;
}


class Server{

	private:
		zmq::context_t ctx;
	public:
		bool is_running;
	private:
		zmq::socket_t uchan;
		zmq::socket_t notif;

		vector<Gpio> gpios;
		vector<zmq::pollitem_t> pollits;
		map <string,Gpio::valtype> vcache;

		void send_a_key(const string & key){
			auto vit = vcache.find(key);
			if( vit == vcache.end()){
				send_all();
				return;
			}
			string msg = key + ":" + std::to_string((*vit).second);
			L::log << " sending " << msg << "to update chan" << std::endl;
			uchan.send(msg.c_str(),msg.size(),0);
		}

		void send_all(){
			for (auto &x : vcache){
				string msg = x.first + ":" + std::to_string(x.second);
				L::log << "mass_sending " << msg << "to update chan" << std::endl;
				uchan.send(msg.c_str(), msg.size(),0);
			}
		}
	public:
		Server(struct me_config cfg) :
			is_running(true),
			uchan(ctx,ZMQ_XPUB),
			notif(ctx,ZMQ_PAIR)
	{
		for (auto num : cfg.gpionums){
			gpios.emplace_back(num);
		}
		for (auto &i : gpios){
			L::log << i.get_gpionum() << std::endl;
			i.init_stage_2();
		}

		string uchanaddr(cfg.uchanprfx + std::to_string(cfg.uchan_port));
		const int eins = 1;
		uchan.setsockopt(ZMQ_XPUB_VERBOSE, &eins, sizeof(eins));
		uchan.bind(uchanaddr.c_str());
		notif.bind("inproc://notify");
		pollits.emplace_back();
		pollits.back().socket = uchan;
		pollits.back().events = ZMQ_POLLIN;
		pollits.emplace_back();
		pollits.back().socket = notif;
		pollits.back().events = ZMQ_POLLIN;
		std::thread gpiothread(gpiopolllop, std::ref(gpios),std::ref(ctx));
		gpiothread.detach();
	}

	void runloop(){
		//zmq::message_t msg;
		char buf[12];
		zmq::poll(&(pollits[0]), pollits.size(), /* timeout*/ 1000);
		if (pollits[0].revents & ZMQ_POLLIN){
			//new subscriber
			uchan.recv(buf,11,0);
			string key(buf);
			send_a_key(key);
			L::log << L::YELLOW << "New Subscriber for" <<
				L::GREEN << buf << L::NORMAL << std::endl;
		}
		if (pollits[1].revents & ZMQ_POLLIN){
			//new value
			notif.recv(buf,11,0);
			L::log << L::BGBLUE << L::YELLOW << " Value notification: " << buf << 
				L::NORMAL << std::endl;
		}
	}


	void stop(){
		is_running = false;
		uchan.close();
	}

};

Server *TheServer;

void sigfunction(int sig)
{
        switch (sig)
        {
                case SIGINT:
                        std::cerr << "SIGINT received\n";
                        if (TheServer)
                                TheServer->stop();
                break;

                case SIGCHLD:
                        std::cerr << "SIGCHLD received\n";
                        if (TheServer)
                                TheServer->stop();
                break;

                case SIGHUP:
                        std::cerr << "SIGHUP received\n";
                        if (TheServer)
                                TheServer->stop();
                break;

                case SIGTERM:
                        std::cerr << "SIGTERM received\n";
                        if (TheServer)
                                TheServer->stop();
                break;

                default:
                        std::cerr << "Signal:" << sig <<  "recived" << std::endl;
                break;
        }
}




int main(int argc, char *argv[])
{
	Server srv = Server(cfg);
	TheServer = &srv;
	
        signal(SIGINT, sigfunction);
        signal(SIGCHLD, sigfunction);
        signal(SIGHUP, sigfunction);
        signal(SIGTERM, sigfunction);

	while(srv.is_running){
		srv.runloop();
	}
	return 0;
}


