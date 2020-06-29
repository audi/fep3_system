/**
* @file
*
* @copyright
* @verbatim
Copyright @ 2020 Audi AG. All rights reserved.

This Source Code Form is subject to the terms of the Mozilla
Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

If it is not possible or desirable to put the notice in a particular file, then
You may include the notice in a location (such as a LICENSE file in a
relevant directory) where a recipient would be likely to look for such a notice.

You may add additional accurate notices of copyright ownership.
@endverbatim
*/
#pragma once

#include "fep_system/fep_system_types.h"
#include "fep_system/rpc_component_proxy.h"
#include "rpc_services/participant_info/participant_info_rpc_intf.h"
#include "rpc_services/participant_statemachine/participant_statemachine_rpc_intf.h"
#include "rpc_services/clock/clock_service_rpc_intf.h"
#include "rpc_services/data_registry/data_registry_rpc_intf.h"
#include "rpc_services/configuration/configuration_rpc_intf.h"
#include "rpc_services/logging/logging_rpc_intf.h"
#include "system_logger_intf.h"

#include <string>
#include <chrono>

namespace fep3
{
/**
 * @brief The ParticipantProxy will provide common system access to the participants system interfaces (RPC Services).
 * use fep3::System to connect
 *
 * With the getRPCComponentProxy following interfaces are supported:
 * \li fep3::rpc::IRPCParticipantInfo
 * \li fep3::rpc::IRPCDataRegistry
 * \li fep3::rpc::IRPCParticipantStateMachine
 */
class FEP3_SYSTEM_EXPORT ParticipantProxy final
{
public:
    /**
     * @brief Construct a new Participant Proxy object
     *
     */
    ParticipantProxy() = default;
    /**
     * @brief DTOR
     *
     */
    ~ParticipantProxy();
    /**
     * @brief Construct a new Participant Proxy object
     *
     * @param participant_name name of the participant
     * @param participant_url participants server url of the participant
     * @param system_name name of the system
     * @param system_url url of the system
     * @param logger logger to log values to.
     * @param default_timeout default timeout for each rpc request
     */
    ParticipantProxy(const std::string& participant_name,
        const std::string& participant_url,
        const std::string& system_name,
        const std::string& system_url,
        ISystemLogger& logger,
        std::chrono::milliseconds default_timeout);
    /**
     * @brief Construct a new Participant Proxy object
     *
     * @param other the other to move
     */
    ParticipantProxy(ParticipantProxy&& other);
    /**
     * @brief move the other to the current instance
     *
     * @param other the other
     * @return ParticipantProxy& value of this
     */
    ParticipantProxy& operator=(ParticipantProxy&& other);
    /**
     * @brief Construct a new Participant Proxy object
     *
     * @param other the other to copy
     */
    ParticipantProxy(const ParticipantProxy& other);
    /**
     * @brief copies another participant prox instance to the current
     *
     * @param other the other to copy
     * @return ParticipantProxy&
     */
    ParticipantProxy& operator=(const ParticipantProxy& other);

    /**
     * @brief check if underlying participant instance is invalid
     *
     * @return true is valid
     * @return false is not valid
     */
    operator bool() const
    {
        return static_cast<bool>(_impl);
    }

    /**
     * Helper function to copy content.
     *
     * @param other the participant to copy values to
     */
    void copyValuesTo(ParticipantProxy& other) const;

    /**
    * set priority that the participant should use when the system is triggered
    * into state FS_READY.
    * /note the initializing priority has nothing in common with the start priority
    * and will be evaluated separately
    * The lower the priority the later the participant will be triggered.
    * Negative values are allowed
    *
    * @param [in] priority    priority, the participant should use for loading, initialization
    * @see @ref fep3::System::load, fep3::System::initialize
    */
    void setInitPriority(int32_t priority);
    /**
     * @brief Get the Init Priority
     *
     * @return int32_t the value of priority
     * @see @ref fep3::System::load, fep3::System::initialize
     */
    int32_t getInitPriority() const;
    /**
     * set priority that the participant should use when the system is triggered
     * into state running.
     * /note the starting priority has nothing in common with the init priority
     * and will be evaluated separately
     * The lower the priority the later the participant will be triggered.
     * Negative values are allowed
     *
     * @param [in] priority    priority, the participant should use for initialization
     * @see @ref fep3::System::start, @ref fep3::System::pause
     */
    void setStartPriority(int32_t priority);
    /**
     * @brief Get the Start Priority
     *
     * @return int32_t the value of priority
     * @see @ref fep3::System::start, @ref fep3::System::pause
     */
    int32_t getStartPriority() const;

    /**
     * @brief Get the Name of the participant
     *
     * @return std::string  the name of the participant
     */
    std::string getName() const;

    /**
     * @brief Get the Url of the participant
     *
     * @return std::string  the url of the participant
     */
    std::string getUrl() const;

    /**
     * @brief sets additional information might be needed internally
     *
     * @param key additional information identifier
     * @param value the value
     */
    void setAdditionalInfo(const std::string& key, const std::string& value);

    /**
     * @brief gets additional information corresponding with the participant
     *
     * @param key additional information identifier you want to retrieve
     * @param value_default the value if the key is not found
     *
     * @return std::string the value found or the value_default
     */
    std::string getAdditionalInfo(const std::string& key, const std::string& value_default) const;

    /**
     * @brief this internal function searches and connect to the participants RPC Service by name and id.
     * Please use i.e. getRPCComponentProxy<rpc::IParticipantStateMachine>(...)
     *
     * @param component_name the rpc server name of the rpc component
     * @param component_iid  the rpc server interface identifier of the rpc component to request
     * @param proxy_ptr will contain the valid interface if available
     * @return true the server is reachable
     * @return false the server is not reachable, the server object does not exist or the given interface is not supported
     * \throw runtime_error See exception for more information
     */
    bool getRPCComponentProxy(const std::string& component_name,
        const std::string& component_iid,
        IRPCComponentPtr& proxy_ptr) const;
    /**
     * @brief this internal function searches and connect to the participants RPC Server by id.
     * It uses the first found.
     * Please use i.e. getRPCComponentProxyByIID<rpc::IParticipantStateMachine>(...)
     *
     * @param component_iid  the rpc component service interface identifier looking for
     * @param proxy_ptr will contain the valid interface if available
     * @return true the server is reachable
     * @return false the server is not reachable, the server object does not exist or the given interface is not supported
     * \throw runtime_error See exception for more information
     */
    bool getRPCComponentProxyByIID(const std::string& component_iid,
        IRPCComponentPtr& proxy_ptr) const;

    /**
     * This will retrieve a rpc server object interface.
     * The fep3::ParticipantProxy does support following interfaces at the moment:
     * \li see @ref fep3::ParticipantProxy
     *
     * @tparam T RPC Interfaces which are valid (see fep3::rpc)
     * @return rpc_component<T> the fep_rpc_client interface
     * @retval empty RPCComponent<T> if not found.
     */
    template<typename T>
    RPCComponent<T> getRPCComponentProxy() const
    {
        RPCComponent<T> component_proxy;
        if (getRPCComponentProxy(fep3::rpc::getRPCDefaultName<T>(), fep3::rpc::getRPCIID<T>(), component_proxy))
        {
            return component_proxy;
        }
        else
        {
            component_proxy.reset();
            return component_proxy;
        }
    }

    /**
     * This will retrieve a rpc server object interface by iid. it returns the first found.
     * The fep3::ParticipantProxy does support following interfaces at the moment:
     * \li see @ref fep3::ParticipantProxy
     *
     * @tparam T RPC Interfaces which are valid (see fep3::rpc)
     * @return rpc_component<T> the fep_rpc_client interface
     * @retval empty RPCComponent<T> if not found.
     */
    template<typename T>
    RPCComponent<T> getRPCComponentProxyByIID() const
    {
        RPCComponent<T> component_proxy;
        if (getRPCComponentProxyByIID(fep3::rpc::getRPCIID<T>(), component_proxy))
        {
            return component_proxy;
        }
        else
        {
            component_proxy.reset();
            return component_proxy;
        }
    }
    /// @cond no_documentation
private:
    struct Implementation;
    std::shared_ptr<Implementation> _impl;
    /// @endcond no_documentation
};
}
