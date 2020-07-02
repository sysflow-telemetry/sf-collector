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

#include "sysdig/sdcont.h"

using sysdig::SDContainer;

SDContainer::SDContainer() {}

SDContainer::~SDContainer() {}

std::string SDContainer::getName() {
  return container ? container->m_name : "";
}

std::string SDContainer::getImage() {
  return container ? container->m_image : "";
}

std::string SDContainer::getImageTag() {
  return container ? container->m_imagetag : "";
}

std::string SDContainer::getID() {
  return container ? container->m_id : "";
}

std::string SDContainer::getImageID() {
  return container ? container->m_imageid : "";
}

sysflow::ContainerType SDContainer::getType() {
  return container ? static_cast<sysflow::ContainerType>(container->m_type)
                   : sysflow::ContainerType::CT_CUSTOM;
}

bool SDContainer::getPriv() {
  return container ? container->m_privileged : false;
}
