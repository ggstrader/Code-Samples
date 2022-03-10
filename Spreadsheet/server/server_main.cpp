/*
////////////////////   NOTE   ///////////////////
One of the biggest design decisions of this server is
double buffering the Lobby and Sheet servers. All Commands
from every non-main thread are delayed by putting them in
the lobby or individual server's write_buffer. Then they
are executed sequentially from the read_buffer by a single
thread.

This decision was made to avoid any problems whatsoever
with concurrency, and you will find there is not a single
lock in this project as a result. push_back into a vector
is thread safe so all messages are executed in the exact
order that they make it to push_back.
/////////////////////////////////////////////




---- Coding Style ----
Formatting:
- Types (class, typedef, template)
    MyType
- public functions
    MyFunc();
-private functions
    myFunc();
-parameters, arguments, variables
    my_variable

Header order:
- Includes
- Macros
- global typedefs
- global variables and enum classes
- Class (smaller first):
    - private class/struct forward declarations
    - private typedefs
    - public typedefs
    - private member variables
    - public member variables
    - public constructor/destructor/function declarations
    - private constructor/destructor/function declarations
    - any necessary public definitions
    - any necessary private definitions

CPP order:
- Includes
- Usings
- static variable definitions
- group definitions by conceptual similarity and order of execution
- local helper functions
*/

#include "serverlobby.h"
#include <iostream>

using namespace std;


int main(int ac, char **av) {
  ServerLobby lobby;
  lobby.Start();
	return 0;
}
