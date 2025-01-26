#pragma once

#include "RE/Skyrim.h"
#include "REX/REX/Singleton.h"
#include "SKSE/SKSE.h"

#include <ClibUtil/simpleINI.hpp>
#include <spdlog/sinks/basic_file_sink.h>

#define DLLEXPORT __declspec(dllexport)

namespace logger = SKSE::log;
namespace string = clib_util::string;
namespace ini = clib_util::ini;

using namespace std::literals;

namespace stl
{
	using namespace SKSE::stl;

	template <class F, std::size_t vtbl_idx, class T>
	void write_vfunc()
	{
		REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[vtbl_idx] };
		T::func = vtbl.write_vfunc(T::idx, T::thunk);
	}

	template <class F, class T>
	void write_vfunc()
	{
		write_vfunc<F, 0, T>();
	}
}

#include "Version.h"

#ifdef SKYRIM_AE
#	define OFFSET(se, ae) ae
#else
#	define OFFSET(se, ae) se
#endif

using Key = std::uint32_t;
