# Copyright 2022 g10 Code GmbH
# Software engineering by Ingo Klöcker <dev@ingo-kloecker.de>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR
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
# SPDX-License-Identifier: BSD-2-Clause

#[=======================================================================[.rst:
FindLibGpgError
---------------

Try to find the Libgpg-error library.

This will define the following variables:

``LibGpgError_FOUND``
    True if (the requested version of) Libgpg-error was found
``LibGpgError_VERSION``
    The version of the Libgpg-error library which was found
``LibGpgError_LIBRARIES``
    Libraries you need to link when using Libgpg-error This can be passed to
    target_link_libraries() instead of the ``LibGpgError::LibGpgError`` target.
``LibGpgError_INCLUDE_DIRS``
    Include directories needed to use Libgpg-error This should be passed to
    target_include_directories() if the target is not used for linking.
``LibGpgError_DEFINITIONS``
    Compile definitions to use when compiling code that uses Libgpg-error
    This should be passed to target_compile_options() if the target is not
    used for linking.

If ``LibGpgError_FOUND`` is TRUE, it will also define the following imported target:

``LibGpgError::LibGpgError``
    The Libgpg-error library

#]=======================================================================]

find_package(PkgConfig QUIET)
pkg_check_modules(PC_LibGpgError QUIET gpg-error)

set(LibGpgError_VERSION ${PC_LibGpgError_VERSION})
set(LibGpgError_DEFINITIONS ${PC_LibGpgError_CFLAGS_OTHER})

find_path(LibGpgError_INCLUDE_DIR
    NAMES
        gpg-error.h
    HINTS
        ${PC_LibGpgError_INCLUDE_DIRS}
)
find_library(LibGpgError_LIBRARY
    NAMES
        gpg-error
    HINTS
        ${PC_LibGpgError_LIBRARY_DIRS}
)

if(LibGpgError_INCLUDE_DIR AND NOT LibGpgError_VERSION)
    # The version is given in the format MAJOR.MINOR optionally followed
    # by an intermediate "beta" version given as -betaNUM, e.g. "1.47-beta7".
    file(STRINGS "${LibGpgError_INCLUDE_DIR}/gpg-error.h" LibGpgError_VERSION_STR
         REGEX "^#[\t ]*define[\t ]+GPG_ERROR_VERSION[\t ]+\"([0-9])+\\.([0-9])+(-[a-z0-9]*)?\".*")
    string(REGEX REPLACE "^.*GPG_ERROR_VERSION[\t ]+\"([0-9]+\\.[0-9]+(-[a-z0-9]*)?)\".*$"
           "\\1" LibGpgError_VERSION_STR "${LibGpgError_VERSION_STR}")

    set(LibGpgError_VERSION "${LibGpgError_VERSION_STR}")

    unset(LibGpgError_VERSION_STR)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibGpgError
    REQUIRED_VARS
        LibGpgError_LIBRARY
        LibGpgError_INCLUDE_DIR
        LibGpgError_VERSION
    VERSION_VAR
        LibGpgError_VERSION
)

if(LibGpgError_FOUND AND NOT TARGET LibGpgError::LibGpgError)
    add_library(LibGpgError::LibGpgError UNKNOWN IMPORTED)
    set_target_properties(LibGpgError::LibGpgError PROPERTIES
        IMPORTED_LOCATION "${LibGpgError_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${LibGpgError_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${LibGpgError_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(
    LibGpgError_INCLUDE_DIR
    LibGpgError_LIBRARY
)

if(LibGpgError_FOUND)
    set(LibGpgError_LIBRARIES ${LibGpgError_LIBRARY})
    set(LibGpgError_INCLUDE_DIRS ${LibGpgError_INCLUDE_DIR})
endif()

include(FeatureSummary)
set_package_properties(LibGpgError PROPERTIES
    DESCRIPTION "Runtime library for all GnuPG components"
    URL https://www.gnupg.org/software/libgpg-error
)
