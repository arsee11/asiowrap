///timer.h
//

#ifndef TIMER_H
#define TIMER_H

#include <functional>
#include <boost/asio.hpp>


namespace asiow{

class Timer;

using TimeoutDelegate = std::function<void(const Timer*)>;

class Timer
{
	using steady_timer=boost::asio::steady_timer;
	using io_context=boost::asio::io_context;
	using handler_t = std::function<void(const boost::system::error_code& ec)>;

public:
	Timer(io_context& ioc, int ms);
	void start(const TimeoutDelegate& d);
	void stop();

private:
	steady_timer _timer;
	handler_t _handler=nullptr; 
	int _expire_ms;
	TimeoutDelegate _timeout_d=nullptr;
};

}

#endif /*TIMER_H*/
