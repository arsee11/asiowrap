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

protected:
	void doReceive();

private:
	void onSent(const boost::system::error_code& ec, size_t slen);
	void doPostSend(const item_ptr& item);
	void doClose();

private:
	static const int MAX_RECV_SIZE=65536;
	uint8_t _recv_buf[MAX_RECV_SIZE];
	
protected:
	bool _isopen=false;
	std::queue<item_ptr, std::list<item_ptr> > _item_queue;
	OnRecvDelegate _onrecv_d;
	OnSentDelegate _onsent_d;
	OnErrorDelegate _onerror_d;
};


}

#endif /*CONNECTION_H*/
