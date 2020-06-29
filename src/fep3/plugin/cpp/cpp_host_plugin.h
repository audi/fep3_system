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

#include <fep3/fep3_participant_version.h>
#include <fep3/plugin/base/fep3_calling_convention.h>
#include <fep3/fep3_participant_types.h>
#include <fep3/plugin/cpp/cpp_plugin_intf.h>
#include <fep3/plugin/base/host_plugin_base.h>
#include <memory>
#include <sstream>
#include <string>

namespace fep3
{
namespace plugin
{
namespace cpp
{
namespace arya
{
    class HostPlugin
        : public HostPluginBase
    {
        public:
            explicit HostPlugin(const std::string& file_path);
            HostPlugin(HostPlugin&&) = default;
            virtual ~HostPlugin();

            /**
             * Gets the version namespace of the plugin as string
             * @return The version namespace of the plugin as string
             */
            virtual std::string getVersionNamespace() const;

            /**
             * Creates an object of type @p t from within the plugin
             * by calling the factory function symbol \p factory_function_symbol_name in the plugin.
             *
             * @tparam t The type of the object to be created
             * @tparam argument_types Types of arguments to be forwarded to the factory function
             * @param factory_function_symbol_name The name of the factory function symbol
             * @param arguments Arguments to be forwarded to the factory function
             * @return Unique pointer to the created object if the @p factory_function_symbol_name
             *          was found, empty unique pointer otherwise
             */
            template<typename t, typename... argument_types>
            std::unique_ptr<t> create(const std::string& factory_function_symbol_name, argument_types&&... arguments) const
            {
                typedef t* (FEP3_PLUGIN_CALL FactoryFunction)(argument_types...);
                const auto& factory_function = get<FactoryFunction>(factory_function_symbol_name);
                if (nullptr == factory_function)
                {
                    std::ostringstream error;
                    error << "The plugin '" << getFilePath() << "' does not provide an appropriate '"
                        << factory_function_symbol_name << "' function."
                        << "Note: This participant uses version " << FEP3_PARTICIPANT_LIBRARY_VERSION_MAJOR
                        << "." << FEP3_PARTICIPANT_LIBRARY_VERSION_MINOR
                        << " of the fep_participant library, the plugin's version is " << getVersionNamespace() << ".";
                    throw std::runtime_error(error.str());
                }
                // note: the factory function transfers ownership of the created object to the caller
                const auto& pointer_to_created_object = factory_function(std::forward<argument_types>(arguments)...);
                return std::unique_ptr<t>(pointer_to_created_object);
            }

        private:
            std::string _version_namespace{"unknown"};
    };
} // namespace arya
using arya::HostPlugin;
} // namespace cpp
} // namespace plugin
} // namespace fep3