#include "../mcast_receiver.h"
#include <iostream>
#include <unordered_set>
#include <thread>


using namespace std;
using namespace std::placeholders;
using namespace asiow;


void onRecv(const UdpEndpoint& ep, void* msg, size_t size)
{
	cout<<"recv("<<size<<") from:"<<ep<<endl;
}

void worker_thr(UdpPeer* udp, UdpEndpoint* ep)
{
	while(true)
	{
		this_thread::sleep_for(chrono::seconds(1));

		const char *msg = "hello,world!";
		udp->sendto(msg, strlen(msg), *ep);
	}
};

int main()
{
	boost::asio::io_context ioc;

	UdpEndpoint mep=makeUdpEndpoint("239.0.0.1", 30001);
	MCastReceiver mcaster(ioc, mep);
	mcaster.listenOnRecv(&onRecv); 

	if( !mcaster.open() )
	{
		cout<<"open failed"<<endl;
		return 1;
	}

	udppeer_ptr udp = UdpPeer::create(ioc);
	udp->open();
	thread th(worker_thr, udp.get(), &mep);
	ioc.run();

	return 0;
}
