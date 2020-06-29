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
 *
 */

#pragma once

namespace fep3
{
namespace logging
{
/**
 * this is a namespace for porting convenience
 * this will be removed in further releases 
 */
namespace legacy
{
///Category for the log
enum Category : uint32_t
{
    ///any category for logging events that does not fit into any of the other
    CATEGORY_NONE = 0,
    ///Marks log event event that occurred on system level.
    ///There might be log events on participant category before
    CATEGORY_SYSTEM = 1,
    ///Marks log event event that occurred on participant level.
    /// there might be log events on component or element category before
    CATEGORY_PARTICIPANT = 2,
    ///Marks log event event that occurred on component level
    ///these components are StateMachine, DataRegistry, SimulationBus
    CATEGORY_COMPONENT = 3,
    ///Marks log event event that occurred on element functionality level
    ///the element category is for user level implemented within the element functionality
    ///one log may be: "ESP did only found 3 wheels, where is the fourth?" (Decide by your own which fep::logging::Severity this will be.)
    CATEGORY_ELEMENT = 4
};
} // namespace legacy
using legacy::Category;
} // namespace logging
} // namespace fep3