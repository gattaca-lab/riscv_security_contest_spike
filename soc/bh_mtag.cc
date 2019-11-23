#include "riscv/sim.h"
#include "riscv/mmu.h"
#include "bh_debug.h"
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

    reg_t untag_address(reg_t addr) const;
    unsigned va2tag(reg_t addr) const;
    unsigned extract_tag(reg_t addr) const;
    bool validate_address(reg_t addr) const;

    std::vector<imtag> imtags;
    std::unordered_map<unsigned, unsigned> tag_mem;

    uint64_t MASK_UNTAG = (1u << 26u) - 1;
    uint64_t MASK_TAG   = 0xfu << 26u;
    uint64_t MASK_VALIDATION = ~(MASK_TAG | MASK_UNTAG);
    uint64_t GRANULE_SIZE = 16;

    const unsigned INVALID_MAPPING = ~0u;
};
reg_t s_mtag_impl::untag_address(reg_t addr) const {
  return (addr & MASK_UNTAG) | (addr & MASK_VALIDATION);
}
unsigned s_mtag_impl::extract_tag(reg_t addr) const {
  return ((addr & MASK_TAG) >> 26) & 0xf;
}
unsigned s_mtag_impl::va2tag(reg_t addr) const {
  if (!validate_address(addr)) {
    return INVALID_MAPPING;
  }
  reg_t untagged = untag_address(addr);
  return untagged / GRANULE_SIZE;
}
bool s_mtag_impl::validate_address(reg_t addr) const {
  return (addr & MASK_VALIDATION) == 0;
}

mtag_ext_t::mtag_ext_t(s_mtag_impl& impl, processor_t& p)
    : impl_(impl), proc_(p) {
    mmu_t* mmu = p.get_mmu();
    if (!mmu) {
        throw std::logic_error("the processor does not have initilized mmu");
    }
    mmu->EXT_attach_mtags(this);
}

bool mtag_ext_t::check_tag(reg_t addr, size_t size, op_type type) const {
  if (mode_ == mode::none) {
    return true;
  }
  if (mode_ == mode::ic) {
    LOG_MSG(en_logv::always, "  WARNING: IC-LEVEL tag match is not implemented!\n");
    return true;
  }
  if ((mode_ == mode::dc) && (type == op_type::I)) {
    return true;
  }
  const unsigned SRC_TAG = impl_.extract_tag(addr);
  for(unsigned i = impl_.va2tag(addr),
      e = impl_.va2tag(addr + size - 1) + 1; i < e; ++i) {
    unsigned dst_tag = ~0u;
    load_tag(i * 16, dst_tag);
    if (SRC_TAG != dst_tag) {
      LOG_MSG(en_logv::error,
              "  ATTENTION: INTRUDER DETECTED "
              "[dst_tag=%u,src_tag=%u]!\n", dst_tag, SRC_TAG);
      LOG_MSG(en_logv::error,
              "  Breach ID: 0x%08lx (pure: 0x%08lx)\n",
              addr, untag_address(addr));

      // mtag hack
      // The purpose of this hack is not to inroduce an additional
      // processor-level signal/flag. We try to piggy-back on existing
      // functionality. I believe that such implementation does not
      // contradict RISCV spec
      // (but our beehive SOC does not function this way).
      proc_.get_state()->mip |= MIP_EXT_MTAG_IP;
      proc_.get_state()->mie |= MIP_EXT_MTAG_IP;
      // to ensure that we bail out from "iffecient" execution path
      proc_.get_state()->pessimize_execution = true;
      proc_.get_mmu()->flush_icache();

      return false;
    }
  }
  return true;
}
reg_t mtag_ext_t::untag_address(reg_t addr) const {
  return impl_.untag_address(addr);
}
bool mtag_ext_t::store_tag(reg_t addr, unsigned tag) {
    if (!impl_.validate_address(addr)) {
        LOG_MSG(en_logv::error, "mtag::store_tag - address validation failed\n");
        return false;
    }
    unsigned t_a = impl_.va2tag(addr);
    if (t_a == impl_.INVALID_MAPPING) {
        LOG_MSG(en_logv::error, "mtag::store_tag no va->tag mapping\n");
        return false;
    }
    impl_.tag_mem[t_a] = tag;
    LOG_MSG(en_logv::noise, "mtag::store_tag - writing %u->@%u\n", tag, t_a);
    return true;
}
bool mtag_ext_t::load_tag(reg_t addr, unsigned& tag) const {
    tag = 0;
    if (!impl_.validate_address(addr)) {
        LOG_MSG(en_logv::error, "mtag::load_tag - address validation failed\n");
        return false;
    }
    unsigned t_a = impl_.va2tag(addr);
    if (t_a == impl_.INVALID_MAPPING) {
        LOG_MSG(en_logv::error, "mtag::load_tag no va->tag mapping\n");
        return false;
    }
    auto it = impl_.tag_mem.find(t_a);
    if (it == impl_.tag_mem.end()) {
        // NOTE: should be error, but for some tests (RIPE) this is too noisy.
        LOG_MSG(en_logv::debug,
                "mtag::load_tag tries to access "
                "unitialized tag memory, returning \"0\"@%u\n", t_a);
        return true;
    }
    tag = it->second;
    LOG_MSG(en_logv::noise, "reading %u<-@%u\n", tag, t_a);
    return true;
}
void mtag_ext_t::set_mode(mode m) {
    mode_ = m;
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
    LOG_MSG(en_logv::always, "initializing MTAG extention...\n");
    impl_ = make_unique<s_mtag_impl>();
    s_mtag_impl& impl = *impl_;
    const size_t nprocs = sim.nprocs();
    impl.imtags.resize(nprocs);
    for (size_t i = 0; i < nprocs; ++i) {
        impl.imtags[i] = make_unique<mtag_ext_t>(impl, *sim.get_core(i));
        LOG_MSG(en_logv::always, "   +MTAG attached to core #%d\n", (unsigned)i);
    }
    return true;
}
