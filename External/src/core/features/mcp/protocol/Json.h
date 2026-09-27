#pragma once

#include "../../../../sdk/sdk.h"

#include <cstdint>
#include <cstdio>
#include <sstream>
#include <string>

namespace Cheat {
namespace Features {
namespace McpBridge {
namespace detail {

inline bool McpValid(std::uint64_t addr)
{
	return addr != 0 && memory && memory->IsConnected();
}

inline std::string JsonEscape(const std::string& s)
{
	std::string out;
	out.reserve(s.size() + 8);

	for (unsigned char c : s)
	{
		switch (c)
		{
		case '"':  out += "\\\""; break;
		case '\\': out += "\\\\"; break;
		case '\b': out += "\\b"; break;
		case '\f': out += "\\f"; break;
		case '\n': out += "\\n"; break;
		case '\r': out += "\\r"; break;
		case '\t': out += "\\t"; break;
		default:
			if (c < 0x20)
			{
				char buf[8];
				std::snprintf(buf, sizeof(buf), "\\u%04x", (unsigned)c);
				out += buf;
			}

			else
			{
				out += (char)c;
			}
			break;
		}
	}

	return out;
}

inline std::string QueryParam(const std::string& query, const char* key)
{
	std::string prefix = std::string(key) + "=";
	std::size_t pos = 0;

	while (pos < query.size())
	{
		std::size_t amp = query.find('&', pos);
		std::size_t len = std::string::npos;
		if (amp != std::string::npos)
			len = amp - pos;

		std::string part = query.substr(pos, len);

		if (part.rfind(prefix, 0) == 0)
		{
			std::string v = part.substr(prefix.size());
			// %20 / + and the rest, too lazy to do properly
			std::string out;
			for (std::size_t i = 0; i < v.size(); ++i)
			{
				if (v[i] == '+')
				{
					out.push_back(' ');
				}

				else if (v[i] == '%' && i + 2 < v.size())
				{
					auto hex = [](char c) -> int
					{
						if (c >= '0' && c <= '9') return c - '0';
						if (c >= 'a' && c <= 'f') return c - 'a' + 10;
						if (c >= 'A' && c <= 'F') return c - 'A' + 10;
						return -1;
					};

					int hi = hex(v[i + 1]);
					int lo = hex(v[i + 2]);
					if (hi >= 0 && lo >= 0)
					{
						out.push_back((char)((hi << 4) | lo));
						i += 2;
					}

					else
					{
						out.push_back(v[i]);
					}
				}

				else
				{
					out.push_back(v[i]);
				}
			}
			return out;
		}

		if (amp == std::string::npos)
			break;

		pos = amp + 1;
	}

	return {};
}

inline std::uint64_t ParseAddr(const std::string& s)
{
	if (s.empty())
		return 0;

	try
	{
		return (std::uint64_t)std::stoull(s, nullptr, 0);
	}
	catch (...)
	{
		return 0;
	}
}

inline std::string InstanceJson(const RBX::RbxInstance& inst)
{
	if (!McpValid(inst.Addr))
		return "{}";

	char addr[32];
	std::snprintf(addr, sizeof(addr), "0x%llX", (unsigned long long)inst.Addr);

	std::ostringstream o;
	o << "{\"name\":\"" << JsonEscape(inst.GetName())
	  << "\",\"class\":\"" << JsonEscape(inst.GetClass())
	  << "\",\"address\":\"" << addr << "\"}";
	return o.str();
}

}
}
}
}
