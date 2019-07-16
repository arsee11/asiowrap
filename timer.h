///timer.h
//

#ifndef TIMER_H
#define TIMER_H

#include <functional>
#include "context_task.h"


namespace asiow{

class Timer;

using timer_ptr = std::shared_ptr<Timer>;
using TimeoutDelegate = std::function<void(const timer_ptr&)>;

class Timer : public std::enable_shared_from_this<Timer>
	    , public ContextTask<boost::asio::steady_timer>
	      
{
	using handler_t = std::function<void(const boost::system::error_code& ec)>;

public:
	static timer_ptr create(int ms);
	void start(const TimeoutDelegate& d);
	void stop();

private:
	Timer(int ms);
private:
	handler_t _handler=nullptr; 
	int _expire_ms;
	TimeoutDelegate _timeout_d=nullptr;
};

}

#endif /*TIMER_H*/
