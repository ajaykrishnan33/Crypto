encrypt:
	gcc encrypt.c sbox_gen.c key_expansion.c -o encrypt

decrypt:
	gcc decrypt.c sbox_gen.c key_expansion.c -o decrypt
