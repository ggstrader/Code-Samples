#include <iostream>
#include <map>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "networking.h"

using namespace boost;
using namespace boost::asio;
using std::istream;
using std::string;
using std::istreambuf_iterator;

long long Connection::unique_id = 0;
Event<void(Connection::pointer)> Connection::eDisconnect;

boost::asio::ip::tcp::acceptor & Connection::GetAcceptor()
{
  static ip::tcp::acceptor acc(Connection::GetService(),ip::tcp::endpoint(ip::tcp::v4(), 2112));
  return acc;
}
boost::asio::io_service & Connection::GetService()
{
  static io_service service;
  return service;
}
/**Connection lazy initializes the io_service and acceptor. This is thread sa
 * fe
  since the first Connection is created syncronously**/
Connection::Connection() :
      service(Connection::GetService()),
      acceptor(Connection::GetAcceptor()),
      socket(Connection::GetService())
{
  id = SSTR(unique_id++);
}
Connection::~Connection() { }

Connection::pointer Connection::GetConnection() {
  return Connection::pointer(new Connection());
}

void Connection::AsyncAccept(IHandshake *caller) {
  pointer client = shared_from_this();
  acceptor.async_accept(socket, [caller,client](const boost::system::error_code& err){caller->StartHandshake(client, err);});
}

void Connection::AsyncRead(ICallback *caller) {
  pointer client = shared_from_this();
  async_read_until(socket, input_buffer, DELIMITER, [caller, client](const boost::system::error_code& err, std::size_t length){caller->Callback(client,err);} );
}
string Connection::SyncRead()
{
  boost::system::error_code err;
  read_until(socket, input_buffer, DELIMITER,  err);
  return GetData();
}

string Connection::GetData() {
  string msg((istreambuf_iterator<char>(&input_buffer)), istreambuf_iterator<char>());
  return msg;
}

void Connection::SendData(string _message) {
  try{
    async_write(socket, buffer(_message), bind(&Connection::SendDataCallback, shared_from_this()));
  }
  catch(...)
  {
    //triggering an event allows the networking code to be decoupled from server code
    Connection::eDisconnect(shared_from_this());
  }
}
void Connection::SendDataCallback() {}

void Connection::Stop() {
  print("Closing socket...\n");
  socket.close();
}

void Connection::ResetPings() {
  pings_since_response = 0;
}
