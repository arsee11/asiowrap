///udppeer.h
//

#ifndef UDP_PEER_H
#define UDP_PEER_H

#include <functional>
#include <boost/asio.hpp>
#include "errordef.h"
#include "endpoints.h"
#include "network_thread.h"
#include "utils.h"
#include <memory>
#include <queue>


namespace asiow{


class UdpPeer;
using udppeer_ptr = std::shared_ptr<UdpPeer>;

class UdpPeer : public std::enable_shared_from_this<UdpPeer>
{
public:
	using io_context= boost::asio::io_context;

protected:
	using socket = boost::asio::ip::udp::socket;
	using error_code = boost::system::error_code;
	using udp = boost::asio::ip::udp;

public:
	///recv msg callback 
	///@param 1 the msg recv from network, ower by the UdpPeer instnace, not thread safe.
	///       if caller want to push the msg to another thread, must make a copy.
	///@param 2 size of msg in bytes.  
	using OnRecvDelegate = std::function<void (const UdpEndpoint&, void*, size_t) >;

	using OnSentDelegate = std::function<void(const UdpEndpoint&, size_t) >;

public:
	static udppeer_ptr create(io_context& ioc, const std::string& local_ip, uint16_t port){
		return udppeer_ptr(new UdpPeer(ioc, local_ip, port) );
	}

	static udppeer_ptr create(io_context& ioc, uint16_t port){
		return udppeer_ptr(new UdpPeer(ioc, port) );
	}

	static udppeer_ptr create(io_context& ioc ){
		return udppeer_ptr(new UdpPeer(ioc ) );
	}

	virtual ~UdpPeer(){}

	void listenOnRecv(const OnRecvDelegate& cb){ _onrecv_d = cb; }
	void listenOnSent(const OnSentDelegate& cb){ _onsent_d = cb; }

	virtual bool open();

	//use async sendto method
	void postSendto(const void* msg, size_t len, const UdpEndpoint& remote);

	void sendto(const void* buf, size_t size, const UdpEndpoint& remote);

	void close();

	size_t sentQueueSize(){
			return 0;
	}

	//boost::asio options
	template<class Opt>
	bool setOption(const Opt& opt) {
		try {
			_socket.set_option(opt);
			return true;
		}
		catch (boost::system::system_error& e) {
			//cout << e.what() << endl;
			return false;
		}
	}

	int fd(){ return _socket.native_handle(); }

	void set_thread(NetworkThread* val){ _thread= val; }

	///return local ip addr and port number.
	std::tuple<std::string, uint16_t> local_addr(){ return std::make_tuple(_local_ip, _local_port); }

protected:
	UdpPeer(io_context& ioc, const std::string& local_ip, uint16_t port);
	UdpPeer(io_context& ioc, uint16_t port);
	UdpPeer(io_context& ioc );

	std::string _local_ip;
	uint16_t _local_port;

	void doRecvFrom();
	void doSend(const void* msg, size_t len, const UdpEndpoint& remote);
	void doSend(const item_ptr& item, const UdpEndpoint& remote);

	void doPostSendto(const udpitem_ptr& item);
	void onSentTo(const boost::system::error_code& ec, size_t slen, const UdpEndpoint& remote);
	bool isOpen(){ return _isopen; }

protected:
	OnRecvDelegate _onrecv_d=nullptr;
	OnSentDelegate _onsent_d = nullptr;

	socket _socket;
	udp::endpoint _sender_ep;

	static const int MAX_RECV_SIZE=1024*10;
	uint8_t _recv_buf[MAX_RECV_SIZE];

	NetworkThread* _thread=nullptr;

	std::queue<item_ptr, std::list<udpitem_ptr> > _item_queue;
	bool _isopen = false;
};

}


#endif /*UDP_PEER_H*/
