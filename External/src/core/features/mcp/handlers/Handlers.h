#pragma once

#include "../../../globals/globals.h"
#include "../../../../memory/memory.h"
#include "../../../../sdk/sdk.h"
#include "../../../variables/variables.h"
#include "../protocol/Json.h"

#include <cctype>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

namespace Cheat {
namespace Features {
namespace McpBridge {
namespace detail {

inline std::uint64_t DmU64(std::uintptr_t off)
{
	const auto dm = Globals::dataModel.Addr;
	if (!McpValid(dm))
		return 0;
	return memory->read<std::uint64_t>(dm + off);
}

inline RBX::RbxInstance DataModelRoot()
{
	if (!memory || !memory->IsConnected())
		return {};

	const auto dm = Globals::dataModel.Addr;
	if (!McpValid(dm))
		return {};

	return RBX::RbxInstance(dm);
}

inline RBX::RbxInstance ResolvePath(const RBX::RbxInstance& root, const std::string& path)
{
	if (!root.Addr)
		return {};

	if (path.empty() || path == "/" || path == ".")
		return root;

	RBX::RbxInstance cur = root;
	std::size_t i = 0;
	if (!path.empty() && (path[0] == '/' || path[0] == '\\'))
		i = 1;

	while (i < path.size())
	{
		std::size_t j = path.find_first_of("/\\", i);
		if (j == std::string::npos)
			j = path.size();

		std::string part = path.substr(i, j - i);
		i = j + 1;

		if (part.empty() || part == ".")
			continue;

		if (part == "game" || part == "Game" || part == "DataModel")
			continue;

		auto child = cur.FindFirstChild(part);
		if (!child || !McpValid(child.Addr))
			return {};

		cur = child;
	}

	return cur;
}

inline RBX::RbxInstance ResolveTarget(const std::string& query)
{
	RBX::RbxInstance root = DataModelRoot();
	if (!root.Addr)
		return {};

	std::string addr_s = QueryParam(query, "addr");
	if (!addr_s.empty())
	{
		std::uint64_t a = ParseAddr(addr_s);
		if (McpValid(a))
			return RBX::RbxInstance((std::uintptr_t)a);

		return {};
	}

	std::string path = QueryParam(query, "path");
	if (!path.empty())
		return ResolvePath(root, path);

	// defaults to workspace, otherwise dm
	if (McpValid(Globals::workspace.Addr))
		return RBX::RbxInstance(Globals::workspace.Addr);

	return root;
}

inline std::string HandleHealth()
{
	bool attached = memory && memory->IsConnected();

	std::uint64_t place = 0;
	std::uint64_t game = 0;
	if (attached)
	{
		place = DmU64(Offsets::DataModel::PlaceId);
		game = DmU64(Offsets::DataModel::GameId);
	}

	const char* att = "false";
	if (attached)
		att = "true";

	std::ostringstream o;
	o << "{\"ok\":true,\"attached\":" << att
	  << ",\"placeId\":" << place
	  << ",\"gameId\":" << game << "}";
	return o.str();
}

inline std::string HandleInfo()
{
	if (!memory || !memory->IsConnected())
		return "{\"error\":\"not_attached\"}";

	const auto dm = Globals::dataModel.Addr;
	if (!McpValid(dm))
		return "{\"error\":\"no_datamodel\"}";

	bool loaded = memory->read<std::uint8_t>(dm + Offsets::DataModel::GameLoaded) != 0;

	std::string job;
	const auto jp = memory->read<std::uintptr_t>(dm + Offsets::DataModel::JobId);
	if (McpValid(jp))
		job = memory->read_string(jp);

	std::ostringstream o;
	o << "{\"placeId\":" << memory->read<std::uint64_t>(dm + Offsets::DataModel::PlaceId)
	  << ",\"gameId\":" << memory->read<std::uint64_t>(dm + Offsets::DataModel::GameId)
	  << ",\"placeVersion\":" << 0
	  << ",\"creatorId\":" << memory->read<std::uint64_t>(dm + Offsets::DataModel::CreatorId)
	  << ",\"jobId\":\"" << JsonEscape(job) << "\""
	  << ",\"gameLoaded\":" << (loaded ? "true" : "false")
	  << ",\"dataModel\":\"0x" << std::hex
	  << (unsigned long long)dm << std::dec << "\"";

	if (McpValid(Globals::workspace.Addr))
	{
		o << ",\"workspace\":\"0x" << std::hex
		  << (unsigned long long)Globals::workspace.Addr
		  << std::dec << "\"";
	}

	o << "}";
	return o.str();
}

inline std::string HandleChildren(const std::string& query)
{
	RBX::RbxInstance target = ResolveTarget(query);
	if (!McpValid(target.Addr))
		return "{\"error\":\"not_found\"}";

	auto kids = target.GetChildList();
	int n = (int)kids.size();
	if (n > 256)
		n = 256;

	std::ostringstream o;
	o << "{\"parent\":" << InstanceJson(target) << ",\"count\":";
	o << kids.size() << ",\"children\":[";
	for (int i = 0; i < n; ++i)
	{
		if (i)
			o << ',';
		o << InstanceJson(kids[(std::size_t)i]);
	}

	if ((int)kids.size() > n)
		o << "],\"truncated\":true}";

	else
		o << "],\"truncated\":false}";

	return o.str();
}

inline void SearchRecurse(const RBX::RbxInstance& node, const std::string& needle_lower,
                   std::vector<RBX::RbxInstance>& out, int depth)
{
	// depth 8 / 80 hits, further makes no sense
	if (depth > 8 || (int)out.size() >= 80)
		return;

	if (!McpValid(node.Addr))
		return;

	auto kids = node.GetChildList();
	for (const auto& c : kids)
	{
		if ((int)out.size() >= 80)
			return;

		if (!McpValid(c.Addr))
			continue;

		std::string name = c.GetName();
		std::string lower = name;
		for (char& ch : lower)
			ch = (char)std::tolower((unsigned char)ch);

		if (lower.find(needle_lower) != std::string::npos)
			out.push_back(c);

		SearchRecurse(c, needle_lower, out, depth + 1);
	}
}

inline std::string HandleSearch(const std::string& query)
{
	std::string name = QueryParam(query, "name");
	if (name.empty())
		return "{\"error\":\"missing_name\"}";

	int limit = 80;
	std::string lim = QueryParam(query, "limit");
	if (!lim.empty())
	{
		try
		{
			limit = std::stoi(lim);
		}
		catch (...) {}

		if (limit < 1) limit = 1;
		if (limit > 80) limit = 80;
	}

	RBX::RbxInstance root = DataModelRoot();
	std::string path = QueryParam(query, "path");
	if (!path.empty())
	{
		root = ResolvePath(root, path);
	}

	else if (McpValid(Globals::workspace.Addr))
	{
		root = RBX::RbxInstance(Globals::workspace.Addr);
	}

	if (!McpValid(root.Addr))
		return "{\"error\":\"no_root\"}";

	std::string needle = name;
	for (char& ch : needle)
		ch = (char)std::tolower((unsigned char)ch);

	std::vector<RBX::RbxInstance> hits;
	hits.reserve((std::size_t)limit);
	SearchRecurse(root, needle, hits, 0);

	std::ostringstream o;
	o << "{\"query\":\"" << JsonEscape(name) << "\",\"count\":" << hits.size() << ",\"results\":[";

	int n = (int)hits.size();
	if (n > limit)
		n = limit;

	for (int i = 0; i < n; ++i)
	{
		if (i)
			o << ',';
		o << InstanceJson(hits[(std::size_t)i]);
	}

	o << "]}";
	return o.str();
}

inline std::string HandlePath(const std::string& query)
{
	RBX::RbxInstance target = ResolveTarget(query);
	if (!McpValid(target.Addr))
		return "{\"error\":\"not_found\"}";

	// walk up parents to dm
	std::vector<std::string> parts;
	RBX::RbxInstance cur = target;
	for (int guard = 0; guard < 64 && McpValid(cur.Addr); ++guard)
	{
		parts.push_back(cur.GetName());
		auto parent = cur.GetParent();
		if (!parent || !McpValid(parent.Addr))
			break;

		if (parent.Addr == Globals::dataModel.Addr)
		{
			parts.push_back("DataModel");
			break;
		}

		cur = parent;
	}

	std::string path;
	for (auto it = parts.rbegin(); it != parts.rend(); ++it)
	{
		if (!path.empty())
			path += '/';
		path += *it;
	}

	std::ostringstream o;
	o << "{\"instance\":" << InstanceJson(target)
	  << ",\"path\":\"" << JsonEscape(path) << "\"}";
	return o.str();
}

inline std::string Dispatch(const std::string& path, const std::string& query)
{
	if (!variables::Misc::mcp)
		return "{\"error\":\"mcp_disabled\"}";

	if (path == "/health")
		return HandleHealth();

	if (path == "/info")
		return HandleInfo();

	if (path == "/children")
		return HandleChildren(query);

	if (path == "/search")
		return HandleSearch(query);

	if (path == "/path")
		return HandlePath(query);

	return "{\"error\":\"unknown_endpoint\"}";
}

}
}
}
}
