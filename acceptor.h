///acceptor.h
//

#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <functional>
#include <memory>
#include <cstdint>
#include "connection.h"

namespace asiow{

class Aceeptor;

using acceptor_ptr = std::shared_ptr<Aceeptor>;

class Aceeptor: public std::enable_shared_from_this<Aceeptor>
{
protected:
	using acceptor = boost::asio::ip::tcp::acceptor;
	using socket = boost::asio::ip::tcp::socket;
	using io_context = boost::asio::io_context;
	using endpoint = boost::asio::ip::tcp::endpoint;
	using error_code = boost::system::error_code;
	using tcp = boost::asio::ip::tcp;
	
public:
	static acceptor_ptr create(io_context& ioc, const std::string& local_ip, uint16_t port){
		return acceptor_ptr(new Aceeptor(ioc, local_ip, port) );
	}

	static acceptor_ptr create(io_context& ioc,  uint16_t port){
		return acceptor_ptr(new Aceeptor(ioc, port) );
	}


	Aceeptor(io_context& ioc, const std::string& local_ip, uint16_t port);
	Aceeptor(io_context& ioc, uint16_t port);

	virtual ~Aceeptor(){};

	void listenOnConnected(const OnConnectedDelegate& cb);
	void setExecutor(ExeScope* e){ _executor = e; }

	void start();
	void close();

	///return local ip addr and port number.
	std::tuple<std::string, uint16_t> netaddr();
	int fd(){ return _acceptor.native_handle(); }


private:

	void doClose();
	void doAccept();

	OnConnectedDelegate _onconn_d=nullptr;
	acceptor _acceptor;
	ExeScope _executor=nullptr;
};

}


#endif /*ACCEPTOR_H*/
