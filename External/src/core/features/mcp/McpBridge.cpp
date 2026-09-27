#define NOMINMAX
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "McpBridge.h"

#pragma comment(lib, "ws2_32.lib")

#include "execute/Server.h"

namespace Cheat {
namespace Features {
namespace McpBridge {

void Start()
{
	if (detail::g_run.exchange(true))
		return;

	detail::g_thread = std::thread(detail::Loop);
}

void Stop()
{
	if (!detail::g_run.exchange(false))
		return;

	detail::CloseListen();
	if (detail::g_thread.joinable())
		detail::g_thread.join();
}

bool Running()
{
	return detail::g_run.load(std::memory_order_relaxed);
}

}
}
}
