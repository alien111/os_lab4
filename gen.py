import random

f = open('input.txt', 'w')

f.write('50\n')

for i in range(50):
	a = str(random.randint(-10000, 10000))
	f.write(a)
	f.write(' ')

f.close()