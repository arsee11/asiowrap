///networkpool.h
//

#ifndef NETWORK_POOL_H
#define NETWORK_POOL_H

#include "endpoints.h"
#include <boost/asio.hpp>
#include "tcpclient.h"
#include "udppeer.h"
#include "tcpserver.h"
#include "network_thread.h"

namespace asiow{


	//extern boost::asio::io_context _io_context;
class NetworkPool
{
	using io_context = boost::asio::io_context;

public:
	static NetworkPool& instance(){
		static NetworkPool self;
		return self;
	}

	NetworkPool():_work(boost::asio::make_work_guard(_io_context))
	{}
	
	~NetworkPool(){
		if(_is_inited)
			uninit();
	}

	udppeer_ptr createUdpPeer(const UdpEndpoint& localep=UdpEndpoint()){
		udppeer_ptr udp = UdpPeer::create(_io_context, localep.address().to_string(), localep.port() );
		udp->set_thread(_thread.get());
		if( !udp->open() )
		{
			return nullptr;
		}
		return udp;
	}

	///@return MCastReceiver pointer, caller must manager the ownership.
	template<class MCaster>
	MCaster* createMCastReceiver(const UdpEndpoint& mcastep){
		MCaster* udp = new MCaster(_io_context, mcastep); 
		if( !udp->open() )
		{
			delete udp;
			return nullptr;
		}
		return udp;
	}

	///@return TcpServer pointer, caller must manager the ownership.
	template<class TCP>
	tcpserver_ptr createTcpServer(const TcpEndpoint& ep) {
		try {
			TCP *tcp = new TCP(_io_context, ep.port());
			tcp->set_thread(_thread.get());
			tcp->start();
			return tcpserver_ptr(tcp);
		}catch (boost::system::system_error& e) {
			cout << e.what() << endl;
			return nullptr;
		}
		
	}

	tcpclient_ptr createTcpClient(const TcpEndpoint& localep=TcpEndpoint()){
		tcpclient_ptr cli = TcpClient::create(_io_context, localep.address().to_string(), localep.port());
		if(!cli->open())
			return nullptr;

		cli->set_thread(_thread.get());
		return cli;
	}

	///@return Timer pointer, caller must manager the ownership.
	template<class Timer>
	Timer* createTimer(int expire_ms){
		Timer* t = new Timer(_io_context, expire_ms );
		return t;
	}

	void init();
	void uninit();

private:
	void start();

private:
	io_context _io_context;
	boost::asio::executor_work_guard<io_context::executor_type>  _work;
	std::unique_ptr<NetworkThread>  _thread;
	bool _is_inited=false;
};

}//asiow

#endif /*NETWORK_POOL_H*/
