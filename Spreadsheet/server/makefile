all: Server
Server: server_main.o networking.o serverlobby.o server.o cell.o Utilities.o tiny.o
	g++ -pthread server_main.o networking.o serverlobby.o server.o cell.o Utilities.o tinyxml.o tinyxmlparser.o tinyxmlerror.o /usr/local/lib/libboost_system.a -o a.out
server_main.o: server_main.cpp
	g++ -c -std=c++11 server_main.cpp
networking.o: networking.cpp networking.h
	g++ -c -std=c++11 networking.cpp
serverlobby.o: serverlobby.cpp serverlobby.h
	g++ -c -std=c++11 -pthread serverlobby.cpp
server.o: server.cpp server.h
	g++ -c -std=c++11 -pthread server.cpp
cell.o: cell.cpp cell.h
	g++ -c -std=c++11 cell.cpp
Utilities.o: Utilities.cpp Utilities.h
	g++ -c -std=c++11 Utilities.cpp
tiny.o: tinyxml.cpp tinyxmlparser.cpp tinyxmlerror.cpp
	g++ -c -std=c++11 tinyxml.cpp tinyxmlparser.cpp tinyxmlerror.cpp
clean:
	rm -rf *.o
