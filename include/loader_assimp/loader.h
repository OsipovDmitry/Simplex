#ifndef LOADER_ASSIMP_LOADER_H
#define LOADER_ASSIMP_LOADER_H

#include <filesystem>

#include <core/forwarddecl.h>

#include <loader_assimp/assimpglobal.h>

namespace simplex
{
namespace loader_assimp
{

std::shared_ptr<core::Node> LOADER_ASSIMP_GLOBAL_SHARED_EXPORT load(const std::filesystem::path&);

}
}

#endif // LOADER_ASSIMP_LOADER_H
