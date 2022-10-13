# TCPex

The class cTCPex is a C++ wrapper for Windows TCP sockets.

Demo programs

- `client_console`  Connects to server and sends complete lines when typed to server.
- `client_gui` Connects to server.  Sand lines to server on button click.  Displays server replies.
- `server_echo_console`  Accepts single client connection.  Echoes messages received back to client.
- `server_echo_gui` Accepts single client connection.  Displays and echoes messages received back to client.
- `server_job_gui` Accepts multiple clients.  Performs jobs of specified duration.  Option to process jobs in one shared thread or in seperate threads for each client.
- `bbserver` Accepts multiple clients.  Writes messages to a file. Documentation at https://github.com/JamesBremner/tcpex/blob/main/doc/C__%20project%20(1).pdf
- `server_relay` Accepts two clients on different ports.  Anything sent from one client is resent to the other client

