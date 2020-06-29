/**
 * Export header for the FEP SDK.
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
 
#ifndef _FEP3_SYSTEM_EXPORT_H_INCLUDED_
#define _FEP3_SYSTEM_EXPORT_H_INCLUDED_

#ifdef WIN32
    #ifdef FEP3_SYSTEM_LIB_DO_EXPORT
        /// Macro switching between export / import of the fep sdk shared object
        #define FEP3_SYSTEM_EXPORT __declspec( dllexport )
    #else   // FEP_SYSTEM_LIB_DO_EXPORT
        /// Macro switching between export / import of the fep sdk shared object
        #define FEP3_SYSTEM_EXPORT __declspec( dllimport )
    #endif
#else   // WIN32
    #ifdef FEP3_SYSTEM_LIB_DO_EXPORT
        /// Macro switching between export / import of the fep sdk shared object
        #define FEP3_SYSTEM_EXPORT __attribute__ ((visibility("default")))
    #else   // FEP_SYSTEM_LIB_DO_EXPORT
        /// Macro switching between export / import of the fep sdk shared object
        #define FEP3_SYSTEM_EXPORT
    #endif
#endif
#endif // _FEP3_SYSTEM_EXPORT_H_INCLUDED_
