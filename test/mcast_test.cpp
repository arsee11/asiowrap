#include "../mcast_receiver.h"
#include "../networkpool.h"

#include <iostream>
#include <unordered_set>
#include <thread>


using namespace std;
using namespace std::placeholders;
using namespace asiow;


void onRecv(const UdpEndpoint& ep, void* msg, size_t size)
{
	cout<<"recv("<<size<<" bytes from:"<<ep<<"):"<<(char*)msg<<endl;
}

int main()
{
	NetworkPool::instance().init(1);

	UdpEndpoint mep=makeUdpEndpoint("239.0.0.1", 30001);
	mcast_receiver_ptr mcaster = MCastReceiver::create(mep);
	mcaster->listenOnRecv(&onRecv); 

	if( !mcaster->open() )
	{
		cout<<"open failed"<<endl;
		return 1;
	}

	getchar();
	return 0;
}
