#ifndef BH_TIMER_H_H02E87LB
#define BH_TIMER_H_H02E87LB

class sim_t;
class processor_t;

#include "riscv/devices.h"

class bh_timer_t: public abstract_device_t {
public:
  static const reg_t DEVICE_BASE = 0x4000000;
  static const reg_t MMIO_SIZE   = 0x20;

  bh_timer_t();
  ~bh_timer_t();
  virtual bool load (reg_t addr, size_t len, uint8_t* bytes) override;
  virtual bool store (reg_t addr, size_t len, const uint8_t* bytes) override;

  virtual const char* name() const override { return "BH_TIMER"; }
  virtual std::string description() const override {
      return "---the TIMER implementation of beehive SOC";
  }

  bool initialize(sim_t& sim);

  bool inc_timer();

private:
  processor_t* ptr_proc = nullptr;
  uint32_t mtime_inc_lo = 0;
  uint32_t mtime_tmp_lo = 0;
  uint32_t mtime_cur_lo = 0;
  uint32_t mtime_cmp_lo = 0;
  uint32_t mtime_inc_hi = 0;
  uint32_t mtime_tmp_hi = 0;
  uint32_t mtime_cur_hi = 0;
  uint32_t mtime_cmp_hi = 0;
};

#endif /* end of include guard: BH_TIMER_H_H02E87LB */

