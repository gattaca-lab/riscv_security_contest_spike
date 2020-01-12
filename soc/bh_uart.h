#ifndef UART_H_3VHTJY1B
#define UART_H_3VHTJY1B

#include "riscv/devices.h"

class bh_uart_t: public abstract_device_t {
public:
  static constexpr uint64_t ADDR_BASE = 0x8000000;
  static constexpr uint64_t OFFSET_TX = 0x4;
  static constexpr uint64_t ADDR_TX = ADDR_BASE + OFFSET_TX;

  ~bh_uart_t();

  bool init(std::string io_port);
  virtual bool load (reg_t addr, size_t len, uint8_t* bytes) override;
  virtual bool store (reg_t addr, size_t len, const uint8_t* bytes) override ;

  virtual const char* name() const override { return "BH_UART"; }
  virtual std::string description() const override {
      return "---the UART of beehive SOC";
  }


private:
  int io_port_ = -1;
};

#endif /* end of include guard: UART_H_3VHTJY1B */
