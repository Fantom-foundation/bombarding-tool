#include <iostream>

#include "CLI11.hpp"
#include "configuration.hpp"
#include "executor.hpp"
#include "bombarder.hpp"



void initialize(const blomb::ChainConfig& cfg, blomb::ChainConfigArguments& args) {
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
    size_t transaction_count = 0;
    size_t work_time = 0;

    blomb::ChainConfigArguments cfg_args;
    CLI::App app{"Blomb - A blockchain bombarding tool"};

    app.add_option("-c,--config", config_file, "Blockchain configuration")->required();
    auto work_time_option = app.add_option("-t,--time", work_time, "Bombarding time in seconds")
        ->check(CLI::PositiveNumber);
    auto transaction_count_option = app.add_option("--transactions,-x", transaction_count, "Number of transactions to send")
        ->check(CLI::PositiveNumber)->excludes(work_time_option);

    blomb::init_chain_cmd_arguments(app, cfg_args);

    CLI11_PARSE(app, argc, argv)

    blomb::ChainConfig cfg{config_file, cfg_args};
    cfg.pre_init_script();

    initialize(cfg, cfg_args);

    return 0;
}
