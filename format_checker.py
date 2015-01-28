import sys
import itertools
def to_list(s,vocab,V):
	p= list(s)
	x=[]
	for char in p:
		if (char=="-"):
			x.append(V)
		else:
			x.append(vocab[char])
	return x
def calc(s1,s2,vocab,MC,V):
	cost=0
	x1 = to_list(s1,vocab,V)
	x2= to_list(s2,vocab,V)
	for i in range(0,len(x1)):
		cost+=MC[x1[i]][x2[i]]
	return cost

def check(file1,file2):
	f1 = open(file1)
	f2=open(file2)
	initial = f1.readline()
	while (bool(initial) &(len(initial)>1)) :		
		V = int(f1.readline())
		vocab ={}
		voc_arr = f1.readline()[0:-1].split(", ")
		for i in range(0,len(voc_arr)):
			vocab[voc_arr[i]] =i
		all_list =[]
		K = int(f1.readline())
		for i in range(0,K):
			all_list.append(f1.readline().split()[0])
		CC = int(f1.readline())
		MC = [[0 for x in range(V+1)] for x in range(V+1)] 
		for i in range(0,V+1):
			p = f1.readline().split()
			for j in range(0,len(p)):
				MC[i][j] = int(p[j])
		all_list2=[]
		for i in range(0,K):
			s =f2.readline().split()[0]
			if (i==0):
				length = len(s)
			else:
				if (len(s) != length ):
					return False
			all_list2.append(s)
		f1.readline()
		initial = f1.readline()
		cost = 0
		for (s1,s2) in zip(all_list,all_list2):
			p1 = list(s1)
			p2=list(s2)
			i=0
			j=0
			try:
				while (j<len(p2)):
					if (p2[j] == "-"):
						j=j+1
						cost = cost +CC
					else:
						if (p2[j] == p1[i]):
							i=i+1
							j=j+1
						else:
							return "False"
			except IndexError:
				return False
			if (i!= len(p1)):
				return False
		for (s1,s2) in itertools.combinations(all_list2,2):
			cost += calc(s1,s2,vocab,MC,V)
		print cost
	f1.close()
	f2.close()
	return True
if __name__ == '__main__':
	print check(sys.argv[1],sys.argv[2])