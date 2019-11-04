# Beehive-RISCV SOC

The functionality which is specific to behive soc should be implemented as
part of this library. Direct modification of spike should be avoided
(if possible).


## How to build

```
../beehive-riscv-isa-sim/configure --with-isa=RV32IMC --enable-histogram --prefix=/home/<username>/prefix
make install -j16
```

## How to run

```
spike \
    -m0:256K \
    --soc=beehive:uart_file=-- \
    --pc=0 \
    -g -l \
    /home/apink/security/build/tests/compliance_rv32i_I-ADD-01.S/test.elf
    2>exec_log
```
