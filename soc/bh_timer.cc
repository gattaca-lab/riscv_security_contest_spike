#include "riscv/sim.h"
#include "bh_timer.h"

#include <memory>
#include <chrono>

const reg_t bh_timer_t::DEVICE_BASE;

bh_timer_t::bh_timer_t() {
}
bh_timer_t::~bh_timer_t() {
}
bool bh_timer_t::load (reg_t addr, size_t len, uint8_t* bytes) {
  const size_t ADDR_MTIME     = 0x0;
  const size_t ADDR_MTIMECMP  = 0x8;
  const size_t ADDR_MTIMECTRL = 0x10;
  const size_t ADDR_AE_BASE   = 0x18;

  if (!len)
    return false;
  if (!bytes)
    return false;
  if (!ptr_sim)
    return false;

  if ((addr >= ADDR_MTIME) && (addr + len <= ADDR_MTIME + 8)) {
    fprintf(stderr, "  WARNING [bh_timer]: mtime is not implemented\n");
    std::memset(bytes, 0, len);
  }
  else if ((addr >= ADDR_MTIMECMP) && (addr + len <= ADDR_MTIMECMP + 8)) {
    fprintf(stderr, "  WARNING [bh_timer]: mtimecmp is not implemented\n");
    std::memset(bytes, 0, len);
  }
  else if ((addr >= ADDR_MTIMECTRL) && (addr + len <= ADDR_MTIMECTRL + 8)) {
    fprintf(stderr, "  WARNING [bh_timer]: mtimectrl is not implemented\n");
    std::memset(bytes, 0, len);
  }
  else if ((addr >= ADDR_AE_BASE) && (addr + len <= ADDR_AE_BASE + 8)) {
    auto now = std::chrono::high_resolution_clock::now();
    uint64_t nanos =
      std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch())
        .count();
    std::memcpy(bytes, (uint8_t*)&nanos + (addr - ADDR_AE_BASE), len);
  }
  else {
    return false;
  }
  return true;
}
bool bh_timer_t::store (reg_t addr, size_t len, const uint8_t* bytes) {
  if ((addr >= 0) && ((addr + len) <= MMIO_SIZE)) {
    fprintf(stderr,
            "  WARNING [bh_timer]: stores to timer are ignored"
            "; addr = [%08lx, %lu]\n", addr, len);
    return true;
  }
  return false;
}
bool bh_timer_t::initialize(sim_t& sim) {
  ptr_sim = &sim;
  return true;
}

