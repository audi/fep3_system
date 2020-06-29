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
#define _FEP3_PARTICIPANT_INCLUDED_STATIC
#include <fep3/plugin/cpp/cpp_plugin_intf.h>
#include <fep3/plugin/cpp/cpp_plugin_component_factory_intf.h>
#include <fep3/components/service_bus/service_bus_intf.h>
#include <fep3/plugin/cpp/cpp_host_plugin.h>
#include <memory>
#include <string>
#include <list>
#include <mutex>

namespace fep3
{
namespace arya
{
class IServiceBusConnection : public fep3::arya::IServiceBus
{
public:
    ~IServiceBusConnection() = default;
};
}

class ServiceBusFactory
{
private:
    ServiceBusFactory() = default;
    void initialize();
public:
    static ServiceBusFactory& get();
    
    std::shared_ptr<arya::IServiceBusConnection> createOrGetServiceBusConnection(
        const std::string& system_name,
        const std::string& system_url);

private:
    std::list<std::pair<std::shared_ptr<plugin::cpp::HostPlugin> , std::shared_ptr<arya::ICPPPluginComponentFactory>>> _plugins;
    std::map<std::string, std::weak_ptr<arya::IServiceBusConnection>> _service_bus_connections;
    std::recursive_mutex _sync_get;

};
}
