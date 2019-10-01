#include <iostream>

#include <chrono>

#include "CLI11.hpp"
#include "configuration.hpp"
#include "executor.hpp"
#include "bombarder.hpp"
#include "load.hpp"



void initialize(const blomb::ChainConfig& cfg, blomb::ChainConfigArguments& args)
{
    blomb::execute(cfg.pre_init_script());

    size_t node_count = std::stoull(args["n"]);
    for (size_t i = 0; i < node_count; ++i) {
        args["i"] = std::to_string(i);
        blomb::execute(cfg.node_init_script());
    }
    args.del("i");

    blomb::execute(cfg.post_init_script());
}

int main(int argc, char* argv[])
{
    std::string config_file;
    size_t max_transaction_count = 0;
    size_t max_work_time = 0;
    size_t num_wallets = 10;

    blomb::ChainConfigArguments cfg_args;
    CLI::App app{"Blomb - A blockchain bombarding tool"};

    app.add_option("-c,--config", config_file, "Blockchain configuration")->required();
    app.add_option("-t,--time", max_work_time, "Max bombarding time in seconds")
        ->check(CLI::PositiveNumber);
    app.add_option("--transactions,-x", max_transaction_count, "Max number of transactions to send")
        ->check(CLI::PositiveNumber);
    app.add_option("--wallets,-w", max_transaction_count, "Number of wallets")
        ->check(CLI::PositiveNumber);

    blomb::init_chain_cmd_arguments(app, cfg_args);

    CLI11_PARSE(app, argc, argv)

    blomb::ChainConfig cfg{config_file, cfg_args};
    cfg.pre_init_script();

    initialize(cfg, cfg_args);
    blomb::Bombarder bombarder{cfg, cfg_args, num_wallets};

    bombarder.start_bombarding(max_transaction_count);
    if (max_work_time) {
        std::this_thread::sleep_for(std::chrono::seconds(max_work_time));
        bombarder.stop_bombarding();
    }

    return 0;
}
