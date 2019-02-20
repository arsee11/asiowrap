///connector.h
//

#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "connection.h"

namespace asiow{


class Connector;
using connector_ptr = std::shared_ptr<Connector>;

class Connector : public std::enable_shared_from_this<Connector>
{
	using socket = boost::asio::ip::tcp::socket;
	using io_context = boost::asio::io_context;
	
public:
	//trow std::runtime_error when failed.
	static connector_ptr create(io_context& ioc, const std::string& local_ip, uint16_t local_port);
	static connector_ptr create(io_context& ioc, uint16_t local_port);
	static connector_ptr create(io_context& ioc);

	void setExecutor(ExeScope* e){ _executor = e; }
	void listenOnConnected(const OnConnectedDelegate& cb);

	connection_ptr connect(const std::string& remote_ip, uint16_t remote_port);
	void postConnect(const std::string& remote_ip, uint16_t remote_port);

protected:
	Connector(io_context& ioc, const std::string& local_ip, uint16_t local_port);
	Connector(io_context& ioc, uint16_t local_port);
	Connector(io_context& ioc);

	void initSock(socket& sock);
	void doConnect(const std::string& remote_ip, uint16_t remote_port);

	io_context &_ioc;
	ExeScope _executor=nullptr;
};

}


#endif /*TCP_CLIENT_H*/
