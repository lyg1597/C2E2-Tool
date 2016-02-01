import sys

filename = sys.argv[-1]
datalist = []
filedata=open('../wd/ReachSetProperty1','r')
for line in filedata:
	if line.rstrip():
  		dataLine=line.rstrip().split()
  	datalist.append(dataLine)
comparelist = []
comparedata = open("../goldenout/"+filename,'r')
for line in comparedata:
	if line.rstrip():
		dataLine=line.rstrip().split()
	comparelist.append(dataLine)
error = 0
if len(datalist)!=len(comparelist):
	print("Test Faild, The result file size does not equal to goldenout file size")
	error = 1
variablenum = len(datalist[1])
#print(variablenum)
#print(datalist[0][0])
for i in range (len(datalist)):
	if datalist[i][0] == '%':
		continue
	else:
		if datalist[i][0] != comparelist[i][0]:
			print("The result doesn't match goldenout, test failed")
			error = 1
			break
		else:
			for j in range (variablenum-1):
				#print(datalist[i][j+1])
				comparevalue = float(datalist[i][1+j])-float(comparelist[i][1+j])
				if comparevalue <0:
					comparevalue = -comparevalue
				if comparevalue>0.01:
					print("The result doesn't match goldenout,test failed")
					error =1
					break
if error ==0:
	print("Test Passed! All result matches goldenout")

