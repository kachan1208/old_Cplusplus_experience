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
    bool write(const char* filename);

    int getInt(const char* key, int def);
    float getFloat(const char* key, float def);
    const char* getString(const char* key, const char* def);

    void set(const char* key, int value);
    void set(const char* key, float value);
    void set(const char* key, const char* value);
private:
    typedef std::map<std::string, std::string> KeyValueMap;
    KeyValueMap ini;
// Members
};
