#pragma once

#include <sys/time.h>
#include <cstdio>
#include <string>
#include <ctime>
#include <chrono>

namespace util
{

       /* inline std::string to_str(time_t val)*/
	//{
		//struct tm * timeinfo = localtime(&val);
		//char buffer[80];
		//strftime(buffer, 80, "%Y-%m-%dT%H:%M:%S.%jZ", timeinfo);

		//return std::string(buffer);
	/*}*/
	
	inline std::string get_timestamp()
	{
		timeval curTime;
		int milli;
		char buffer_time[60];
		char buffer[80];
		gettimeofday(&curTime, NULL);
		milli = curTime.tv_usec / 1000;
		strftime(buffer_time, 80, "%Z %Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
		snprintf(buffer, 80, "%s.%0d", buffer_time, milli);
		return std::string(buffer);
	}


}
