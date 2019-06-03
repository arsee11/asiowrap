#include "../udppeer.h"
#include "../networkpool.h"
#include <iostream>


using namespace std;
using namespace std::placeholders;
using namespace asiow;


class UdpControl 
{
public:
	UdpControl(UdpPeer* udp):_udp(udp){}

	void onRecv(const UdpEndpoint& remote, void* msg, size_t size)
	{
		cout<<"recv("<<size<<") from["<<remote<<"]"<<(char*)msg<<endl;
		_udp->postSendto(msg, size, remote);
	}


	void onError( NetError e)
	{
		cout<<"error ("<<e<<")"<<endl;
	}

private:
	UdpPeer* _udp;
}; 

int main()
{
	NetworkPool::instance().init(2);
	udppeer_ptr udp = UdpPeer::create(10010);
	UdpControl udpctrl(udp.get());
	udp->listenOnRecv( std::bind(&UdpControl::onRecv, &udpctrl, _1, _2, _3) );
	//udp->listenOnError( std::bind(&UdpControl::onError, &udpctrl, _1) );

	if( !udp->open() )
	{
		cout<<"open failed"<<endl;
		return 1;
	}

	getchar();
	NetworkPool::instance().uninit();
	return 0;
}
