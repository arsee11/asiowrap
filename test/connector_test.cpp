#include "../connector.h"
#include "../networkpool.h"
#include <iostream>
#include <list>



using namespace std;
using namespace std::placeholders;
using namespace asiow;


void onRecv(const connection_ptr& conn, void* msg, size_t size)
{
	cout<<"onRecv thread:"<<NetworkThread::get_curid()<<endl;
	string ip;
	uint16_t port;
	tie(ip, port) = conn->remote_addr();
	cout<<"recv("<<size<<") from["<<ip<<":"<<port<<"]:"<<(char*)msg<<endl;
}

void onSent(const connection_ptr& conn, size_t size)
{
	cout<<"onSent thread:"<<NetworkThread::get_curid()<<endl;
	string ip;
	uint16_t port;
	tie(ip, port) = conn->remote_addr();
	cout<<"have sent("<<size<<") to["<<ip<<":"<<port<<"]"<<endl;
}

void onError(const connection_ptr& conn, NetError e)
{
	cout<<"onError thread:"<<NetworkThread::get_curid()<<endl;
	string ip;
	uint16_t port;
	tie(ip, port) = conn->remote_addr();
	cout<<"error ("<<e<<") on["<<ip<<":"<<port<<"]"<<endl;
	conn->close();
	exit(0);
}

void onConnected(const connection_ptr& conn)
{
	cout<<"onConnected thread:"<<NetworkThread::get_curid()<<endl;
	if(conn == nullptr)
	{
		cout<<"connect failed"<<endl;
		return;
	}
	
	conn->postSend("abc", 3);
	//conn->send("abc", 3);
}

#if 0
int main()
{
	NetworkPool::instance().init(2);

	connector_ptr tcp = Connector::create();
	connection_ptr conn = tcp->connect("127.0.0.1", 10086, onRecv, onSent, onError );
	if(conn == nullptr)
	{
		cout<<"connect failed"<<endl;
		return 0; 
	}
	
	while(true)
	{

		cout<<"call send thread:"<<NetworkThread::get_curid()<<endl;
		conn->postSend("abc", 3);
		//conn->send("abc", 3);
		std::this_thread::sleep_for(std::chrono::seconds(3));

	}

	cout<<"call close thread:"<<NetworkThread::get_curid()<<endl;
	conn->close();

	return 0;
}

#else
int main()
{
	NetworkPool::instance().init(2);

	connector_ptr tcp = Connector::create();
	tcp->listenOnConnected(onConnected);
	tcp->postConnect("127.0.0.1", 10086, onRecv, onSent, onError );

	getchar();

	return 0;
}
#endif
