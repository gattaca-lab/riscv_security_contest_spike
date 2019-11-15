#include "soc/bh_soc_impl.h"
#include "soc/bh_uart.h"

#include "soc/soc.h"

#include <sstream>

bool bh_soc_registered =
    abstract_soc_t::register_impl("beehive",
        [] (sim_t& sim, const char* options) {
            return std::unique_ptr<abstract_soc_t>(new bh_soc_t(sim, options));
        });

bool get_str_val(const std::string& input,
                 const std::string& arg,
                 std::string& output) {
    output.clear();
    if (input.find(arg) == 0) {
        output = input.substr(arg.size());
        return true;
    }
    return false;
}
void bh_soc_t::add_device(bus_t& bus)
{
    bus.add_device(bh_uart_t::ADDR_BASE, &uart_);
}
bh_soc_t::bh_soc_t(sim_t& sim, const char* options)
    :sim_(sim)
{
    bh_soc_options soc_opts;

    std::string opt_string(options);
    std::string parsed;
    std::stringstream input_stringstream(opt_string);

    std::string str_arg;
    if (getline(input_stringstream, parsed, ':'))
    {
        if (get_str_val(parsed, "uart_file=", str_arg)) {
            soc_opts.io_port = str_arg;
        }
    }
    uart_.init(soc_opts.io_port);
    mtag_.initialize(sim);
}

