///timer.h
//

#include "timer.h"
#include "networkpool.h"
#include <chrono>

namespace asiow{

timer_ptr Timer::create(int ms)
{
	return timer_ptr(new Timer(ms));
}

Timer::Timer(int ms)
	:ContextTask(NetworkPool::instance().getThread())
	,_expire_ms(ms)
{
	this->_socket.expires_after( std::chrono::milliseconds(_expire_ms) );
}

void Timer::start(const TimeoutDelegate& timeout)
{
	_timeout_d = timeout;

	assert(_timeout_d);

	_handler = [this](const boost::system::error_code& ec)
		{
			if (ec != boost::asio::error::operation_aborted)
			{
				_timeout_d(this->shared_from_this());
				this->_socket.expires_after(std::chrono::milliseconds(_expire_ms));
				this->_socket.async_wait(_handler);
			}
		};

	this->_socket.async_wait(_handler);
}

void Timer::stop()
{
	this->_socket.cancel();
}

}
