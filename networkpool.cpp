///networkpool.cpp
//

#include "networkpool.h"

namespace asiow{

void NetworkPool::init(int num_of_threads)
{
	if(num_of_threads <=0 )
		throw std::runtime_error("NetworkPool::init num_of_threads must > 0");

	if(_is_inited)
		return ;
		
  	for (int i = 0; i < num_of_threads; ++i)
  	{
  	  thread_ptr thread(new NetworkThread);
  	  _threads.push_back(thread);
  	}

	_is_inited=true;
}

void NetworkPool::uninit()
{
	if(!_is_inited ) 
		return;

	for(auto i : _threads)
		i->stop();

	_is_inited=false;
}

thread_ptr NetworkPool::getThread()
{
	if(_threads.size() == 0 )
		throw std::runtime_error("NetworkPool is not init");

	auto ithr = _threads.begin();
	thread_ptr thr= *ithr;
	if(_threads.size() == 1 )
		return thr;

	++ithr;

	for(; ithr!= _threads.end(); ++ithr)
	{
		if(thr->taskCount() > (*ithr)->taskCount())
			thr = *ithr;
	}

	return thr;
}
}
