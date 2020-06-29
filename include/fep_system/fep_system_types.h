/**
 * Include system type header for the FEP system library.
 *
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
 
#ifndef _FEP_SYSTEM_TYPES_H_INCLUDED_
#define _FEP_SYSTEM_TYPES_H_INCLUDED_

#include <cstdint>
#include "fep_system_export.h"

/**
 * FEP 3 SDK Library namespace
 */
namespace fep3
{
/**
 * FEP 3 SDK version namespace for all interfaces and public implementations introduced in version 3.0.
 */
namespace arya
{
/**
 * FEP 3 SDK namespace for helper and implementation utilities. Do not directly use them in your code! 
 */
namespace detail
{
} // namespace detail
} // namespace arya


/**
 * FEP 3 SDK Libraries namespace for base functionality and helper classes can be used for convenience
 */
namespace base
{
/**
 * FEP 3 SDK Libraries version namespace for base functionality or helper classes introduced in version 3.0.
 */
namespace arya
{
} // namespace arya
} // namespace cpp

/**
 * FEP 3 SDK Libraries namespace for rpc functionality (remote procedure call)
 */
namespace rpc
{
/**
 * FEP 3 SDK Libraries version namespace for rpc functionality (remote procedure call) version 3.0.
 */
namespace arya
{
} // namespace arya
} // namespace rpc

} // namespace fep3

#endif // _FEP_SYSTEM_TYPES_H_INCLUDED_
