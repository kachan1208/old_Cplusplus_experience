#include "Settings.h"
#include <fstream>
#include <string>
#include <map>
#include <stdlib.h>




Settings::Settings()
{
}

Settings::~Settings()
{
}



bool Settings::read(const char* filename)
{
    FILE * fp = fopen(filename, "r");
    if (!fp)
        return false;

    char buf[256];
    std::string section;
    while (!feof(fp))
    {
        if (!fgets(buf, 256, fp))
            break;

        std::string line(buf);
        std::string::size_type pos = line.find_first_of("\t\r\n");
        if (pos == std::string::npos || pos == 0)
            continue;

        if (line[0] == '[')
        {
            section = line.substr(1, pos-2);//обрезаем []
            continue;
        }

           std::string::size_type pos2 = line.find_first_of("=");
        if (pos2 != std::string::npos)
            pos = ++pos2;

        std::string name(line.substr(0, --pos2));//удаляем =
        std::string value = line.substr(pos);
        pos = value.find_first_of("\t\r\n");
        if (pos != std::string::npos)
            value = value.substr(0, pos);//добываем значение

        ini[section+"."+name] = value;//создаем мапу
    }

    fclose(fp);
    return true;
}

int Settings::getInt(const char* key, int def)
{
    KeyValueMap::iterator value = ini.find(key);
    if (value == ini.end())
        return def;

    return atoi(value->second.c_str());
}

float Settings::getFloat(const char* key, float def)
{
    KeyValueMap::iterator value = ini.find(key);
    if (value == ini.end())
        return def;

    return atof(value->second.c_str());
}

std::string Settings::getString(const char* key, std::string def)
{
    KeyValueMap::iterator value = ini.find(key);
    if (value == ini.end())
        return def;

    return value->second;
}
