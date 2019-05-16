#ifndef __SF_LOGGER
#define __SF_LOGGER
#include <iostream>

using namespace std;
/*#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/propertyconfigurator.h"
#include "log4cxx/helpers/exception.h"
 
using namespace log4cxx;
using namespace log4cxx::helpers;*/

//static char* m_logger;
#define m_logger 1
/*#define SF_DEBUG(logger, message) do { \
      cout << "DEBUG: " << message << endl; }while(0)
#define SF_WARN(logger, message) do { \
      cout << "WARN: " << message << endl; }while(0)
#define SF_INFO(logger, message) do { \
      cout << "INFO: " << message << endl; }while(0)
#define SF_ERROR(logger, message) do { \
      cout << "ERROR: " << message << endl; }while(0)*/
#define SF_DEBUG(logger, message) 0
#define SF_WARN(logger, message)  0
#define SF_INFO(logger, message)  0
#define SF_ERROR(logger, message) 0
#endif
