///acceptor.cpp
//

#include "acceptor.h"
#include "networkpool.h"
#include <iostream>

using namespace boost::asio;
using namespace std;

namespace asiow{


Acceptor::Acceptor(const std::string& local_ip, uint16_t port)
	:ContextTask(NetworkPool::instance().getThread())
{
	_acceptor.reset(new acceptor(_thread->getContext(), endpoint(ip::make_address(local_ip), port)));
	socket_base::reuse_address opt(true);
	_acceptor->set_option(opt);
}

Acceptor::Acceptor(uint16_t port)
	:Acceptor("", port)
{
}

void Acceptor::listenOnConnected(const OnConnectedDelegate& cb)
{
	_onconn_d = cb;
}

void Acceptor::doAccept()
{
	_acceptor->async_accept( 
		[this](const error_code& ec, boost::asio::ip::tcp::socket sock)
		{
			if(!ec)
			{
				if(_onconn_d != nullptr)
				{
					
				//	cout<<"r ip:"<<sock.remote_endpoint().address().to_string()<<endl;
				//	cout<<"r port"<<sock.remote_endpoint().port()<<endl;
					
					connection_ptr conn(new Connection(std::move(sock)));
					_onconn_d(conn);
				}
				doAccept();
			}
			else
				cout<<"Acceptor::doAccept error:"<<ec.message()<<endl;

		});
}

void Acceptor::start()
{
	doAccept();
}

void Acceptor::close()
{
	acceptor_ptr svr = shared_from_this();
	_thread->post([svr](){
		svr->doClose();
	});

}

void Acceptor::doClose()
{
	cout<<"do close, executor:"<<NetworkThread::get_curid()<<endl;
	_onconn_d=nullptr;
	_acceptor->close();
}



}//asiow

