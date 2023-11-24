#include <iostream>
#include <chrono>

// Include main library
#include "../src/wspp.h"

// Include helpers
#include "../include/ansijson.h"
#include "../include/hz_timer.h"

std::vector<std::string> chat_log;

void WSPP::WSPP_onConnection(SOCKET *client, int index) {
    std::cout << "New connection with Client" << index << std::endl;

    std::string msg = "Server: Client";
    msg += std::to_string(index) + " enters the chatroom";
    chat_log.push_back(msg);

    // Update all clients with chat log
    {
        std::string concatenated_chat;
        for (const auto& message : chat_log)
            concatenated_chat += message + "\n";

        for (auto iclient : wspp_clients)
            WSPP_emit(iclient, concatenated_chat);
    }
}

void WSPP::WSPP_onDisconnection(SOCKET *client, int index) {
    std::cout << "Client" << index << " has disconnected" << std::endl;

    std::string msg = "Server: Client";
    msg += std::to_string(index) + " has left the chatroom";
    chat_log.push_back(msg);

    // Update all clients with chat log
    {
        std::string concatenated_chat;
        for (const auto& message : chat_log)
            concatenated_chat += message + "\n";

        for (auto iclient : wspp_clients)
            WSPP_emit(iclient, concatenated_chat);
    }
}

bool WSPP::WSPP_onMessage(SOCKET *client, std::string data, int index=0)
{
    // Get connection struct
    auto connection = std::find_if(wspp_connections.begin(), wspp_connections.end(),
                                   [client](const WSPP_Connection& connection) { return connection.client == client; }
    );

    // Print received data
    struct aJSON *data_json = decodeAJSON((char*) data.c_str());
    if (!data_json) return 0;

    if (data_json->key && !strcmp(data_json->key, "message")) {
        std::string msg = "Client";
        msg += std::to_string(index) + ": " + std::string(data_json->string);
        chat_log.push_back(msg);

        // Handle commands
        {
            // Clear chat
            if (data_json->string && !strcmp(data_json->string, "/clear"))
                chat_log.clear();

            // Send the number of total current connections
            if (data_json->string && !strcmp(data_json->string, "/connections")) {
                chat_log.pop_back();
                std::string msg = "Server";
                msg += ": Total connections (" + std::to_string(WSPP_totalConnections()) + ")";
                chat_log.push_back(msg);
            }

            // Disconnect from client
            if (data_json->string && !strcmp(data_json->string, "/WSPP_disconnect"))
                WSPP_disconnect(client);
        }

        // Update all clients with chat log
        {
            std::string concatenated_chat;
            for (const auto& message : chat_log)
                concatenated_chat += message + "\n";

            for (auto client : wspp_clients)
                WSPP_emit(client, concatenated_chat);
        }
    }

    // Deallocate
    freeAJSON(data_json);

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
