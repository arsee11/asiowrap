///connection.cpp
//

#include "connection.h"
#include <boost/asio.hpp>
#include <iostream>
#include <queue>


using namespace boost::asio;
using namespace boost;
using namespace std;

namespace asiow{


Connection::Connection(socket&& sock)
	:Connection(sock.local_endpoint().address().to_string()
		,sock.local_endpoint().port()
		,sock.remote_endpoint().address().to_string()
		,sock.remote_endpoint().port()
	)
	,_socket( std::move(sock))
{
	_isopen = true;
}

Connection::~Connection()
{
	//cout<<"Connection::~Connection:"<<std::this_executor::get_id()<<endl;
}

void Connection::send(const void* msg, size_t len)
{
	//if current executor is the network executor, so exec
	if(_executor == nullptr || _executor->get_id() == NetworkThread::get_curid() )
		doSend(msg, len);
	//else post to the network executor to exec
	else
	{
		item_ptr item(new Item(msg, len) ); 
		connection_ptr conn = shared_from_this();
		_executor->post([item, conn](){ 
			static_cast<Connection*>(conn.get())->doSend(item); }
			);
	}
}

////exec in single network executor
void Connection::doSend(const void* msg, size_t len)
{
	//cout<<"Connection::doSend executor:"<<std::this_executor::get_id()<<endl;
	//string ip;
	//uint16_t port;
	//tie(ip, port) = this->remote_addr();
	//cout<<"Connection::doSend("<<len<<") to["<<ip<<":"<<port<<"]"<<endl;

	if(!isOpen())
	{
		cout<<"connection::doSend() error: not open"<<endl; 
		return;
	}

	try{
		boost::asio::write(_socket, buffer(msg, len) );
		if (_onsent_d != nullptr)
			_onsent_d(shared_from_this(), len);

	}catch(system::system_error& e){
		cout<<"Connection::doSend failure:"<<e.what()<<endl;
		if(_onerror_d != nullptr)
			_onerror_d(shared_from_this(), ERR_ON_SEND);
	}
}

////exec in single network executor
void Connection::doSend(const item_ptr& item)
{
	doSend(item->data, item->size);
}

void Connection::postSend(const void* msg, size_t len)
{
	item_ptr item(new Item(msg, len) ); 
	if(_executor == nullptr || _executor->get_id() == NetworkThread::get_curid() )
		doPostSend(item);
	else
	{
		connection_ptr conn = shared_from_this();
		_executor->post([item, conn](){ 
			static_cast<Connection*>(conn.get())->doPostSend(item); }
			);
	}
}

////exec in single network executor
void Connection::doPostSend(const item_ptr& item)
{
	//cout<<"Connection::doPostSend executor:"<<std::this_executor::get_id()<<endl;
	//string ip;
	//uint16_t port;
	//tie(ip, port) = this->remote_addr();
	//cout<<"Connection::doPostsend("<<item->size<<") to["<<ip<<":"<<port<<"]"<<endl;
		 
	if(!isOpen())
	{
		cout<<"connection::doPostSend() error: not open"<<endl; 
		return;
	}

	if(_item_queue.size() > 0 )
	{
		//cout<<"connection::doPostSend() operation busying "<<endl; 
		_item_queue.push(item);
		return;
	}
	_item_queue.push(item);
	const item_ptr& titem = _item_queue.front();

	connection_ptr shared_me = shared_from_this();
	//async_write ensure all data in buffer will be writen
	async_write(_socket, buffer(titem->data, titem->size),[shared_me](const system::error_code& ec, size_t slen){
			static_cast<Connection*>(shared_me.get())->onSent(ec, slen); 
		}
	);
}

////exec in single network executor
void Connection::onSent(const system::error_code& ec, size_t slen)
{
	//cout<<"Connection::onSent executor:"<<std::this_executor::get_id()<<endl;
	if(!ec)
	{
		//cout<<"Connection had sent("<<slen<<")"<<endl;
		connection_ptr shared_me = shared_from_this();
		if (_onsent_d != nullptr)
			_onsent_d(shared_me, slen);

		_item_queue.pop();
		if(_item_queue.size() > 0 )
		{
			const item_ptr& titem = _item_queue.front();
			//cout<<"Connection::onSend continue("<<titem->size<<")"<<endl;			
			//async_write ensure all data in buffer will be writen
			async_write(_socket, buffer(titem->data, titem->size),[shared_me](const system::error_code& ec, size_t slen){
					static_cast<Connection*>(shared_me.get())->onSent(ec, slen); 
					}
			);
		}

	}
	else
	{
		cout<<"connection::onError() error:"<<ec.message()<<endl;
		if(_onerror_d != nullptr)
			_onerror_d(shared_from_this(), ERR_ON_SEND);

	}
}

////exec in single network executor
void Connection::doReceive()
{
	if( !_isopen)
		return;	

	connection_ptr shared_me = shared_from_this();
	_socket.async_receive(buffer(_recv_buf, MAX_RECV_SIZE),
		[shared_me](system::error_code ec, size_t rlen)
		{
			//cout<<"Connection::doReceive executor:"<<std::this_executor::get_id()<<endl;
			Connection* conn = static_cast<Connection*>(shared_me.get());
			if(!ec)
			{
				if(conn->_onrecv_d != nullptr)
					conn->_onrecv_d(shared_me, conn->_recv_buf, rlen);

				conn->doReceive();
			}	
			else 
			{
				cout<<"connection::doReceive() error:"<<ec.message()<<endl;
				if(conn->_isopen && conn->_onerror_d != nullptr)
					conn->_onerror_d(shared_me, ERR_ON_RECEIVE );
			
			}
		}
	);
}


void Connection::close()
{
	if(_executor == nullptr || _executor->get_id() == NetworkThread::get_curid() )
		doClose();
	else
	{
		connection_ptr conn = shared_from_this();
		_executor->post([conn](){ 
			static_cast<Connection*>(conn.get())->doClose(); }
			);
	}
}

////exec in single network executor
void Connection::doClose()
{
	//cout<<"Connection::doClose executor:"<<std::this_executor::get_id()<<endl;
	_onerror_d=nullptr;
	_onrecv_d=nullptr;
	//cout<<"connection::doClose()["<<_remote_ip<<":"<<_remote_port<<"]"<<endl;
	if(isOpen() )
	{
		try{
			_socket.close();
		}catch(system::system_error& e){
			//cout<<"connection::doClose()["<<_remote_ip<<":"<<_remote_port<<"]:";
			cout<<e.what()<<endl;
		}

		_isopen = false;
	}
}

}//asiow


