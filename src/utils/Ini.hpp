#ifndef INI_H
#define INI_H

#include <map>
#include <string>


class Ini {
public:
    using Entries = std::map<std::string, std::string>;
    using Categories = std::map<std::string, Entries>;
private:
    Categories categories;

    std::string filename;
    bool modified;
    bool newFile;
public:
    explicit Ini(const std::string& filename);
    ~Ini();

    void load();
    void write(bool bForce = false);

    bool isNew() const;

    Categories::iterator begin();
    Categories::iterator end();
    Entries* getEntry(const std::string& category);
    bool hasCategory(const std::string& category);
    Entries& expectCategory(const std::string& category);
    void deleteCategory(const std::string& category);
    bool getEntry(const std::string& category, const std::string& entry, std::string& data);
    bool getEntry(Entries& entries, const std::string& entry, std::string& data);
    void setEntry(const std::string& category, const std::string& entry, const std::string& data);
    void setEntry(Entries& entries, const std::string& entry, const std::string& data);
};

#endif
