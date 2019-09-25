#pragma once

#include <thread>
#include <random>
#include <atomic>

#include "executor.hpp"
#include "configuration.hpp"



namespace blomb {

namespace detail {
static inline void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

static inline void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

static inline void trim(std::string& s)
{
    ltrim(s);
    rtrim(s);
}
}

class Bombarder {
private:
    const ChainConfig& config;
    ChainConfigArguments& arguments;
    std::thread worker;
    std::atomic_bool stopped{false};
    std::vector<std::string> wallets;

    std::uniform_int_distribution<size_t> wallets_distribution;
    std::mt19937 random;

public:
    Bombarder(const ChainConfig& config, ChainConfigArguments& arguments, size_t num_wallets)
        : config(config), arguments(arguments)
    {
        init_wallets(num_wallets);
        wallets_distribution = std::uniform_int_distribution<size_t>{0, wallets.size() - 1};
    }

    void start_bombarding(size_t max_transactions = 0)
    {
        worker = std::thread{
            [this, max_transactions]() {
                bombard(max_transactions);
            }
        };
    }

    void stop_bombarding()
    {
        stopped.store(true, std::memory_order_relaxed);
    }

    ~Bombarder()
    {
        if (worker.joinable())
            worker.join();
    }

private:
    void init_wallets(size_t how_many)
    {
        auto init_script = config.wallet_init_script();

        while (how_many--) {
            if (init_script.empty()) {
                std::cout << "Warning: wallet_init script is empty" << std::endl;
                wallets.emplace_back(std::to_string(how_many));
                continue;
            }

            Process p{init_script.front(), true};
            p.wait_termination();
            std::string addr = p.read_output();
            detail::trim(addr);
            wallets.emplace_back(addr);
        }
    }

    void perform_transaction()
    {
        arguments["from"] = next_wallet();
        arguments["to"] = next_wallet();
        arguments["sum"] = "1";

        execute(config.transaction_script());

        arguments.del("from");
        arguments.del("to");
        arguments.del("sum");
    }

    void bombard(size_t max_transactions)
    {
        for (size_t i = 0;
             !stopped.load(std::memory_order_relaxed) && (!max_transactions || i < max_transactions);
             ++i) {
            perform_transaction();
        }
    }

    const std::string& next_wallet() {
        return wallets[wallets_distribution(random)];
    }
};

}
