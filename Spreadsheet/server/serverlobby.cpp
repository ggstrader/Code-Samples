#include "Utilities.h"
#include "serverlobby.h"
#include <chrono>
#include <stdexcept>
#include <fstream>

using namespace std;
using namespace boost::asio;

ServerLobby::ServerLobby() : ServerLobby(FILE_DIRECTORY + "*.xml"){};
ServerLobby::ServerLobby(string alternateDirectory) : cmd_listener(), service(){
	print("\n		Initializing Master Server...\n\n");
	read_buffer = new Buffer();
	write_buffer = new Buffer();
	spread_sheets = Util::getDirectoryFiles(alternateDirectory);
	print("		Local spreadsheets:");
	for (auto name : spread_sheets) {
		print("  - " + name);
		servers[name] = new Server(name);
	}
  running = false;
}
ServerLobby::~ServerLobby() {
	delete read_buffer;
	delete write_buffer;
}


void ServerLobby::Start() {
  if(running) return;
	running = true;
	cmd_listener = thread([this]() { this->cmdListener(); });
	clientListener();
	service = thread([](){Connection::GetService().run();});
	print("\n		Master Server ready for incoming clients...\n\n");
	updateLoop();
}
void ServerLobby::updateLoop() {
	while (running) {
		for (MSG msg : *read_buffer)
			executeMessage(msg);
		if(!running) break;//a message can set running to false
		read_buffer->clear();
		Util::SwapPointers(read_buffer, write_buffer);
    this_thread::sleep_for(chrono::milliseconds(200));
	}
}
void ServerLobby::executeMessage(MSG msg) {
	if(!running) return;
	cmdMsg type = MCAST(msg, cmdMsg);
	switch (type) {
	case cmdMsg::EXIT: Shutdown(); break;
  case cmdMsg::INITIATE: initiateClient(msg[NAME], msg.client); break;
	case cmdMsg::LIST: for (string s : spread_sheets) print(s); break;
	case cmdMsg::DELETE: DeleteSpreadsheet(msg[NAME]) ?
			 print("Deleted " << msg[NAME] + "\n") : print("Deletion failed.\n"); break;
	}
	print("");
}


void ServerLobby::clientListener() {
	if (!running) return;
	Connection::pointer new_connection = Connection::GetConnection();
	new_connection->AsyncAccept(this);
}
void ServerLobby::StartHandshake(Connection::pointer client, const boost::system::error_code &error) {
  print("Client has made contact. Assigned id: " << client->id);
  try {
    if (error) throw 0;
		//Call SyncRead since Async callback loop isn't going yet
    MSG msg = Util::Parse(client->SyncRead());
    if (MCAST(msg, iMsg) != iMsg::REGISTER) { print("Registration failed"); throw 0; }
    print("Register received, sending available sheets.\n");
    client->SendData(Util::Message(oMsg::CONNECT, &spread_sheets));
    client->AsyncRead(this);//Starts read loop
  } catch (...) { print("Error communicating with client."); }
	clientListener();
}
void ServerLobby::Callback(Connection::pointer client, const boost::system::error_code &error) {
	if (!error) {
		print("Client #" << client->id <<" successfully accepted");
		MSG msg = Util::Parse(client->GetData());
		write_buffer->push_back(MSG(MSTR(cmdMsg::INITIATE), msg[NAME], "", client));
	}
	else {
		print("Error communicating with client.");
		client->Stop();
	}
}
void ServerLobby::initiateClient(string & file_name, Connection::pointer client) {
  try {
    Server *sheet;
    if (!Util::TryGetValue(file_name, servers, sheet)) {
			file_name += ".xml";
      sheet = new Server(file_name);
      servers[file_name] = sheet;
      spread_sheets.push_back(file_name);
      ofstream outfile(FILE_DIRECTORY + file_name);
    }
    sheet->InitiateClient(client);
		print("Client #"<< client->id<<" placed in spreadsheet: " << file_name);
  } catch (...) {
    print("Error in loading file, requesting again");
    client->SendData(Util::Message(oMsg::LOAD_ERROR));
    client->AsyncRead(this);
  }
}


void ServerLobby::cmdListener() {
  print("\nEnter 'exit' 'ls' or 'delete'");
  Connection::pointer dummy;
  while (running) {
    getline(cin, input);
    if (input == "ls")
      write_buffer->push_back(MSG(MSTR(cmdMsg::LIST),"",""));
    else if (input == "exit") {
      write_buffer->push_back(MSG(MSTR(cmdMsg::EXIT),"",""));
      break;
    } else if (input == "delete") {
      print("Enter spreadsheet name:\n");
      getline(cin, input);
      DeleteSpreadsheet(input);
    } else
      print("Invalid command.");
    print("");
  }
}

//causes a seg fault sometimes, but this isn't required to we
//spent time elsewhere
bool ServerLobby::DeleteSpreadsheet(std::string &file_name) {
	auto pair = servers.find(file_name);
	if (pair == servers.end())
	return false;
	try {
		delete pair->second;
		servers.erase(file_name);
		Util::DeleteValue(file_name, spread_sheets);
		// TODO: delete file
		return true;
	} catch (std::exception &e) {
		return false; // you done messed up
	}
}
void ServerLobby::Shutdown() {
	running = false;
	cmd_listener.join();
	Connection::GetService().stop();
	service.join();
	for (auto server : servers)
		delete server.second;
}
