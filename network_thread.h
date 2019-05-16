///network_thread.h 
//
//

#ifndef NETWORK_THREAD_H
#define NETWORK_THREAD_H 

#include <memory>
#include <boost/asio.hpp>
#include <iostream>

using namespace std;

namespace asiow{

using thread_id = std::thread::id;

class NetworkThread
{
	using Thread = std::thread;
	using io_context = boost::asio::io_context;

  	typedef boost::asio::executor_work_guard<io_context::executor_type> io_context_work;

public:
	//return current thread's id
	static thread_id get_curid(){ return std::this_thread::get_id(); }

	NetworkThread()
		:_workg(boost::asio::make_work_guard(_io_context))
	{
		_thread.reset( new Thread( [this](){ _io_context.run();} ) );
	}

	~NetworkThread()
	{
		if(_thread!=nullptr)
		{
			try{
				this->stop(); 
			}catch(...)
			{}
		}
	} 

	io_context& getContext(){ return _io_context; }

	int taskCount(){
		lock_guard g(_mtx);
		return _task_count;
	}

	void increaseTask(){
		lock_guard g(_mtx);
		_task_count++;
	}

	void decreaseTask(){
		lock_guard g(_mtx);
		_task_count--;
	}

	void stop(){ 
		if(_is_stop)
			return;

		try{
			_is_stop = true;
			_io_context.stop();
			_thread->join(); 
		}catch(...)
		{}
	}

	thread_id get_id(){ return ( _is_stop ? thread_id() : _thread->get_id()); }

	template<typename Task>
	void post(const Task& t ){ if(_is_stop) return;

		boost::asio::post(_io_context, t);
	}

private:
	io_context _io_context;
	io_context_work _workg;
	int _task_count;
	using lock_guard = std::lock_guard<std::mutex>;
	std::mutex _mtx;

	std::unique_ptr<Thread> _thread;
	volatile bool _is_stop=false;
};
	
using thread_ptr=std::shared_ptr<NetworkThread>;

}//asiow

#endif /*NETWORK_THREAD_H*/
