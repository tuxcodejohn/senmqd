
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

using std::string;
using std::vector;

static  string const  syspath = {"/sys/class/gpio/"};

class Gpio {
	public:
		typedef int valtype;
	private:

		unsigned m_gpionum;
		valtype m_buffer;

		string valfilename;
		int fd;
		typedef std::function<void(valtype)> callback;
		vector<callback> cb_registry;

	public:
		int get_fd()const {return fd;}

		int get_gpionum()const  {
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

		Gpio(int gpionum):
			m_gpionum(gpionum),
			fd(0)
		{
			//try export
			std::ofstream exporter((syspath + "export").c_str(), std::ios::out);
			exporter << gpionum;
			exporter.close();
			//dont catch... just fail

			//try set output direction
			std::ofstream fs(syspath+ "gpio" + std::to_string(gpionum) + "/direction", std::ios::out);
			fs << "in";
			fs.close();
			//try set edge triggd
			std::ofstream fs2(syspath+ "gpio" + std::to_string(gpionum) + "/edge", std::ios::out);
			fs2 << "both";
			fs2.close();

			valfilename = syspath+ "gpio" +  std::to_string(gpionum) + "/value";
		}

		void init_stage_2(void){
			fd = open(valfilename.c_str(), O_RDONLY |O_NONBLOCK);
			if(fd==-1){
				throw(std::runtime_error("blah"));
			}
			read_update();

		}

		~Gpio(){
			close(fd);
		}


		virtual valtype get(){
			return m_buffer;
		}
};

