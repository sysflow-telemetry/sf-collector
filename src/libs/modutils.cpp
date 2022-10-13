/** Copyright (C) 2022 IBM Corporation.
 *
 * Authors:
 * Frederico Araujo <frederico.araujo@ibm.com>
 * Teryl Taylor <terylt@ibm.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/
#include "modutils.h"

#include "logger.h"
#include "sysflowexception.h"
#include <limits.h>
#include <stdlib.h>
#define FALCO_STR "falco "
#define FALCO_STATE "Live"

CREATE_LOGGER_2("sysflow.modutils");

void modutils::checkProbeExistsPermits(std::string &path) {
  char *abspath = realpath(path.c_str(), NULL);
  if (abspath == NULL) {
    if (errno == EACCES) {
      throw sfexception::SysFlowException(
          std::string("A portion of probe path '") + path +
              std::string("' is permission denied."),
          sfexception::ProbeAccessDenied);
    } else if (errno == EIO) {
      throw sfexception::SysFlowException(
          std::string("Error reading probe file system path '") + path +
              std::string("'."),
          sfexception::ErrorReadingFileSystem);
    } else if (errno == ENOENT) {
      throw sfexception::SysFlowException(
          std::string("Probe does not appear to exist '") + path +
              std::string("'."),
          sfexception::ProbeNotExist);
    } else if (errno == ENOTDIR) {
      throw sfexception::SysFlowException(
          std::string("A component of the probe prefix is not a directory'") +
              path + std::string("'."),
          sfexception::ProbeCheckError);
    }
  } else {
    free(abspath);
  }
}

void modutils::checkForFalcoKernMod() {
  FILE *fd;
  size_t len;
  ssize_t bytes;
  char *line = NULL;
  char *modname = NULL;
  char *state = NULL;
  bool found = false;
  if ((fd = fopen("/proc/modules", "r")) != NULL) {
    while ((bytes = getline(&line, &len, fd)) != -1) {
      modname = strstr(line, FALCO_STR);
      if (modname && modname == line) {
        SF_INFO(m_logger, "Found Kernel Module: " << line)
        state = strstr(line, FALCO_STATE);
        if (!state) {
          SF_WARN(m_logger, "Falco module is not live.")
        }
        found = true;
        break;
      }
    }
    fclose(fd);
    if (!found) {
      throw sfexception::SysFlowException(std::string("Kernel module ") +
                                              std::string(FALCO_STR) +
                                              std::string("is not loaded."),
                                          sfexception::ProbeNotLoaded);
    }
  }
  if (line) {
    free(line);
  }
}
