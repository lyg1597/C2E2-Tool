import pylab as py
from matplotlib.collections import PatchCollection
import numpy as np
import time 
from math import floor as floor

varNum=3
varPlotTuple=(1,2)

fig,ax=py.subplots()
fig.set_size_inches(1,1)
grid=np.array([[0 for i in xrange(0,100)] for j in xrange(0,100)])


start=time.clock()
#f=open("./VanDerPolPlot.txt")
f=open("./Brussellator.txt")
x=[]
y=[]
data=[[],[]]
for line in f:
  if line.rstrip():
    dataLine=line.rstrip().split()
    data[0].append(float(dataLine[varPlotTuple[0]]))
    data[1].append(float(dataLine[varPlotTuple[1]]))
end=time.clock()
print("Reading: "+str(end-start))

start=time.clock()

points=list(zip(data[0],data[1]))

xMin=min(data[0])
xMax=max(data[0])
yMin=min(data[1])
yMax=max(data[1])

partition=128

xDiff=(xMax-xMin)/partition
yDiff=(yMax-yMin)/partition
if xDiff<yDiff:
  yN=int((yMax-yMin)/xDiff)
  yDiff=(yMax-yMin)/yN
else:
  xN=int((xMax-xMin)/yDiff)
  xDiff=(xMax-xMin)/xN

rectDict={}
xPrevLowIndex,xPrevHighIndex,yPrevLowIndex,yPrevHighIndex=-1,-1,-1,-1
for p in xrange(0,len(points),2):
  xLowIndex=int((points[p][0]-xMin)/xDiff)
  yLowIndex=int((points[p][1]-yMin)/yDiff)
  xHighIndex=int((points[p+1][0]-xMin)/xDiff)
  yHighIndex=int((points[p+1][1]-yMin)/yDiff)

  if xLowIndex==xPrevLowIndex and xHighIndex==xPrevHighIndex and \
     yLowIndex==yPrevLowIndex and yHighIndex==yPrevHighIndex:
    continue
    
  xPrevLowIndex=xLowIndex
  xPrevHighIndex=xHighIndex
  yPrevLowIndex=yLowIndex
  yPrevHighIndex=yHighIndex

  for y in xrange(yLowIndex,yHighIndex+1):
    yCoorLow=yMin+y*yDiff
    yCoorHigh=yCoorLow+yDiff
    for x in xrange(xLowIndex,xHighIndex+1):
      xCoorLow=xMin+x*xDiff
      xCoorHigh=xCoorLow+xDiff
      coor=(x,y)
      if not coor in rectDict:
        rectDict[coor]=py.Polygon([(xCoorLow,yCoorLow),(xCoorLow,yCoorHigh),(xCoorHigh,yCoorHigh),(xCoorHigh,yCoorLow)],facecolor='b',edgecolor='b')

rectCol=PatchCollection(rectDict.values(),match_original=True)
ax.add_collection(rectCol)

end=time.clock()
print("Plotting: "+str(end-start))
py.axis('scaled')
py.show()
