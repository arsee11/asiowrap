///udppeer.cpp
//

#include "udppeer.h"
#include <iostream>
#include <boost/asio/error.hpp>
#include "networkpool.h"

using namespace boost::asio;
using namespace boost;
using namespace std;

namespace asiow{


UdpPeer::UdpPeer(const std::string& local_ip, uint16_t local_port)
	:ContextTask(NetworkPool::instance().getThread())
	,_local_ip(local_ip)
	,_local_port(local_port)
{
}


UdpPeer::UdpPeer(uint16_t local_port)
	:UdpPeer("", local_port)
{}

UdpPeer::UdpPeer()
	:UdpPeer("", 0)
{}


bool UdpPeer::open()
{
	error_code ec;
	_socket.open(udp::v4(), ec);		
	if( ec )
	{
		return false;
	}

	if( !_local_ip.empty() && _local_port > 0)
		_socket.bind(udp::endpoint(ip::make_address(_local_ip), _local_port), ec );
	else if(_local_port > 0 )
		_socket.bind(udp::endpoint(udp::v4(), _local_port), ec );
	else
		_socket.bind(udp::endpoint(udp::v4(), 0), ec );

	if( ec )
	{
		return false;
	}

	
	doRecvFrom();	
	_isopen = true;
	return true;
	
}


void UdpPeer::close()
{
	udppeer_ptr shared_me = shared_from_this();
	_thread->post([shared_me](){
		shared_me->doClose(); 
		}
	);
	
}

void UdpPeer::doClose()
{
	_isopen = false;
	_onrecv_d = nullptr;
	_socket.close();
}

void UdpPeer::postSendto(const void* msg, size_t len, const UdpEndpoint& remote)
{
	//cout << "UdpPeer::postSendto thread:" << std::this_thread::get_id() << endl;
	udpitem_ptr item(new UdpItem(msg, len, remote));
	udppeer_ptr shared_me = shared_from_this();
	_thread->post([item, shared_me](){
		shared_me->doPostSendto(item); }
	);
}

void UdpPeer::doPostSendto(const udpitem_ptr& item)
{
	//cout<<"UdpPeer::doPostSendto thread:"<<std::this_thread::get_id()<<endl;
	if (!isOpen())
	{
		cout << "UdpPeer::doPostSendto() error: not open" << endl;
		return;
	}

	if (_item_queue.size() > 0)
	{
		_item_queue.push(item);
		return;
	}
	_item_queue.push(item);

	const udpitem_ptr& titem = _item_queue.front();
	udppeer_ptr shared_me = shared_from_this();
	UdpEndpoint ep = titem->remote;
	_socket.async_send_to(buffer(titem->data, titem->size), titem->remote, [shared_me, ep](const system::error_code& ec, size_t slen){
		shared_me->onSentTo(ec, slen, ep);
	}
	);
}

void UdpPeer::onSentTo(const system::error_code& ec, size_t slen, const UdpEndpoint& remote)
{
	//cout << "UdpPeer::onSentTo thread:" << std::this_thread::get_id() << endl;
	if (!isOpen())
		return;
		
	if (_onsent_d != nullptr)
		_onsent_d(remote, slen);

	_item_queue.pop();
	if (_item_queue.size() > 0)
	{
		const udpitem_ptr& titem = _item_queue.front();
		udppeer_ptr shared_me = shared_from_this();
		UdpEndpoint ep = titem->remote;
		//async_write ensure all data in buffer will be writen
		_socket.async_send_to(buffer(titem->data, titem->size), titem->remote, [shared_me, ep](const system::error_code& ec, size_t slen){
			shared_me->onSentTo(ec, slen, ep);
		}
		);
	}

	if (ec)
	{
		cout<<"UdpPeer::onSentTo error:"<<ec.message()<<endl;
	}
}

void UdpPeer::doRecvFrom()
{
	//printf("UdpPeer::doRecvFrom() onrecv from\n");
	udppeer_ptr peer = shared_from_this();
	_socket.async_receive_from(buffer(_recv_buf, MAX_RECV_SIZE), _sender_ep,
		[peer](error_code ec, size_t rlen)
		{
			if (!peer->_isopen)
			{
				return;
				//printf("UdpPeer::doRecvFrom() onrecv err %d, %s\n",ec.value(), ec.message().c_str());
			}
			
			if(!ec)
			{
				if(peer->_onrecv_d != nullptr)
				{
					UdpEndpoint remote = peer->_sender_ep;
					peer->_onrecv_d(remote, peer->_recv_buf, rlen);
				}
				
			}	
			else
				printf("UdpPeer::doRecvFrom() onrecv err %d, %s\n",ec.value(), ec.message().c_str());

			peer->doRecvFrom();
		}
	);
}


}

