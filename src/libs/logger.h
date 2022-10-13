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

#ifndef __SF_LOGGER
#define __SF_LOGGER
#include <glog/logging.h>

#include <iostream>

#define CREATE_LOGGER(ClassName, logger)
#define CREATE_LOGGER_2(logger)
#define CREATE_MAIN_LOGGER()
#define DEFINE_LOGGER()
#define m_logger

#define CONFIGURE_LOGGER(logConfig) google::InitGoogleLogging(logConfig);
#define SHUTDOWN_LOGGER() google::ShutdownGoogleLogging();
#define CATCH_LOGGER_EXCEPTION()

#define SF_TRACE(logger, message) VLOG(2) << message;
#define SF_DEBUG(logger, message) VLOG(1) << message;
#define SF_INFO(logger, message) LOG(INFO) << message;
#define SF_WARN(logger, message) LOG(WARNING) << message;
#define SF_ERROR(logger, message) LOG(ERROR) << message;

#endif
