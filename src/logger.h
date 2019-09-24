#ifndef __SF_LOGGER
#define __SF_LOGGER
/** Copyright (C) 2019 IBM Corporation.
*
* Authors:
* Teryl Taylor <terylt@ibm.com>
* Frederico Araujo <frederico.araujo@ibm.com>
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


#include <iostream>

using namespace std;
#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/propertyconfigurator.h"
#include "log4cxx/helpers/exception.h"
 
using namespace log4cxx;
using namespace log4cxx::helpers;

#define CREATE_LOGGER(ClassName, logger) LoggerPtr ClassName::m_logger(Logger::getLogger(logger));
#define CREATE_LOGGER_2(logger) LoggerPtr m_logger(Logger::getLogger(logger));
#define CREATE_MAIN_LOGGER() static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("sysflow.main"));
#define DEFINE_LOGGER() static log4cxx::LoggerPtr m_logger;

#define CONFIGURE_LOGGER(logConfig) do { \
        if (!logConfig.empty()) { \
            PropertyConfigurator::configure(logConfig.c_str()); \
        }  \
        else  \
        {     \
            BasicConfigurator::configure(); \
        } } while(0)

#define CATCH_LOGGER_EXCEPTION() \
        catch(Exception& ex)  \
        {                     \
            cerr << ex.what() << endl; \
            return 1;     \
        }
//static char* m_logger;
//#define m_logger 1
/*#define SF_DEBUG(logger, message) do { \
      cout << "DEBUG: " << message << endl; }while(0)
#define SF_WARN(logger, message) do { \
      cout << "WARN: " << message << endl; }while(0)
#define SF_INFO(logger, message) do { \
      cout << "INFO: " << message << endl; }while(0)
#define SF_ERROR(logger, message) do { \
      cout << "ERROR: " << message << endl; }while(0)*/

/*
#define SF_DEBUG(logger, message) 0
#define SF_WARN(logger, message)  0
#define SF_INFO(logger, message)  0
#define SF_ERROR(logger, message) 0
*/

#define SF_DEBUG(logger, message) LOG4CXX_DEBUG(logger, message)
#define SF_WARN(logger, message)  LOG4CXX_WARN(logger, message)
#define SF_INFO(logger, message)  LOG4CXX_INFO(logger, message)
#define SF_ERROR(logger, message) LOG4CXX_ERROR(logger, message)

#define IS_DEBUG_ENABLED(logger) logger->isDebugEnabled()
#endif
