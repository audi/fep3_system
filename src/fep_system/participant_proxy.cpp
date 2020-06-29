/*
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

#include <fep_system/participant_proxy.h>
#include <private_participant_proxy.hpp>

namespace fep3
{

ParticipantProxy::~ParticipantProxy()
{
}

ParticipantProxy::ParticipantProxy(const std::string& participant_name,
    const std::string& participant_url,
    const std::string& system_name,
    const std::string& system_discovery_url,
    ISystemLogger& logger,
    std::chrono::milliseconds default_timeout)
{
    _impl.reset(new Implementation(participant_name,
                                   participant_url,
                                   system_name,
                                   system_discovery_url,
                                   logger,
                                   default_timeout));
}
ParticipantProxy::ParticipantProxy(ParticipantProxy&& other)
{
    _impl = other._impl;
}

ParticipantProxy& ParticipantProxy::operator=(ParticipantProxy&& other)
{
    _impl = other._impl;
    return *this;
}

ParticipantProxy::ParticipantProxy(const ParticipantProxy& other) : _impl(other._impl)
{
}

ParticipantProxy& ParticipantProxy::operator=(const ParticipantProxy& other)
{
    _impl = other._impl;
    return *this;
}

void ParticipantProxy::copyValuesTo(ParticipantProxy& other) const
{
    _impl->copyValuesTo(*(other._impl));
}

void ParticipantProxy::setInitPriority(int32_t priority)
{
    _impl->setInitPriority(priority);
}

void ParticipantProxy::setStartPriority(int32_t priority)
{
    _impl->setStartPriority(priority);
}

int32_t ParticipantProxy::getInitPriority() const
{
    return _impl->getInitPriority();
}

int32_t ParticipantProxy::getStartPriority() const
{
    return _impl->getStartPriority();
}

std::string ParticipantProxy::getName() const
{
    return _impl->getParticipantName();
}

std::string ParticipantProxy::getUrl() const
{
    return _impl->getParticipantURL();
}


void ParticipantProxy::setAdditionalInfo(const std::string& key, const std::string& value)
{
    _impl->setAdditionalInfo(key, value);
}

std::string ParticipantProxy::getAdditionalInfo(const std::string& key, const std::string& value_default) const
{
    return  _impl->getAdditionalInfo(key, value_default);
}

bool ParticipantProxy::getRPCComponentProxy(const std::string& component_name,
    const std::string& component_iid,
    IRPCComponentPtr& proxy_ptr) const
{
    return _impl->getRPCComponentProxy(component_name,
        component_iid,
        proxy_ptr);
}

bool ParticipantProxy::getRPCComponentProxyByIID(const std::string& component_iid,
    IRPCComponentPtr& proxy_ptr) const
{
    return _impl->getRPCComponentProxyByIID(component_iid,
        proxy_ptr);
}
}

