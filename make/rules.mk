# Let the compiler generate prerequisites.

%.o: %.c
	@echo "[CC] $@ "
	@echo $(shell pwd)
	@mkdir -p $(dir $@)
	@$(CC) $(CPUFLAGS) $(CPPFLAGS) $(CFLAGS) -MD -c -o $@ $<
# TODO sed magic!

%.a:
	mkdir -p $(dir $@)
	$(AR) rcs $@ $^

%.elf:
	@echo "[LD] $@"
	@mkdir -p $(dir $@)
	@$(CC) $(CPUFLAGS) $(CFLAGS) $^ --output $@ $(LDFLAGS)

%.hex: %.elf
	@echo "[OBJCOPY] $@"
	@$(OBJCOPY) -O $(HEXFORMAT) -R .eeprom $< $@

%.lss: %.elf
	@echo "[OBJDUMP] $@"
	@$(OBJDUMP) -h -S $< > $@

%.sym: %.elf
	@echo "[NM] $@"
	@$(NM) -n $< > $@

