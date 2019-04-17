#ifndef __SF_FILE_TYPES_
#define __SF_FILE_TYPES_
/*enum class FileTypes : char {
     SF_FILE = 'f',
     SF_IPv4 = 4,
     SF_IPv6 = 6,
     SF_UNIX = 'u',
     SF_PIPE = 'p'
};*/
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
