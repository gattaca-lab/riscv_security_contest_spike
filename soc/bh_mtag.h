#include <memory>

#include "devices.h"

class sim_t;
struct s_mtag_impl;

class mtag_ext_t {
public:
  mtag_ext_t(s_mtag_impl& impl, processor_t& p);
  enum class mode { none, dc, ic, both };
  enum class op_type { L, S, I };

  bool check_tag(reg_t addr, size_t size, op_type type) const;
  bool store_tag(reg_t addr, unsigned tag);
  bool load_tag(reg_t addr, unsigned& tag);
  void set_mode(mode m);

private:
  mode mode_ = mode::none;
  s_mtag_impl& impl_;
  processor_t& proc_;
};

// WARNING: this device is not implemented yet
class bh_mtag_t: public abstract_device_t {
public:
  bh_mtag_t();
  ~bh_mtag_t();
  virtual bool load (reg_t addr, size_t len, uint8_t* bytes) override;
  virtual bool store (reg_t addr, size_t len, const uint8_t* bytes) override;

  virtual const char* name() const override { return "BH_MTAGS"; }
  virtual std::string description() const override {
      return "---the MTAGS of beehive SOC";
  }

  bool initialize(sim_t& sim);

private:
  std::unique_ptr<s_mtag_impl> impl_;
};

