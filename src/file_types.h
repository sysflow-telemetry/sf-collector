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

#ifndef __SF_FILE_TYPES_
#define __SF_FILE_TYPES_
enum FileType {
  SF_FILE = 'f',
  SF_DIR = 'd',
  SF_IPv4 = '4',
  SF_IPv6 = '6',
  SF_UNIX = 'u',
  SF_PIPE = 'p',
  SF_UNK = '?'
};

#endif
