
#pragma once

#include "log.h"
#include <cstdio>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <functional>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

using std::string;
using std::vector;

static	string const  syspath = {"/sys/class/gpio/"};

enum gpio_direction
{
	DIRECTION_IN,
	DIRECTION_OUT
};

struct gpio_conf
{
		int num;
		gpio_direction direction;
};

class Gpio {
	public:
		typedef int valtype;
	private:

		unsigned m_gpionum;
		gpio_direction m_direction;
		valtype m_buffer;

		string valfilename;
		int fd;
		typedef std::function<void(valtype)> callback;
		vector<callback> cb_registry;

	public:
		int get_fd()const {return fd;}

		int get_gpionum()const	{
			return m_gpionum;};
		
		void register_for_valchange(const callback & cb){
			cb_registry.push_back(cb);
		}
		
		void read_update(){
			valtype newval;
			char value[4]={};
			if ( read(fd, value,3) == -1){
				L::log <<"cannot read value!" << std::endl;
				return ;
			}
			L::log << m_gpionum << "has value "<< value << std::endl;
			newval = std::atoi(value);
			if (newval != m_buffer){
				m_buffer = newval;
				for (auto &cb: cb_registry){
					cb(newval);
				}
			}
			lseek(fd,0,SEEK_SET);
		}

		bool write_value(int value)
		{
			char buf[16];
			snprintf(buf, 15, "%i", value);
			if (write(fd, buf, strlen(buf)) == -1) {
				L::log <<"cannot write value!" << std::endl;
				return false;
			}
			return true;
		}

		Gpio(gpio_conf &conf):
			m_gpionum(conf.num),
			m_direction(conf.direction),
			fd(0)
		{
			//try export
			std::ofstream exporter((syspath + "export").c_str(), std::ios::out);
			exporter << conf.num;
			exporter.close();
			//dont catch... just fail

			//try set output direction
			std::ofstream fs(syspath+ "gpio" + std::to_string(conf.num) + "/direction", std::ios::out);
			fs << (m_direction == DIRECTION_IN ? "in" : "out");
			fs.close();
			//try set edge triggd
			std::ofstream fs2(syspath+ "gpio" + std::to_string(conf.num) + "/edge", std::ios::out);
			fs2 << "both";
			fs2.close();

			valfilename = syspath+ "gpio" +	 std::to_string(conf.num) + "/value";
		}

		void init_stage_2(void){
			fd = open(valfilename.c_str(),
				  (m_direction == DIRECTION_IN) ? (O_RDONLY | O_NONBLOCK) : O_WRONLY
				);
			if(fd==-1){
				throw(std::runtime_error("blah"));
			}
			if (m_direction == DIRECTION_IN) {
				read_update();
			} else {
				// Init with 0
				write_value(0);
			}

		}

		~Gpio(){
			close(fd);
		}


		virtual valtype get(){
			return m_buffer;
		}
};

