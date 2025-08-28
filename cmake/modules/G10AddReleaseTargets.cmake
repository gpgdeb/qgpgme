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
G10AddReleaseTargets
--------------------

Add custom targets ``gen-swdb``, ``release``, and ``sign-release``.

::

  g10_add_release_targets(
      [VERSION <version>]
      [SWDB_PREFIX <prefix>]
      [ARCHIVE_FORMAT <format>]
      [RELEASE_ARCHIVE_SUFFIX archive_suffix]
  )


This function adds the custom targets ``gen-swdb``, ``release``, and
``sign-release`` to the project. These targets are used when releasing a new
version of a project.

The ``gen-swdb`` target generates the swdb entries for the release archive.
The entries are prefixed with ``prefix``. They are written to a file named
``<PROJECT_NAME>-<version>.swdb``.

The ``release`` target creates a tarball by running the distcheck target
in a subfolder ``dist`` of the build folder.

The ``sign-release`` target signs the tarball and uploads everything to the
location of the released tarball archives. This location is defined by the
variable ``RELEASE_ARCHIVE`` in ``~/.gnupg-autogen.rc`` which is suffixed by
``archive_suffix``.

If ``VERSION`` is not given then ``PROJECT_VERSION`` is used.

If ``PREFIX`` is not given then the lower-cased ``PROJECT_NAME`` is used.

If ``ARCHIVE_FORMAT`` is not given then ``tar.gz`` is used. It must match
the format specified for the dist targets.

If ``RELEASE_ARCHIVE_SUFFIX`` is not given then ``PROJECT_NAME`` is used.
#]=======================================================================]

function(G10_ADD_RELEASE_TARGETS)
    set(options)
    set(one_value_keywords VERSION SWDB_PREFIX ARCHIVE_FORMAT RELEASE_ARCHIVE_SUFFIX)
    set(multi_value_keywords)

    cmake_parse_arguments(PARSE_ARGV 0 arg "${options}" "${one_value_keywords}" "${multi_value_keywords}")

    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown keywords given to G10_ADD_RELEASE_TARGETS(): \"${arg_UNPARSED_ARGUMENTS}\"")
    endif()

    if(arg_VERSION)
        set(version ${arg_VERSION})
    else()
        set(version ${PROJECT_VERSION})
    endif()
    if(arg_SWDB_PREFIX)
        set(swdb_prefix ${arg_SWDB_PREFIX})
    else()
        string(TOLOWER ${PROJECT_NAME} swdb_prefix)
    endif()
    if(arg_ARCHIVE_FORMAT)
        set(archive_format ${arg_ARCHIVE_FORMAT})
    else()
        set(archive_format "tar.gz")
    endif()
    if(arg_RELEASE_ARCHIVE_SUFFIX)
        set(release_archive_suffix ${arg_RELEASE_ARCHIVE_SUFFIX})
    else()
        set(release_archive_suffix ${PROJECT_NAME})
    endif()

    set(release_name "${PROJECT_NAME}-${version}")

    if(NOT TARGET gen-swdb)
        configure_file("${CMAKE_SOURCE_DIR}/cmake/modules/g10_generate_swdb.sh.in" "g10_generate_swdb.sh" @ONLY)
        execute_process(
            COMMAND chmod +x g10_generate_swdb.sh
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
        add_custom_target(gen-swdb
            COMMENT "Generating SWDB entries as ${release_name}.swdb..."
            COMMAND "${CMAKE_BINARY_DIR}/g10_generate_swdb.sh"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
    endif()

    if(NOT TARGET release)
        configure_file("${CMAKE_SOURCE_DIR}/cmake/modules/g10_release.sh.in" "g10_release.sh" @ONLY)
        execute_process(
            COMMAND chmod +x g10_release.sh
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
        add_custom_target(release
            COMMAND "${CMAKE_BINARY_DIR}/g10_release.sh"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
    endif()

    if(NOT TARGET sign-release)
        configure_file("${CMAKE_SOURCE_DIR}/cmake/modules/g10_sign-release.sh.in" "g10_sign-release.sh" @ONLY)
        execute_process(
            COMMAND chmod +x g10_sign-release.sh
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
        add_custom_target(sign-release
            COMMAND "${CMAKE_BINARY_DIR}/g10_sign-release.sh"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
    endif()
endfunction()
