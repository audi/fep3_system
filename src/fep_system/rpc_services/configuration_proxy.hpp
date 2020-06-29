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
#include <regex>

//this will be installed !!
#include "rpc_services/configuration/configuration_rpc_intf.h"
#include <fep_system_stubs/configuration_service_proxy_stub.h>
#include "base/properties/property_type.h"
#include "base/properties/property_type_conversion.h"
#include "system_logger_intf.h"

#include <a_util/strings.h>

#define FEP3_CONFIG_LOG_RESULT(_res_, _participant_name_, _component_name_, _method_, _path_) { \
_logger.log(logging::Severity::error, _participant_name_, \
_component_name_, \
a_util::strings::format("Can not %s '%s' due to following error : (%d - %s)  ", \
    _method_.c_str(), \
    _path_.c_str(), \
    _res_.getErrorCode(), \
    _res_.getErrorLabel(), \
    _res_.getDescription())); }

#define FEP3_CONFIG_LOG_AND_THROW_RESULT(_res_, _participant_name_, _component_name_, _method_, _path_) \
FEP3_CONFIG_LOG_RESULT(_res_, _participant_name_, _component_name_, _method_, _path_) \
throw std::runtime_error{ "Could not execute " + _participant_name_ + "->" + _component_name_ + "->" + _method_ + " - Error: " + _res_.getErrorLabel() };

namespace fep3
{
namespace rpc
{
namespace arya
{

using RPCConfigProxy = RPCServiceClientProxy< rpc_proxy_stub::RPCConfigurationServiceProxy,
    IRPCConfiguration>;

using RPCConfigClient = RPCServiceClient< rpc_proxy_stub::RPCConfigurationServiceProxy,
    IRPCConfiguration>;

class ConfigurationProxy : public RPCConfigProxy
{
private:
    friend class ConnectionInterfaceProperty;

    class ConfigurationProperty : public RPCConfigClient,
                                  public IProperties
    {
        std::string                       _property_path;
        std::string                       _participant_name;
        std::string                       _component_name;
        ISystemLogger&                    _logger;
        const std::regex                  _property_path_regex = std::regex("([/]?([a-zA-Z0-9_]+[/]?)*)");

    public:
        ConfigurationProperty() = delete;
        ~ConfigurationProperty() = default;
        ConfigurationProperty(ConfigurationProperty&&) = delete;
        ConfigurationProperty(const ConfigurationProperty&) = delete;
        ConfigurationProperty& operator=(ConfigurationProperty&&) = delete;
        ConfigurationProperty& operator=(const ConfigurationProperty&) = delete;
        ConfigurationProperty(
            const std::string& participant_name,
            const std::string& component_name,
            const std::shared_ptr<rpc::IRPCRequester>& rpc,
            std::string property_path,
            ISystemLogger& logger) : 
            RPCConfigClient(component_name, rpc),
            _property_path(std::move(property_path)),
            _logger(logger),
            _component_name(component_name),
            _participant_name(participant_name)
        {
        }

        bool setProperty(const std::string& name,
            const std::string& value,
            const std::string& type)
        {
            std::string path = _property_path + normalizeName(name);
            if (!isPropertyPathValid(path))
            {
                fep3::Result result(ERR_INVALID_ARG);
                FEP3_CONFIG_LOG_RESULT(result,
                    _participant_name,
                    _component_name,
                    std::string("setProperty"), path);
                return false;
            }
            else
            {
                int32_t retval = GetStub().setProperty(path, type, value);
                if (retval == 0)
                {
                    return true;
                }
                else
                {
                    fep3::Result ret_code(retval);
                    FEP3_CONFIG_LOG_RESULT(ret_code,
                        _participant_name,
                        _component_name,
                        std::string("setProperty"), path);
                    return false;
                }
            }
        }

        std::string getProperty(const std::string& name) const
        {
            std::string path = _property_path + normalizeName(name);
            if (!isPropertyPathValid(path))
            {
                fep3::Result result(ERR_INVALID_ARG);
                FEP3_CONFIG_LOG_RESULT(result,
                    _participant_name,
                    _component_name,
                    std::string("getProperty"), path);
                return "";
            }
            else
            {
                std::string type = GetStub().getProperty(path)["type"].asString();
                if (type.empty())
                {
                    FEP3_CONFIG_LOG_RESULT(fep3::Result(ERR_PATH_NOT_FOUND),
                        _participant_name, 
                        _component_name,
                        std::string("getProperty"),
                        path);
                    return "";
                }
                else
                {
                    std::string value = GetStub().getProperty(path)["value"].asString();
                    return value;
                }
            }
        }

        std::string getPropertyType(const std::string& name) const
        {
            std::string path = _property_path + normalizeName(name);
            if (!isPropertyPathValid(path))
            {
                fep3::Result result(ERR_INVALID_ARG);
                FEP3_CONFIG_LOG_RESULT(result,
                    _participant_name,
                    _component_name,
                    std::string("getPropertyType"),
                    path);
                return "";
            }
            else
            {

                std::string type = GetStub().getProperty(path)["type"].asString();
                if (type.empty())
                {
                    FEP3_CONFIG_LOG_RESULT(fep3::Result(ERR_PATH_NOT_FOUND),
                        _participant_name,
                        _component_name,
                        std::string("getPropertyType"),
                        path);
                    return "";
                }
                else
                {
                    return type;
                }
            }
        }

        bool isEqual(const IProperties& properties) const
        {
            Properties<IProperties> mirrored_properties;
            auto prop_names = getPropertyNames();
            for (const auto& prop_name : prop_names)
            {
                std::string path = _property_path + prop_name;
                auto val = GetStub().getProperty(path);
                mirrored_properties.setProperty(prop_name, val["value"].asString(), val["type"].asString());
            }
            return mirrored_properties.isEqual(properties);
        }

        void copy_to(IProperties& properties) const
        {
            Properties<IProperties> mirrored_properties;
            auto prop_names = getPropertyNames();
            for (const auto& prop_name : prop_names)
            {
                std::string path = _property_path + prop_name;
                auto val = GetStub().getProperty(path);
                properties.setProperty(prop_name, val["value"].asString(), val["type"].asString());
            }
        }

        std::vector<std::string> getPropertyNames() const
        {
            auto props = GetStub().getProperties(_property_path);
            return a_util::strings::split(props, ",");
        }

    private:
        /**
        * @brief Check a property path which includes the property name for validity.
        * Currently only the '/' syntax is considered valid.
        * '.' syntax is not supported.
        *
        * @param property_path the propery path to be checked
        *
        * @return bool success boolean
        */
        bool isPropertyPathValid(const std::string& property_path) const
        {
            return std::regex_match(property_path, _property_path_regex);
        }

        /**
        * @brief Remove leading and concluding '/' characters a property name.
        *
        * @param property_name the propery name to be normalized
        *
        * @return string the normalized property name
        */
        std::string normalizeName(const std::string& property_name) const
        {
            std::string normalized_name = property_name;

            if (normalized_name.at(0) == '/')
            {
                normalized_name = normalized_name.substr(1, normalized_name.size());
            }
            if (normalized_name.at(normalized_name.size() - 1) == '/')
            {
                normalized_name = normalized_name.substr(0, normalized_name.size() - 1);
            }

            return normalized_name;
        }
    };

public:
    using RPCConfigProxy::GetStub;
    ConfigurationProxy(const std::string& participant_name,
        const std::string& rpc_component_name,
        const std::shared_ptr<rpc::IRPCRequester>& rpc,
        ISystemLogger& logger) : 
        RPCConfigProxy(rpc_component_name, rpc),
        _logger(logger),
        _participant_name(participant_name),
        _component_name(rpc_component_name),
        _rpc(rpc)
    {
    }

    std::string normalizePath(const std::string& property_path) const
    {
        if (property_path == "/" || property_path.empty())
        {
            return "/";
        }
        else
        {
            if (property_path.at(property_path.size() - 1) == '/')
            {
                return property_path;
            }
            else
            {
                std::string retval = property_path + "/";
                return retval;
            }
        }
    }

    /**
     * @brief Get a list of the signal names going in
     *
     * @return std::vector<std::string> signal list names
     */
    std::shared_ptr<IProperties> getProperties(const std::string& property_path) override
    {
        std::string normalized_path = normalizePath(property_path);

        if (GetStub().exists(normalized_path))
        {
            return std::make_shared<ConfigurationProperty>(
                _participant_name,
                _component_name,
                _rpc,
                normalized_path,
                _logger);
        }
        else
        {
            FEP3_CONFIG_LOG_AND_THROW_RESULT(fep3::Result(ERR_PATH_NOT_FOUND),
                _participant_name,
                _component_name,
                std::string("getProperties"),
                property_path);
        }
    }
    /**
     * @brief Get a list of the signal names going out
     *
     * @return std::vector<std::string> signal list names
     */
    std::shared_ptr<const IProperties> getProperties(const std::string& property_path) const override
    {
        std::string normalized_path = normalizePath(property_path);

    	if (GetStub().exists(normalized_path))
        {
            return std::make_shared<ConfigurationProperty>(
                _participant_name,
                _component_name,
                _rpc,
                normalized_path,
                _logger);
        }
        else
        {
            FEP3_CONFIG_LOG_AND_THROW_RESULT(fep3::Result(ERR_PATH_NOT_FOUND),
                _participant_name,
                _component_name,
                std::string("getProperties"),
                property_path);
        }
    }

private:
    ISystemLogger&                    _logger;
    std::string                       _participant_name;
    std::string                       _component_name;
    std::shared_ptr<rpc::IRPCRequester> _rpc;
};

}
}
}

