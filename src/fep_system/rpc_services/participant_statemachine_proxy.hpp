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
#include <fep_system_stubs/participant_statemachine_proxy_stub.h>

#include "rpc_services/participant_statemachine/participant_statemachine_rpc_intf.h"

#define CALL_WITH_TIMEOUT(_call_, _while_) \
try \
{ \
    _call_ \
} \
catch(const std::exception& ex) \
{ \
    throw std::runtime_error(std::string() + "timeout while " + _while_  + ": " + ex.what()); \
}



namespace fep3
{
namespace rpc
{
//we use the namesapce here to create versiones Proxies if something changed in future
namespace arya
{
class ParticipantStateMachineProxy : public RPCServiceClientProxy< rpc_proxy_stub::RPCStateMachineProxy,
    IRPCParticipantStateMachine>
{
private:
    typedef rpc::RPCServiceClientProxy< rpc_proxy_stub::RPCStateMachineProxy,
        IRPCParticipantStateMachine > base_type;

public:
    using base_type::GetStub;
    ParticipantStateMachineProxy(
        std::string rpc_component_name,
        std::shared_ptr<rpc::IRPCRequester> rpc) :
        base_type(rpc_component_name, rpc)
    {

    }

    static std::map<std::string, rpc::IRPCParticipantStateMachine::State>& getStateMap()
    {
        static std::map<std::string, rpc::IRPCParticipantStateMachine::State> state_map;
        if (state_map.size() == 0)
        {
            state_map["Loaded"] = rpc::IRPCParticipantStateMachine::State::loaded;
            state_map["Initialized"] = rpc::IRPCParticipantStateMachine::State::initialized;
            state_map["Paused"] = rpc::IRPCParticipantStateMachine::State::paused;
            state_map["Unloaded"] = rpc::IRPCParticipantStateMachine::State::unloaded;
            state_map["Running"] = rpc::IRPCParticipantStateMachine::State::running;
        }
        return state_map;
    }

    static IRPCParticipantStateMachine::State fromStateMap(const std::string& value)
    {
        auto ret_val = getStateMap().find(value);
        if (ret_val != getStateMap().end())
        {
            return ret_val->second;
        }
        else
        {
            return rpc::IRPCParticipantStateMachine::State::undefined;
        }

    }

    IRPCParticipantStateMachine::State getState() const
    {
        try
        {
            return fromStateMap(GetStub().getCurrentStateName());
        }
        catch (...)
        {
            return rpc::IRPCParticipantStateMachine::State::unreachable;
        }
    }
    void load() override
    {
        auto change_succeeded = false;
        CALL_WITH_TIMEOUT({ change_succeeded = GetStub().load(); }, "loading");
        if (!change_succeeded)
        {
            throw std::logic_error("state machine load denied");
        }
    }
    void unload() override
    {
        auto change_succeeded = false;
        CALL_WITH_TIMEOUT({ change_succeeded = GetStub().unload(); }, "unloading");
        if (!change_succeeded)
        {
            throw std::logic_error("state machine load denied");
        }
    }

    void initialize() override
    {
        auto change_succeeded = false;
        CALL_WITH_TIMEOUT({ change_succeeded = GetStub().initialize(); }, "initializing");
        if (!change_succeeded)
        {
            throw std::logic_error("state machine initialize denied");
        }
    }
    void deinitialize() override
    {
        auto change_succeeded = false;
        CALL_WITH_TIMEOUT({ change_succeeded = GetStub().deinitialize(); }, "deinitializing");
        if (!change_succeeded)
        {
            throw std::logic_error("state machine deinitialize denied");
        }
    }
    void start() override
    {
        auto change_succeeded = false;
        CALL_WITH_TIMEOUT({ change_succeeded = GetStub().start(); }, "starting");
        if (!change_succeeded)
        {
            throw std::logic_error("state machine start denied");
        }
    }
    void stop() override
    {
        auto change_succeeded = false;
        CALL_WITH_TIMEOUT({ change_succeeded = GetStub().stop(); }, "stopping");
        if (!change_succeeded)
        {
            throw std::logic_error("state machine stop denied");
        }
    }
    void pause() override
    {
        auto change_succeeded = false;
        CALL_WITH_TIMEOUT({ change_succeeded = GetStub().pause(); }, "pausing");
        if (!change_succeeded)
        {
            throw std::logic_error("state machine pause denied");
        }
    }

    void shutdown() override
    {
        auto change_succeeded = false;
        CALL_WITH_TIMEOUT({ change_succeeded = GetStub().exit(); }, "shutdowning");
        if (!change_succeeded)
        {
            throw std::logic_error("state machine shutdown denied");
        }
    }

};
}
}
}
