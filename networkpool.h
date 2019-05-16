///networkpool.h
//

#ifndef NETWORK_POOL_H
#define NETWORK_POOL_H

#include "network_thread.h"

namespace asiow{


//not thread safe
class NetworkPool
{

public:
	static NetworkPool& instance(){
		static NetworkPool self;
		return self;
	}

	NetworkPool()
	{}
	
	~NetworkPool(){
		if(_is_inited)
			uninit();
	}

	void init(int num_of_threads);
	void uninit();
	thread_ptr getThread();

private:
	bool _is_inited=false;
  	std::vector<thread_ptr> _threads;
};

}//asiow

#endif /*NETWORK_POOL_H*/
