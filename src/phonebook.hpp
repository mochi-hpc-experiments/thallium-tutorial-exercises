#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

class phonebook {

    std::unordered_map<std::string, uint64_t> m_content;

    public:

    void insert(std::string name, uint64_t number) {
        m_content.emplace(std::move(name), number);
    }

    uint64_t lookup(const std::string& name) const {
        auto it = m_content.find(name);
        return it == m_content.end() ? 0 : it->second;
    }
};
