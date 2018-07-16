import matplotlib.pyplot as plt

samples = []
W2_1 = []
W2_2 = []
W2_3 = []
W2_4 = []
W2_5 = []

data1 = open('graph_data5.txt', 'r')
data2 = open('graph_data11.txt', 'r')
data3 = open('graph_data17.txt', 'r')
data4 = open('graph_data23.txt', 'r')
data5 = open('graph_data31.txt', 'r')

for line in data1:
	line = line.strip()
	s, t1, w1, x1, nq1, t2, w2, nq2, edelta, vdelta = line.split(',')
	samples.append(int(s))
	W2_1.append(float(w2))

for line in data2:
	line = line.strip()
	s, t1, w1, x1, nq1, t2, w2, nq2, edelta, vdelta = line.split(',')
	W2_2.append(float(w2))

for line in data3:
	line = line.strip()
	s, t1, w1, x1, nq1, t2, w2, nq2, edelta, vdelta = line.split(',')
	W2_3.append(float(w2))

for line in data4:
	line = line.strip()
	s, t1, w1, x1, nq1, t2, w2, nq2, edelta, vdelta = line.split(',')
	W2_4.append(float(w2))

for line in data5:
	line = line.strip()
	s, t1, w1, x1, nq1, t2, w2, nq2, edelta, vdelta = line.split(',')
	W2_5.append(float(w2))

data1.close()
data2.close()
data3.close()
data4.close()
data5.close()
plt.plot(samples, W2_1)
plt.plot(samples, W2_2)
plt.plot(samples, W2_3)
plt.plot(samples, W2_4)
plt.plot(samples, W2_5)

plt.legend(['Seed = 5', 'Seed = 11', 'Seed = 17', 'Seed = 23', 'Seed = 31'], loc='upper left')

plt.title('E[W2]')
plt.xlabel('Samples')

plt.show()