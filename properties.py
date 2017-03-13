
from test import *
import numpy as np


def mask(m, x):
	mbin = bin(m)[2:].zfill(8)
	xbin = bin(x)[2:].zfill(8)

	val = 0

	for i in range(8):
		val = val ^ (int(mbin[i])*int(xbin[i]))

	return val


def linear_approximation_table(sbox):
	lat = [ [ 0 for j in range(256) ] for i in range(256) ]

	for i in range(256):
		for j in range(256):
			for x in range(256):
				if mask(i, x) ^ mask(j, sbox[x])==0:
					lat[i][j] += 1
			lat[i][j] -= 128

	return lat

def get_all_lcs(arr):
	res = []
	for i in range(256):
		x = bin(i)[2:].zfill(8)
		y = [int(c) for c in x]
		val = 0
		for j in range(8):
			val = val ^ (y[j]*arr[j])
		res.append(1-2*val)
	return res

def generate_fhat(sbox):
	arr = []
	for x in sbox:
		y = bin(x)[2:].zfill(8)
		z = [int(c) for c in y]
		arr.append(get_all_lcs(z))
	return np.matrix(arr)

def generate_walsh_hadamard(n):
	if n==0:
		return np.matrix([1])

	h_prev = generate_walsh_hadamard(n-1)
	temp1 = np.vstack((h_prev, h_prev))
	temp2 = np.vstack((h_prev, -h_prev))
	h_curr = np.hstack((temp1, temp2))

	return h_curr

def non_linearity(sbox):
	arr = []
	for x in sbox:
		y = bin(x)[2:].zfill(8)
		z = [1-2*int(c) for c in y]
		arr.append(z)

	whm = generate_walsh_hadamard(8)
	min_diff = 1000
	for i in range(8):				# boolean fns
		for j in range(256):		# j rows of the whm
			diff = 0
			for k in range(256):	# k elements in each row
				if arr[k][i]!=whm[j,k]:	# each row of whm represents a boolean func
					diff += 1
			if diff<min_diff:
				min_diff = diff
	return min_diff


def lat_optimized(sbox):
	H = generate_walsh_hadamard(8)
	F = generate_fhat(sbox)
	lat = H*F
	lat = lat/2
	return lat

def compute():
	inp = read_aes()
	sbox = init(inp)
	lat = linear_approximation_table(sbox)
	for i in range(256):
		for j in range(256):
			print lat[i][j] + " ",
		print ""

def evaluate_probs(probs):
	return np.square(probs).sum()

def make_results(aes_probs, avg_probs):
	aes_err = evaluate_probs(aes_probs)
	avg_err = evaluate_probs(avg_probs)
	print "AES error:" + str(aes_err)
	print "Avg.error:" + str(avg_err)

	aes_maxbias = np.absolute(aes_probs).max()
	avg_maxbias = np.absolute(avg_probs).max()
	print "AES max bias:" + str(aes_maxbias)
	print "Avg max bias:" + str(avg_maxbias)


def do_ntimes(n):
	inp = read_aes()
	avg_lat = None
	for i in range(n):
		sbox = init(inp)
		lat = lat_optimized(sbox)
		if avg_lat is None:
			avg_lat = lat
		else:
			avg_lat += lat
	avg_lat = avg_lat/(n*1.0)
	avg_probs = avg_lat/256.0

	aes_sbox = np.array(np.matrix(inp).reshape((256,)))[0]
	aes_lat = lat_optimized(aes_sbox)
	aes_probs = aes_lat/256.0

	make_results(aes_probs, avg_probs)

def balancedness(sbox):
	pass
