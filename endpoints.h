///endpoints.h
//

#ifndef ENDPOINTS_H
#define ENDPOINTS_H

#include <functional>
#include <list>
#include <boost/asio.hpp>
#include "errordef.h"

namespace asiow{

using UdpEndpoint = boost::asio::ip::udp::endpoint;
using TcpEndpoint = boost::asio::ip::tcp::endpoint;

inline UdpEndpoint makeUdpEndpoint(const std::string ipv4, uint16_t port){
	if(ipv4.empty())
		return UdpEndpoint(boost::asio::ip::udp::v4(), port);

	return UdpEndpoint(boost::asio::ip::make_address(ipv4), port);
}

inline TcpEndpoint makeTcpEndpoint(const std::string ipv4, uint16_t port){
	if(ipv4.empty())
		return TcpEndpoint(boost::asio::ip::tcp::v4(), port);

	return TcpEndpoint(boost::asio::ip::make_address(ipv4), port);
}

template<class Endpoint>
struct EndpointHash
{
	inline size_t operator()(const Endpoint& ep){
		size_t h1 = std::hash<std::string>{}(ep.address().to_string());
		size_t h2 = std::hash<unsigned short>{}(ep.port());
		return h1^(h2<<1);
	}
};


}


#endif /*ENDPOINTS_H*/
