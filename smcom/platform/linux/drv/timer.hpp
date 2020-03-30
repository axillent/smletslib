#ifndef _STAVRP_LINUX_DRV_TIMER_H
#define _STAVRP_LINUX_DRV_TIMER_H

#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#include "../../../stavrp.hpp"

namespace STAVRP {
namespace Linux {

//---------------------------------------------------------------------
// Interface Server
//---------------------------------------------------------------------
template <stavrp_funcp handler = nullfunc>
class TimerDrv {
protected:
	static struct Data {
		volatile bool	flag;
	} data;
	static void Handler(int signum) {
		data.flag = true;
		if( handler != nullfunc ) handler();
	}
public:
	static void StartMSec(uint32_t msec) {
		struct sigaction sa;
		struct itimerval timer;

		data.flag = false;

		memset(&sa, 0, sizeof(sa));
		sa.sa_handler = &Handler;
		sigaction(SIGALRM, &sa, NULL);

		timer.it_value.tv_sec = 0;
		timer.it_value.tv_usec = msec * 1000;
		timer.it_interval.tv_sec = 0;
		timer.it_interval.tv_usec = msec * 1000;

		setitimer(ITIMER_REAL, &timer, NULL);
	}
	static bool Get() { return data.flag; }
	static void Clear() { data.flag = false; }
};
template <stavrp_funcp handler> typename TimerDrv<handler>::Data TimerDrv<handler>::data;

}}

#endif
