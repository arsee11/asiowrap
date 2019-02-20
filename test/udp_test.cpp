#include "../udppeer.h"
#include <iostream>
#include <unordered_set>



using namespace std;
using namespace std::placeholders;
using namespace asiow;


class UdpControl 
{
public:
	UdpControl(UdpPeer* udp):_udp(udp){}

	void onRecv(const UdpEndpoint& remote, void* msg, size_t size)
	{
		string ip=remote.address().to_string();
		uint16_t port = remote.port();
		cout<<"recv("<<size<<") from["<<remote<<"]"<<(char*)msg<<endl;
		if(_endpoints.find(remote) == _endpoints.end() )
			_endpoints.insert(remote);

		cout<<"size of endpooints:"<<_endpoints.size()<<endl;
		for(auto& i : _endpoints)
			_udp->sendto(msg, size, i);
			
	}


	void onError( NetError e)
	{
		cout<<"error ("<<e<<")"<<endl;
	}

private:

	std::unordered_set<UdpEndpoint, EndpointHash<UdpEndpoint> > _endpoints;	
	UdpPeer* _udp;
}; 

int main()
{
	boost::asio::io_context ioc;

	udppeer_ptr udp = UdpPeer::create(ioc, 10010);
	UdpControl udpctrl(udp.get());
	udp->listenOnRecv( std::bind(&UdpControl::onRecv, &udpctrl, _1, _2, _3) );
	udp->listenOnError( std::bind(&UdpControl::onError, &udpctrl, _1) );

	if( !udp->open() )
	{
		cout<<"open failed"<<endl;
		return 1;
	}

	ioc.run();

	return 0;
}
