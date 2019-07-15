#include "../acceptor.h"
#include "../networkpool.h"
#include <iostream>
#include <list>



using namespace std;
using namespace std::placeholders;
using namespace asiow;

class Server 
{
public:
	Server(std::string lip, uint16_t lport)
	{

		_acceptor = Acceptor::create(lip, lport);
		_acceptor->listenOnConnected( std::bind(&Server::onConnected, this, _1));
		_acceptor->start();
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
		conn->start();
		_conns.push_back(conn);
		
	}
	
	void onRecv(const connection_ptr& conn, void* msg, size_t size)
	{
		string ip;
		uint16_t port;
		tie(ip, port) = conn->remote_addr();
		cout<<"recv("<<size<<") from["<<ip<<":"<<port<<"]:"<<(char*)msg<<endl;
		for(auto& i : _conns)
			i->postSend(msg, size);
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
	acceptor_ptr _acceptor;
}; 

int main()
{
	NetworkPool::instance().init(2);

	Server svr("127.0.0.1", 10086);
	getchar();

	return 0;
}
