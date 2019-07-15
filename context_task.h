///context_task.h
//

#ifndef	CONTEXT_TASK_H 
#define CONTEXT_TASK_H 

#include <boost/asio.hpp>
#include "network_thread.h"
		
namespace asiow{

class FakeSocket
{
public:
	using native_handle_type = int; 
	FakeSocket(boost::asio::io_context&)
	{
	}
};

///not thread safy
template<class Socket>
class ContextTask 
{

protected:
	using socket = Socket;
	//using native_handle_type = typename socket::native_handle_type;

public:
	ContextTask(thread_ptr thr, socket&& sock)
		:_socket(std::move(sock))
		,_thread(thr)
	{
		_thread->increaseTask();
	}
		
	ContextTask(thread_ptr thr)
		:ContextTask(thr, socket(thr->getContext()))
	{
	}

	virtual ~ContextTask(){
		_thread->decreaseTask();
	}

	
protected:
	socket _socket;
	thread_ptr _thread;
};


}

#endif /*CONTEXT_TASK_H */
