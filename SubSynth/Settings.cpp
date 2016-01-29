#include "Settings.h"

#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <string.h>
#include <cstdlib>
#include <QDebug>




Settings::Settings()
{
    ini.clear();
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

        std::string key = (section.empty()) ? name : section + "." + name;
        ini[key] = value;//создаем мапу
    }

    fclose(fp);
    return true;
}

bool Settings::write(const char* filename)
{
    FILE* fp;
    fp = fopen(filename, "wb");

    for (KeyValueMap::iterator it = ini.begin(); it != ini.end(); ++it)
    {
        std::string line = it->first + "=" + it->second + "\n";
        fputs(line.c_str(), fp);
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

    float val = 0.0f;
    std::istringstream istr(value->second);
    istr >> val;
    return val;
}

const char* Settings::getString(const char* key, const char* def)
{
    KeyValueMap::iterator value = ini.find(key);
    if (value == ini.end())
        return def;

    return value->second.c_str();
}
void Settings::set(const char *key, int value)
{
    std::stringstream s;
    s << value;
    std::string val(s.str());
    ini[key] = val;
}

void Settings::set(const char *key, float value)
{
    std::ostringstream s;
    s << value;
    std::string val(s.str());
    ini[key] = val;
}

void Settings::set(const char *key, const char *value)
{
    std::string val(value);
    ini[key] = val;
}
