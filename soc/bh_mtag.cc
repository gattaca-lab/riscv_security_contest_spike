#include "riscv/sim.h"
#include "bh_mtag.h"

#include <unordered_map>
#include <exception>
#include <memory>
#include <vector>

// NOTE: we don't need this one for C++14
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

struct s_mtag_impl {
    using imtag = std::unique_ptr<mtag_ext_t>;

    std::vector<imtag> imtags;
    std::unordered_map<unsigned, unsigned> tag_mem;
};

mtag_ext_t::mtag_ext_t(s_mtag_impl& impl, processor_t& p)
    : impl_(impl), proc_(p) {
    p.EXT_attach_mtags(this);
}
bool mtag_ext_t::check_tag(reg_t addr, size_t size, op_type type) const {
    (void)addr;(void)size;(void)type;
    return false;
}
bool mtag_ext_t::store_tag(reg_t addr, unsigned tag) {
    (void)addr;(void)tag;
    return false;
}
bool mtag_ext_t::load_tag(reg_t addr, unsigned& tag) {
    tag = 0;
    (void)addr;(void)tag;
    return false;
}
void mtag_ext_t::set_mode(mode m) {
    (void)m;
    return;
}

bh_mtag_t::bh_mtag_t() {
}
bh_mtag_t::~bh_mtag_t() {
}
bool bh_mtag_t::load (reg_t addr, size_t len, uint8_t* bytes) {
    (void)addr;(void)len;(void)bytes;
    throw std::logic_error("should not happen (load)");
}
bool bh_mtag_t::store (reg_t addr, size_t len, const uint8_t* bytes) {
    (void)addr;(void)len;(void)bytes;
    throw std::logic_error("should not happen (store)");
}
bool bh_mtag_t::initialize(sim_t& sim) {
    fprintf(stderr, "initializing MTAG extention...\n");
    impl_ = make_unique<s_mtag_impl>();
    s_mtag_impl& impl = *impl_;
    const size_t nprocs = sim.nprocs();
    impl.imtags.resize(nprocs);
    for (size_t i = 0; i < nprocs; ++i) {
        impl.imtags[i] = make_unique<mtag_ext_t>(impl, *sim.get_core(i));
        fprintf(stderr, "   +MTAG attached to core #%d\n", (unsigned)i);
    }
    return true;
}
