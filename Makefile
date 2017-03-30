all:
	gcc encrypt.c sbox_gen.c key_expansion.c -o encrypt