# Common compiler settings for g10 Code; based on KDECompilerSettings.cmake
#
# Copyright 2014 Alex Merry <alex.merry@kde.org>
# Copyright 2013 Stephen Kelly <steveire@gmail.com>
# Copyright 2012-2013 Raphael Kubo da Costa <rakuco@FreeBSD.org>
# Copyright 2007 Matthias Kretz <kretz@kde.org>
# Copyright 2006-2007 Laurent Montel <montel@kde.org>
# Copyright 2006-2013 Alex Neundorf <neundorf@kde.org>
# Copyright 2021 Friedrich W. H. Kossebau <kossebau@kde.org>
# Copyright 2025 g10 Code GmbH
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# SPDX-License-Identifier: BSD-3-Clause

# This macro is for adding definitions that affect the underlying
# platform API.  It makes sure that configure checks will also have
# the same defines, so that the checks match compiled code.
macro (_G10_ADD_PLATFORM_DEFINITIONS)
    add_definitions(${ARGV})
    set(CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS} ${ARGV})
endmacro()

include(CheckSymbolExists)
check_symbol_exists("__GLIBC__" "stdlib.h" LIBC_IS_GLIBC)
if (LIBC_IS_GLIBC)
    # Enable everything in GNU libc.  Any code using non-portable features
    # needs to perform feature tests, but this ensures that any such features
    # will be found if they exist.
    #
    # NB: we do NOT define _BSD_SOURCE, as with GNU libc that requires linking
    # against the -lbsd-compat library (it changes the behaviour of some
    # functions).  This, however, means that strlcat and strlcpy are not
    # provided by glibc.
    _g10_add_platform_definitions(-D_GNU_SOURCE)
endif ()

if (UNIX)
    # Enable extra API for using 64-bit file offsets on 32-bit systems.
    _g10_add_platform_definitions(-D_LARGEFILE64_SOURCE)

    include(CheckCXXSourceCompiles)

    # By default (in glibc, at least), on 32bit platforms off_t is 32 bits,
    # which causes a SIGXFSZ when trying to manipulate files larger than 2Gb
    # using libc calls (note that this issue does not occur when using QFile).
    check_cxx_source_compiles("
#include <sys/types.h>
 /* Check that off_t can represent 2**63 - 1 correctly.
    We can't simply define LARGE_OFF_T to be 9223372036854775807,
    since some C++ compilers masquerading as C compilers
    incorrectly reject 9223372036854775807.  */
#define LARGE_OFF_T (((off_t) 1 << 62) - 1 + ((off_t) 1 << 62))
  int off_t_is_large[(LARGE_OFF_T % 2147483629 == 721 && LARGE_OFF_T % 2147483647 == 1) ? 1 : -1];
  int main() { return 0; }
" _OFFT_IS_64BIT)

    if (NOT _OFFT_IS_64BIT)
        _g10_add_platform_definitions(-D_FILE_OFFSET_BITS=64)
    endif ()
endif()

if (WIN32)
    # Speeds up compile times by not including everything with windows.h
    # See http://msdn.microsoft.com/en-us/library/windows/desktop/aa383745%28v=vs.85%29.aspx
    _g10_add_platform_definitions(-DWIN32_LEAN_AND_MEAN)

    if (KDE_INTERNAL_COMPILERSETTINGS_LEVEL VERSION_GREATER_EQUAL 5.240.0 OR QT_MAJOR_VERSION STREQUAL "6")
        # Target Windows 10
        # This enables various bits of new API
        # See http://msdn.microsoft.com/en-us/library/windows/desktop/aa383745%28v=vs.85%29.aspx
        # Windows 10 is the default by Qt6 hence we do not need the next line, but we keep it disabled
        # to not to start from scratch in case we want to target a different version in the future
        # _g10_add_platform_definitions(-D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00 -D_WIN32_IE=0x0A00)
    else()
        # Target Windows Vista
        # This enables various bits of new API
        # See http://msdn.microsoft.com/en-us/library/windows/desktop/aa383745%28v=vs.85%29.aspx
        _g10_add_platform_definitions(-D_WIN32_WINNT=0x0600 -DWINVER=0x0600 -D_WIN32_IE=0x0600)
    endif()

    # Use the Unicode versions of Windows API by default
    # See http://msdn.microsoft.com/en-us/library/windows/desktop/dd317766%28v=vs.85%29.aspx
    _g10_add_platform_definitions(-DUNICODE -D_UNICODE)

    # As stated in http://msdn.microsoft.com/en-us/library/4hwaceh6.aspx M_PI only gets defined
    # if _USE_MATH_DEFINES is defined, with mingw this has a similar effect as -D_GNU_SOURCE on math.h
    _g10_add_platform_definitions(-D_USE_MATH_DEFINES)

    # Don't define MIN and MAX in windows.h
    # the defines break the use of std::max
    _g10_add_platform_definitions(-DNOMINMAX)
endif()



############################################################
# Language and toolchain features
############################################################

# Pick sensible versions of the C and C++ standards.
if (NOT CMAKE_C_STANDARD)
    # if (KDE_INTERNAL_COMPILERSETTINGS_LEVEL VERSION_GREATER_EQUAL 5.85.0)
    #     set(CMAKE_C_STANDARD 99)
    #     set(CMAKE_C_STANDARD_REQUIRED TRUE)
    #     set(CMAKE_C_EXTENSIONS OFF)
    # else()
        set(CMAKE_C_STANDARD 90)
    # endif()
endif()
if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
endif()

# Default to hidden visibility for symbols
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
#set(CMAKE_VISIBILITY_INLINES_HIDDEN 1) # wasn't enabled in autotools
if (POLICY CMP0063)
    # No sane project should be affected by CMP0063, so suppress the warnings
    # generated by the above visibility settings in CMake >= 3.3
    cmake_policy(SET CMP0063 NEW)
endif()



############################################################
# Better diagnostics (warnings, errors)
############################################################

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-shadow")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 5.0.0)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsuggest-override")
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 5.0.0)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wzero-as-null-pointer-constant")
    endif()
endif()
