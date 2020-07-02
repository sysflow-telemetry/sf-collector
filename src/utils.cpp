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

#include "utils.h"
#include "datatypes.h"
#include "logger.h"
#include "sysflowcontext.h"

static NFKey s_nfdelkey;
static NFKey s_nfemptykey;
static bool s_keysinit = false;
static OID s_oiddelkey;
static OID s_oidemptykey;

CREATE_LOGGER_2("sysflow.utils");

void initKeys() {
  s_nfdelkey.ip1 = 1;
  s_nfdelkey.ip2 = 1;
  s_nfdelkey.port1 = 1;
  s_nfdelkey.port2 = 1;
  s_nfemptykey.ip1 = 1;
  s_nfemptykey.ip2 = 0;
  s_nfemptykey.port1 = 1;
  s_nfemptykey.port2 = 1;
  s_oidemptykey.hpid = 2;
  s_oidemptykey.createTS = 2;
  s_oiddelkey.hpid = 1;
  s_oiddelkey.createTS = 1;
  s_keysinit = true;
}

void utils::generateFOID(const string &key, FOID *foid) {
  SHA1(reinterpret_cast<const unsigned char *>(key.c_str()), key.size(),
       foid->begin());
}

NFKey *utils::getNFEmptyKey() {
  if (!s_keysinit) {
    initKeys();
  }
  return &s_nfemptykey;
}

NFKey *utils::getNFDelKey() {
  if (!s_keysinit) {
    initKeys();
  }
  return &s_nfdelkey;
}

OID *utils::getOIDEmptyKey() {
  if (!s_keysinit) {
    initKeys();
  }
  return &s_oidemptykey;
}

OID *utils::getOIDDelKey() {
  if (!s_keysinit) {
    initKeys();
  }
  return &s_oiddelkey;
}

time_t utils::getExportTime(context::SysFlowContext *cxt) {
  time_t now = utils::getCurrentTime(cxt);
  struct tm exportTM = *localtime(&now);
  exportTM.tm_sec += cxt->getNFExportInterval(); // add 30 seconds to the time
  return mktime(&exportTM);                      // normalize iti
}

avro::ValidSchema utils::loadSchema(const char *filename) {
  avro::ValidSchema result;
  try {
    std::ifstream ifs(filename);
    avro::compileJsonSchema(ifs, result);
  } catch (avro::Exception &ex) {
    SF_ERROR(m_logger, "Unable to load schema file from "
                           << filename << " Error: " << ex.what());
    throw;
  }
  return result;
}

fs::path utils::getCanonicalPath(const string &fileName) {
  fs::path p(fileName);
  try {
    p = fs::weakly_canonical(p);
  } catch (...) {
    SF_WARN(m_logger, "Unable to compute canonical path from " << fileName);
  }
  return p;
}

