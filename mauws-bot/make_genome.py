#!/usr/bin/env python

import random

f = open("genome.dna", "w")

f.write("\n")
for x in range(50000):
	f.write("s")
	f.write(str(random.randint(0, 16 * 8 * 4 - 1)))
	f.write("o")
	f.write(str(random.randint(0, 8 * 8 * 5 - 1)))
	f.write("w")
	f.write(str(random.uniform(-0.01, 0.01)))
	f.write("\n")
f.close