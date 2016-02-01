import pylab as py
from matplotlib.collections import PatchCollection
import time 
from math import floor as floor

varNum=2
varPlotTuple=(0,1)

fig,ax=py.subplots()
fig.set_size_inches(1,1)

start=time.clock()
f=open("./ReachSetP1")
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

print(xMin,xMax,yMin,yMax)

partition=128

xDiff=(xMax-xMin)/partition
yDiff=(yMax-yMin)/partition
if xDiff<yDiff:
  yN=int((yMax-yMin)/xDiff)
  yDiff=(yMax-yMin)/yN
else:
  xN=int((xMax-xMin)/yDiff)
  xDiff=(xMax-xMin)/xN

yN=int((yMax-yMin)/yDiff)
xN=int((xMax-xMin)/xDiff)
rectData=[[0 for x in xrange(0,xN+1)] for y in xrange(0,yN+1)]

rectList=[]
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
    for x in xrange(xLowIndex,xHighIndex+1):
      if not rectData[y][x]:
        rectData[y][x]=1

for y in xrange(0,yN+1):
  xRight,xLeft,xPrev=-1,-1,0
  yCoorLow=yMin+y*yDiff
  yCoorHigh=yCoorLow+yDiff
  for x in xrange(0,xN+1):
    cur=rectData[y][x]
    if cur and not xPrev:
      xLeft=x
    if not cur and xPrev or x==xN and cur:
      xRight=x
    if not xLeft==-1 and not xRight==-1:
      xCoorLow=xMin+xLeft*xDiff
      xCoorHigh=xMin+xRight*xDiff
      rectList.append(py.Polygon([(xCoorLow,yCoorLow),(xCoorLow,yCoorHigh),(xCoorHigh,yCoorHigh),(xCoorHigh,yCoorLow)],facecolor='b',edgecolor='b'))
      xLeft,xRight=-1,-1
    xPrev=cur

rectCol=PatchCollection(rectList,match_original=True)
ax.add_collection(rectCol)

end=time.clock()
print("Plotting: "+str(end-start))
py.axis('scaled')
py.show()
