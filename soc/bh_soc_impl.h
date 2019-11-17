#ifndef SOC_IMPL_H_EMGHAGJN
#define SOC_IMPL_H_EMGHAGJN

#include "soc/soc.h"

#include "soc/bh_uart.h"
#include "soc/bh_mtag.h"
#include "soc/bh_timer.h"

struct bh_soc_options {
    std::string io_port;
};

class bh_soc_t: public abstract_soc_t {
public:
    void add_device(bus_t& bus) override;
    bh_soc_t(sim_t& sim, const char* options);

private:
    sim_t& sim_;

    bh_uart_t uart_;
    bh_mtag_t mtag_;
    bh_timer_t timer_;
};


#endif /* end of include guard: SOC_IMPL_H_EMGHAGJN */
