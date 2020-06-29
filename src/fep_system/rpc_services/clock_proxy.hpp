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

#include <components/service_bus/rpc/fep_rpc_stubs_client.h>
#include <fep_system_stubs/clock_proxy_stub.h>

#include "rpc_services/clock/clock_service_rpc_intf.h"

namespace fep3
{
namespace rpc
{
//we use the namesapce here to create versiones Proxies if something changed in future
namespace arya
{
class ClockServiceProxy : public RPCServiceClientProxy< rpc_proxy_stub::RPCClockServiceProxy,
    IRPCClockService>
{
private:
    typedef rpc::RPCServiceClientProxy< rpc_proxy_stub::RPCClockServiceProxy,
        IRPCClockService > base_type;

public:
    using base_type::GetStub;
    ClockServiceProxy(
        std::string rpc_component_name,
        std::shared_ptr<rpc::IRPCRequester> rpc) :
        base_type(rpc_component_name, rpc)
    {

    }

    std::vector<std::string> getClockNames() const override
    {
        try
        {
            auto value = GetStub().getClockNames();
            return a_util::strings::split(value, ",");
        }
        catch (const std::exception&)
        {
            return {};
        }
    }

    std::string getMainClockName() const override
    {
        try
        {
            return GetStub().getMainClockName();
        }
        catch (const std::exception&)
        {
            return {};
        }
    }

    int64_t getTime(const std::string& clock_name) const override
    {
        try
        {
            auto value = GetStub().getTime(clock_name);
            return a_util::strings::toInt64(value);
        }
        catch (const std::exception&)
        {
            return -1;
        }
    }

    
    ClockType getType(const std::string& clock_name) const override
    {
        try
        {
            auto value = GetStub().getType(clock_name);
            if (value == 0)
            {
                return ClockType::continuous_clock;
            }
            else if (value == 1)
            {
                return ClockType::discrete_clock;
            }
            else
            {
                return ClockType::invalid;
            }
        }
        catch (const std::exception&)
        {
            return ClockType::invalid;
        }
    }

};
}
}
}
