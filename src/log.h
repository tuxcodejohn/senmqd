#pragma once
#include <iostream>
#include "timestamp.h"

namespace L {
	static const char * const BLUE      = "\033[34m";
	static const char * const BROWN     = "\033[33m";
	static const char * const CYAN      = "\033[36m";
	static const char * const GREEN     = "\033[32m";
	static const char * const MAGENTA   = "\033[35m";
	static const char * const RED       = "\033[31m";
	static const char * const YELLOW    = "\033[33;1m";

	static const char * const BGBLUE    = "\033[44m";
	static const char * const BGCYAN    = "\033[45m";
	static const char * const BGGREEN   = "\033[42m";
	static const char * const BGMAGENTA = "\033[46m";
	static const char * const BGRED     = "\033[41m";
	static const char * const BGYELLOW  = "\033[43m";

	static const char * const NORMAL    = "\033[0m";


	class Logstream : public std::ostream {
		public:
			std::ostream & mystream;

			Logstream(std::ostream & parentstream):
				mystream(parentstream)
		{
		}

			template <typename T>
				friend std::ostream & operator<<(const Logstream & left, T fup){
					return ((static_cast<std::ostream&>(left.mystream) <<
								util::get_timestamp() + "  ") << fup);
				}

			void normal(void){mystream << NORMAL << std::dec ; mystream.flush();}
			void set_hex(void){mystream << std::hex;}

	};
	extern 	Logstream log; //hacky object....
}
