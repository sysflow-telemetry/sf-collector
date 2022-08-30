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

#ifndef __SYSFLOW_EXCEPTION__
#define __SYSFLOW_EXCEPTION__

#include <stdexcept>
#include <string>

namespace sfexception {

enum SysFlowError {
  LibsError,
  ProbeAccessDenied,
  ProbeNotExist,
  ErrorReadingFileSystem,
  NameTooLong,
  ProbeCheckError,
  ProbeNotLoaded,
  DriverLibsMismatch,
  EventParsingError,
  ProcResourceNotFound,
  OperationNotSupported
};

class SysFlowException : public std::runtime_error {
private:
  SysFlowError m_code;
  void setErrorCode(std::string message);

public:
  SysFlowException(std::string message);
  SysFlowException(std::string message, SysFlowError code)
      : std::runtime_error(message), m_code(code) {}
  SysFlowError getErrorCode() { return m_code; }
};

SysFlowError getErrorCodeFromScap(int32_t ec);

} // namespace sfexception

#endif