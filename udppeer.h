///udppeer.h
//

#ifndef UDP_PEER_H
#define UDP_PEER_H

#include <functional>
#include <boost/asio.hpp>
#include "errordef.h"
#include "endpoints.h"
#include "context_task.h"
#include "utils.h"
#include <memory>
#include <queue>


namespace asiow{


class UdpPeer;
using udppeer_ptr = std::shared_ptr<UdpPeer>;

//not thread safy
class UdpPeer : public std::enable_shared_from_this<UdpPeer>
	      , public ContextTask<boost::asio::ip::udp::socket>
{

protected:
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

	static udppeer_ptr create(const std::string& local_ip, uint16_t port){
		return udppeer_ptr(new UdpPeer(local_ip, port) );
	}


	static udppeer_ptr create(uint16_t port){
		return udppeer_ptr(new UdpPeer(port) );
	}


	static udppeer_ptr create(){
		return udppeer_ptr(new UdpPeer() );
	}


	virtual ~UdpPeer(){
	}

	void listenOnRecv(const OnRecvDelegate& cb){ _onrecv_d = cb; }
	void listenOnSent(const OnSentDelegate& cb){ _onsent_d = cb; }

	virtual bool open();

	//use async sendto method
	void postSendto(const void* msg, size_t len, const UdpEndpoint& remote);

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

	///return local ip addr and port number.
	std::tuple<std::string, uint16_t> local_addr(){ return std::make_tuple(_local_ip, _local_port); }

protected:
	UdpPeer(const std::string& local_ip, uint16_t port);
	UdpPeer(uint16_t port);
	UdpPeer();


	std::string _local_ip;
	uint16_t _local_port;

	void doRecvFrom();
	void doSend(const void* msg, size_t len, const UdpEndpoint& remote);
	void doSend(const item_ptr& item, const UdpEndpoint& remote);

	void doPostSendto(const udpitem_ptr& item);
	void onSentTo(const boost::system::error_code& ec, size_t slen, const UdpEndpoint& remote);
	bool isOpen(){ return _isopen; }
	void doClose();

protected:
	OnRecvDelegate _onrecv_d=nullptr;
	OnSentDelegate _onsent_d = nullptr;

	udp::endpoint _sender_ep;

	static const int MAX_RECV_SIZE=1024*10;
	uint8_t _recv_buf[MAX_RECV_SIZE];

	std::queue<item_ptr, std::list<udpitem_ptr> > _item_queue;
	bool _isopen = false;
};

}


#endif /*UDP_PEER_H*/
