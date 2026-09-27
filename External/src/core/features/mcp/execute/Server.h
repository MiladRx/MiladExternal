#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "../handlers/Handlers.h"
#include "../protocol/Http.h"

#include <atomic>
#include <mutex>
#include <string>
#include <thread>

namespace Cheat {
namespace Features {
namespace McpBridge {
namespace detail {

inline std::atomic<bool> g_run{ false };
inline std::thread g_thread;
inline SOCKET g_listen = INVALID_SOCKET;
inline std::mutex g_sock_mtx;

inline void HandleClient(SOCKET client)
{
	char buf[4096];
	int got = recv(client, buf, sizeof(buf) - 1, 0);
	if (got <= 0)
	{
		closesocket(client);
		return;
	}
	buf[got] = 0;

	// only GET /path?q HTTP/1.1, ignore the rest
	std::string req(buf, buf + got);
	if (req.rfind("GET ", 0) != 0)
	{
		SendResponse(client, 405, "Method Not Allowed", "{\"error\":\"method\"}");
		closesocket(client);
		return;
	}

	std::size_t sp = req.find(' ', 4);
	if (sp == std::string::npos)
	{
		SendResponse(client, 400, "Bad Request", "{\"error\":\"bad_request\"}");
		closesocket(client);
		return;
	}

	std::string url = req.substr(4, sp - 4);
	std::string path = url;
	std::string query;
	std::size_t q = url.find('?');
	if (q != std::string::npos)
	{
		path = url.substr(0, q);
		query = url.substr(q + 1);
	}

	if (path.size() > 1 && path.back() == '/')
		path.pop_back();

	std::string body = Dispatch(path, query);
	int status = 200;
	const char* text = "OK";

	if (body.find("\"error\"") != std::string::npos)
	{
		if (body.find("mcp_disabled") != std::string::npos ||
			body.find("not_attached") != std::string::npos)
		{
			status = 503;
			text = "Service Unavailable";
		}

		else if (body.find("not_found") != std::string::npos ||
			body.find("unknown_endpoint") != std::string::npos)
		{
			status = 404;
			text = "Not Found";
		}

		else
		{
			status = 400;
			text = "Bad Request";
		}
	}

	SendResponse(client, status, text, body);
	closesocket(client);
}

inline void CloseListen()
{
	std::lock_guard<std::mutex> lk(g_sock_mtx);
	if (g_listen != INVALID_SOCKET)
	{
		closesocket(g_listen);
		g_listen = INVALID_SOCKET;
	}
}

inline void Loop()
{
	WSADATA wsa{};
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		g_run = false;
		return;
	}

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_sock == INVALID_SOCKET)
	{
		WSACleanup();
		g_run = false;
		return;
	}

	BOOL yes = TRUE;
	setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR,
		(const char*)&yes, sizeof(yes));

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(3847);
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

	if (bind(listen_sock, (sockaddr*)&addr, sizeof(addr)) != 0 ||
		listen(listen_sock, 8) != 0)
	{
		closesocket(listen_sock);
		WSACleanup();
		g_run = false;
		return;
	}

	{
		std::lock_guard<std::mutex> lk(g_sock_mtx);
		g_listen = listen_sock;
	}

	// select with timeout, otherwise Stop hangs
	while (g_run.load(std::memory_order_relaxed))
	{
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(listen_sock, &fds);
		timeval tv{};
		tv.tv_sec = 0;
		tv.tv_usec = 250000;

		int sel = select(0, &fds, nullptr, nullptr, &tv);
		if (sel <= 0)
			continue;

		sockaddr_in peer{};
		int peer_len = sizeof(peer);
		SOCKET client = accept(listen_sock, (sockaddr*)&peer, &peer_len);
		if (client == INVALID_SOCKET)
			continue;

		char ip[INET_ADDRSTRLEN]{};
		inet_ntop(AF_INET, &peer.sin_addr, ip, sizeof(ip));
		if (std::string(ip) != "127.0.0.1")
		{
			closesocket(client);
			continue;
		}

		HandleClient(client);
	}

	CloseListen();
	WSACleanup();
}

}
}
}
}
