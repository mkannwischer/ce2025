# ML-DSA Project Makefile  
PROJECT_C_SOURCES = test.c sign.c packing.c polyvec.c poly.c ntt.c reduce.c rounding.c symmetric-shake.c fips202.c
PROJECT_ASM_SOURCES = 
PROJECT_C_OBJS = $(addprefix obj/,$(PROJECT_C_SOURCES:.c=.c.o))
PROJECT_ASM_OBJS = $(addprefix obj/,$(PROJECT_ASM_SOURCES:.S=.S.o))
PROJECT_OBJS = $(PROJECT_C_OBJS) $(PROJECT_ASM_OBJS)

include ../common/common.mk