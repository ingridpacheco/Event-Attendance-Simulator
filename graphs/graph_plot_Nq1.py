import matplotlib.pyplot as plt

samples = []
Nq1_1 = []
Nq1_2 = []
Nq1_3 = []
Nq1_4 = []
Nq1_5 = []

data1 = open('graph_data5.txt', 'r')
data2 = open('graph_data11.txt', 'r')
data3 = open('graph_data17.txt', 'r')
data4 = open('graph_data23.txt', 'r')
data5 = open('graph_data31.txt', 'r')

for line in data1:
	line = line.strip()
	s, t1, w1, x1, nq1, t2, w2, nq2, edelta, vdelta = line.split(',')
	samples.append(int(s))
	Nq1_1.append(float(nq1))

for line in data2:
	line = line.strip()
	s, t1, w1, x1, nq1, t2, w2, nq2, edelta, vdelta = line.split(',')
	Nq1_2.append(float(nq1))

for line in data3:
	line = line.strip()
	s, t1, w1, x1, nq1, t2, w2, nq2, edelta, vdelta = line.split(',')
	Nq1_3.append(float(nq1))

for line in data4:
	line = line.strip()
	s, t1, w1, x1, nq1, t2, w2, nq2, edelta, vdelta = line.split(',')
	Nq1_4.append(float(nq1))

for line in data5:
	line = line.strip()
	s, t1, w1, x1, nq1, t2, w2, nq2, edelta, vdelta = line.split(',')
	Nq1_5.append(float(nq1))

data1.close()
data2.close()
data3.close()
data4.close()
data5.close()
plt.plot(samples, Nq1_1)
plt.plot(samples, Nq1_2)
plt.plot(samples, Nq1_3)
plt.plot(samples, Nq1_4)
plt.plot(samples, Nq1_5)

plt.legend(['Seed = 5', 'Seed = 11', 'Seed = 17', 'Seed = 23', 'Seed = 31'], loc='upper left')

plt.title('E[Nq1]')
plt.xlabel('Samples')

plt.show()