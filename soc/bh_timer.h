#ifndef BH_TIMER_H_H02E87LB
#define BH_TIMER_H_H02E87LB

#include "riscv/devices.h"

class bh_timer_t: public abstract_device_t {
public:
  static const reg_t DEVICE_BASE = 0x40000000;
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

private:
  sim_t* ptr_sim = nullptr;
};

#endif /* end of include guard: BH_TIMER_H_H02E87LB */

