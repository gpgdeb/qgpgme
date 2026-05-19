# Copyright 2020-2023 Alexander Lohnau <alexander.lohnau@gmx.de>
# Copyright 2022 Ahmad Samir <a.samirh78@gmail.com>
# Copyright 2023 Johannes Zarl-Zierl <johannes@zarl-zierl.at>
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

#[=======================================================================[.rst:
G10GitHooks
-----------

Configure the usual Git hooks for g10 Code projects.

::

  g10_configure_git_hooks()

This function will enable the pre-commit.sample hook (e.g. to disallow trailing
whitespace) and the commit-msg hook in build-aux.
If a custom hooks directory is set via ``core.hooksPath``, a warning is issued.

Example usage:

.. code-block:: cmake

  include(G10GitHooks)
  g10_configure_git_hooks()

Since 5.79
#]=======================================================================]

function(G10_CONFIGURE_GIT_HOOKS)
    set(_oneValueArgs "")
    set(_multiValueArgs CHECKS CUSTOM_SCRIPTS)
    cmake_parse_arguments(ARG "" "${_oneValueArgs}" "${_multiValueArgs}" ${ARGN})

    if(NOT UNIX)
        return()
    endif()

    if(NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
        message(STATUS "Project is not top level project - pre-commit hook not installed")
        return()
    endif()

    set(git_dir "${CMAKE_SOURCE_DIR}/.git")
    if (NOT IS_DIRECTORY ${git_dir})
        # ignore secondary git work trees and tarballs
        return()
    endif()

    find_package(Git QUIET)
    if (NOT GIT_FOUND)
        return()
    endif()

    set(git_hooks_dir "${git_dir}/hooks")
    execute_process(COMMAND "${GIT_EXECUTABLE}" config --get core.hooksPath
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        RESULT_VARIABLE _gitresult
        OUTPUT_VARIABLE _gitoutput
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(_gitresult EQUAL 0 AND NOT ${git_hooks_dir} EQUAL "${_gitoutput}")
        message(WARNING "Git is configured to use '${_gitoutput}' for hooks. The generated commit hooks will likely not be executed.")
    endif()

    if(EXISTS "${git_hooks_dir}/pre-commit.sample" AND NOT EXISTS "${git_hooks_dir}/pre-commit")
        message(STATUS
            "*** Activating trailing whitespace git pre-commit hook. ***\n"
            "    To deactivate this pre-commit hook again move .git/hooks/pre-commit\n"
            "    and .git/hooks/pre-commit.sample out of the way.")
        execute_process(COMMAND cp -p "${git_hooks_dir}/pre-commit.sample" "${git_hooks_dir}/pre-commit"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
        execute_process(COMMAND chmod +x "${git_hooks_dir}/pre-commit")
    endif()
    if(EXISTS "${CMAKE_SOURCE_DIR}/build-aux/git-hooks/commit-msg" AND NOT EXISTS "${git_hooks_dir}/commit-msg")
        message(STATUS "*** Activating commit log message check hook. ***")
        file(COPY "${CMAKE_SOURCE_DIR}/build-aux/git-hooks/commit-msg"
            DESTINATION "${git_hooks_dir}")
        execute_process(COMMAND chmod +x "${git_hooks_dir}/commit-msg")
    endif()
endfunction()
