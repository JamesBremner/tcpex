#include <winsock2.h>
#include <vector>
#include <functional>
#include <queue>
#include <ws2tcpip.h>

namespace raven
{
    namespace set
    {

        /// A C++ wrapper for windows TCP sockets

        class cTCPex
        {
        public:
            enum class eEvent
            {
                accept,
                read,
                disconnect,
            };

            typedef std::function<void(int, eEvent, const std::string &)>
                eventHandler_t;
            typedef std::function<std::string(int, const std::string &)>
                processor_t;

            cTCPex();

            /// @brief Connect to server
            /// @param server_address 
            /// @param server_port 
            /// @param readHandler 
            /// @return true if connected

            bool connect_to_server(
                const std::string &server_address,
                const std::string &server_port,
                processor_t readHandler);

            /** start server

             @param[in] ServerPort listens for clients
             @param[in] eventHandler handler to call when something happens
             @param[in] process handler to call when message arrives from client
             @param[in] maxClient max number of clients, default 1

             Starts listening for client connection requests.

             The eventHandler function will be called when
             - a client connects
             - a message is received from client
             - a client diconnects
             and runs in the thread connected to the client

             eventHandler signature:  void h(
                int client,
                eEvent type,
                const std::string& msg )

             client the index among connected clients of client that sent the message
             type of event
             msg the message received from the client

             Threading

            When the server starts,
                - a new thread is started that listens for connection requests.
                    runs until the application exits
                - a new thread is started that checks for jobs waiting to be processed
                    runs until the application exits
            When a new client connects,
                - a new thread is started that listens to the client.
                    This thread runs until the client disconnects
                    The eventHandler runs in this thread


             This method returns immediatly, leaving the client connect listening thread running.

             */

            void start_server(
                const std::string &ServerPort,
                eventHandler_t eventHandler,
                processor_t processor,
                int maxClient = 1);

            /// Send message to client
            void send(
                const std::string &msg,
                int client = 0);
            void send(
                const std::vector<unsigned char> &msg,
                int client = 0);

            /// Get last message from peer
            std::string readMsg() const;

            /// True if peer connected
            bool isConnected(int client = 0) const;

            int countConnectedClients();

            int maxClient() const;

            /** set line accumulation option
             * @param f false for instant processing of everthing received from client
             * @param f true wait to process input until complete line received
             */

            void lineAccumulator(bool f)
            {
                myFrameLines = f;
            }

            /** set job threading option
             *
             * @param f true all jobs from all clients run in same thread
             *
             *       PRO:
             * - no data access synchronize needed
             * - long jobs do not block the server
             *
             *       CON:
             *  - long jobs may block execution of other jobs
             *
             * @param f false jobs run in separate client threads
             *
             *      PRO:
             * - long jobs do not block jobs from other clients
             *
             *      CON:
             * - long jobs block server for their client
             * - careful data access synchronization required
             *
             */
            void sharedProcessingThread(bool f)
            {
                mySharedProcessingThread = f;
            }

        private:
            bool myFrameLines;
            bool mySharedProcessingThread;
            std::string myServerPort;
            SOCKET myAcceptSocket;               //< socket listening for clients
            std::vector<SOCKET> myConnectSocket; //< sockets connected to clients
            std::string myRemoteAddress;
            char myReadbuf[1024];
            eventHandler_t myEventHandler;
            processor_t myProcessor;

            void initWinSock();

            int addConnectedSocket(SOCKET s);

            /** Wait for client connection requests
             *
             * runs in its own thread
             *
             * Never returns
             */
            void acceptBlock();

            /** Wait for messages from client
             *
             * runs in its own thread, invoking the read handler when a message is recvd
             *
             * Does not return until the client disconnects
             */
            void readBlock(int client);

            void jobThread();

            /// Wait for client connection request
            int acceptClientMultiple();

            /// Wait for message from peer
            int read(int client = 0);

            /// @brief Construct socket that listens for client connetion requests
            void acceptSocketCtor();

            /** Accumulate complete lines ( terminated by \n or \t pt \m\r )
             * @param msg message received
             * @return line received, "" if no line available
             */
            std::string nextLine(const std::string &msg);

            int clientPort(int client);

            SOCKET clientSocket(int client);

            class cJob
            {
            public:
                int client;
                const std::string &msg;

                cJob(int c,
                     const std::string &m)
                    : client(c),
                      msg(m)
                {
                }
            };

            std::queue<cJob> myJobQ;
        };

    }
}
