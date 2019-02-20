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
public:
	using Thread = std::thread;
	using io_context = boost::asio::io_context;

	//return current thread's id
	static thread_id get_curid(){ return std::this_thread::get_id(); }

	using RunFunc = std::function<void()>;

	NetworkThread(io_context& ioc, const RunFunc& f)
		:_io_context(ioc)
		,_run_func(f)
	{
		_thread.reset( new Thread( [this](){ this->run();} ) );
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

	void stop(){ 
		if(_is_stop)
			return;

		try{
			_is_stop = true;
			_thread->join(); 
		}catch(...)
		{}
	}

	thread_id get_id(){ return ( _is_stop ? thread_id() : _thread->get_id()); }

	template<typename Task>
	void post(const Task& t ){
		if(_is_stop)
			return;

		boost::asio::post(_io_context, t);
	}

private:
	void run(){
		while(!_is_stop)
		{
			if(_run_func != nullptr)
				_run_func();
		}

		//cout<<"network thread exit"<<endl;
	}

private:
	std::unique_ptr<Thread> _thread;
	io_context& _io_context;
	RunFunc _run_func=nullptr;
	volatile bool _is_stop=false;
};
	

}//asiow

#endif /*NETWORK_THREAD_H*/
