# Copyright 2025 g10 Code GmbH
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
FindGpgme
---------------

Try to find the GpgME library.

This will define the following variables:

``Gpgme_FOUND``
    True if (the requested version of) GpgME was found
``Gpgme_VERSION``
    The version of the GpgME library which was found
``Gpgme_LIBRARIES``
    Libraries you need to link when using GpgME This can be passed to
    target_link_libraries() instead of the ``Gpgme::Gpgme`` target.
``Gpgme_INCLUDE_DIRS``
    Include directories needed to use GpgME This should be passed to
    target_include_directories() if the target is not used for linking.
``Gpgme_DEFINITIONS``
    Compile definitions to use when compiling code that uses GpgME
    This should be passed to target_compile_options() if the target is not
    used for linking.

If ``Gpgme_FOUND`` is TRUE, it will also define the following imported target:

``Gpgme::Gpgme``
    The GpgME library

#]=======================================================================]

find_package(PkgConfig QUIET)
pkg_check_modules(PC_Gpgme QUIET gpgme)

set(Gpgme_VERSION ${PC_Gpgme_VERSION})
set(Gpgme_DEFINITIONS ${PC_Gpgme_CFLAGS_OTHER})

find_path(Gpgme_INCLUDE_DIR
    NAMES
        gpgme.h
    HINTS
        ${PC_Gpgme_INCLUDE_DIRS}
)
find_library(Gpgme_LIBRARY
    NAMES
        gpgme
    HINTS
        ${PC_Gpgme_LIBRARY_DIRS}
)

if(Gpgme_INCLUDE_DIR AND NOT Gpgme_VERSION)
    # The version is given in the format MAJOR.MINOR.PATCH optionally followed
    # by an intermediate "beta" version given as -betaNUM, e.g. "1.24.2-beta7".
    file(STRINGS "${Gpgme_INCLUDE_DIR}/gpgme.h" Gpgme_VERSION_STR
         REGEX "^#[\t ]*define[\t ]+GPGME_VERSION[\t ]+\"([0-9])+\\.([0-9])+\\.([0-9])+(-[a-z0-9]*)?\".*")
    string(REGEX REPLACE "^.*GPGME_VERSION[\t ]+\"([0-9]+\\.[0-9]+\\.[0-9]+(-[a-z0-9]*)?)\".*$"
           "\\1" Gpgme_VERSION_STR "${Gpgme_VERSION_STR}")

    set(Gpgme_VERSION "${Gpgme_VERSION_STR}")

    unset(Gpgme_VERSION_STR)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Gpgme
    REQUIRED_VARS
        Gpgme_LIBRARY
        Gpgme_INCLUDE_DIR
        Gpgme_VERSION
    VERSION_VAR
        Gpgme_VERSION
)

if(Gpgme_FOUND AND NOT TARGET Gpgme::Gpgme)
    add_library(Gpgme::Gpgme UNKNOWN IMPORTED)
    set_target_properties(Gpgme::Gpgme PROPERTIES
        IMPORTED_LOCATION "${Gpgme_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${Gpgme_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${Gpgme_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(
    Gpgme_INCLUDE_DIR
    Gpgme_LIBRARY
)

if(Gpgme_FOUND)
    set(Gpgme_LIBRARIES ${Gpgme_LIBRARY})
    set(Gpgme_INCLUDE_DIRS ${Gpgme_INCLUDE_DIR})
endif()

include(FeatureSummary)
set_package_properties(Gpgme PROPERTIES
    DESCRIPTION "Interface library for GnuPG"
    URL https://www.gnupg.org/software/gpgme/
)
