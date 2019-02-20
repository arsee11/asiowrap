#include "../tcpserver.h"
#include "../tcpclient.h"
#include "../networkpool.h"
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

	~Server(){
		cout<<"~Server()"<<endl;
		for(auto& c : _conns)
			c->close();

		_conns.clear();
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
			//i->send(msg, size);
			i->postSend(msg, size);
	}

	void onSent(const connection_ptr& conn, size_t size)
	{
		string ip;
		uint16_t port;
		tie(ip, port) = conn->remote_addr();
		cout<<"have sent("<<size<<") to["<<ip<<":"<<port<<"]"<<endl;
	}

	void onError(const connection_ptr& conn, NetError e)
	{
		conn->close();
		_conns.remove(conn);
	}


	list<connection_ptr> _conns;	
}; 

connection_ptr g_conn;

void onRecvData(const connection_ptr& conn, void* msg, size_t size)
{
	string ip;
	uint16_t port;
	tie(ip, port) = conn->remote_addr();
	cout<<"recv("<<size<<") from["<<ip<<":"<<port<<"]:"<<(char*)msg<<endl;
}

void onError(const connection_ptr& conn, NetError e)
{
	string ip;
	uint16_t port;
	tie(ip, port) = conn->remote_addr();
	cout<<"error ("<<e<<") on["<<ip<<":"<<port<<"]"<<endl;
	conn->close();
}

int main()
{
	NetworkPool np;
	np.init();

	TcpEndpoint ep(boost::asio::ip::tcp::v4(), 10086);
	Server* svr = np.createTcpServer<Server>(ep);

	/*
	tcpclient_ptr cli = np.createTcpClient();
	cli->connect("10.100.23.241", 10086);
	cli->listenOnRecv( onRecvData );
	cli->listenOnError( onError );
	//char* buf = new char[1024*1024*10];
	const char* buf = "hello,world";//new char[1024*1024*10];
	while(cli->isOpen() && true)
	{
		cout<<"call send thread:"<<NetworkThread::get_curid()<<endl;
		//cli->postSend(buf, 12);
		cli->send(buf, 12);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	*/

	getchar();
	cout<<"call close thread:"<<NetworkThread::get_curid()<<endl;
	svr->close();
	delete svr;
	//cli->close();
	np.uninit();
	return 0;
}
