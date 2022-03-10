#ifndef UTILITIES
#define UTILITIES

#include <array>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "networking.h"

//Simple macro for conversion to string obtained from Stack Overflow
#define SSTR( x ) static_cast< std::ostringstream & >( \
  ( std::ostringstream() << std::dec << x ) ).str()
#define print(x) std::cout << x << std::endl
#define MCAST(msg, type) static_cast<type>(std::stoi(msg[TYPE]))
#define MSTR(type) SSTR((int)type)

static int DELIMITER = '\u0003';
static std::string FILE_DIRECTORY = "files/";

class Connection;
typedef boost::shared_ptr<Connection> pointer;

//Input, Output, and Command Messages
enum class iMsg { REGISTER, INITIATE, DISCONNECT, LOAD, PING, RESPONSE, EDIT, REVERT, UNDO, FOCUS, UNFOCUS };
enum class oMsg { CONNECT, LOAD_ERROR, DISCONNECT, PING, RESPONSE, FULL_STATE, CHANGE, FOCUS, UNFOCUS };
enum class cmdMsg { EXIT, LIST, DELETE, INITIATE };
//used for explicit MSG indeces access
enum INDEX {TYPE,NAME,CONTENTS};
/** MSG Expects messages in form [iMsg, cellName/sheetName, Contents] **/
struct MSG {
  pointer client;
  std::array<std::string, 3> attrib;
  MSG(std::string a, std::string b, std::string c) : attrib({a,b,c}) {}
  MSG(std::string a, std::string b, std::string c, pointer p) :
  attrib({a,b,c}) { client = p;}
  std::string & operator[](INDEX i){
    return attrib[i];
  }
};

class Util {

public:
	static std::string Message(oMsg, std::vector<std::string> * = NULL);
	static std::string Message(oMsg, std::vector<std::string>); //for using initializer list
	static MSG Parse(std::string);
    /** adapted from SO, Expects path containing '*' **/
	static std::vector<std::string> getDirectoryFiles(const std::string &);
  static void DeleteValue(std::string &, std::vector<std::string> &);

  template<typename T>
  static void SwapPointers(T *& current, T *& next) {
  	T *temp = current;
  	current = next;
  	next = temp;
  }

	template <typename T>
	static T *TryGetCreate(std::string key, std::map<std::string, T *> t_collection) {
		auto pair = t_collection.find(key);
		T *t_value;
		if (pair == t_collection.end()) {
			t_value = new T();
			t_collection[key] = t_value;
		} else
			t_value = pair->second;
		return t_value;
	}

  template <typename T>
	static bool TryGetValue(std::string key, std::map<std::string, T *> t_collection, T *& out_value) {
		auto pair = t_collection.find(key);
		if (pair != t_collection.end()) {
			out_value = pair->second;
      return true;
		} else return false;
	}

	static std::vector<std::string> Split(std::string, char);
};



#endif // UTILITIES
