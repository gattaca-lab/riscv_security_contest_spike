#include "devices.h"
#include <stdio.h>
#include <sstream>

#include "soc/bh_debug.h"

std::string rom_device_t::description() const {
    std::stringstream ss;
    ss << "rom size: " << data.size() <<
        " (" << std::hex << "0x" << data.size() << ")";
    return ss.str();
}
std::string mem_t::description() const {
    std::stringstream ss;
    ss << "chunk size: " << len <<
        " (" << std::hex << "0x" << len << ")";
    return ss.str();
}
std::string clint_t::description() const {
    return "timer?";
}
std::string mmio_plugin_device_t::description() const {
    return "mmio_plugin";
}
void bus_t::add_device(reg_t addr, abstract_device_t* dev)
{
  if (!dev) {
      return;
  }
  // Searching devices via lower_bound/upper_bound
  // implicitly relies on the underlying std::map 
  // container to sort the keys and provide ordered
  // iteration over this sort, which it does. (python's
  // SortedDict is a good analogy)
  devices[addr] = dev;
  LOG_MSG(en_logv::info, "adding device [%s], @%#x\n", dev->name(), (unsigned)addr);
  LOG_MSG(en_logv::info, "   +%s\n", dev->description().c_str());
}

bool bus_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  // Find the device with the base address closest to but
  // less than addr (price-is-right search)
  auto it = devices.upper_bound(addr);
  if (devices.empty() || it == devices.begin()) {
    // Either the bus is empty, or there weren't 
    // any items with a base address <= addr
    return false;
  }
  // Found at least one item with base address <= addr
  // The iterator points to the device after this, so
  // go back by one item.
  it--;

  LOG_MSG(en_logv::debug, 
          "bus_t::load input addr: %x, result: %x, base: %x\n",
          (int)addr, (int)(addr - it->first), (int)it->first);

  return it->second->load(addr - it->first, len, bytes);
}

bool bus_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  // See comments in bus_t::load
  auto it = devices.upper_bound(addr);
  if (devices.empty() || it == devices.begin()) {
    return false;
  }
  it--;

  LOG_MSG(en_logv::debug,
          "bus_t::store input addr: %x, result: %x, base: %x\n",
          (int)addr, (int)(addr - it->first), (int)it->first);

  return it->second->store(addr - it->first, len, bytes);
}

std::pair<reg_t, abstract_device_t*> bus_t::find_device(reg_t addr)
{
  // See comments in bus_t::load
  auto it = devices.upper_bound(addr);
  if (devices.empty() || it == devices.begin()) {
    LOG_MSG(en_logv::debug, "bust_t::find_device no device found, returning default\n");
    return std::make_pair((reg_t)0, (abstract_device_t*)NULL);
  }
  it--;
  return std::make_pair(it->first, it->second);
}

// Type for holding all registered MMIO plugins by name.
using mmio_plugin_map_t = std::map<std::string, mmio_plugin_t>;

// Simple singleton instance of an mmio_plugin_map_t.
static mmio_plugin_map_t& mmio_plugin_map()
{
  static mmio_plugin_map_t instance;
  return instance;
}

void register_mmio_plugin(const char* name_cstr,
                          const mmio_plugin_t* mmio_plugin)
{
  std::string name(name_cstr);
  if (!mmio_plugin_map().emplace(name, *mmio_plugin).second) {
    throw std::runtime_error("Plugin \"" + name + "\" already registered!");
  }
}

mmio_plugin_device_t::mmio_plugin_device_t(const std::string& name,
                                           const std::string& args)
  : plugin(mmio_plugin_map().at(name)), user_data((*plugin.alloc)(args.c_str()))
{
}

mmio_plugin_device_t::~mmio_plugin_device_t()
{
  (*plugin.dealloc)(user_data);
}

bool mmio_plugin_device_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  return (*plugin.load)(user_data, addr, len, bytes);
}

bool mmio_plugin_device_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  return (*plugin.store)(user_data, addr, len, bytes);
}
