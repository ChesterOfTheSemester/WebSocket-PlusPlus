# WebSocket++ 0.1

WebSocket++ is a C++ library for WebSocket server hosting, providing a simple interface for handling WebSocket connections. This library handles handshaking and data framing. It does not use third-party libraries and only the standard socket headers.

## Features
- Lightweight C++ WebSocket library.
- Handles WebSocket connections and manages buffers.
- Supports server-side events like connection, disconnection, and message handling.
- Utilizes threading for efficient communication.

### Note
- As of version 0.1, WebSocket++ only works on Windows platforms. Expect this to change in the near future.

# Examples
- The `/examples` directory contains CPP files for demonstrating various ways to use the WebSocket++ server, along with HTML documents in the `/examples/http` directory for demonstrating client-side connection.
- `example_ping.cpp` Demonstrates receiving messages from the client as JSON formats and automatically returns the message to the client.
- `example_chat.cpp` Demonstrates a simple chatbox that handles multiple connections and provides simple chat commands.

## Usage
1. Include the necessary headers.
2. Create an instance of `WSPP` by specifying the port.
3. Implement callback functions for connection, disconnection, and message handling.
4. Use `WSPP_emit` to send messages to clients.
5. Check the connection status with `WSPP_isConnected`.
6. Disconnect clients using `WSPP_disconnect`.
7. Get the total number of connections with `WSPP_totalConnections`.

```cpp
#include "wspp.h"

void WSPP::WSPP_onConnection(SOCKET *client, int index) {
    // Handle connection event
}

void WSPP::WSPP_onDisconnection(SOCKET *client, int index) {
    // Handle disconnection event
}

bool WSPP::WSPP_onMessage(SOCKET *client, std::string data, int index) {
    // Handle incoming message
    return true; // Return true to erase the message from the buffer
}

int main() {
    WSPP server(8080); // Specify the desired port
    // Implement callback functions as needed

    // Main program logic...

    return 0;
}
```

**Note:** Make sure to handle exceptions appropriately in your application.

## In-house Dependencies
- [ANSIJSON](https://github.com/ChesterOfTheSemester/ANSIJSON)
- [hz_timer](https://github.com/ChesterOfTheSemester/HZ-Timer)

For more details, refer to the [GitHub repository](https://github.com/ChesterOfTheSemester/WebSocket-PlusPlus).

## MIT License

> Copyright (c) 2023 Chester Abrahams
>
> Permission is hereby granted, free of charge, to any person obtaining a copy
> of this software and associated documentation files (the "Software"), to deal
> in the Software without restriction, including without limitation the rights
> to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
> copies of the Software, and to permit persons to whom the Software is
> furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in all
> copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
> FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
> AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
> LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
> OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
> SOFTWARE.

## About the Author

- **Author**: Chester Abrahams
- **Portfolio**: [atomiccoder.com](https://atomiccoder.com)
- **LinkedIn**: [linkedin.com/in/atomiccoder](https://www.linkedin.com/in/atomiccoder)