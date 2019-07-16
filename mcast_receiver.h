///mcast_receiver.h
//

#ifndef MCAST_RECEIVER_H
#define MCAST_RECEIVER_H

#include <functional>
#include <list>
#include <boost/asio.hpp>
#include "errordef.h"
#include "endpoints.h"
#include "udppeer.h"

namespace asiow{

class MCastReceiver;
using mcast_receiver_ptr = std::shared_ptr<MCastReceiver>;

class MCastReceiver:public UdpPeer
{
public:
	static mcast_receiver_ptr create(const std::string& local_ip, const UdpEndpoint& mcast_ep){
		return mcast_receiver_ptr(new MCastReceiver(local_ip, mcast_ep) );
	}

	static mcast_receiver_ptr create(const UdpEndpoint& mcast_ep){
		return mcast_receiver_ptr(new MCastReceiver(mcast_ep) );
	}

	bool open();

private:
	MCastReceiver(const std::string& local_ip, const UdpEndpoint& mcast_ep); 
	MCastReceiver(const UdpEndpoint& mcast_ep);

	void sendto(const void* buf, size_t size, const UdpEndpoint& remote){}

private:
	UdpEndpoint _mcast_ep;
};

}


#endif /*MCAST_RECEIVER_H*/
