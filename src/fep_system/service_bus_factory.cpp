/**
* @file
*
* @copyright
* @verbatim
Copyright @ 2020 AUDI AG. All rights reserved.

This Source Code Form is subject to the terms of the Mozilla
Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

If it is not possible or desirable to put the notice in a particular file, then
You may include the notice in a location (such as a LICENSE file in a
relevant directory) where a recipient would be likely to look for such a notice.

You may add additional accurate notices of copyright ownership.
@endverbatim
*/
#include "service_bus_factory.h"
#include <fep3/plugin/base/fep3_calling_convention.h>
#include <a_util/filesystem.h>
#include <a_util/xml.h>

namespace helper
{

static constexpr const char* const plugin_filename = "fep3_system.plugins";

a_util::filesystem::Path getBinaryFilePath()
{
    a_util::filesystem::Path current_binary_file_path;
#ifdef WIN32
    HMODULE hModule = nullptr;
    if (GetModuleHandleEx
    (GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
        , (LPCTSTR)getBinaryFilePath
        , &hModule
    ))
    {
        std::vector<wchar_t> file_path_buffer;
        DWORD number_of_copied_characters = 0;
        // note: to support paths with length > MAX_PATH we have do trial-and-error
        // because GetModuleFileName does not indicate if the path was truncated
        while (number_of_copied_characters >= file_path_buffer.size())
        {
            file_path_buffer.resize(file_path_buffer.size() + MAX_PATH);
            number_of_copied_characters = GetModuleFileNameW(hModule, &file_path_buffer[0], static_cast<DWORD>(file_path_buffer.size()));
        }
        file_path_buffer.resize(number_of_copied_characters);
        current_binary_file_path = std::string(file_path_buffer.cbegin(), file_path_buffer.cend());
    }
#else   // WIN32
    Dl_info dl_info;
    dladdr(reinterpret_cast<void*>(getBinaryFilePath), &dl_info);
    current_binary_file_path = dl_info.dli_fname;
#endif
    return current_binary_file_path.getParent();
}

class SystemConfigFile
{
    public:
        struct Item
        {
            std::string _source_type;
            std::string _source_file_reference;
        };
    public:

        void load(const std::string& file)
        {
            { //reset
                _items.clear();
                _current_path.clear();
            }

            a_util::xml::DOM loaded_file;
            if (!loaded_file.load(file))
            {
                auto error_message = a_util::strings::format("can not loaded %s - Error : %s",
                    file.c_str(),
                    loaded_file.getLastError().c_str());
                throw std::runtime_error(error_message);
            }

            a_util::xml::DOMElement schema_version;
            if (loaded_file.getRoot().findNode("schema_version", schema_version))
            {
                auto schema_version_string = schema_version.getData();
                if (a_util::strings::trim(schema_version_string) == "1.0.0")
                {

                }
                else
                {
                    auto error_message = a_util::strings::format("can not loaded %s - Error : wrong schema version found : expect %s - found %s",
                        file.c_str(),
                        "1.0.0",
                        schema_version_string.c_str());
                    throw std::runtime_error(error_message);
                }
            }
            else
            {
                auto error_message = a_util::strings::format("can not loaded %s - Error : %s",
                    file.c_str(),
                    "no schema version tag found");
                throw std::runtime_error(error_message);
            }


            { //sets the path
                a_util::filesystem::Path filepath = file;
                if (filepath.isRelative())
                {
                    filepath = a_util::filesystem::getWorkingDirectory().append(filepath);
                    filepath.makeCanonical();
                }
                _current_path = filepath;
            }

            a_util::xml::DOMElementList plugins;
            if (loaded_file.getRoot().findNodes("plugin", plugins))
            {
                { //validate it 

                    for (const auto& comp_node : plugins)
                    {
                        auto source_node = comp_node.getChild("source");
                        if (source_node.isNull())
                        {
                            auto error_message = a_util::strings::format("can not loaded %s - Error : %s",
                                file.c_str(),
                                "no source node for plugin tag found");
                            throw std::runtime_error(error_message);
                        }
                    }
                }
                { // fill items
                    for (const auto& comp_node : plugins)
                    {
                        auto source_node = comp_node.getChild("source");
                        auto source_type = source_node.getAttribute("type");
                        auto source_file_string = source_node.getData();
                        a_util::strings::trim(source_file_string);
                        a_util::filesystem::Path source_file;
                        if (!source_file_string.empty())
                        {
                            source_file = source_file_string;
                        }
                        if (!source_file.isEmpty() && source_file.isRelative())
                        {
                            //we make it relative to the File! (not the workingdirectory!!)
                            source_file = a_util::filesystem::Path(_current_path).getParent().append(source_file);
                            source_file.makeCanonical();
                        }
                        _items.push_back({ source_type, source_file });
                    }
                }
            }
            else
            {
                //everything is fine, but there are no plugins in the file defined
            }
        }

        std::string getCurrentPath() const
        {
            return _current_path;
        }
        const std::vector<Item> getItems() const
        {
            return _items;
        }
    private:
        std::string _current_path;
        std::vector<Item> _items;
};
}

namespace fep3
{

namespace arya
{
class ServiceBusConnection : public IServiceBusConnection
{
public:
    explicit ServiceBusConnection(std::shared_ptr<IComponent> sb_component) : _sb_component(sb_component)
    {
        _wrapped_sb = reinterpret_cast<IServiceBus*>(sb_component->getInterface(fep3::getComponentIID<IServiceBus>()));
    }

    fep3::Result createSystemAccess(const std::string& system_name,
        const std::string& system_discovery_url,
        bool is_default = false) override
    {
        return _wrapped_sb->createSystemAccess(system_name, system_discovery_url, is_default);
    }
    fep3::Result releaseSystemAccess(const std::string& system_name) override
    {
        return _wrapped_sb->releaseSystemAccess(system_name);
    }
    std::shared_ptr<IParticipantServer> getServer() const override
    {
        return _wrapped_sb->getServer();
    }
    std::shared_ptr<IParticipantRequester> getRequester(const std::string& far_participant_server_name) const override
    {
        return _wrapped_sb->getRequester(far_participant_server_name);
    }
    std::shared_ptr<ISystemAccess> getSystemAccess(const std::string& system_name) const override
    {
        return _wrapped_sb->getSystemAccess(system_name);
    }
    std::shared_ptr<IParticipantRequester> getRequester(const std::string& far_server_url, bool is_url) const
    {
        return _wrapped_sb->getRequester(far_server_url, is_url);
    }

private:
    std::shared_ptr<IComponent> _sb_component;
    arya::IServiceBus* _wrapped_sb;
};
} //end namspace arya

void ServiceBusFactory::initialize()
{
    // we load the plugin her and set _service_bus_factory
    auto file_path = helper::getBinaryFilePath();
    file_path.append(helper::plugin_filename);
    helper::SystemConfigFile file;
    file.load(file_path);
    auto plugins_to_load = file.getItems();
    for (const auto& plugin_item : plugins_to_load)
    {
        if (plugin_item._source_type == "cpp-plugin")
        {
            auto loaded_plugin = std::make_shared<plugin::cpp::HostPlugin>(plugin_item._source_file_reference);
            std::shared_ptr<arya::ICPPPluginComponentFactory> factory = loaded_plugin->create<arya::ICPPPluginComponentFactory>(
                SYMBOL_fep3_plugin_cpp_arya_getFactory);
            if (!factory)
            {
                auto error_message = a_util::strings::format("can not loaded plugin %s - Error : %s ",
                    plugin_item._source_type.c_str(),
                    "the file does not contain a factory components");
                throw std::runtime_error(error_message);
            }
            else
            {
                _plugins.push_back({ loaded_plugin, factory });
            }
        }
    }
}

std::shared_ptr<arya::IServiceBusConnection> ServiceBusFactory::createOrGetServiceBusConnection(
    const std::string& system_name,
    const std::string& system_url)
{
    std::lock_guard<decltype(_sync_get)> lock(_sync_get);

    std::string key = system_name + system_url;
    std::vector<std::string> erase_list;
    std::shared_ptr<arya::IServiceBusConnection> found_connection;

    //look for what is expired and look for the key in one loop
    for (auto& service_bus : _service_bus_connections)
    {
        std::shared_ptr<arya::IServiceBusConnection> check_counter = service_bus.second.lock();
        if (!check_counter)
        {
            erase_list.push_back(service_bus.first);
        }
        else
        {
            if (service_bus.first == key)
            {
                found_connection = service_bus.second.lock();
                break;
            }
        }
    }
    
    //erase what is expired
    for (const auto& current_erase : erase_list)
    {
        _service_bus_connections.erase(current_erase);
    }

    //we found it
    if (found_connection)
    {
        return found_connection;
    }
    //we did not found it
    else
    {
        for (const auto& plugin : _plugins)
        {
            auto service_bus = plugin.second->createComponent(fep3::getComponentIID<arya::IServiceBusConnection>());
            if (service_bus)
            {
                auto created_connection = std::make_shared<arya::ServiceBusConnection>(std::move(service_bus));
                auto creation_result = created_connection->createSystemAccess(system_name, system_url);
                if (fep3::isFailed(creation_result))
                {
                    throw std::runtime_error(
                        std::string("can not system access in service bus connection - Error :") +
                        creation_result.getDescription());
                }
                _service_bus_connections[key] = created_connection;
                return created_connection;
            }
        }
        throw std::runtime_error(
            "can not create a service bus connection. no plugin supports "
            + fep3::getComponentIID<arya::IServiceBusConnection>());
    }
}

ServiceBusFactory& ServiceBusFactory::get()
{
    static ServiceBusFactory serv_bus_fac;
    if (serv_bus_fac._plugins.empty())
    {
        serv_bus_fac.initialize();
    }
    return serv_bus_fac;
}

}
