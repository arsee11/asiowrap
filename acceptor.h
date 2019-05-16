///acceptor.h
//

#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <functional>
#include <memory>
#include <cstdint>
#include "connection.h"
#include "context_task.h"

namespace asiow{

class Acceptor;

using acceptor_ptr = std::shared_ptr<Acceptor>;

class Acceptor: public std::enable_shared_from_this<Acceptor>
	      , public ContextTask<FakeSocket>
{
protected:
	using acceptor = boost::asio::ip::tcp::acceptor;
	using endpoint = boost::asio::ip::tcp::endpoint;
	using error_code = boost::system::error_code;
	using tcp = boost::asio::ip::tcp;
	
public:
	static acceptor_ptr create(const std::string& local_ip, uint16_t port){
		return acceptor_ptr(new Acceptor(local_ip, port) );
	}

	static acceptor_ptr create(uint16_t port){
		return acceptor_ptr(new Acceptor(port) );
	}



	virtual ~Acceptor(){
	}

	void listenOnConnected(const OnConnectedDelegate& cb);

	void start();
	void close();

	///return local ip addr and port number.
	std::tuple<std::string, uint16_t> netaddr();
	int fd(){ return _acceptor->native_handle(); }


private:
	Acceptor(const std::string& local_ip, uint16_t port);
	Acceptor(uint16_t port);

	void doClose();
	void doAccept();

	OnConnectedDelegate _onconn_d=nullptr;
	std::unique_ptr<acceptor> _acceptor;
};

}


#endif /*ACCEPTOR_H*/
