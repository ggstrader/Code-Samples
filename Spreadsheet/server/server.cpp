#include "tinyxml.h"
#include "Utilities.h"
#include "server.h"
#include <chrono>
using namespace std;

Server::Server(std::string filename) : server_main(){
  cells = new CellMap();
  clients = new ClientMap();
  write_buffer = new Buffer();
  read_buffer = new Buffer();
  global_history = new CellVec();
	sheet_name = filename;
	is_active = false;
  //Add this sheet as a listener for disconnect events
  Connection::eDisconnect += [this](Connection::pointer p) {
    write_buffer->push_back(MSG(MSTR(iMsg::DISCONNECT), "", "", p));
  };
}
Server::~Server() {
	close();
  delete cells;
	delete clients;
  delete write_buffer;
  delete read_buffer;
	delete global_history;
}

void Server::Start() {
  loadSpreadsheet(sheet_name);
	is_active = true;
  print("   Started spreadsheet: " << sheet_name);
  server_main = thread([this](){this->updateLoop();});
  server_main.detach();
}
void Server::updateLoop(){
  int updates = 0;
  while(write_buffer->size() < 1);//wait for first client so we don't disconnect in ping update
  while (is_active) {
    this_thread::sleep_for(chrono::milliseconds(sleep_time_ms));
    updateSpreadsheet();
    updatePings(updates);
    updates++;
    handleDisconnects();
	}
}
void Server::updateSpreadsheet() {
	for (MSG msg : *read_buffer) {
		executeMessage(msg);
		updateClients(msg);
		SaveSpreadsheet();
	}
	read_buffer->clear();
	Util::SwapPointers(read_buffer, write_buffer);
}
void Server::executeMessage(MSG &msg) {
  iMsg type = MCAST(msg,iMsg);
	switch (type) {
  case iMsg::FOCUS: case iMsg::UNFOCUS: break;
  case iMsg::INITIATE: (*clients)[msg.client->id] = msg.client; break;
  case iMsg::DISCONNECT: disconnects.insert(msg.client); break;
  case iMsg::UNDO: msg[NAME] = undo(msg[CONTENTS]); break;
	default:
    Cell *cell;
    auto pair = cells->find(msg[NAME]);
    if(pair == cells->end())
    {
      cell = new Cell(msg[NAME]);
      cells->insert(std::pair<string,Cell*>(cell->name,cell));
    }
    else cell = pair->second;
		global_history->push_back(cell);
		switch (type) {
		case iMsg::EDIT: cell->Edit(msg[CONTENTS]); break;
		case iMsg::REVERT: cell->Revert(msg[CONTENTS]); break;
		}
	}
}
void Server::updateClients(MSG &_change) {
  iMsg type = MCAST(_change, iMsg);
	string message;
	string change = _change[NAME] + ':' + _change[CONTENTS];
	switch(type)
	{
    case iMsg::UNDO:
    case iMsg::EDIT:
    case iMsg::REVERT: message = Util::Message(oMsg::CHANGE, {change}); break;
		case iMsg::FOCUS: message = Util::Message(oMsg::FOCUS, {change}); break;
    case iMsg::UNFOCUS: message = Util::Message(oMsg::UNFOCUS, {change}); break;
	}
	for (auto pair : *clients)
		pair.second->SendData(message);
}
void Server::updatePings(int & updates)
{
  if(updates < ping_time) return;
  updates = 0;
  for (auto pair : *clients){
    pair.second->SendData(Util::Message(oMsg::PING));
    if(pair.second->pings_since_response > pings_before_disconnect){
      pair.second->pings_since_response += 1;
      Connection::eDisconnect(pair.second);
    }
  }
}

void Server::handleDisconnects()
{
  for(Connection::pointer client : disconnects){
    print("Disconnected client #" << client->id);
    client->SendData(Util::Message(oMsg::DISCONNECT));
    client->Stop();
    clients->erase(client->id);
  }
  disconnects.clear();
  //if(clients->size() < 1) close();
}

void Server::InitiateClient(Connection::pointer client) {
  if (!is_active) Start();
	write_buffer->push_back(MSG(MSTR(iMsg::INITIATE), "", "", client));
	client->SendData(Util::Message(oMsg::FULL_STATE, getFullState()));
  client->AsyncRead(this);
}
void Server::Callback(Connection::pointer client, const boost::system::error_code &error) {
  if(!is_active) return;
  if (error) { Connection::eDisconnect(client); return; }
  string inbound = client->GetData();
  vector<string> messages = Util::Split(inbound, DELIMITER);
  for(string message : messages){
	  MSG msg = Util::Parse(message);
    iMsg type = MCAST(msg, iMsg);
	  switch (type) {
	  	case iMsg::DISCONNECT: Connection::eDisconnect(client);	break;
		  case iMsg::RESPONSE: client->ResetPings(); break;
		  case iMsg::PING: client->SendData(Util::Message(oMsg::RESPONSE));	break;
      case iMsg::FOCUS: msg[CONTENTS] = client->id; write_buffer->push_back(msg); break;
      case iMsg::UNFOCUS: msg[NAME] = client->id; write_buffer->push_back(msg); break;
	  	default:
	    	write_buffer->push_back(msg); // pushback is thread-safe so no need to lock
	}
  }

  client->AsyncRead(this);
}


vector<string> Server::getFullState() {
  vector<string> state;
  for (auto pair : *cells) {
    string contents = pair.first + ':' + pair.second->GetCurrent();
    state.push_back(contents);
  }
  return state;
}
string Server::undo(string & contents) {
  if(global_history->empty()) return "A1";//send random cell with blank contents
  Cell * last_edited = global_history->back();
  string name = last_edited->name;
  last_edited->Undo(contents);
  global_history->pop_back();
  return name;
}


void Server::SaveSpreadsheet() {
	TiXmlDocument doc;
	for (auto pair : *cells) {
    string current = pair.second->GetCurrent();
    if(current == "") continue;
		TiXmlElement *cell_name = new TiXmlElement(pair.first);
		TiXmlText *contents = new TiXmlText(current);
		cell_name->LinkEndChild(contents);
		doc.LinkEndChild(cell_name);
	}
	doc.SaveFile(FILE_DIRECTORY + sheet_name);
}
void Server::loadSpreadsheet(std::string file) {
	TiXmlDocument doc = TiXmlDocument(FILE_DIRECTORY + file);
	bool loaded = doc.LoadFile();
	if (loaded) {
		while (!doc.NoChildren()) {
			TiXmlElement *current_node = doc.FirstChildElement();
			string key = current_node->ValueStr();
			string contents = current_node->FirstChild()->ValueStr();
			(*cells)[key] = new Cell(key, contents);
			doc.RemoveChild(current_node);
		}
	}
}
void Server::close() {
  is_active = false;
  for(auto pair : *clients) {
    pair.second->Stop();
    clients->erase(pair.second->id);
  }
  for(auto pair : *cells)
    delete pair.second;
    print("Killed Server");
}
