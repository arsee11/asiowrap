///connector.cpp
//

#include "connector.h"
#include "networkpool.h"
#include <iostream>

using namespace boost::asio::ip;
using namespace boost::asio;
using namespace boost;

namespace asiow{

connector_ptr Connector::create(const std::string& local_ip, uint16_t local_port)
{
	return connector_ptr( new Connector(local_ip, local_port));
}

connector_ptr Connector::create(uint16_t local_port)
{
	return connector_ptr( new Connector(local_port));
}

connector_ptr Connector::create()
{
	return connector_ptr( new Connector());
}


Connector::Connector(const std::string& local_ip, uint16_t local_port)
	:ContextTask(NetworkPool::instance().getThread())
	,_local_ip(local_ip)
	,_local_port(local_port)
{
}

Connector::Connector(uint16_t local_port)
	:Connector( "", local_port)
{
}

Connector::Connector()
	:Connector("", 0)
{
}

void Connector::initSock(socket& sock)
{
	system::error_code ec;
	sock.open(tcp::v4(), ec);		
	if( ec )
		throw std::runtime_error(ec.message());

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

connection_ptr Connector::connect(const std::string& remote_ip, uint16_t remote_port
				 ,OnRecvDelegate recvd, OnSentDelegate sentd,  OnErrorDelegate errd   )
{
	try{
		socket sock(_thread->getContext());
		initSock(sock);
		sock.connect( tcp::endpoint(ip::make_address(remote_ip), remote_port) ); 
		connection_ptr conn(new Connection(std::move(sock)));
		conn->listenOnRecv( recvd);
		conn->listenOnError( errd);
		conn->listenOnSent( sentd);
		conn->start();
		return connection_ptr(conn);

	}catch(system::system_error& e){
		std::cout<<"Connector::connect:"<<e.what()<<std::endl;
		return nullptr;
	}catch(std::runtime_error& e){
		std::cout<<"Connector::connect:"<<e.what()<<std::endl;
		return nullptr;
	}
}

void Connector::postConnect(const std::string& remote_ip, uint16_t remote_port
			 ,OnRecvDelegate recvd, OnSentDelegate sentd,  OnErrorDelegate errd)
{
	socket sock(_thread->getContext());
	initSock(sock);
	connection_ptr conn(new Connection(std::move(sock)));
	connector_ptr me = shared_from_this();
	conn->getSocket().async_connect( tcp::endpoint(ip::make_address(remote_ip), remote_port),
		[conn, me, recvd, sentd, errd](const boost::system::error_code& ec){
			if(!ec)
			{
				if(me->_onconn_d != nullptr)
				{
					conn->listenOnRecv( recvd);
					conn->listenOnError( errd);
					conn->listenOnSent( sentd);
					conn->start();
					me->_onconn_d(conn);
				}
			}
			else
			{
				std::cout<<"post connect failed:"<<ec.message()<<endl;
				if(me->_onconn_d != nullptr)
					me->_onconn_d(nullptr);
			}
}
	);
}

}//asiow
