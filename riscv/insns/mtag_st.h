{
    reg_t addr = RS1 + insn.s_imm();
    if (!MMU.get_imtag().store_tag(addr, RS2)) {
        fprintf(stderr,
                "  WARNING: could not STORE tag for address: [0x%08lx]\n",
                addr);
    }
}

