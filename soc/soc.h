#ifndef SOC_H_ZRH4FGRJ
#define SOC_H_ZRH4FGRJ

/*
 Soc represents a collection of model-specific functinality
*/

#include "riscv/devices.h"

#include <memory>

class sim_t;
class bus_t;

class abstract_soc_t
{
public:
    virtual void add_device(bus_t& bus) = 0;

    using factory_function =
        std::unique_ptr<abstract_soc_t> (*) (sim_t& sim, const char* options);

    static bool register_impl(std::string impl, factory_function);
    static std::unique_ptr<abstract_soc_t> create(const char* desc, sim_t& sim);
};

#endif /* end of include guard: SOC_H_ZRH4FGRJ */
