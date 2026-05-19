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
G10AddDistTargets
-----------------

Add custom targets ``dist`` and ``distcheck`` for packaging the sources.

::

  g10_add_dist_targets(
      [VERSION <version>]
      [ARCHIVE_FORMAT <format>]
      [EXTRA_FILES <filename> [...]]
  )


This function adds the custom targets ``dist`` and ``distcheck`` to the project.
These targets are similar to the targets with the same names created by
automake.

The ``dist`` target can be used to create a tarball of the sources.
It uses ``git archive`` for creating the tarball so that the target can only be
used with git clones of the project and not with extracted tarballs. The created
package is named ``<PROJECT_NAME>-<version>.<format>``.

The ``distcheck`` target creates and extracts a tarball of the project and then
configures, builds, installs, uninstalls, installs with DESTDIR, and uninstalls
with DESTDIR.

If ``VERSION`` is not given then ``PROJECT_VERSION`` is used.

If ``ARCHIVE_FORMAT`` is not given then ``tar.gz`` is used. ``git archive``
needs to support the given format.

With ``EXTRA_FILES`` files that are not part of HEAD, e.g. a file generated
with cmake, can be added to the tarball.
#]=======================================================================]

function(G10_ADD_DIST_TARGETS)
    set(options)
    set(one_value_keywords VERSION ARCHIVE_FORMAT)
    set(multi_value_keywords EXTRA_FILES)

    cmake_parse_arguments(PARSE_ARGV 0 arg "${options}" "${one_value_keywords}" "${multi_value_keywords}")

    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown keywords given to G10_ADD_DIST_TARGETS(): \"${arg_UNPARSED_ARGUMENTS}\"")
    endif()

    if(arg_VERSION)
        set(version ${arg_VERSION})
    else()
        set(version ${PROJECT_VERSION})
    endif()
    if(arg_ARCHIVE_FORMAT)
        set(archive_format ${arg_ARCHIVE_FORMAT})
    else()
        set(archive_format "tar.gz")
    endif()
    if(arg_EXTRA_FILES)
        set(extra_arguments)
        foreach(extra_file IN LISTS arg_EXTRA_FILES)
            set(extra_arguments "${extra_arguments} --add-file \"${extra_file}\"")
        endforeach()
    endif()

    if (NOT TARGET dist)
        set(g10_dist_archive_name "${PROJECT_NAME}-${version}")
        set(g10_dist_archive_format "${archive_format}")
        set(g10_dist_git_archive_extra_arguments "${extra_arguments}")
        configure_file("${CMAKE_SOURCE_DIR}/cmake/modules/g10_dist.sh.in" "g10_dist.sh" @ONLY)
        execute_process(
            COMMAND chmod +x g10_dist.sh
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
        add_custom_target(dist
            COMMENT "Packaging HEAD of sources as ${g10_dist_archive_name}.${g10_dist_archive_format}..."
            COMMAND "${CMAKE_BINARY_DIR}/g10_dist.sh"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        )
    endif()

    if (NOT TARGET distcheck)
        set(g10_distcheck_archive_name "${PROJECT_NAME}-${version}")
        set(g10_distcheck_archive_format "${archive_format}")
        configure_file("${CMAKE_SOURCE_DIR}/cmake/modules/g10_distcheck.sh.in" "g10_distcheck.sh" @ONLY)
        execute_process(
            COMMAND chmod +x g10_distcheck.sh
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
        add_custom_target(distcheck
            COMMAND "${CMAKE_BINARY_DIR}/g10_distcheck.sh"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
    endif()

    add_dependencies(distcheck dist)
endfunction()
