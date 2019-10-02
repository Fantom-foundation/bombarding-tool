#pragma once

#include <thread>
#include <random>
#include <atomic>
#include <ctime>
#include <chrono>

#include "executor.hpp"
#include "configuration.hpp"
#include "load.hpp"


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
        blomb::Load load_meter{};
        double cpu_usage = 0;
        double memory_usage = 0;
        size_t iterations_count = 0;
        auto t1 = std::chrono::high_resolution_clock::now();
        for (size_t i = 0;
             !stopped.load(std::memory_order_relaxed) && (!max_transactions || i < max_transactions);
             ++i) {
            perform_transaction();
            cpu_usage += load_meter.get_cpu_usage();
            memory_usage += load_meter.get_memory_usage();
            ++iterations_count;
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        double elapsed_secs = (double)duration / 1000.0;

        std::cout << duration;
        load_meter.print_load(cpu_usage / (double)iterations_count, memory_usage / (double)iterations_count, (double)iterations_count / elapsed_secs);
    }

    const std::string& next_wallet() {
        return wallets[wallets_distribution(random)];
    }
};

}
