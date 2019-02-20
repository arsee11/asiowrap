///networkpool.cpp
//

#include "networkpool.h"

namespace asiow{


	//boost::asio::io_context _io_context;

void NetworkPool::init()
{
	if(_is_inited)
		return ;
		
	_thread.reset( new NetworkThread(_io_context, [this](){ this->start(); }) );
	_is_inited=true;
}

void NetworkPool::start()
{
		_io_context.run();
}
void NetworkPool::uninit()
{
	if(!_is_inited ) 
		return;

	_io_context.stop();
	if(_thread != nullptr)
	{
		_thread->stop();
	}
	_is_inited=false;
}

}
