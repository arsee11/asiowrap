///connector.cpp
//

#include "connector.h"
#include <iostream>

using namespace boost::asio::ip;
using namespace boost::asio;
using namespace boost;

namespace asiow{

connector_ptr Connector::create(io_context& ioc, const std::string& local_ip, uint16_t local_port)
{
	return connector_ptr( new Connector(ioc, local_ip, local_port));
}

connector_ptr Connector::create(io_context& ioc, uint16_t local_port)
{
	return connector_ptr( new Connector(ioc,local_port));
}

connector_ptr Connector::create(io_context& ioc)
{
	return connector_ptr( new Connector(ioc));
}


Connector::Connector(io_context& ioc, const std::string& local_ip, uint16_t local_port)
	:sock(ioc)
	,_local_ip(local_ip)
	,_local_port(local_port)
{
}

Connector::Connector(io_context& ioc, uint16_t local_port)
	:Connector(ioc, "", local_port)
{
}

Connector::Connector(io_context& ioc)
	:Connector(ioc, "", 0)
{
}

void Connector::initSock(socket& sock)
{
	system::error_code ec;
	sock.open(tcp::v4(), ec);		
	if( ec )
		return false;

	if( !_local_ip.empty() && _local_port > 0)
		sock.bind(tcp::endpoint(ip::make_address(_local_ip), _local_port), ec );
	else if(_local_port > 0 )
		sock.bind(tcp::endpoint(tcp::v4(), _local_port), ec );
	else
		sock.bind(tcp::endpoint(tcp::v4(), 0), ec );

	if( ec )
		throw std::runtime_error(ec.message());

	
	if( _local_ip.empty())
		_local_ip = sock.local_endpoint().address().to_string();

	if( _local_port <= 0)
		_local_port = sock.local_endpoint().port();
}

connection_ptr Connector::connect(const std::string& remote_ip, uint16_t remote_port)
{
	try{
		socket sock(_ioc);
		initSock(sock);
		sock.connect( tcp::endpoint(ip::make_address(remote_ip), remote_port) ); 
		TcpConnection* tconn = new TcpConnection( std::move(sock)); 
		tconn->setExecutor(_executor);
		tconn->start();
		return connection_ptr(tconn);

	}catch(system::system_error& e){
		std::cout<<"Connector::connect:"<<e.what()<<std::endl;
		return nullptr;
	}
	}catch(std::runtime_error& e){
		std::cout<<"Connector::connect:"<<e.what()<<std::endl;
		return nullptr;
	}
}

void Connector::postConnect(const std::string& remote_ip, uint16_t remote_port)
{
	if(_executor != nullptr)
	{
		acceptor_ptr svr = shared_from_this();
		_executor->post([svr, remote_ip, remote_port](){
			svr->doConnect(remote_ip, remote_port);
		});
	}
}

void Connector::doConnect(const std::string& remote_ip, uint16_t remote_port)
{
	if(_onconn_d != nullptr)
	{
		connection_ptr conn = connection(remote_ip, remote_port);
		_onconn_d(conn);
	}
}

}//asiow
