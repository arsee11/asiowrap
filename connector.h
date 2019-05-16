///connector.h
//

#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "connection.h"
#include "context_task.h"

namespace asiow{


class Connector;
using connector_ptr = std::shared_ptr<Connector>;

class Connector : public std::enable_shared_from_this<Connector>
	        , public ContextTask<boost::asio::ip::tcp::socket>
{
	
public:
	//trow std::runtime_error when failed.
	static connector_ptr create(const std::string& local_ip, uint16_t local_port);
	static connector_ptr create(uint16_t local_port);
	static connector_ptr create();

	void listenOnConnected(const OnConnectedDelegate& cb);

	connection_ptr connect(const std::string& remote_ip, uint16_t remote_port);
	void postConnect(const std::string& remote_ip, uint16_t remote_port);

protected:
	Connector(const std::string& local_ip, uint16_t local_port);
	Connector(uint16_t local_port);
	Connector();

	void initSock(socket& sock);
	void doConnect(const std::string& remote_ip, uint16_t remote_port);

	std::string _local_ip;
	uint16_t _local_port;
	OnConnectedDelegate _onconn_d;
};

}


#endif /*TCP_CLIENT_H*/
