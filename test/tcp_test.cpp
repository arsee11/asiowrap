#include "../tcpserver.h"
#include <iostream>
#include <list>



using namespace std;
using namespace std::placeholders;
using namespace asiow;

class Server : public TcpServer
{
public:
	Server(io_context& ioc, std::string lip, uint16_t lport)
		:TcpServer(ioc, lip, lport)
	{

		TcpServer::listenOnConnected( std::bind(&Server::onConnected, this, _1));
	}

	Server(io_context& ioc, uint16_t lport)
		:TcpServer(ioc, lport)
	{

		TcpServer::listenOnConnected( std::bind(&Server::onConnected, this, _1));
	}

private:
	void onConnected(const connection_ptr& conn)
	{
		std::string ip; 
		uint16_t port;
		tie(ip, port) = conn->remote_addr();
		cout<<"new conn["<<ip<<":"<<port<<"]"<<endl;
		conn->listenOnRecv( std::bind(&Server::onRecv, this, _1, _2, _3) );
		conn->listenOnError( std::bind(&Server::onError, this, _1, _2) );
		_conns.push_back(conn);
		
	}
	
	void onRecv(const connection_ptr& conn, void* msg, size_t size)
	{
		string ip;
		uint16_t port;
		tie(ip, port) = conn->remote_addr();
		cout<<"recv("<<size<<") from["<<ip<<":"<<port<<"]:"<<(char*)msg<<endl;
		for(auto& i : _conns)
		//	i->postSend(msg, size);
			i->send(msg, size);
	}


	void onError(const connection_ptr& conn, NetError e)
	{
		string ip;
		uint16_t port;
		tie(ip, port) = conn->remote_addr();
		cout<<"error ("<<e<<") on["<<ip<<":"<<port<<"]"<<endl;
		if(e == ERR_ON_RECEIVE)
		{
			conn->close();
			_conns.remove(conn);
		}
	}


	list<connection_ptr> _conns;	
}; 

int main()
{
	boost::asio::io_context ioc;
	//Server svr(ioc, "127.0.0.1", 10086);
	Server svr(ioc, 10086);
	svr.start();
	ioc.run();

	return 0;
}
