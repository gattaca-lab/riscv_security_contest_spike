#include "soc/soc.h"
#include "soc/bh_soc_impl.h"
#include "soc/bh_debug.h"

#include <memory>
#include <unordered_map>

// to enforce linker not to remove the symbol
extern bool bh_soc_registered;
// dirty hack: we define a function which is use a symbol required for
// registaration... And we define an unused funcion, thus forcing linker
// not to discard registration routines
void unused () {
  LOG_MSG(en_logv::always, "registed impl: %d\n", bh_soc_registered);
}

namespace {

struct FactoryStorage
{
    static FactoryStorage& Instance ();

    std::unordered_map<std::string, abstract_soc_t::factory_function> registry;

private:
    FactoryStorage() {}
};
FactoryStorage& FactoryStorage::Instance () {
    static FactoryStorage impl;
    return impl;
}

}

bool abstract_soc_t::register_impl(std::string impl, factory_function func)
{
    auto& f = FactoryStorage::Instance();
    if (f.registry.find(impl) == f.registry.end()) {
        f.registry.insert({impl, func});
        LOG_MSG(en_logv::info,
                "note: registered <%s>\n", impl.c_str());
        return true;
    }
    LOG_MSG(en_logv::error,
            "error: factory function for <%s> soc is already registered!\n",
            impl.c_str());
    throw std::runtime_error("already registered");
    return false;
}
std::unique_ptr<abstract_soc_t> abstract_soc_t::create(
        const char* desc,
        sim_t& sim)
{
    const auto& f = FactoryStorage::Instance();
    std::string soc_name;
    std::string opt;
    if (desc) {
        std::string s_desc(desc);
        size_t pos = s_desc.find_first_of(':');

        if (std::string::npos == pos) {
            soc_name = s_desc;
        } else {
            soc_name = s_desc.substr(0, pos);
            opt = s_desc.substr(pos + 1); // pos == s_desc.size() is legal
        }
    }

    auto it = f.registry.find(soc_name);
    if (it == f.registry.end()) {
        LOG_MSG(en_logv::error,
                "error: could not find factory function for <%s>:<%s>\n",
                soc_name.c_str(),
                opt.c_str());
        return nullptr;
    }
    return it->second(sim, opt.c_str());
}

