#include "../connector.h"
#include <iostream>
#include <list>



using namespace std;
using namespace std::placeholders;
using namespace asiow;


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
	boost::asio::io_context ioc;
	connector_ptr tcp = Connector::create(ioc);

	if( !tcp->open())
		cout<<"open failed"<<endl;

	connetion_ptr conn = tcp->connect("10.100.23.241", 10086) )
	if(conn == nullptr)
		cout<<"connect failed"<<endl;
	else
	{
		conn->listenOnRecv( onRecvData );
		conn->listenOnError( onError );
	}
	
	char buf[1024*1204];
	while(tcp->isOpen() && true)
	{

		cout<<"call send thread:"<<NetworkThread::get_curid()<<endl;
		conn->postSend("abc", 3);
		//conn->send("abc", 3);

		ioc.poll();
	}

	cout<<"call close thread:"<<NetworkThread::get_curid()<<endl;
	conn->close();

	return 0;
}
