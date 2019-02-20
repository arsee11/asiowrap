///udpserver.h
//

#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <functional>
#include <memory>
#include <stdint>

namespace asiow{


class UdpPeer : public Connection
{
public:
	UdpPeer(const std::string& local_ip, uint16_t port);

	int sendto(const void* buf, size_t size, const connection_ptr& peer);

	void close();

	///return local ip addr and port number.
	std::tuple<std::string, uint16_t> netaddr();

private:
	void postSend(const void* msg, size_t len)override{};

private:
	OnConnectedDelegate _onconn_d;
};

}


#endif /*UDP_SERVER_H*/
