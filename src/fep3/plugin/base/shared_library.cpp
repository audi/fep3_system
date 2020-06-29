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

#include <a_util/filesystem.h>
#include <a_util/strings.h>

#include "shared_library.h"

namespace fep3
{
namespace plugin
{
namespace arya
{

SharedLibrary::SharedLibrary(const std::string& file_path, bool prevent_unloading)
    : _file_path(file_path)
    , _prevent_unloading(prevent_unloading)
{
    std::string trimmed_file_path = file_path;
    a_util::strings::trim(trimmed_file_path);
    a_util::filesystem::Path full_file_path{trimmed_file_path};
    auto file_name = full_file_path.getLastElement().toString();
    full_file_path.removeLastElement();

    // add prefix
#ifndef WIN32
    if(0 != file_name.find("lib"))
    {
        file_name = "lib" + file_name;
    }
#endif
    full_file_path.append(file_name);
    if (full_file_path.getExtension().empty())
    {
        // add extension
#ifdef WIN32
        full_file_path.replaceExtension("dll");
#else
        full_file_path.replaceExtension("so");
#endif
    }

    const auto& full_file_path_string = full_file_path.toString();
#ifdef WIN32
    _library_handle = ::LoadLibrary(full_file_path_string.c_str());
    if (!_library_handle)
    {
        throw std::runtime_error("unable to load shared library '" + file_path + "'");
    }
#else
    _library_handle = ::dlopen(full_file_path_string.c_str(), RTLD_LAZY);
    if (!_library_handle)
    {
        throw std::runtime_error("unable to load shared library '" + file_path + "': " + dlerror());
    }
#endif
}

SharedLibrary::~SharedLibrary()
{
    if(!_prevent_unloading)
    {
        if(nullptr != _library_handle)
        {
#ifdef WIN32
            ::FreeLibrary(_library_handle);
#else
            ::dlclose(_library_handle);
#endif
        }
    }
}

SharedLibrary::SharedLibrary(SharedLibrary&& rhs)
    : _library_handle(std::move(rhs._library_handle))
    , _file_path(std::move(rhs._file_path))
    , _prevent_unloading(std::move(rhs._prevent_unloading))
{
    rhs._library_handle = nullptr;
}

std::string SharedLibrary::getFilePath() const
{
    return _file_path;
}

} // namespace arya
} // namespace plugin
} // namespace fep3
