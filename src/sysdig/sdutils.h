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

#ifndef _SD_UTILS_
#define _SD_UTILS_
#include "sysflow.h"
#include <fstream>
#include <sinsp.h>
#include <string>

namespace sysdig {
int64_t getFlags(sinsp_evt *ev);
bool isCloneThreadSet(sinsp_evt *ev);
int64_t getFD(sinsp_evt *ev);
bool isMapAnonymous(sinsp_evt *ev);
int64_t getIntParam(sinsp_evt *ev, string pname);
bool isInContainer(sinsp_evt *ev);
int64_t getSysCallResult(sinsp_evt *ev);
string getPath(sinsp_evt *ev, const string &paraName);
string getAbsolutePath(sinsp_threadinfo *ti, int64_t dirfd,
                       const string &fileName);
string getAbsolutePath(sinsp_threadinfo *ti, const string &fileName);
int64_t getFD(sinsp_evt *ev, const string &paraName);

} // namespacea sysdig
#endif
