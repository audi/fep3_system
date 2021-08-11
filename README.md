## ℹ️ This repository is archived 

It is now maintained at https://github.com/cariad-tech


---

<!---
   Copyright @ 2020 Audi AG. All rights reserved.

       This Source Code Form is subject to the terms of the Mozilla
       Public License, v. 2.0. If a copy of the MPL was not distributed
       with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

   If it is not possible or desirable to put the notice in a particular file, then
   You may include the notice in a location (such as a LICENSE file in a
   relevant directory) where a recipient would be likely to look for such a notice.

   You may add additional accurate notices of copyright ownership.
-->
FEP 3 SDK - System Library
============================

## Description ##

This installed package contains (or pulls in) the FEP SDK - System Library

The FEP System Library connects the FEP Service Bus to discover and configure and control a system.

## How to use ###

The FEP System Library provides a CMake >= 3.5 configuration. Here's how to use it from your own CMake projects:

    find_package(fep3_system REQUIRED)

You can append the FEP System Libray Target to your existing targets to add a dependency:

    target_link_libraries(my_existing_target PRIVATE fep3_system)
    #this helper macro will install ALL necessary library dependency to the targets directory
    fep3_system_install(my_existing_target)
    
    #this helper macro will deploy as post build process ALL necessary library dependency to the targets build directory
    fep3_system_deploy(my_existing_target)

Note that the fep_system target will transitively pull in all required include directories and libraries.

### Build Environment ####

The libraries are build and tested only under following compilers and operating systems: 
* Windows 10 x64 with Visual Studio C++ 2015 Update 3.1 (Update 3 and KB3165756)
* Windows 10 x64 with Visual Studio C++ 2017 but only supporting together with toolset v140
* Linux Ubuntu 16.04 LTS x64 with GCC 5.4 and libstdc++11 (C++11 ABI)

