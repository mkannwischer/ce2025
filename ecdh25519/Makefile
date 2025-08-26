# ECDH25519 Project Makefile
# Modify the source lists below to add your own C and assembly files

# Add your C source files here:
PROJECT_C_SOURCES = test.c fe25519.c group.c smult.c
# Add your assembly source files here:
PROJECT_ASM_SOURCES = 

# Convert sources to object file paths
PROJECT_C_OBJS = $(addprefix obj/,$(PROJECT_C_SOURCES:.c=.c.o))
PROJECT_ASM_OBJS = $(addprefix obj/,$(PROJECT_ASM_SOURCES:.S=.S.o))
PROJECT_OBJS = $(PROJECT_C_OBJS) $(PROJECT_ASM_OBJS)

# Include the common build infrastructure
include ../common/common.mk