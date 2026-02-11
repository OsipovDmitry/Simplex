#ifndef CORE_RESOURCE_LOADER_H
#define CORE_RESOURCE_LOADER_H

#include <algorithm>
#include <filesystem>
#include <list>
#include <memory>
#include <unordered_map>

#include <utils/logger.h>
#include <utils/noncopyble.h>

#include <core/inamedobject.h>

namespace simplex
{
namespace core
{

template <typename ResourceType>
class LoaderBase : public INamedObject
{
    NONCOPYBLE(LoaderBase<ResourceType>)
public:
    ~LoaderBase() override = default;

    const std::string& name() const override
    {
        return m_name;
    }

    virtual std::shared_ptr<ResourceType> loadResource(const std::filesystem::path&) const = 0;

protected:
    LoaderBase(const std::string& name)
        : m_name(name)
    {}

private:
    std::string m_name;
};

template <typename ResourceType>
class ResourceLoaderBase : public INamedObject
{
    NONCOPYBLE(ResourceLoaderBase<ResourceType>)
public:
    ~ResourceLoaderBase() override
    {
        while (!m_loaders.empty())
            unregisterLoader(m_loaders.front());
    }

    const std::string& name() const override
    {
        return m_name;
    }

    void registerLoader(const std::shared_ptr<LoaderBase<ResourceType>>& loader)
    {
        if (auto it = std::find(m_loaders.begin(), m_loaders.end(), loader); it != m_loaders.end())
        {
            LOG_WARNING << "Resource loader \"" << ResourceLoaderBase<ResourceType>::name() << "\" already has a loader \"" <<
                loader->name() << "\"";
        }
        else
        {
            m_loaders.push_back(loader);
        }
    }

    void unregisterLoader(const std::shared_ptr<LoaderBase<ResourceType>>& loader)
    {
        if (auto it = std::find(m_loaders.begin(), m_loaders.end(), loader); it == m_loaders.end())
        {
            LOG_WARNING << "Resource loader \"" << ResourceLoaderBase<ResourceType>::name() << "\" doesn't have a loader \"" <<
                loader->name() << "\" yet";
        }
        else
        {
            m_loaders.erase(it);
        }
    }

    std::shared_ptr<ResourceType> loadOrGet(const std::filesystem::path& fileName)
    {
        const auto absoluteFileName = std::filesystem::canonical(fileName);

        if (auto resource = findResource(absoluteFileName.string()))
            return resource;

        std::shared_ptr<ResourceType> result;
        for (auto it = m_loaders.begin(); (!result) && (it != m_loaders.end()); ++it)
            result = (*it)->loadResource(absoluteFileName);

        if (result)
        {
            addResource(absoluteFileName.string(), result);
        }
        else
        {
            LOG_ERROR << "Failed to load resource " << absoluteFileName;
        }

        return result;
    }

protected:
    ResourceLoaderBase(const std::string& name)
        : m_name(name)
    {}

    std::shared_ptr<ResourceType> findResource(const std::string& key) const
    {
        std::shared_ptr<ResourceType> resource;
        if (auto it = m_resources.find(key); it != m_resources.end())
            resource = it->second;
        return resource;
    }

    void addResource(const std::string& key, const std::shared_ptr<ResourceType>& resource)
    {
        if (findResource(key))
        {
            LOG_ERROR << " Resource loader \"" << ResourceLoaderBase<ResourceType>::name() <<
                "\" already has the resource with name \"" << key << "\"";
        }
        else
        {
            m_resources.insert({ key, resource });
        }
    }

private:
    std::string m_name;
    std::list<std::shared_ptr<LoaderBase<ResourceType>>> m_loaders;
    std::unordered_map<std::string, std::shared_ptr<ResourceType>> m_resources;
};

}
}

#endif // CORE_RESOURCE_LOADER_H
