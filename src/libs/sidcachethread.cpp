/** Copyright (C) 2023 IBM Corporation.
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

#include "sidcachethread.h"
#include "stdio.h"

sid::SIDCacheThread::SIDCacheThread() : m_conf(SC_CONFIG_INITIALIZER) {}

sid::SIDCacheThread::~SIDCacheThread() {}

void sid::SIDCacheThread::run() {
  printf("Starting sc runloop...\n");
  sc_set_config(&m_conf);
  sc_runloop();
}

void sid::SIDCacheThread::stop() {
  printf("Stopping sc runloop..\n");
  sc_close();
  sleep(m_conf.poll_secs);
  printf("sc runloop stopped..\n");
}