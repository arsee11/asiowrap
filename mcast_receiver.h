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


class MCastReceiver:public UdpPeer
{
public:
	MCastReceiver(const std::string& local_ip, const UdpEndpoint& mcast_ep); 
	MCastReceiver(const UdpEndpoint& mcast_ep);

	bool open();

private:
	void sendto(const void* buf, size_t size, const UdpEndpoint& remote){}

private:
	UdpEndpoint _mcast_ep;
};

}


#endif /*MCAST_RECEIVER_H*/
