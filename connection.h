///connection.h
//

#ifndef CONNECTION_H
#define CONNECTION_H

#include <functional>
#include <memory>
#include <tuple>
#include <cstdint>
#include <queue>
#include <list>
#include <boost/asio.hpp>

#include "errordef.h"
#include "utils.h"
#include "context_task.h"
		
namespace asiow{

class Connection;

using connection_ptr = std::shared_ptr<Connection>;

///call back when recv message from Connection
using OnRecvDelegate = std::function<void (const connection_ptr&, void*, size_t) >;

///call back when sent message completed
using OnSentDelegate = std::function<void(const connection_ptr&, size_t) >;

///call back when Connection raise an error
using OnErrorDelegate = std::function<void(const connection_ptr&, NetError) >;

///call back when a Connection connected  to the  Server
using OnConnectedDelegate = std::function<void (const connection_ptr&)>;


///not thread safy
class Connection : public std::enable_shared_from_this<Connection>
	         , public ContextTask<boost::asio::ip::tcp::socket>
{

public:
	Connection(socket&& sock);

	virtual ~Connection();

	void start(){ doReceive();}
	void postSend(const void* msg, size_t len);
	void close();
	bool isOpen(){ return _isopen; }
	int fd(){ return _socket.native_handle();}

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

	void listenOnRecv( OnRecvDelegate recvd){ _onrecv_d = recvd; }
	void listenOnError( OnErrorDelegate errd){ _onerror_d = errd; }
	void listenOnSent( OnSentDelegate sentd){ _onsent_d = sentd; }

	std::tuple<std::string, uint16_t> local_addr(){return std::make_tuple("", 0);} 
	std::tuple<std::string, uint16_t> remote_addr(){return std::make_tuple(_remote_ip, _remote_port);} 

	socket& getSocket(){ return this->_socket; }
protected:
	void doReceive();

private:
	void onSent(const boost::system::error_code& ec, size_t slen);
	void doPostSend(const item_ptr& item);
	void doClose();

private:
	static const int MAX_RECV_SIZE=1024*1024;
	uint8_t _recv_buf[MAX_RECV_SIZE];
	
	bool _isopen=false;
	std::queue<item_ptr, std::list<item_ptr> > _item_queue;
	OnRecvDelegate _onrecv_d;
	OnSentDelegate _onsent_d;
	OnErrorDelegate _onerror_d;

	std::string _remote_ip;
	uint16_t _remote_port;
};


}

#endif /*CONNECTION_H*/
