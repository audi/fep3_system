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

#include <fep3/plugin/base/shared_library.h>
#include <fep3/plugin/base/plugin_base_intf.h>

namespace fep3
{
namespace plugin
{
namespace arya
{

/**
 * Class representing the participant library version
 */
class ParticipantLibraryVersion
{
public:
    ParticipantLibraryVersion() = default;
    ParticipantLibraryVersion
        (const std::string& id
        , int32_t major
        , int32_t minor
        , int32_t patch
        , int32_t build
        )
        : _id(id)
        , _major(major)
        , _minor(minor)
        , _patch(patch)
        , _build(build)
    {}
    ParticipantLibraryVersion(const fep3_plugin_base_ParticipantLibraryVersion& participant_library_version)
        : _id(participant_library_version._id)
        , _major(participant_library_version._major)
        , _minor(participant_library_version._minor)
        , _patch(participant_library_version._patch)
        , _build(participant_library_version._build)
    {}
    bool operator==(const ParticipantLibraryVersion& rhs) const
    {
        return  _id == rhs._id
            && _major == rhs._major
            && _minor == rhs._minor
            && _patch == rhs._patch
            && _build == rhs._build
            ;
    }
private:
    std::string _id;
    int32_t _major{0};
    int32_t _minor{0};
    int32_t _patch{0};
    int32_t _build{0};
};

class HostPluginBase
    //: public boost::dll::shared_library
    : public SharedLibrary
{
public:
    explicit HostPluginBase(const std::string& file_path, bool prevent_unloading = false);
    HostPluginBase(HostPluginBase&&) = default;
    virtual ~HostPluginBase();

    virtual std::string getPluginVersion() const;
    virtual ParticipantLibraryVersion getParticipantLibraryVersion() const;

protected:
    std::string _plugin_version{"unknown"};
    ParticipantLibraryVersion _participant_library_version{};
};

} // namespace arya
using arya::ParticipantLibraryVersion;
using arya::HostPluginBase;
} // namespace plugin
} // namespace fep3