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
#ifndef SF_SD_CONT
#define SF_SD_CONT
#include "api/sfinspector.h"
#include "sysflow.h"
#include <sinsp.h>
#include <string>
namespace sysdig {
class SDContainer : public api::SysFlowContainer {
private:
  sinsp_container_info::ptr_t container;

public:
  SDContainer();
  virtual ~SDContainer();
  inline void init(sinsp_container_info::ptr_t cont) {
    container = cont;
  }
  std::string getName();
  std::string getImage();
  std::string getImageTag();
  std::string getID();
  std::string getImageID();
  sysflow::ContainerType getType();
  bool getPriv();
};
} // namespace sysdig
#endif
