# ML-KEM Project Makefile
PROJECT_C_SOURCES = test.c kem.c indcpa.c poly.c polyvec.c ntt.c reduce.c cbd.c verify.c symmetric-shake.c fips202.c
PROJECT_ASM_SOURCES = 
PROJECT_C_OBJS = $(addprefix obj/,$(PROJECT_C_SOURCES:.c=.c.o))
PROJECT_ASM_OBJS = $(addprefix obj/,$(PROJECT_ASM_SOURCES:.S=.S.o))
PROJECT_OBJS = $(PROJECT_C_OBJS) $(PROJECT_ASM_OBJS)

include ../common/common.mk