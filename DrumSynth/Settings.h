
#ifndef SETTINGS_H
#define SETTINGS_H
#include <string>
#include <map>


class Settings
{
public:
// Constructor/Destructor
    Settings();
    ~Settings();

// Implementation
    bool read(const char* filename);

    int getInt(const char* key, int def);
    float getFloat(const char* key, float def);
    std::string getString(const char* key, std::string def);

private:
// Members
    typedef std::map<std::string, std::string> KeyValueMap;
    KeyValueMap ini;
};

#endif // SETTIzzzzzzzzzzzzzZZaesdfwefwe+*
