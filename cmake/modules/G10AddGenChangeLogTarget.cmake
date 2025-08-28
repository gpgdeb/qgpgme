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
G10AddGenChangeLogTarget
------------------------

Add custom target ``gen-ChangeLog`` for generating a ChangeLog file.

::

  g10_add_gen_changelog_target(
..       [SINCE <date>]
..       [FOOTER <file>]
  )

# This function adds a custom target named ``gen-ChangeLog`` to the project
# which can be used to generate a ChangeLog file from the commit log messages.
# ``gitlog-to-changelog`` is used for generating the ChangeLog. It can be
# found in the gnupg repository.
#
# If ``SINCE`` is given then its value is passed to ``git log`` to start the
# ChangeLog at this date.
#
# If ``FOOTER`` is given then the content of the given file is appended to
# the generated ChangeLog.
#]=======================================================================]

# save the location of the header template while CMAKE_CURRENT_LIST_DIR
# has the value we want
set(_G10_GENERATE_CHANGELOG_TEMPLATE "${CMAKE_CURRENT_LIST_DIR}/g10_generate_ChangeLog.cmake.in")

function(G10_ADD_GEN_CHANGELOG_TARGET)
    if(TARGET gen-ChangeLog)
        # gen-ChangeLog target is already defined
        return()
    endif()

    set(options)
    set(one_value_keywords SINCE FOOTER)
    set(multi_value_keywords)

    cmake_parse_arguments(PARSE_ARGV 0 arg "${options}" "${one_value_keywords}" "${multi_value_keywords}")

    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown keywords given to G10_ADD_GEN_CHANGELOG_TARGET(): \"${arg_UNPARSED_ARGUMENTS}\"")
    endif()

    if(arg_SINCE)
        set(changelog_since ${arg_SINCE})
    endif()
    if(arg_FOOTER)
        set(changelog_footer ${arg_FOOTER})
    endif()

    configure_file(
        "${_G10_GENERATE_CHANGELOG_TEMPLATE}"
        "${CMAKE_BINARY_DIR}/g10_generate_ChangeLog.cmake"
        IMMEDIATE
        @ONLY
    )

    add_custom_target(gen-ChangeLog
        COMMENT "Generating ChangeLog..."
        COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/g10_generate_ChangeLog.cmake"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
endfunction()
