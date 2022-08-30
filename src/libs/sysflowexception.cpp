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

#include "sysflowexception.h"
#include "scap.h"

sfexception::SysFlowException::SysFlowException(std::string message)
    : std::runtime_error(message) {
  setErrorCode(message);
}

void sfexception::SysFlowException::setErrorCode(std::string message) {
  m_code = LibsError;
  std::size_t found = message.find("Driver supports API version");
  if (found != std::string::npos) {
    m_code = DriverLibsMismatch;
    return;
  }

  found = message.find("missing api_version section");
  if (found != std::string::npos) {
    m_code = DriverLibsMismatch;
    return;
  }

  found = message.find("Make sure you have root credentials and that the falco "
                       "module is loaded.");
  if (found != std::string::npos) {
    m_code = ProbeAccessDenied;
    return;
  }
}

/*
#define SCAP_SUCCESS 0
#define SCAP_FAILURE 1
#define SCAP_TIMEOUT -1
#define SCAP_ILLEGAL_INPUT 3
#define SCAP_NOTFOUND 4
#define SCAP_INPUT_TOO_SMALL 5
#define SCAP_EOF 6
#define SCAP_UNEXPECTED_BLOCK 7
#define SCAP_VERSION_MISMATCH 8
#define SCAP_NOT_SUPPORTED 9
#define SCAP_FILTERED_EVENT 10
*/

sfexception::SysFlowError sfexception::getErrorCodeFromScap(int32_t ec) {
  SysFlowError err = LibsError;
  switch (ec) {
  case SCAP_SUCCESS:
  case SCAP_FAILURE:
  case SCAP_TIMEOUT:
  case SCAP_EOF:
    break;
  case SCAP_ILLEGAL_INPUT:
  case SCAP_INPUT_TOO_SMALL:
  case SCAP_UNEXPECTED_BLOCK:
    err = EventParsingError;
    break;
  case SCAP_NOTFOUND:
    err = ProcResourceNotFound;
    break;
  case SCAP_VERSION_MISMATCH:
    err = DriverLibsMismatch;
    break;
  case SCAP_NOT_SUPPORTED:
    err = OperationNotSupported;
    break;
  default:
    break;
  }
  return err;
}
