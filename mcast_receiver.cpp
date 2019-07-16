///mcast_receiver.cpp
//

#include "mcast_receiver.h"
#include <iostream>

using namespace boost::asio;
using namespace boost;
using namespace std;

namespace asiow{


MCastReceiver::MCastReceiver(const std::string& local_ip,const UdpEndpoint& mcast_ep)
	:UdpPeer(local_ip, 0)
	,_mcast_ep(mcast_ep)
{}

MCastReceiver::MCastReceiver(const UdpEndpoint& mcast_ep)
	:MCastReceiver("", mcast_ep)
{}

bool MCastReceiver::open()
{
	error_code ec;
	_socket.open(udp::v4(), ec);		
	if( ec )
		return false;

	_socket.set_option(socket::reuse_address(true));
	if( !_local_ip.empty())
		_socket.bind(udp::endpoint(ip::make_address(_local_ip), _mcast_ep.port()), ec );
	else
		_socket.bind(udp::endpoint(udp::v4(), _mcast_ep.port()), ec );

	if( ec )
		return false;
	
	_socket.set_option(ip::multicast::join_group(_mcast_ep.address()), ec);
	if( ec )
		return false;

	this->_isopen = true;
	doRecvFrom();	
	return true;
	
}

}

