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

#include <fep_system/fep_system.h>
#include <a_util/system/system.h>
#include <service_bus_factory.h>
#include "a_util/process.h"
#include "system_logger.h"
#include <map>
#include <mutex>
#include <thread>
#include <algorithm>
#include <iterator>

#include <fep3/components/clock/clock_service_intf.h>
#include <fep3/components/clock_sync/clock_sync_service_intf.h>
#include <fep3/components/scheduler/scheduler_service_intf.h>
#include <fep3/base/properties/property_type.h>

using namespace a_util::strings;
namespace
{
    std::string replaceDotsWithSlashes(const std::string& initial_string)
    {
        std::string temp_string = initial_string;
        std::replace(temp_string.begin(), temp_string.end(), '.', '/');
        return temp_string;
    }
}

namespace fep3
{
    static constexpr int min_timeout = 500;
    static constexpr int timeout_divident = 10;

    struct System::Implementation
    {
    public:
        explicit Implementation(const std::string& system_name)
            : _system_name(system_name),
              _system_discovery_url(arya::IServiceBusConnection::ISystemAccess::_use_default_url)
        {
            //we need to use _use_default_url here => only this is to use the default
            //if we do not do this and use empty, discovery is switched off
            _service_bus_connection = ServiceBusFactory::get().createOrGetServiceBusConnection(system_name, _system_discovery_url);
            _logger->initRPCService(_system_name);
        }

        explicit Implementation(const std::string& system_name,
                                const std::string& system_discovery_url)
            : _system_name(system_name),
              _system_discovery_url(system_discovery_url)
        {
            //if system_discovery_url is empty ... it will be switched off
            _service_bus_connection = ServiceBusFactory::get().createOrGetServiceBusConnection(system_name, system_discovery_url);
            _logger->initRPCService(_system_name);
        }

        Implementation(const Implementation& other) = delete;
        Implementation& operator=(const Implementation& other) = delete;

        // why move constructor is not called?
        Implementation& operator=(Implementation&& other)
        {
            _system_name = std::move(other._system_name);
            _system_discovery_url = std::move(other._system_discovery_url);
            _participants = std::move(other._participants);
            _logger = std::move(other._logger);
            _service_bus_connection = other._service_bus_connection;
            return *this;
        }

        ~Implementation()
        {
            clear();
        }

        std::vector<std::string> mapToStringVec() const
        { 
            std::vector<std::string> participants;
            for (const auto& p : _participants)
            {
                participants.push_back(p.getName());
            }
            return participants;
        }

        std::vector<ParticipantProxy> mapToProxyVec() const
        {
            return _participants;
        }

        std::map<int32_t, std::vector<ParticipantProxy>> getParticipantsSortedbyStartPrio()
        {
            std::map<int32_t, std::vector<ParticipantProxy>> participants_sorted_by_prio;
            std::vector<ParticipantProxy> participants;
            for (const auto& part : _participants)
            {
                auto prio = part.getStartPriority();
                participants_sorted_by_prio[prio].push_back(part);
            }
            return std::move(participants_sorted_by_prio);
        }

        std::map<int32_t, std::vector<ParticipantProxy>> getParticipantsSortedbyInitPrio()
        {
            std::map<int32_t, std::vector<ParticipantProxy>> participants_sorted_by_prio;
            std::vector<ParticipantProxy> participants;
            for (const auto& part : _participants)
            {
                auto prio = part.getInitPriority();
                participants_sorted_by_prio[prio].push_back(part);
            }
            return std::move(participants_sorted_by_prio);
        }

        void for_each_ordered_reverse(std::map<int32_t, std::vector<ParticipantProxy>>& sorted_parts,
            const std::function<void(ParticipantProxy&)>& call)
        {
            //reverse order of prio
            for (auto current_prio = sorted_parts.rbegin();
                 current_prio != sorted_parts.rend();
                 ++current_prio)
            {
                //normal order of parts having the same prio(unfortunatelly this is by name at the moment)
                auto& current_prio_parts = current_prio->second;
                for (auto& part_to_call : current_prio_parts)
                {
                    call(part_to_call);
                }
            }
        }

        void for_each_ordered(std::map<int32_t, std::vector<ParticipantProxy>>& sorted_parts,
            const std::function<void(ParticipantProxy&)>& call)
        {
            //normal order of prio
            for (auto current_prio = sorted_parts.begin();
                current_prio != sorted_parts.end();
                ++current_prio)
            {
                //reverse order of parts having the same prio (unfortunatelly this is by name at the moment)
                auto& current_prio_parts = current_prio->second;
                for (auto part_to_call = current_prio_parts.rbegin();
                     part_to_call != current_prio_parts.rend();
                     ++part_to_call)
                {
                    call(*part_to_call);
                }
            }
        }

        void reverse_state_change(std::chrono::milliseconds,
            const std::string& logging_info,
            bool init_false_start_true,
            const std::function<void(RPCComponent<rpc::IRPCParticipantStateMachine>&)>& call_at_state)
        {
            if (_participants.empty())
            {
                _logger->log(logging::Severity::warning, "",
                    _system_name, "No participants within the current system");
                return;
            }
            std::string error_message;
            std::map<int32_t,std::vector<ParticipantProxy>> sorted_part;
            if (!init_false_start_true)
            {
                sorted_part = getParticipantsSortedbyInitPrio();
            }
            else
            {
                sorted_part = getParticipantsSortedbyStartPrio();
            }
            for_each_ordered_reverse(sorted_part,
                [&](ParticipantProxy& proxy)
            {
                auto state_machine = proxy.getRPCComponentProxyByIID<rpc::IRPCParticipantStateMachine>();
                if (state_machine)
                {
                    try
                    {
                        call_at_state(state_machine);
                    }
                    catch (const std::exception& ex)
                    {
                        error_message += std::string(" ") + ex.what();
                    }
                }
            });
            if (!error_message.empty())
            {
                FEP3_SYSTEM_LOG_AND_THROW(
                    _logger,
                    logging::Severity::fatal,
                    "",
                    _system_name,
                    error_message);
            }

            _logger->log(logging::Severity::info, "",
                _system_name, "system " + logging_info + " successfully");
        }

        void normal_state_change(std::chrono::milliseconds,
            const std::string& logging_info,
            bool init_false_start_true,
            const std::function<void(RPCComponent<rpc::IRPCParticipantStateMachine>&)>& call_at_state)
        {
            if (_participants.empty())
            {
                _logger->log(logging::Severity::warning, "",
                    _system_name, "No participants within the current system");
                return;
            }
            std::string error_message;
            std::map<int32_t, std::vector<ParticipantProxy>> sorted_part;
            if (!init_false_start_true)
            {
                sorted_part = getParticipantsSortedbyInitPrio();
            }
            else
            {
                sorted_part = getParticipantsSortedbyStartPrio();
            }
            for_each_ordered(sorted_part,
                [&](ParticipantProxy& proxy)
            {
                auto state_machine = proxy.getRPCComponentProxyByIID<rpc::IRPCParticipantStateMachine>();
                if (state_machine)
                {
                    try
                    {
                        call_at_state(state_machine);
                    }
                    catch (const std::exception& ex)
                    {
                        error_message += std::string(" ") + ex.what();
                    }
                }
            });
            if (!error_message.empty())
            {
                FEP3_SYSTEM_LOG_AND_THROW(
                    _logger,
                    logging::Severity::fatal,
                    "",
                    _system_name,
                    error_message);
            }

            _logger->log(logging::Severity::info, "",
                _system_name, "system " + logging_info + " successfully");
        }
        

        void setSystemState(System::AggregatedState state, std::chrono::milliseconds timeout)
        {
            if (state == System::AggregatedState::unreachable
                || state == System::AggregatedState::undefined)
            {
                FEP3_SYSTEM_LOG_AND_THROW(_logger,
                    logging::Severity::error,
                    "",
                    getName(),
                    "Invalid setSystemState call at system " + getName());
            }
            auto currentState = getSystemState(timeout);
            if (currentState._state == System::AggregatedState::unreachable)
            {
                FEP3_SYSTEM_LOG_AND_THROW(_logger,
                    logging::Severity::error,
                    "",
                    getName(),
                    "At least one participant is unreachable, can not set homogenous state of the system " + getName());
            }
            else if (currentState._state == System::AggregatedState::undefined)
            {
                FEP3_SYSTEM_LOG_AND_THROW(_logger,
                    logging::Severity::error,
                    "",
                    getName(),
                    "No participant has a statemachine, can not set homogenous state of the system " + getName());
            }
            else if (currentState._state == state)
            {
                if (currentState._homogeneous)
                {
                    return;
                }
                else
                {
                    FEP3_SYSTEM_LOG_AND_THROW(_logger,
                        logging::Severity::error,
                        "",
                        getName(),
                        "No homogenous state of the participants, setSystemState is not possible at system " + getName());
                }
            }
            else if (currentState._state > state)
            {
                if (currentState._state == System::AggregatedState::running)
                {
                    if (state == System::AggregatedState::paused)
                    {
                        pause(timeout);
                    }
                    else
                    {
                        stop(timeout);
                    }
                }
                else if (currentState._state == System::AggregatedState::paused)
                {
                    stop(timeout);
                }
                else if (currentState._state == System::AggregatedState::initialized)
                {
                    deinitialize(timeout);
                }
                else if (currentState._state == System::AggregatedState::loaded)
                {
                    unload(timeout);
                }
                setSystemState(state, timeout);
                return;
            }
            else if (currentState._state < state)
            {
                if (currentState._state == System::AggregatedState::unloaded)
                {
                    load(timeout);
                }
                else if (currentState._state == System::AggregatedState::loaded)
                {
                    initialize(timeout);
                }
                else if (currentState._state == System::AggregatedState::initialized)
                {
                    if (state == System::AggregatedState::paused)
                    {
                        pause(timeout);
                    }
                    else
                    {
                        start(timeout);
                    }
                }
                else if (currentState._state == System::AggregatedState::paused)
                {
                    start(timeout);
                }
                setSystemState(state, timeout);
                return;
            }
        }

        void load(std::chrono::milliseconds timeout )
        {
            reverse_state_change(timeout, "loaded", false, 
                [&](RPCComponent<rpc::IRPCParticipantStateMachine>& state_machine)
                { 
                    if (state_machine)
                    {
                        state_machine->load();
                    }
                });
        }

        void unload(std::chrono::milliseconds timeout )
        {
            normal_state_change(timeout, "unloaded", false,
                [&](RPCComponent<rpc::IRPCParticipantStateMachine>& state_machine)
            {
                if (state_machine)
                {
                    state_machine->unload();
                }
            });
        }

        void initialize(std::chrono::milliseconds timeout)
        {
            reverse_state_change(timeout, "initialized", false,
                [&](RPCComponent<rpc::IRPCParticipantStateMachine>& state_machine)
            {
                if (state_machine)
                {
                    state_machine->initialize();
                }
            });
        }

        void deinitialize(std::chrono::milliseconds timeout )
        {
            normal_state_change(timeout, "deinitialized", false,
                [&](RPCComponent<rpc::IRPCParticipantStateMachine>& state_machine)
            {
                if (state_machine)
                {
                    state_machine->deinitialize();
                }
            });
        }

        void start(std::chrono::milliseconds timeout)
        {
            reverse_state_change(timeout, "started", true,
                [&](RPCComponent<rpc::IRPCParticipantStateMachine>& state_machine)
            {
                if (state_machine)
                {
                    state_machine->start();
                }
            });
        }
        
        void pause(std::chrono::milliseconds timeout )
        {
            reverse_state_change(timeout, "paused", false,
                [&](RPCComponent<rpc::IRPCParticipantStateMachine>& state_machine)
            {
                if (state_machine)
                {
                    state_machine->pause();
                }
            });
        }
        void stop(std::chrono::milliseconds timeout )
        {
            normal_state_change(timeout, "stopped", false,
                [&](RPCComponent<rpc::IRPCParticipantStateMachine>& state_machine)
            {
                if (state_machine)
                {
                    state_machine->stop();
                }
            });
        }

        void shutdown(std::chrono::milliseconds )
        {
            if (_participants.empty())
            {
                _logger->log(logging::Severity::warning, "",
                    _system_name + ".system", "No participants within the current system");
                return;
            }
            std::string error_message;
            //shutdown has no prio
            for (auto& part : _participants)
            {
                auto state_machine = part.getRPCComponentProxyByIID<rpc::IRPCParticipantStateMachine>();
                if (state_machine)
                {
                    try
                    {
                        state_machine->shutdown();
                    }
                    catch (const std::exception& ex)
                    {
                        error_message += std::string(" ") + ex.what();
                    }
                }
            }
            if (!error_message.empty())
            {
                FEP3_SYSTEM_LOG_AND_THROW(
                    _logger,
                    logging::Severity::fatal,
                    "",
                    _system_name,
                    error_message);
            }
            _logger->log(logging::Severity::info, "",
                _system_name, "system shutdowned successfully");
        }

        std::string getName()
        {
            return _system_name;
        }

        std::string getUrl()
        {
            return _system_discovery_url;
        }

        typedef std::map<std::string, rpc::arya::IRPCParticipantStateMachine::State> PartStates;
        //system state is aggregated
        //timeout can be only used if the RPC will 
        // support the changing of it or use for every single Request
        PartStates getParticipantStates(std::chrono::milliseconds )
        {
            
            PartStates states;
            for (auto part : _participants)
            {
                RPCComponent<rpc::arya::IRPCParticipantInfo> part_info;
                RPCComponent<rpc::arya::IRPCParticipantStateMachine> state_machine;
                part_info = part.getRPCComponentProxyByIID<rpc::arya::IRPCParticipantInfo>();
                state_machine = part.getRPCComponentProxyByIID<rpc::arya::IRPCParticipantStateMachine>();
                if (state_machine)
                {
                    //the participant can not be connected ... maybe it was shutdown or whatever
                    states[part.getName()] = state_machine->getState();
                }
                else
                {
                    if (!part_info)
                    {
                        //the participant can not be connected ... maybe it was shutdown or whatever
                        states[part.getName()] = { rpc::arya::IRPCParticipantStateMachine::State::unreachable };
                    }
                    else
                    {
                        //the participant has no state machine, this is ok 
                        //... i.e. a recorder will have no states and a signal listener tool will have no states
                        states[part.getName()] = { rpc::arya::IRPCParticipantStateMachine::State::unreachable };
                    }
                }
            }
            return std::move(states);
        }

        static System::State getAggregatedState(const PartStates& states)
        {
            //we begin at the highest value
            SystemAggregatedState aggregated_state = SystemAggregatedState::running;
            //we have a look if at least one of it is set
            bool first_set = false;
            bool homogeneous_value = true;
            for (const auto& item : states)
            {
                rpc::arya::IRPCParticipantStateMachine::State current_part_state = item.second;
                //we were at least one times in this loop
                if (current_part_state != rpc::arya::IRPCParticipantStateMachine::State::undefined)
                {
                    if (current_part_state < aggregated_state)
                    {
                        if (first_set)
                        {
                            //is not homogenous because some of the already checked states has an higher value
                            homogeneous_value = false;
                        }
                        aggregated_state = current_part_state;
                    }
                    if (current_part_state > aggregated_state)
                    {
                        //is not homogenous because some of the already checked states has an lower value
                        homogeneous_value = false;
                    }
                }
                //this is to check if this loop was already entered before we change 
                first_set = true;
            }
            //no participant has a statemachine, then we are also undefined
            if (!first_set) 
            {
                return { rpc::arya::IRPCParticipantStateMachine::State::undefined };
            }
            else
            {
                return { homogeneous_value , aggregated_state };
            }
        }

        System::State getSystemState(std::chrono::milliseconds timeout)
        {
            auto states = getParticipantStates(timeout);
            return getAggregatedState(states);
        }

        void registerMonitoring(IEventMonitor* monitor)
        {
            _logger->registerMonitor(monitor);
        }

        void releaseMonitoring()
        {
            _logger->releaseMonitor();
        }

        void setSeverityLevel(logging::Severity level)
        {
            _logger->setSeverityLevel(level);
        }

        void clear()
        {
            _participants.clear();
        }

        void add(const std::string& participant_name, const std::string& participant_url)
        {
            auto part_found = getParticipant(participant_name, false);
            if (part_found)
            {
                FEP3_SYSTEM_LOG_AND_THROW(_logger,
                    logging::Severity::fatal,
                    "",
                    _system_name,
                    "Try to add a participant with name "
                    + participant_name + " which already exists.");
            }
            _participants.push_back(ParticipantProxy(participant_name,
                participant_url,
                _system_name,
                _system_discovery_url,
                *_logger.get(),
                PARTICIPANT_DEFAULT_TIMEOUT));
        }

        void remove(const std::string& participant_name)
        {
            auto found = _participants.begin();
            for (;
                found != _participants.end();
                ++found)
            {
                if (found->getName() == participant_name)
                {
                    break;
                }
            }
            if (found != _participants.end())
            {
                _participants.erase(found);
            }
        }

        ParticipantProxy getParticipant(const std::string& participant_name, bool throw_if_not_found) const
        {
            for (auto& part_found : _participants)
            {
                if (part_found.getName() == participant_name)
                {
                    return part_found;
                }
            }
            if (throw_if_not_found)
            {
                FEP3_SYSTEM_LOG_AND_THROW(_logger,
                    logging::Severity::fatal,
                    "",
                    _system_name,
                    "No Participant with the name " + participant_name + " found");
            }
            return {};
        }

        std::vector<ParticipantProxy> getParticipants() const
        {
            return mapToProxyVec();
        }

        void setPropertyValue(const std::string& participant,
            const std::string& node,
            const std::string& property_name,
            const std::string& value,
            const std::string& type) const
        {
            const auto property_normalized = replaceDotsWithSlashes(property_name);

            auto part = getParticipant(participant, false);
            if (part)
            {
                
                auto config_rpc_client = part.getRPCComponentProxyByIID<fep3::rpc::IRPCConfiguration>();
                auto props = config_rpc_client->getProperties(node);
                if (!props)
                {
                    throw std::runtime_error(format("access to properties node %s not possible", node));
                }

                if (!props->setProperty(property_normalized, value, type))
                {                           
                    const auto message = format("property %s could not be set for the following participant: %s"
                        , property_normalized.c_str()
                        , part.getName().c_str());

                    throw std::runtime_error(message);
                }    
            }
            else
            {
                FEP3_SYSTEM_LOG_AND_THROW(_logger,
                    logging::Severity::fatal,
                    "",
                    _system_name,
                    format("participant %s within system %s not found to configure %s",
                        participant.c_str(),
                        _system_name.c_str(),
                        property_normalized.c_str()));
            }
        }

        void setPropertyValueToAll(const std::string& node,
            const std::string& property_name,
            const std::string& value,
            const std::string& type,
            const std::string& except_participant = std::string(),
			const bool throw_on_failure = true) const
        {
            const auto property_normalized = replaceDotsWithSlashes(property_name);
            auto failing_participants = std::vector<std::string>();

            for (const ParticipantProxy& participant : getParticipants())
            {
                if (!except_participant.empty())
                {
                    if (participant.getName() == except_participant)
                    {
                        continue;
                    }
                }

				try
				{
					auto config_rpc_client = participant.getRPCComponentProxyByIID<fep3::rpc::IRPCConfiguration>();
					auto props = config_rpc_client->getProperties(node);
					if (props)
					{
						const auto success = props->setProperty(property_normalized, value, type);
						if (!success)
						{
							failing_participants.push_back(participant.getName());
						}
					}
				}
				catch (const std::exception& /*exception*/)
				{
					failing_participants.push_back(participant.getName());
				}
            }

            if (!failing_participants.empty())
            {
				const auto participants = join(failing_participants, ", ");
				const auto message = format("property %s could not be set for the following participants: %s"
					, property_normalized.c_str()
					, participants.c_str());
            	
            	if (throw_on_failure)
            	{
					throw std::runtime_error(message);
            	}
            	
				FEP3_SYSTEM_LOG(_logger,
					logging::Severity::warning,
					"",
					_system_name,
					message);
            }
        }

		void setSystemProperty(const std::string& path,
			const std::string& type,
			const std::string& value) const
		{
			const auto path_normalized = replaceDotsWithSlashes(path);
			auto split_path = split(path_normalized, std::string(1, '/'));
			split_path.insert(split_path.begin(), "system");

			const auto property_name = split_path.back();

        	split_path.erase(split_path.end() - 1);
        	
			const auto node_path = join(split_path, "/");

        	
			setPropertyValueToAll(node_path, property_name, value, type, "", false);
		}

        void configureTiming(const std::string& master_clock_name, const std::string& slave_clock_name,
            const std::string& scheduler, const std::string& master_element_id, const std::string& master_time_stepsize,
            const std::string& master_time_factor, const std::string& slave_sync_cycle_time) const
        {
            setPropertyValueToAll("/",
                FEP3_CLOCKSYNC_SERVICE_CONFIG_TIMING_MASTER,
                master_element_id, fep3::PropertyType<std::string>::getTypeName());
            setPropertyValueToAll("/",
                FEP3_SCHEDULER_SERVICE_SCHEDULER,
                scheduler, fep3::PropertyType<std::string>::getTypeName());

            if (!master_element_id.empty())
            {
                setPropertyValueToAll("/",
                    FEP3_CLOCK_SERVICE_MAIN_CLOCK,
                    slave_clock_name, fep3::PropertyType<std::string>::getTypeName(), master_element_id);
                setPropertyValue(master_element_id,
                    "/", FEP3_CLOCK_SERVICE_MAIN_CLOCK,
                    master_clock_name, fep3::PropertyType<std::string>::getTypeName());
                if (!master_time_factor.empty())
                {
                    
                    setPropertyValue(master_element_id,
                        "/", FEP3_CLOCK_SERVICE_CLOCK_SIM_TIME_TIME_FACTOR,
                        master_time_factor, fep3::PropertyType<double>::getTypeName());
                }
                if (!master_time_stepsize.empty())
                {
                    setPropertyValue(master_element_id, 
                        "/", FEP3_CLOCK_SERVICE_CLOCK_SIM_TIME_CYCLE_TIME, 
                        master_time_stepsize, fep3::PropertyType<int32_t>::getTypeName());
                }
                if (!slave_sync_cycle_time.empty())
                {
                    setPropertyValueToAll("/",
                        FEP3_CLOCKSYNC_SERVICE_CONFIG_SLAVE_SYNC_CYCLE_TIME,
                        slave_sync_cycle_time, fep3::PropertyType<int32_t>::getTypeName(), master_element_id);
                }
            }
            else
            {
                setPropertyValueToAll("/",
                    FEP3_CLOCK_SERVICE_MAIN_CLOCK, slave_clock_name, fep3::PropertyType<std::string>::getTypeName());
            }
        }

        std::vector<std::string> getCurrentTimingMasters() const
        {
            std::vector<std::string> timing_masters_found;
            for (ParticipantProxy participant : getParticipants())
            {
                auto config_rpc_client = participant.getRPCComponentProxyByIID<fep3::rpc::IRPCConfiguration>();
                auto props = config_rpc_client->getProperties(FEP3_CLOCKSYNC_SERVICE_CONFIG);
                if (props)
                {
                    auto master_found = props->getProperty(FEP3_TIMING_MASTER_PROPERTY);
                    if (!master_found.empty())
                    {
                        auto found_it = std::find(timing_masters_found.cbegin(), timing_masters_found.cend(), master_found);
                        if (found_it != timing_masters_found.cend())
                        {
                            //is already in list
                        }
                        else
                        {
                            timing_masters_found.push_back(master_found);
                        }
                    }
                }
            }
            return timing_masters_found;
        }

        std::map<std::string, std::unique_ptr<IProperties>> getTimingProperties() const
        {
            std::map<std::string, std::unique_ptr<IProperties>> timing_properties;
            for (const ParticipantProxy& participant : getParticipants())
            {
                auto iterator_success = timing_properties.emplace(participant.getName(),
                    std::unique_ptr<IProperties>(new Properties<IProperties>()));
                if (!iterator_success.second)
                {
                    FEP3_SYSTEM_LOG_AND_THROW(_logger, logging::Severity::fatal, "", _system_name,
                        "Multiple Participants with the name " + participant.getName() + " found");
                }
                IProperties& participant_properties = *iterator_success.first->second;
                auto set_if_present = [&participant_properties](const fep3::arya::IProperties& properties, const std::string& config_name) {
                    auto value = properties.getProperty(config_name);
                    if (value.empty())
                    {
                        return false;
                    }
                    return participant_properties.setProperty(config_name, value, properties.getPropertyType(config_name));
                };

                auto config_rpc_client = participant.getRPCComponentProxyByIID<fep3::rpc::IRPCConfiguration>();
                auto clockservice_props = config_rpc_client->getProperties(FEP3_CLOCK_SERVICE_CONFIG);
                if (clockservice_props)
                {
                    set_if_present(*clockservice_props, FEP3_MAIN_CLOCK_PROPERTY);
                }
                auto clocksync_props = config_rpc_client->getProperties(FEP3_CLOCKSYNC_SERVICE_CONFIG);
                if (clocksync_props)
                {
                    if (set_if_present(*clocksync_props, FEP3_TIMING_MASTER_PROPERTY))
                    {
                        if (clockservice_props)
                        {
                            set_if_present(*clockservice_props, FEP3_CLOCK_SIM_TIME_TIME_FACTOR_PROPERTY);
                            set_if_present(*clockservice_props, FEP3_CLOCK_SIM_TIME_CYCLE_TIME_PROPERTY);
                            set_if_present(*clockservice_props, FEP3_TIME_UPDATE_TIMEOUT_PROPERTY);
                        }
                        set_if_present(*clocksync_props, FEP3_SLAVE_SYNC_CYCLE_TIME_PROPERTY);
                    }

                }
                auto scheduler_props = config_rpc_client->getProperties(FEP3_SCHEDULER_SERVICE_CONFIG);
                if (scheduler_props)
                {
                    set_if_present(*scheduler_props, FEP3_SCHEDULER_PROPERTY);
                }
            }
            return timing_properties;
        }

        //mutable std::mutex _system_mutex;
        std::vector<ParticipantProxy> _participants;
        std::shared_ptr<SystemLogger> _logger = std::make_shared<SystemLogger>();
        std::string _system_name;
        std::string _system_discovery_url;
        std::shared_ptr<arya::IServiceBusConnection> _service_bus_connection;
    };

    System::System() : _impl(new Implementation(""))
    {
    }

    System::System(const std::string& system_name) : _impl(new Implementation(system_name))
    {
    }

    System::System(const std::string& system_name,
                   const std::string& system_discovery_url) : _impl(new Implementation(system_name,
                                                                                       system_discovery_url))
    {
    }

    System::System(const System& other) : _impl(new Implementation(other.getSystemName(),
          other.getSystemUrl()))
    {
        auto proxies = other.getParticipants();
        for (const auto& proxy : proxies)
        {
            _impl->add(proxy.getName(), proxy.getUrl());
            auto new_proxy = _impl->getParticipant(proxy.getName(), true);
            proxy.copyValuesTo(new_proxy);
        }
    }

    System& System::operator=(const System& other)
    {
        _impl->_system_name = getSystemName();
        auto proxies = other.getParticipants();
        for (const auto& proxy : proxies)
        {
            _impl->add(proxy.getName(), proxy.getUrl());
            auto new_proxy = _impl->getParticipant(proxy.getName(), true);
            proxy.copyValuesTo(new_proxy);
        }
        return *this;
    }

    System::System(System&& other)
    {
        std::swap(_impl, other._impl);
    }

    System& System::operator=(System&& other)
    {
        std::swap(_impl, other._impl);
        return *this;
    }

    System::~System()
    {
    }

    void System::setSystemState(System::AggregatedState state, std::chrono::milliseconds timeout) const
    {
        _impl->setSystemState(state, timeout);
    }

    void System::load(std::chrono::milliseconds timeout /*= FEP_SYSTEM_TRANSITION_TIME*/) const
    {
        _impl->load(timeout);
    }

    void System::unload(std::chrono::milliseconds timeout /*= FEP_SYSTEM_TRANSITION_TIME*/) const
    {
        _impl->unload(timeout);
    }

    void System::initialize(std::chrono::milliseconds timeout /*= FEP_SYSTEM_TRANSITION_TIME*/) const
    {
        _impl->initialize(timeout);
    }
    void System::deinitialize(std::chrono::milliseconds timeout /*= FEP_SYSTEM_TRANSITION_TIME*/) const
    {
        _impl->deinitialize(timeout);
    }

    void System::start(std::chrono::milliseconds timeout /*= FEP_SYSTEM_TRANSITION_TIME*/) const
    {
        _impl->start(timeout);
    }

    void System::stop(std::chrono::milliseconds timeout/*= FEP_SYSTEM_TRANSITION_TIME*/) const
    {
        _impl->stop(timeout);
    }

    void System::pause(std::chrono::milliseconds timeout /*= FEP_SYSTEM_TRANSITION_TIME*/) const
    {
        _impl->pause(timeout);
    }

    void System::shutdown(std::chrono::milliseconds timeout /*= FEP_SYSTEM_TRANSITION_TIME*/) const
    {
        _impl->shutdown(timeout);
    }
    

    void System::add(const std::string& participant, const std::string& participant_url)
    {
        _impl->add(participant, participant_url);
    }

    void System::add(const std::vector<std::string>& participants)
    {
        for (const auto& participant : participants)
        {
            _impl->add(participant, std::string());
        }
    }

    void System::add(const std::multimap<std::string, std::string>& participants)
    {
        for (const auto& participant : participants)
        {
            _impl->add(participant.first, participant.second);
        }
    }

    void System::remove(const std::string& participant)
    {
        _impl->remove(participant);
    }

    void System::remove(const std::vector<std::string>& participants)
    {
        for (const auto& participant : participants)
        {
            _impl->remove(participant);
        }
    }

    void System::clear()
    {
        _impl->clear();
    }

    System::State System::getSystemState(std::chrono::milliseconds timeout /*= FEP_SYSTEM_DEFAULT_TIMEOUT_MS*/) const
    {
        return _impl->getSystemState(timeout);
    }

    std::string System::getSystemName() const
    {
        return _impl->getName();
    }

    std::string System::getSystemUrl() const
    {
        return _impl->getUrl();
    }

    ParticipantProxy System::getParticipant(const std::string& participant_name) const
    {
        return _impl->getParticipant(participant_name, true);
    }

    std::vector<ParticipantProxy> System::getParticipants() const
    {
        return _impl->getParticipants();
    }

    void System::registerMonitoring(IEventMonitor& pEventListener)
    {
        _impl->registerMonitoring(&pEventListener);
    }

    void System::unregisterMonitoring(IEventMonitor&)
    {
        _impl->releaseMonitoring();
    }

    void System::setSeverityLevel(logging::Severity severity_level)
    {
        _impl->setSeverityLevel(severity_level);
    }

	void System::setSystemProperty(const std::string& path,
		const std::string& type,
		const std::string& value) const
	{
		_impl->setSystemProperty(path, type, value);
	}

    void System::configureTiming(const std::string& master_clock_name, const std::string& slave_clock_name,
        const std::string& scheduler, const std::string& master_element_id, const std::string& master_time_stepsize,
        const std::string& master_time_factor, const std::string& slave_sync_cycle_time) const
    {
        _impl->configureTiming(master_clock_name, slave_clock_name, scheduler, master_element_id, 
            master_time_stepsize, master_time_factor, slave_sync_cycle_time);
    }

    std::vector<std::string> System::getCurrentTimingMasters() const
    {
        return _impl->getCurrentTimingMasters();
    }

    std::map<std::string, std::unique_ptr<IProperties>> System::getTimingProperties() const
    {
        return _impl->getTimingProperties();
    }

    void System::configureTiming3NoMaster() const
    {
        _impl->configureTiming("", FEP3_CLOCK_LOCAL_SYSTEM_REAL_TIME, 
            FEP3_SCHEDULER_CLOCK_BASED, "", "", "", "");
    }

    void System::configureTiming3ClockSyncOnlyInterpolation(const std::string& master_element_id, const std::string& slave_sync_cycle_time) const
    {
        _impl->configureTiming(FEP3_CLOCK_LOCAL_SYSTEM_REAL_TIME,
            FEP3_CLOCK_SLAVE_MASTER_ONDEMAND,
            FEP3_SCHEDULER_CLOCK_BASED,
            master_element_id, "", "", slave_sync_cycle_time);
    }
    void System::configureTiming3ClockSyncOnlyDiscrete(const std::string& master_element_id, const std::string& slave_sync_cycle_time) const
    {
        _impl->configureTiming(FEP3_CLOCK_LOCAL_SYSTEM_REAL_TIME,
            FEP3_CLOCK_SLAVE_MASTER_ONDEMAND_DISCRETE,
            FEP3_SCHEDULER_CLOCK_BASED,
            master_element_id, "", "", slave_sync_cycle_time);
    }
    void System::configureTiming3DiscreteSteps(const std::string& master_element_id, const std::string& master_time_stepsize, const std::string& master_time_factor) const
    {
        _impl->configureTiming(FEP3_CLOCK_LOCAL_SYSTEM_SIM_TIME,
            FEP3_CLOCK_SLAVE_MASTER_ONDEMAND_DISCRETE,
            FEP3_SCHEDULER_CLOCK_BASED,
            master_element_id, master_time_stepsize, master_time_factor, "");
    }

    void System::configureTiming3AFAP(const std::string& master_element_id, const std::string& master_time_stepsize) const
    {
        _impl->configureTiming(FEP3_CLOCK_LOCAL_SYSTEM_SIM_TIME,
            FEP3_CLOCK_SLAVE_MASTER_ONDEMAND_DISCRETE,
            FEP3_SCHEDULER_CLOCK_BASED,
            master_element_id, master_time_stepsize, "0.0", "");
    }

/**************************************************************
* discoveries 
***************************************************************/


    System discoverSystem(std::string name, std::chrono::milliseconds timeout /*= FEP_SYSTEM_DISCOVER_TIME_MS*/)
    {
        return discoverSystemByURL(name, arya::IServiceBusConnection::ISystemAccess::_use_default_url, timeout);
    }
    
    System discoverSystemByURL(std::string name, std::string discover_url, std::chrono::milliseconds timeout)
    {
        // default discover is actually DDS with domain 0
        auto my_discovery_bus = ServiceBusFactory::get().createOrGetServiceBusConnection(name,
            discover_url);
        if (my_discovery_bus)
        {
            //we check if that system access exists ... the service bus connection must create it for me
            auto sys_access = my_discovery_bus->getSystemAccess(name);
            if (!sys_access)
            {
                throw std::runtime_error("can not find a system access on service bus connection to system '"
                    + name + "' at url '" + discover_url);
            }
            auto participants = sys_access->discover(timeout);
            System discovered_system(name, discover_url);
            for (auto& part : participants)
            {
                discovered_system.add(part.first, part.second);
            }
            return std::move(discovered_system);
        }
        throw std::runtime_error("can not create a service bus connection to system '"
            + name + "' at url '" + discover_url + "'");
    }
  
    std::vector<System> discoverAllSystems(std::chrono::milliseconds timeout /*= FEP_SYSTEM_DISCOVER_TIMEOUT*/)
    {
        return discoverAllSystemsByURL(arya::IServiceBusConnection::ISystemAccess::_use_default_url ,timeout);
    }

   
    std::vector<System> discoverAllSystemsByURL(std::string discover_url,
        std::chrono::milliseconds timeout /*= FEP_SYSTEM_DISCOVER_TIMEOUT*/)
    {
        // default discover is actually DDS with domain 0
        auto my_discovery_bus = ServiceBusFactory::get().createOrGetServiceBusConnection(
            arya::IServiceBusConnection::ISystemAccess::_discover_all_systems,
            discover_url);
        if (my_discovery_bus)
        {
            //we check if that system access already exists
            //we check if that system access already exists
            auto sys_access = my_discovery_bus->getSystemAccess(arya::IServiceBusConnection::ISystemAccess::_discover_all_systems);
            if (!sys_access)
            {
                throw std::runtime_error("can not create a system access on service bus connection to discover all systems at url '"
                    + discover_url);
            }
            std::map<std::string, std::unique_ptr<System>> all_systems_map;

            auto all_participants = sys_access->discover(timeout);
            for (auto& part : all_participants)
            {
                auto splitted_id =  a_util::strings::split(part.first, "@", true);
                if (splitted_id.size() > 0)
                {
                    auto existing_system = all_systems_map.find(splitted_id[1]);
                    if (existing_system != all_systems_map.end())
                    {
                        existing_system->second->add(splitted_id[0], part.second);
                    }
                    else
                    {
                        all_systems_map[splitted_id[1]] = std::make_unique<System>(splitted_id[1]);
                        all_systems_map[splitted_id[1]]->add(splitted_id[0], part.second);
                    }
                }
                else
                {
                    //found invalid id / do not know if i inform about that 
                }
            }
            std::vector<System> result_vector_system;
            for (auto& found_sys : all_systems_map)
            {
                result_vector_system.emplace_back(std::move(*found_sys.second.release()));
            }

            return std::move(result_vector_system);
        }
        throw std::runtime_error("can not create a service bus connection to discover all systems at url ''"
            + discover_url + "'");
    }
    
}