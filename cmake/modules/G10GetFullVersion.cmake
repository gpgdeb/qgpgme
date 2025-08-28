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
G10GetFullVersion
-----------------

Get version number including a beta version number for packaging, etc.

::

  g10_get_full_version(
      [TAG_PREFIX <prefix>]
      [VERSION <version>]
  )


This function adds a beta version number to the version string. The result
is stored in the CMake variable ``G10_FULL_VERSION``. The beta version number is
the number of commits since a tag matching one of the following patterns:
* ``<prefix>-<major>.<minor>.[0-9]*``
* ``<prefix>-<major>.[0-9]*-base``
* ``<prefix>-<major>-base``
i.e. the number of commits since the latest minor release with the same minor
version number or the number of commits since the latest base tag for a minor
version or the number of commits since the latest base tag for the major
version.
If the number of commits is 0 then ``G10_FULL_VERSION`` is set to ``<version>``,
i.e. the version number is used as-is. If the number of commits couldn't be
determined then ``G10_FULL_VERSION`` is set to ``<version>-unknown``.
Otherwise, ``G10_FULL_VERSION`` is set to ``<version>-beta<number of commits>``.

If ``TAG_PREFIX`` is given then the function will look for version tags with
this prefix. Otherwise, it will use ``PROJECT_NAME`` as prefix.

If ``VERSION`` is given then the function will use the given value as version
number. Otherwise, it will use ``PROJECT_VERSION``.
#]=======================================================================]

function(G10_GET_FULL_VERSION)
    set(options)
    set(one_value_keywords TAG_PREFIX VERSION)
    set(multi_value_keywords)

    cmake_parse_arguments(PARSE_ARGV 0 arg "${options}" "${one_value_keywords}" "${multi_value_keywords}")

    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown keywords given to G10_GET_FULL_VERSION(): \"${arg_UNPARSED_ARGUMENTS}\"")
    endif()

    if(arg_TAG_PREFIX)
        set(tag_prefix ${arg_TAG_PREFIX})
    else()
        set(tag_prefix ${PROJECT_NAME})
    endif()
    if(arg_VERSION)
        set(version ${arg_VERSION})
        string(REGEX REPLACE "^0*([0-9]+)\\.[0-9]+\\.[0-9]+.*" "\\1" major "${version}")
        string(REGEX REPLACE "^[0-9]+\\.0*([0-9]+)\\.[0-9]+.*" "\\1" minor "${version}")
        string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.0*([0-9]+).*" "\\1" micro "${version}")
    else()
        set(version ${PROJECT_VERSION})
        set(major ${PROJECT_VERSION_MAJOR})
        set(minor ${PROJECT_VERSION_MINOR})
        set(micro ${PROJECT_VERSION_MICRO})
    endif()

    find_package(Git QUIET)
    if (GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
        # determine the number of commits since the latest release or base tag
        set(match_strings "${tag_prefix}-${major}.${minor}.[0-9]*"
                          "${tag_prefix}-${major}.[0-9]*-base"
                          "${tag_prefix}-${major}-base")
        foreach(match_string IN LISTS match_strings)
            execute_process(COMMAND ${GIT_EXECUTABLE} describe --match "${match_string}" --long HEAD
                            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                            OUTPUT_VARIABLE output
                            ERROR_QUIET
            )
            # the command prints "<tag>-<number of commits>-g<abbreviated commit hash>" if a matching tag is found
            string(REGEX REPLACE ".*-([0-9]+)-g[0-9a-f]*[^0-9a-f]*$" "\\1" output "${output}")
            if(NOT output STREQUAL "")
                break()
            endif()
        endforeach()
    endif()
    if(NOT DEFINED output OR output STREQUAL "")
        set(version "${version}-unknown")
    elseif(output GREATER "0")
        set(version "${version}-beta${output}")
    endif()
    set(G10_FULL_VERSION "${version}" PARENT_SCOPE)
endfunction()
