#ifndef SERVER_LOBBY
#define SERVER_LOBBY

#include "Utilities.h"
#include "networking.h"
#include "server.h"
#include <map>
#include <string>
#include <vector>
#include <thread>

/**Order of events:
MAIN THREAD
      1. ServerLobby is created, initializing all servers using spreadsheets
        in the default/given directory.
      2. Start() is called.
      3. The lobby creates a Command listener on a separate thread; starts an
      asyncronous TCP listener loop; and runs the updateLoop on the main thread.
      4. Commands from all listener threads are written to the write buffer.
      5. The updateLoop reads and executes commands syncronously from the read
        buffer on the main thread.
TCP LISTENER
      1. Creates Connection with 'StartHandshake' as the callback:
      2. StartHandshake sends connect_accepted and avaiilable sheets
      3. Callback is set to 'Callback' and we start a new loop.
      4. Callback adds INITIATE to the wrtie_buffer and terminates current loop.
      EXTRA: on the main thread
      1. Creates and saves a new sheet if requested doesn't exist, starts the
        server if not running, hands off Connection to server.
      2. If there are problems executing INITIATE, a LOAD_ERROR is sent and
      we re-request another load command from the same client.

  NOTE: There is only ever really one TCP thread, calls are: clientListener ->
    StartHandshake -> clientListener ->StartHandshake etc. So Connections are
    essentially created synchronously, eliminating the need for locking
**/
class ServerLobby : ICallback, IHandshake {
  typedef std::vector<MSG> Buffer;
private:
  std::vector<std::string> spread_sheets;
  std::map<std::string, Server *> servers;
  Buffer *read_buffer;
  Buffer *write_buffer;
  std::string input;
  bool running;
  std::thread cmd_listener;
  std::thread service;

public:
  ServerLobby();
  ServerLobby(std::string);
  ~ServerLobby();
  void Start();
private:
  void Shutdown();
  void StartHandshake(Connection::pointer, const boost::system::error_code &) override;
  void Callback(Connection::pointer, const boost::system::error_code &) override;
  bool DeleteSpreadsheet(std::string &);

  void updateLoop();
  void executeMessage(MSG);
  void initiateClient(std::string &, Connection::pointer);
  void cmdListener();
  void clientListener();
  std::vector<std::string> getDirectoryFiles(const std::string &pattern);
};

#endif // SERVER_LOBBY
