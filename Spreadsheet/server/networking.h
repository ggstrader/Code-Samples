#ifndef XL_EXCEL_NETWORKING_H
#define XL_EXCEL_NETWORKING_H

#include <string>
#include <queue>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "Utilities.h"
#include "Event.h"

class Connection;

struct IHandshake
{
  virtual void StartHandshake(boost::shared_ptr<Connection>, const boost::system::error_code &) = 0;
};

/**Interface added to decouple server lobby and server from networking code.**/
struct ICallback
{
  virtual void Callback(boost::shared_ptr<Connection>, const boost::system::error_code &) = 0;
};


/** Used by servers for communicating with clients **/
class Connection : public boost::enable_shared_from_this<Connection> {
public:
	typedef boost::shared_ptr<Connection> pointer;

private:
	static long long unique_id;
	boost::asio::io_service & service;
	boost::asio::ip::tcp::acceptor & acceptor;
	boost::asio::ip::tcp::socket socket;
	boost::asio::streambuf input_buffer;
	//TODO: Timer ping_timer;

public:
  static Event<void(pointer)> eDisconnect;
  int pings_since_response;
	std::string id;
	~Connection();
	static pointer GetConnection();

  void AsyncAccept(IHandshake*);
  void AsyncRead(ICallback*);
  std::string SyncRead();
	std::string GetData();
	void SendData(std::string);
	void ResetPings();
  void Stop();

  static boost::asio::io_service & GetService();
private:
	Connection();
	void SendDataCallback();
	void handleRead(const boost::system::error_code &e);
  static boost::asio::ip::tcp::acceptor & GetAcceptor();
};

#endif // XL_EXCEL_NETWORKING_H
