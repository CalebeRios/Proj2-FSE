#include <stdio.h>
#include <stdlib.h>

#include "../inc/cJson.h"
#include "../inc/jsonParser.h"

long file_size;

int transformToWiringPi(int value);

char* read_file(char* directory) {
    char* buffer;
    FILE* file = fopen(directory, "r");

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer = malloc(file_size + 1);
    fread(buffer, 1, file_size, file);
    fclose(file);

    buffer[file_size] = 0;

    return buffer;
}

char* read_string(cJSON* json, char* value) {
    cJSON* container = NULL;

    container = cJSON_GetObjectItemCaseSensitive(json, value);
    if (cJSON_IsString(container))
    {
        return container->valuestring;
    }

    return "";
}

int read_number(cJSON* json, char* value) {
    cJSON* container = NULL;

    container = cJSON_GetObjectItemCaseSensitive(json, value);
    if (cJSON_IsNumber(container))
    {
        return container->valueint;
    }

    return -1;
}

Module* read_modules(cJSON* json, char* value) {
    Module* modules_array = NULL;
    cJSON* modules = NULL;
    cJSON* module = NULL;
    int counter = 0;

    modules = cJSON_GetObjectItemCaseSensitive(json, value);

    if(cJSON_IsArray(modules)) { 
        int size = cJSON_GetArraySize(modules);
        modules_array = malloc(size * sizeof(Module));

        cJSON_ArrayForEach(module, modules) {
            Module mod;

            mod.type = read_string(module, "type");
            mod.tag = read_string(module, "tag");
            mod.gpio = read_number(module, "gpio");
            mod.wiringPi = transformToWiringPi(read_number(module, "gpio"));
            mod.value = 0;

            modules_array[counter] = mod;
            counter++;
        }
    }

    return modules_array;
}

int read_size_array(cJSON* json, char* value) {
    cJSON* modules = NULL;
    
    modules = cJSON_GetObjectItemCaseSensitive(json, value);

    return cJSON_GetArraySize(modules);
}

Configuration parse_json(char* configuration) {
    Configuration config;
    cJSON *configurationJson = cJSON_ParseWithLength(configuration, file_size);

    config.name = read_string(configurationJson, "nome");
    config.ip = read_string(configurationJson, "ip");
    config.port = read_number(configurationJson, "porta");
    config.outputSize = read_size_array(configurationJson, "outputs");
    config.inputSize = read_size_array(configurationJson, "inputs");
    
    config.outputs = read_modules(configurationJson, "outputs");
    config.inputs = read_modules(configurationJson, "inputs");

    return config;
}

int transformToWiringPi(int value) {
    switch (value) {
    case 4:
        return 7;
    case 17:
        return 0;
    case 27:
        return 2;
    case 7:
        return 11;
    case 26:
        return 25;
    case 23:
        return 4;
    case 9:
        return 13;
    case 11:
        return 14;
    case 10:
        return 12;
    case 13:
        return 23;
    case 19:
        return 24;
    case 16:
        return 27;
    case 22:
        return 3;
    case 25:
        return 6;
    case 8:
        return 10;
    case 12:
        return 26;
    case 18:
        return 1;
    case 24:
        return 5;
    case 5:
        return 21;
    default:
        return 22;
    }
}
