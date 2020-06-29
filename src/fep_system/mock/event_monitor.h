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

#include <gmock/gmock.h>

#include <fep_system/event_monitor_intf.h>

namespace fep3
{
namespace mock 
{

struct EventMonitor : public fep3::IEventMonitor
{
    MOCK_METHOD5(onLog, void(std::chrono::milliseconds, logging::Severity, const std::string&, const std::string&, const std::string&));
};

}
} 