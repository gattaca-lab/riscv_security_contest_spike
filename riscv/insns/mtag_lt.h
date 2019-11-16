{
    reg_t addr = RS1 + insn.i_imm();
    unsigned ltag = ~0u;
    if (!MMU.get_imtag().load_tag(addr, ltag)) {
        fprintf(stderr,
                "  WARNING: could not LOAD tag for address: [0x%08lx]\n",
                addr);
        ltag = 0;
    }
    WRITE_RD(ltag);
}

