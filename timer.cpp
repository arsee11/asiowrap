///timer.h
//

#include "timer.h"
#include <chrono>

namespace asiow{

Timer::Timer(io_context& ioc, int ms)
	:_timer(ioc, std::chrono::milliseconds(ms) )
	,_expire_ms(ms)
{
}

void Timer::start(const TimeoutDelegate& timeout)
{
	_timeout_d = timeout;

	assert(_timeout_d);

	_handler = [this](const boost::system::error_code& ec)
		{
			if (ec != boost::asio::error::operation_aborted)
			{
				_timeout_d(this);
				_timer.expires_after(std::chrono::milliseconds(_expire_ms));
				_timer.async_wait(_handler);
			}
		};

	_timer.async_wait(_handler);
}

void Timer::stop()
{
	_timer.cancel();
}

}
