#include "Utilities.h"
#include "sstream"
#include "server.h"
#include <glob.h>
#include <algorithm>

using namespace std;


/**Messaging protocol is to **append a space to each command**, append a newline
to all contents but the last, and append the Delimiter to evry complete message**/
string Util::Message(oMsg type, vector<string> *args) {
  stringstream message;
  switch (type) {
    case oMsg::PING: message << "ping "; break;
    case oMsg::FOCUS: message << "focus "; break;
    case oMsg::UNFOCUS: message << "unfocus "; break;
    case oMsg::CHANGE: message << "change "; break;
    case oMsg::FULL_STATE: message << "full_state "; break;
    case oMsg::DISCONNECT: message << "disconnect "; break;
    case oMsg::RESPONSE: message << "ping_response "; break;
    case oMsg::CONNECT: message << "connect_accepted "; break;
    case oMsg::LOAD_ERROR: message << "file_load_error "; break;
  }

  if (args)
    for (string str : *args)
      message << str << '\n';
  string result = message.str();
  if(type == oMsg::UNFOCUS) result = result.substr(0,result.length() - 1);
  if(args && !args->empty()) result.pop_back(); //remove last newline
  return result += DELIMITER;
}

string Util::Message(oMsg type, vector<string> list) {
	return Message(type, &list);
}

MSG Util::Parse(string inbound) {

	MSG msg{"", "", ""};
	vector<string> v;
	istringstream buffer(inbound);
	for (string word; buffer >> word;)
		v.push_back(word);

	if (v[TYPE] == "ping") msg[TYPE] = MSTR(iMsg::PING);
	else if (v[TYPE] == "undo") msg[TYPE] = MSTR(iMsg::UNDO);
	else if (v[TYPE] == "unfocus") msg[TYPE] = MSTR(iMsg::UNFOCUS);
	else if (v[TYPE] == "register") msg[TYPE] = MSTR(iMsg::REGISTER);
	else if (v[TYPE] == "disconnect") msg[TYPE] = MSTR(iMsg::DISCONNECT);
	else if (v[TYPE] == "ping_response") msg[TYPE] = MSTR(iMsg::RESPONSE);
	else if (v[TYPE] == "load") { msg[TYPE] = MSTR(iMsg::LOAD); msg[NAME] = v[NAME];}
	else if (v[TYPE] == "revert") {
		msg[TYPE] = MSTR(iMsg::REVERT);
		msg[NAME] = v[NAME];
	}
	else if (v[TYPE] == "focus") {
		msg[TYPE] = MSTR(iMsg::FOCUS);
		msg[NAME] = v[NAME];
	}
	else if (v[TYPE] == "edit") {
		msg[TYPE] = MSTR(iMsg::EDIT);
		string temp = v[NAME];
		int colon = temp.find(":");
		msg[NAME] = temp.substr(0, colon);
		msg[CONTENTS] = temp.substr(colon + 1);
	}
  msg[NAME] = msg[NAME].substr(0, msg[NAME].find(DELIMITER));
  msg[CONTENTS] = msg[CONTENTS].substr(0, msg[CONTENTS].find(DELIMITER));
	return msg;
}

vector<string> Util::getDirectoryFiles(const string &pattern) {
  int length = pattern.find("*");
	glob_t glob_result;
	glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
	vector<string> files;
	for (unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
		string local_path = string(glob_result.gl_pathv[i]);
		files.push_back(local_path.erase(0, length));
	}
	globfree(&glob_result);
	return files;
}

void Util::DeleteValue(std::string & value, std::vector<std::string> & vec)
{
  auto itr = std::find(vec.begin(), vec.end(), value);
  if (itr != vec.end()) vec.erase(itr);
}

vector<string> Util::Split(string s, char delim) {
  stringstream ss(s);
  string item;
  vector<string> tokens;
  while (getline(ss, item, delim)) tokens.push_back(item + (char)DELIMITER);
  return tokens;
}
