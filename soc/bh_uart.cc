#include "soc/bh_uart.h"

#include <unistd.h>
#include <fcntl.h>

constexpr uint64_t bh_uart_t::ADDR_BASE;
constexpr uint64_t bh_uart_t::OFFSET_TX;
constexpr uint64_t bh_uart_t::ADDR_TX;

bh_uart_t::~bh_uart_t() {
    if (io_port_ >= 0) {
        close(io_port_);
        io_port_ = -1;
    }
}

bool bh_uart_t::init(std::string io_port) {
    if (io_port.empty()) {
        io_port = "io.txt";
    }
    fprintf(stderr, "initializing uart: io_port=[%s]\n", io_port.c_str());
    if (io_port == "--") {
        io_port_ = STDOUT_FILENO;
    } else {
        io_port_ = open(io_port.c_str(),
                        O_WRONLY | O_TRUNC | O_CREAT,
                        S_IRUSR | S_IWUSR);
        if (io_port_ < 0) {
            fprintf(stderr,
                    "  [uart error]: could not initialize io_port\n");
            exit(EXIT_FAILURE);
            return false;
        }
    }
    return true;
}

bool bh_uart_t::load (reg_t addr, size_t len, uint8_t* bytes) {
    fprintf(stderr, "uart load: %zu\n", addr);
    return true;
}

bool bh_uart_t::store (reg_t addr, size_t len, const uint8_t* bytes) {
    switch(addr) {
    case OFFSET_TX:
        if (len != 1) {
            fprintf(stderr,
                    "  [uart warning] TX invalid data size! (%zu)\n", len);
        }
        write(io_port_, bytes, 1);
        break;
    default:
        fprintf(stderr,
                "  [uart warning] unknown write operation requested "
                "addr = %lx, size = %zu\n", addr, len);
    }
    return true;
}
