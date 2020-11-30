/** Copyright (C) 2019 IBM Corporation.
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

#ifndef READ_ONLY_
#define READ_ONLY_
#define FILE_READS_ENABLED 0
#define FILE_READS_DISABLED 1
#define FILE_READS_SELECT 2
#include "op_flags.h"
#define NUM_PREFIXES 8

static std::string s_paths[NUM_PREFIXES] = {
    "/proc/", "/dev/",   "/sys/",     "//sys/",
    "/lib/",  "/lib64/", "/usr/lib/", "/usr/lib64/"};

inline bool prefix_match(std::string path, std::string match) {
  if (path.length() < match.length()) {
    return false;
  }

  for (size_t i = 0; i < match.length(); i++) {
    if (path[i] != match[i]) {
      return false;
    }
  }
  return true;
}

#define SHOULD_WRITE(ff)                                                       \
  int readMode = m_cxt->getFileRead();                                         \
  bool match = false;                                                          \
  if ((readMode == FILE_READS_DISABLED || readMode == FILE_READS_SELECT) &&    \
      ((ff->fileflow.openFlags & PPM_O_RDONLY) == PPM_O_RDONLY ||              \
       ((ff->fileflow.opFlags & OP_READ_RECV) == OP_READ_RECV &&               \
        (ff->fileflow.opFlags & OP_WRITE_SEND) != OP_WRITE_SEND &&             \
        (ff->fileflow.opFlags & OP_MMAP) != OP_MMAP))) {                       \
    if (readMode != FILE_READS_DISABLED) {                                     \
      for (int i = 0; i < NUM_PREFIXES; i++) {                                 \
        if (prefix_match(ff->flowkey, s_paths[i])) {                           \
          match = true;                                                        \
          break;                                                               \
        }                                                                      \
      }                                                                        \
    } else {                                                                   \
      match = true;                                                            \
    }                                                                          \
  }                                                                            \
  if (!match) {                                                                \
    m_writer->writeFileFlow(&(ff->fileflow));                                  \
  }
#endif
