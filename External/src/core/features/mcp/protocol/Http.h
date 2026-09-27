#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <WinSock2.h>

#include <cstdio>
#include <string>

namespace Cheat {
namespace Features {
namespace McpBridge {
namespace detail {

inline void SendResponse(SOCKET client, int status, const char* status_text, const std::string& body)
{
	char header[256];
	int n = std::snprintf(header, sizeof(header),
		"HTTP/1.1 %d %s\r\n"
		"Content-Type: application/json; charset=utf-8\r\n"
		"Content-Length: %zu\r\n"
		"Connection: close\r\n"
		"Access-Control-Allow-Origin: *\r\n"
		"\r\n",
		status, status_text, body.size());

	if (n > 0)
		send(client, header, n, 0);

	if (!body.empty())
		send(client, body.data(), (int)body.size(), 0);
}

}
}
}
}
