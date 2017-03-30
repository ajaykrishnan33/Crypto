import numpy as np
import random
# key is an array of length l = 16, where each element is a single byte integer [0 - 255]
# sbox is the initial s-box represented as a vector of length 256
def algo1(sbox, key):
    sboxm = [x for x in sbox]
    j = sum(key)%256
    l = 16
    for i in range(256):
        k = (sboxm[i]+sboxm[j])%l
        j = (j+key[k])%256
        temp = sboxm[i]
        sboxm[i] = sboxm[j]
        sboxm[j] = temp

    invsboxm = [0 for x in sboxm]
    for i in range(256):
        invsboxm[sboxm[i]] = i
    return (sboxm, invsboxm)

def split_bytes(bytes):
    new_bytes = []
    for x in bytes:
        f = hex(x)[2:]
        if len(f)==1:
            x1 = 0
            x2 = int(f[0],16)
        elif len(f)==2:
            x1 = int(f[0],16)
            x2 = int(f[1],16)
        new_bytes.append(x1)
        new_bytes.append(x2)
    return new_bytes

# key is an array of length l = 16, where each element is a single byte integer [0 - 255]
# sbox is the initial s-box represented as a 16x16 matrix
def algo2(sbox, key):
    left_key = key[0:8]
    right_key = key[8:]

    row_shifter = np.array(split_bytes(left_key))
    col_shifter = np.array(split_bytes(right_key))

    sbox = np.array(sbox)

    # shift rows
    for i in range(16):
        row = sbox[i,:]
        sbox[i,:] = np.concatenate((row[row_shifter[i]:], row[:row_shifter[[i]]]))

    for i in range(16):
        col = sbox[:,i]
        sbox[:,i] = np.concatenate((col[col_shifter[i]:], col[:col_shifter[[i]]]))

    sboxm = sbox.flatten()
    invsboxm = [0 for x in sboxm]
    for i in range(256):
        invsboxm[sboxm[i]] = i
    return (sboxm, invsboxm)

# key is an array of length l = 16, where each element is a single byte integer [0 - 255]
# sbox is the initial s-box represented as a 16x16 matrix
def algo3(sbox, key):
    sbox1, _ = algo1(list(np.array(sbox).flatten()), key)
    sbox1 = np.array(sbox1).reshape(16,16)
    sboxm, invsboxm = algo2(sbox1, key)
    return (sboxm, invsboxm)

def generate_random_key():
    key = [random.randint(0,255) for i in range(16)]
    return key

def read_aes():
	with open("aes.txt") as f:
		sbox = [[int(x,16) for x in z[:-2].split(' ')] for z in f.readlines()]
	return sbox

def init(sbox):  
    key = generate_random_key()
    # sboxm, _ = algo1(list(np.array(sbox).flatten()), key)
    # sboxm, _ = algo2(sbox, key)
    sboxm, _ = algo3(sbox, key)
    return sboxm

Differentials_table=[[0 for x in range(pow(2,8))]for y in range(pow(2,8))];

def difference_generator(SBox,difference,ip):
	for i in range(pow(2,ip)):
			x1=i;
			x2=i^difference;
			y1=SBox[x1];
			y2=SBox[x2];
			l=y1^y2;
			Differentials_table[difference][l]+=1;


def generate_dat(inp):
	# Differentials_table=[[0 for x in range(pow(2,4))]for y in range(pow(2,4))];
	SBox=init(inp);
	print SBox;
	for i in range(256):
		difference_generator(SBox,i,8);
	return Differentials_table;
final_DDT_table=[[0 for x in range(pow(2,8))]for y in range(pow(2,8))];

def mean_dat():
	for k in range(1000):
		inp = read_aes()
    	table=generate_dat(inp);
    	for i in range(256):
    		for j in range(256):
    			final_DDT_table[i][j]=final_DDT_table[i][j]+table[i][j];
	for i in range(256):
		for j in range(256):
			final_DDT_table[i][j]/=1000.0
	print final_DDT_table;


