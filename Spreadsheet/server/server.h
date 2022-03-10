#ifndef SPD_SERVER
#define SPD_SERVER

#include "cell.h"
#include "Utilities.h"
#include "networking.h"
#include <thread>
#include <set>
#include <mutex>


/** Order of Events:
TCP LISTENER
      1. InitiateClient called by Lobby.
      2. Add client to collection and set callback to callback
      3. Callback Parses message; Executes if client specific; Adds to
        write_buffer if spreadsheet specific.
MAIN THREAD
      1. Start is called by Lobby's Main Thread, create the Server's main thread
        using the updateLoop.
      2. UpdateLoop updates the spreadsheet, in order to do that it:
        executes a message; sends a change to the clients; saves the change;
        repeats till read buffer is empty; swaps the read and write buffers.
**/

class Server : ICallback {
	typedef std::map<std::string, Cell *> CellMap;
  typedef std::vector<Cell *> CellVec;
  typedef std::map<std::string,Connection::pointer> ClientMap;
  typedef std::vector<MSG> Buffer;
private:
	bool is_active;
	ClientMap * clients;
	std::set<Connection::pointer> disconnects;
	CellVec * global_history;
	CellMap * cells;
	Buffer * read_buffer;
	Buffer * write_buffer;
	std::thread server_main;
	int pings_before_disconnect = 6;
	int time_out_sec = 10;
	//Makeshift timer based on sleeptime (who says' it's gotta be accurate?)
	int sleep_time_ms = 100;
	int ping_time = ((float)1000/sleep_time_ms)*time_out_sec;

public:
	std::string sheet_name;

	Server(std::string);
	~Server();
	void Start();
	void InitiateClient(Connection::pointer);
	void Callback(Connection::pointer, const boost::system::error_code &) override;
	void SaveSpreadsheet();
	std::vector<std::string> getFullState();

private:
	void updateLoop();
	void executeMessage(MSG &);
	std::string undo(std::string &);
	void updateSpreadsheet();
	void updateClients(MSG &);
	void updatePings(int&);
	void handleDisconnects();
	void loadSpreadsheet(std::string);
	void close();
};

#endif // SPD_SERVER
