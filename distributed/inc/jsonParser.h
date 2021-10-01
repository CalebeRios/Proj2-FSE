#ifndef _JSON_PARSER_H_
#define _JSON_PARSER_H_

#include "global.h"

char* read_file(char* directory);
Configuration parse_json(char* configuration);

#endif
