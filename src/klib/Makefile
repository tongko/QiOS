arch_dir = ./arch/$(HOSTARCH)

include $(arch_dir)/make.config

CFLAGS += $(KERNEL_ARCH_CFLAGS)
CPPFLAGS += $(KERNEL_ARCH_CPPFLAGS)
ASFLAGS += $(KERNEL_ARCH_ASFLAGS)

cpp_src = $(shell find . -type f -name "*.cpp")
asm_src = $(shell find . \( -name *.asm ! -name *crt*.asm \))

vpath %.cpp ./
vpath %.hpp ../include

define cpp2o
	$(patsubst %.cpp,$(output_dir)/%.o,$(notdir $(1)))
endef

cpp_objs := $(call cpp2o,$(cpp_src))
scpp_objs := $(sort $(cpp_objs))
ifneq ($(words $(cpp_objs)),$(words $(scpp_objs)))
	$(error Found multiple cpp source files with same base name...)
endif

define asm2o
	$(patsubst %.asm,$(output_dir)/%.o,$(notdir $(1)))
endef

asm_objs := $(call asm2o,$(asm_src))
sasm_objs := $(sort $(asm_objs))
ifneq ($(words $(asm_objs)),$(words $(sasm_objs)))
	$(error Found multiple asm source files with same base name...)
endif

.PHONY: all

all:   $(output_dir) $(output_dir)/libklib.a

$(output_dir)/libklib.a: $(asm_objs) $(cpp_objs)
	@printf "\n \e[32;1mCreating QKernel System Framework static library...\e[0m\n"
	@$(AR) rcs $@ $(asm_objs) $(cpp_objs)
	@rm $(output_dir)/*.d $(output_dir)/*.o
	@printf "\n \e[32;1m[OK]\e[0m libklib ==> $(output_dir)/libklib.a\n\n"

# Compilation rule for a asm source file (use echo for testing)
# $(1): asm source file name
define asm_rule
$$(call asm2o,$(1)): $(1)
	@printf " \e[32;1mCompiling\e[0m $$<\n"
	@$$(AS) $$(ASFLAGS) -i $$(arch_dir)/ -MD $$(patsubst %.o,%.d, $$@) $$< -o $$@
endef

# Instantiate compilation rules for all asm source file.
$(foreach s,$(asm_src),$(eval $(call asm_rule,$(s))))

# Compilation rule for a cpp source file (use echo for testing)
# $(1): cpp source file name
define cpp_rule
$$(call cpp2o,$(1)): $(1)
	@printf " \e[32;1mCompiling\e[0m $$<\n"
	@$$(CC) -c $$< -o $$@ -MMD -MF $$(patsubst %.o,%.d, $$@) $$(CFLAGS) $$(CPPFLAGS)
endef

# Instantiate compilation rules for all cpp source file.
$(foreach s,$(cpp_src),$(eval $(call cpp_rule,$(s))))


$(output_dir):
	@printf "\t\e[32;1mMake obj folder\e[0m $<\n"
	@mkdir -p $(output_dir)

-include $(objs:%.o=%.d)