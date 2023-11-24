/* WebSocket++
 * https://github.com/ChesterOfTheSemester/WebSocket-PlusPlus
 * Written by : Chester Abrahams
 * Portfolio  : https://atomiccoder.com
 * LinkedIn   : https://www.linkedin.com/in/atomiccoder/ */

#ifndef WSPP_HEAD
#define WSPP_HEAD

#include <string>
#include <vector>
#include <algorithm>
#include <mutex>
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#endif

#include "../include/hz_timer.h"

#include "wspp_sha1.h"
#include "wspp_b64.h"
#include "wspp_frame.h"

std::vector<struct WSPP_Connection> wspp_connections;
std::vector<SOCKET*> wspp_clients;
std::vector<std::tuple<SOCKET*, std::string, int>> wspp_input_buffer;
std::vector<std::tuple<SOCKET*, std::string, int>> wspp_output_buffer;
int wspp_data_ready = 1;

class WSPP {
public:
    WSPP(int PORT)
    {
        // Create listening thread
        _PORT = PORT;
        std::thread thread_server(threadServerFn, _PORT);
        thread_server.detach();

        // Create thread for handling all connections and managing buffers
        std::thread l(mainLoop);
        l.detach();
    }

    static void WSPP_emit(SOCKET *client, std::string data, int index= -1)
    {
        wspp_output_buffer.push_back(std::make_tuple(client, data, index));
    }

    static bool WSPP_isConnected(SOCKET *client)
    {
        char buffer[1];
        int result = recv(*client, buffer, sizeof(buffer), MSG_PEEK);
        switch (result) {
            case 0: case -1: return false;
            default: return true;
        }
    }

    // Server events
    static void WSPP_onConnection(SOCKET *client, int index);
    static void WSPP_onDisconnection(SOCKET *client, int index);
    static bool WSPP_onMessage(SOCKET *client, std::string data, int index);

    // Utils
    static void WSPP_disconnect(SOCKET *client) { closesocket(*client); }
    static int WSPP_totalConnections() { return wspp_clients.size(); };

private:
    int _PORT;

    static void mainLoop()
    {
        while (true)
        {
            if (!wspp_data_ready) continue;

            // Receive: Iterate input buffer vector
            for (const auto &input : wspp_input_buffer)
            {
                if (&input == 0) continue;

                try {
                    bool callback_result = WSPP_onMessage(std::get<0>(input), std::get<1>(input), std::get<2>(input));

                    // Erase
                    if (callback_result == true)
                        for (auto i = wspp_input_buffer.begin(); i != wspp_input_buffer.end(); ++i)
                            if (std::get<1>(*i) == std::get<1>(input)) {
                                wspp_input_buffer.erase(i);
                                break;
                            }
                } catch (const std::exception& e) {}
            }

            // Send: Iterate output buffer vector and send message data to respective clients
            for (const auto &output : wspp_output_buffer)
            {
                if (&output == 0 || !std::get<0>(output)) continue;

                try {
                    // Send message
                    std::vector<char> frame = frameWSMessage(std::get<1>(output));
                    send(*std::get<0>(output), &frame[0], frame.size(), 0);
                    frame.clear();
                } catch (const std::exception& e) {}
            }

            wspp_output_buffer.clear();
        }
    }

    static void threadServerFn(int PORT)
    {
        wspp_data_ready = 1;

        int client_index = 0;
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData) != 0;
        SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

        sockaddr_in serverAddr = { 0 };
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(PORT);

        bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
        listen(listenSocket, 1);
        std::cout << "Server listening on port " << PORT << "..." << std::endl;

        // Wait for incoming connections
        while (true)
        {
            SOCKET *client_socket = new SOCKET;
            struct sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            char buffer[1024] = { 0 };

            *client_socket = accept(listenSocket, (struct sockaddr*) &clientAddr, &clientAddrLen);
            recv(*client_socket, buffer, 1023, 0);

            std::string request = buffer;
            std::string::size_type key_pos = request.find("Sec-WebSocket-Key: ");

            if (key_pos != std::string::npos)
            {
                // Prepare WS accept key
                std::string key = request.substr(key_pos + 18);
                key = key.substr(0, key.find("\r\n"));
                std::string accept_key = prepareWSAcceptKey(key);

                // Prepare response headers
                std::ostringstream response;
                response << "HTTP/1.1 101 Switching Protocols\n";
                response << "Upgrade: websocket\n";
                response << "Connection: Upgrade\n";
                response << "Sec-WebSocket-Accept: " << accept_key << "\n";
                response << "\r\n";
                send(*client_socket, response.str().c_str(), response.str().length(), 0);

                client_index++;

                // Push new client socket to vector
                wspp_clients.push_back(client_socket);

                // Create new thread and run client
                std::thread t_client(threadClientFn, client_socket, client_index);
                t_client.detach();

                // Create new WSConnection and add to list
                WSPP_Connection con = WSPP_Connection {client_socket, &t_client, 1 };
                wspp_connections.push_back(con);

                // Call event
                WSPP_onConnection(client_socket, client_index);
            }
        }

        closesocket(listenSocket);
        WSACleanup();
    }

    static void threadClientFn(SOCKET *client, int index)
    {
        char buffer[0xFFFF] = { 0 };
        ssize_t bytes_read = 0;

        while (true)
        {
            fd_set read_set;
            FD_ZERO(&read_set);
            FD_SET(*client, &read_set);
            struct timeval timeout = {3, 0}; // Timeout of 3 seconds

            if (select(*client + 1, &read_set, nullptr, nullptr, &timeout) <= 0)
                if (hz_timer.test(1, 0xF0+index, 3000) && !WSPP_isConnected(client)) {
                    WSPP_onDisconnection(client, index);
                    break;
                } else continue;

            if ((bytes_read = recv(*client, buffer, sizeof(buffer), 0)) <= 0)
            {
                WSPP_onDisconnection(client, index);
                break;
            }

            wspp_data_ready = 0;

            // Receive data
            WebSocketFrame frame = parseWSFrame({buffer, buffer + bytes_read});
            std::string payload(frame.payload_data.begin(), frame.payload_data.end());

            // Push message to input buffer
            wspp_input_buffer.push_back({client, payload, index});
            wspp_data_ready = 1;
        }

        // Remove client socket from vector
        wspp_clients.erase(std::remove(wspp_clients.begin(), wspp_clients.end(), client), wspp_clients.end());

        // Close client socket
        closesocket(*client);
    }
};

#endif
