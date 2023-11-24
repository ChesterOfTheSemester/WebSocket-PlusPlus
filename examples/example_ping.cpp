#include <iostream>
#include <chrono>

// Include main library
#include "../src/wspp.h"

// Include helpers
#include "../include/ansijson.h"
#include "../include/hz_timer.h"

void WSPP::WSPP_onConnection(SOCKET *client, int index) {
    std::cout << "New connection with Client" << index << std::endl;
}

void WSPP::WSPP_onDisconnection(SOCKET *client, int index) {
    std::cout << "Client" << index << " has disconnected" << std::endl;
}

bool WSPP::WSPP_onMessage(SOCKET *client, std::string data, int index=0)
{
    // Get connection struct
    auto connection = std::find_if(wspp_connections.begin(), wspp_connections.end(),
                                   [client](const WSPP_Connection& connection) { return connection.client == client; }
    );

    // Print received data
    struct aJSON *data_json = decodeAJSON((char*) data.c_str());
    std::cout << "Received data:\n" << data_json->encode(0) << "\n" << std::endl;
    freeAJSON(data_json);

    // Return message to client
    WSPP_emit(client, data);

    return true;
}

int main()
{
    // Start listening, create connection threads
    WSPP server(4732);

    // Your code
    while (true)
    {

    }

    return 0;
}
