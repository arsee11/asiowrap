///acceptor.cpp
//

#include "acceptor.h"
#include <iostream>

using namespace boost::asio;
using namespace std;

namespace asiow{


Aceeptor::Aceeptor(io_context& ioc, const std::string& local_ip, uint16_t port)
	:_acceptor(ioc, endpoint(ip::make_address(local_ip), port))
{
	socket_base::reuse_address opt(true);
	_acceptor.set_option(opt);
}

Aceeptor::Aceeptor(io_context& ioc, uint16_t port)
	:_acceptor(ioc, endpoint(tcp::v4(), port))
{
}

void Aceeptor::listenOnConnected(const OnConnectedDelegate& cb)
{
	_onconn_d = cb;
}

void Aceeptor::doAccept()
{
	_acceptor.async_accept( 
		[this](const error_code& ec, socket sock)
		{
			if(!ec)
			{
				if(_onconn_d != nullptr)
				{
					
				//	cout<<"r ip:"<<sock.remote_endpoint().address().to_string()<<endl;
				//	cout<<"r port"<<sock.remote_endpoint().port()<<endl;
					
					TcpConnection* tconn = new TcpConnection( std::move(sock)); 
					tconn->setExecutor(_executor);
					connection_ptr conn(tconn);
					_onconn_d(conn);
					conn->start();
				}
				doAccept();
			}
			else
				cout<<"Aceeptor::doAccept error:"<<ec.message()<<endl;

		});
}

void Aceeptor::start()
{
	doAccept();
}

void Aceeptor::close()
{
	if(_executor == nullptr)
	{
		this->doClose();
	}
	else
	{
		acceptor_ptr svr = shared_from_this();
		_executor->post([svr](){
			svr->doClose();
		});
		//cout<<"closed, executor:"<<NetworkThread::get_curid()<<endl;
	}

}

void Aceeptor::doClose()
{
	cout<<"do close, executor:"<<NetworkThread::get_curid()<<endl;
	_onconn_d=nullptr;
	_acceptor.close();
}



}//asiow

