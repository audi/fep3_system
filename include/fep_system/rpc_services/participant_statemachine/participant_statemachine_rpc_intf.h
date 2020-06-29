/**
 * Declaration of the Class IRPCParticipantInfo. (can be reached from over rpc)
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

#include <vector>
#include <string>

#include "participant_statemachine_rpc_intf_def.h"


namespace fep3
{
namespace rpc
{
namespace arya
{
/**
 * @brief Participant State
 * 
 */
enum ParticipantState
{
    ///This state is used if a participant has no StateMachine or is not reachable (maybe shutdowned)
    undefined,
    ///This state is used if a state machine was detected before, but currently the state can not be obtained (no answer)
    unreachable,
    ///valid unloaded state
    unloaded,
    ///valid loaded state
    loaded,
    ///valid initialized state
    initialized,
    ///valid paused state
    paused,
    ///valid runing state
    running
};

/**
* @brief definition of the external service interface of the participant itself
* @see participant_info.json file
*/
class IRPCParticipantStateMachine : public IRPCParticipantStateMachineDef
{
protected:
    /**
     * @brief Destroy the IRPCParticipantInfo object
     *
     */
    virtual ~IRPCParticipantStateMachine() = default;

public:
    /**
     * @brief State for particiants
     * @see ParticipantState
     */
    using State = ParticipantState;
    /**
     * @brief Get the state of the participant
     * 
     * @return see State 
     */
    virtual State getState() const = 0;
    /**
     * @brief sends a load event to the participants state machine
     * @throw logical_error if the event is invalid
     * 
     */
    virtual void load() = 0;
    /**
     * @brief sends a unload event to the participants state machine
     * @throw logical_error if the event is invalid
     * 
     */
    virtual void unload() = 0;
    /**
     * @brief sends a initalize event to the participants state machine
     * @throw logical_error if the event is invalid
     * 
     */
    virtual void initialize() = 0;
    /**
     * @brief sends a deinitialize event to the participants state machine
     * @throw logical_error if the event is invalid
     * 
     */
    virtual void deinitialize() = 0;
    /**
     * @brief sends a start event to the participants state machine
     * @throw logical_error if the event is invalid
     * 
     */
    virtual void start() = 0;
    /**
     * @brief sends a pause event to the participants state machine
     * @throw logical_error if the event is invalid
     * 
     */
    virtual void pause() = 0;
    /**
     * @brief sends a stop event to the participants state machine
     * @throw logical_error if the event is invalid
     * 
     */
    virtual void stop() = 0;
    /**
     * @brief sends a shutdown event to the participants state machine
     * @throw logical_error if the event is invalid
     * 
     */
    virtual void shutdown() = 0;
};
}
using arya::IRPCParticipantStateMachine;
using arya::ParticipantState;
}
}
