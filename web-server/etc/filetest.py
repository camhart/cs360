import sys

with open("largefile1.txt") as f1:
	with open("largefile2.txt") as f2:
		while True:
			l1 = f1.readline()
			l2 = f2.readline()
			if not(l1 or l2):
				break
			if l1 == l2:
				continue
			print "-----------"
			print l1
			print l2
			sys.exit(0)
sys.exit(0)