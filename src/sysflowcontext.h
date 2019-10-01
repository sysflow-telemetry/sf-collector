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

#ifndef _SF_CONTEXT_
#define _SF_CONTEXT_
#include <string>

#include <ctime>

#include "logger.h"
#include <cerrno>
#include <cstring>
#include <sinsp.h>
#include <unistd.h>

using namespace std;
class SysFlowContext {
private:
  bool m_filterCont;
  time_t m_start{};
  int m_fileDuration;
  bool m_hasPrefix;
  string m_outputFile;
  string m_scapFile;
  string m_schemaFile;
  string m_exporterID;
  sinsp *m_inspector;
  int m_nfExportInterval;
  int m_nfExpireInterval;
  bool m_offline;
  string m_filter;
  DEFINE_LOGGER();

public:
  SysFlowContext(bool fCont, int fDur, string oFile, const string &sFile,
                 string schFile, string exporterID, string filter);
  virtual ~SysFlowContext();
  uint64_t timeStamp{};
  string getExporterID();
  inline bool isOffline() { return m_offline; }
  inline sinsp *getInspector() { return m_inspector; }
  inline int getNFExportInterval() { return m_nfExportInterval; }
  inline int getNFExpireInterval() { return m_nfExpireInterval; }
  inline string getOutputFile() { return m_outputFile; }
  inline string getScapFile() { return m_scapFile; }
  inline const char *getSchemaFile() { return m_schemaFile.c_str(); }
  inline bool hasPrefix() { return m_hasPrefix; }
  inline int getFileDuration() { return m_fileDuration; }
  inline bool isFilterContainers() { return m_filterCont; }
};

#endif
