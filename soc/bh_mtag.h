#include "devices.h"

// WARNING: this device is not implemented yet
class bh_mtag_t: public abstract_device_t {
public:
  virtual bool load (reg_t addr, size_t len, uint8_t* bytes) override;
  virtual bool store (reg_t addr, size_t len, const uint8_t* bytes) override;

  virtual const char* name() const override { return "BH_MTAGS"; }
  virtual std::string description() const override {
      return "---the MTAGS of beehive SOC";
  }
private:
};

