#include <utility>

#pragma once

#include <yaml-cpp/yaml.h>
#include <map>
#include <cctype>
#include <sstream>

#include "CLI11.hpp"



namespace blomb {

class ConfigParseException : public std::runtime_error {
public:
    explicit ConfigParseException(const std::string& arg)
        : runtime_error(arg)
    {}

    explicit ConfigParseException(const char* string)
        : runtime_error(string)
    {}
};


class ChainConfigArguments {
private:
    std::map<std::string, std::string> values;

public:
    std::string& operator[] (const std::string& token) {
        return values[token];
    }

    void del(const std::string& token) {
        values.erase(token);
    }

    std::string configure(const std::string& templ) const
    {
        enum State {
            NORMAL, OPEN_BRACES, OPEN_PERCENT
        };
        State state = NORMAL;

        std::ostringstream res;
        std::ostringstream tmp;

        for (char c : templ) {
            switch (state) {
                case NORMAL:
                    if (c == '{')
                        state = OPEN_BRACES;
                    else
                        res << c;
                    break;

                case OPEN_BRACES:
                    if (c == '%')
                        state = OPEN_PERCENT;
                    else {
                        res << '{' << c;
                        state = NORMAL;
                    }
                    break;

                case OPEN_PERCENT:
                    if (c == '}') {
                        auto it = values.find(tmp.str());
                        if (it == values.end())
                            throw ConfigParseException("Unrecognized token: {%" + tmp.str() + "}");
                        tmp.clear();
                        res << it->second;
                        state = NORMAL;
                    } else if (c == '{' || c == '%' || std::isspace(c)) {
                        res << "{%" << tmp.str();
                        if (c != '%')
                            res << c;
                        tmp.clear();
                        state = NORMAL;
                    } else {
                        tmp << c;
                    }
                    break;
            }
        }

        return res.str();
    }
};

void init_chain_cmd_arguments(CLI::App& app, ChainConfigArguments& args) {
    app.add_option("-n,--nodes", args["n"], "Number of nodes")
        ->check(CLI::PositiveNumber)
        ->required();
}


class ChainConfig {
private:
    YAML::Node root;
    const ChainConfigArguments& arguments;

public:
    explicit ChainConfig(const std::string& filename, const ChainConfigArguments& args)
        : arguments(args)
    {
        try {
            root = YAML::LoadFile(filename);
        } catch (YAML::Exception& exc) {
            throw ConfigParseException(exc.what());
        }
    }

    std::vector<std::string> pre_init_script() const
    {
        return get_script("pre_init");
    }

    std::vector<std::string> post_init_script() const
    {
        return get_script("post_init");
    }

    std::vector<std::string> node_init_script() const
    {
        return get_script("node_init");
    }

private:
    std::vector<std::string> get_script(const std::string& name) const
    {
        auto res = get_raw_script(name);
        for (auto& item : res)
            item = arguments.configure(item);
        return res;
    }

    std::vector<std::string> get_raw_script(const std::string& name) const
    {
        const YAML::Node& node = root[name];

        if (!node) {
            return {};
        }

        if (node.IsSequence()) {
            std::vector<std::string> res;
            for (const auto& n : node) {
                res.push_back(n.as<std::string>());
            }
            return res;
        }

        if (node.IsScalar()) {
            return {node.as<std::string>()};
        }

        throw ConfigParseException(name + " must be sequence or string");
    }
};

}
