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

#include "cpp_host_plugin.h"
#include <a_util/filesystem.h>
#include <a_util/strings.h>
#include <fep3/plugin/cpp/cpp_plugin_intf.h>

#ifdef WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#include <string.h>
#endif

namespace fep3
{
namespace plugin
{
namespace cpp
{
namespace arya
{

    HostPlugin::HostPlugin(const std::string& file_path)
        : HostPluginBase
            (file_path
            // right now we must prevent unloading because shared binary management is not implemented in CPP Plugin System
            , true
            )
    {
        // on windows debug and non-debug builds are considered incompatible for C++ interfaces
#ifdef WIN32
        auto is_debug_plugin_function = get<bool()>(SYMBOL_fep3_plugin_cpp_isDebugPlugin);
        if (!is_debug_plugin_function)
        {
            throw std::runtime_error("the plugin '" + file_path + "' does not provide the required '"
                + SYMBOL_fep3_plugin_cpp_isDebugPlugin + "' function");
        }

#ifdef _DEBUG
        if (!is_debug_plugin_function())
        {
            throw std::runtime_error("the plugin '" + file_path + "' is not compiled in debug mode which this executable is");
        }
#else
        if (is_debug_plugin_function())
        {
            throw std::runtime_error("the plugin '" + file_path + "' is compiled in debug mode which this executable is not");
        }
#endif
#endif

        auto get_participant_library_version_function = get<fep3_plugin_base_ParticipantLibraryVersion()>(SYMBOL_fep3_plugin_getParticipantLibraryVersion);
        if(!get_participant_library_version_function)
        {
            throw std::runtime_error("The plugin '" + file_path + "' does not provide an appropriate '"
                + SYMBOL_fep3_plugin_getParticipantLibraryVersion + "' function.");
        }
        _participant_library_version = get_participant_library_version_function();

        auto get_version_namespace_function = get<const char*()>(SYMBOL_fep3_plugin_getVersionNamespace);
        if(!get_version_namespace_function)
        {
            throw std::runtime_error("The plugin '" + file_path + "' does not provide an appropriate '"
                + SYMBOL_fep3_plugin_getVersionNamespace + "' function.");
        }
        _version_namespace = get_version_namespace_function();
    }

    HostPlugin::~HostPlugin()
    {}

    std::string HostPlugin::getVersionNamespace() const
    {
        return _version_namespace;
    }

} // namespace arya
} // namespace cpp
} // namespace plugin
} // namespace fep3