At a high level, this project demonstrates understanding dependency graphs and a general networking protocol

1. I created the Formula, Dependency Graph, and Spreadsheet logic.
2. On a team of two, we created the original spreadsheet GUI with an added feature for Conditional Formatting.

3. Later, on a team of four, I created the server which communicated with other student's clients
4. My team modified one of their spreadsheets to be a client and function with the server based on the protocol

Key Concepts of the Server:
1. Server is split into a Lobby which holds a collection of Spreadsheet Servers.
2. The Lobby is responsible for listening to command line input and beginning the handshake for incomming connections.
3. It then hands off the handshake to a new Server thread.
4. Each Server is single threaded and processes messages for it's own clients.
5. Server Lobby and Servers all have their own main threads which have Double buffered I/O for resonsiveness 
6. The main thread of each Lobby/Server executes input and creates output. Sequential execution prevents race conditions and the need for locking


