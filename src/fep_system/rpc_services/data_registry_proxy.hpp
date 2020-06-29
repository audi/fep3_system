/**
 * @file

   @copyright
   @verbatim
   Copyright @ 2020 Audi AG. All rights reserved.
   
       This Source Code Form is subject to the terms of the Mozilla
       Public License, v. 2.0. If a copy of the MPL was not distributed
       with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
   
   If it is not possible or desirable to put the notice in a particular file, then
   You may include the notice in a location (such as a LICENSE file in a
   relevant directory) where a recipient would be likely to look for such a notice.
   
   You may add additional accurate notices of copyright ownership.
   @endverbatim 
 *
 */

#pragma once
#include <string>

#include <fep3/components/service_bus/rpc/fep_rpc.h>
#include <fep3/base/streamtype/default_streamtype.h>
//this will be installed !!
#include "rpc_services/data_registry/data_registry_rpc_intf.h"
#include <fep_system_stubs/data_registry_proxy_stub.h>
#include "system_logger_intf.h"
#ifdef SEVERITY_ERROR
#undef SEVERITY_ERROR
#endif

namespace fep3
{
namespace rpc
{
namespace arya
{
class DataRegistryProxy : public RPCServiceClientProxy<
    rpc_proxy_stub::RPCDataRegistryProxy,
    IRPCDataRegistry>
{
private:
    typedef RPCServiceClientProxy< rpc_proxy_stub::RPCDataRegistryProxy,
                                   IRPCDataRegistry > base_type;

public:
    using base_type::GetStub;
    DataRegistryProxy(
        std::string rpc_component_name,
        std::shared_ptr<IRPCRequester> rpc) :
        base_type(rpc_component_name, rpc)
    {
    }
    std::vector<std::string> getSignalsIn() const override
    {
        try
        {
            std::string signal_list = GetStub().getSignalInNames();
            return a_util::strings::split(signal_list, ",");
        }
        catch (...)
        {
            return std::vector<std::string>();
        }
    }
    std::vector<std::string> getSignalsOut() const override
    {
        try
        {
            std::string signal_list = GetStub().getSignalOutNames();
            return a_util::strings::split(signal_list, ",");
        }
        catch (...)
        {
            return std::vector<std::string>();
        }
    }
    StreamType getStreamType(const std::string& signal_name) const override
    {
        try
        {
            auto stream_type_return = GetStub().getStreamType(signal_name);
            auto meta_typename = stream_type_return["meta_type"].asString();
            auto properties = stream_type_return["properties"];
            auto names = a_util::strings::split(properties["names"].asString(), ",");
            auto values = a_util::strings::split(properties["names"].asString(), ",");
            auto types = a_util::strings::split(properties["types"].asString(), ",");

            StreamMetaType meta_type(meta_typename);
            StreamType stream_type_created(meta_type);
            auto current_name = names.begin();
            auto current_value = values.begin();
            auto current_type = types.begin();
            while (current_name != names.end() && current_value != values.end() && current_type != types.end())
            {
                stream_type_created.setProperty(*current_name, *current_value, *current_type);
            }
            return stream_type_created;
        }
        catch (...)
        {
            return StreamTypeRaw();
        }
    }

};
}
}
}
