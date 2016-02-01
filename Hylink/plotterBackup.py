import matplotlib
matplotlib.use("AGG")
import pylab as py
import time,subprocess 
from math import floor as floor
from matplotlib.collections import PatchCollection
from itertools import combinations

"""
  plotGraph
  Description: plots the provided data using a grid algorithm
  Inputs: reachSetPath - path to the reach set data
          unsafeSet - parsed unsafe set
          varList - list of variables of the model
          varPlotTuple - two variables selected for plotting
  Outputs: none
  Return: return the matplotlib figure created
"""
def plotGraph(reachSetPath,unsafeSet,varList,modeListLen,varPlotTuple,dispMode,partitions):
  """
  print(varPlotTuple)
  print(reachSetPath)
  fig,ax=py.subplots()
  fig.set_size_inches(1,1)

  #Have this be read from a file instead of having it be an array
  colors=('b','g','r','c','m','y')
  """

  #Note that color stuff should appear here
  #Use this to call the two seperate functions
  figure=None
  if len(varPlotTuple)>2:
    #Note that I may have to do a different thing here
    return plotMultipleVars(reachSetPath,modeListLen,varPlotTuple,)
  else:
    return plotMultipleModes(reachSetPath,unsafeSet,varList,modeListLen,varPlotTuple)

def plotMultipleVars(reachSetPath,modeListLen,varPlotTuple):
  fig,ax=py.subplots()
  fig.set_size_inches(1,1)

  #Have this be read from a file instead of having it be an array
  colors=('b','g','r','c','m','y')

  axes=[ax]+[ax.twinx() for x in xrange(len(varPlotTuple)-2)]
  print(axes)
  extraAxes=axes[1:]
  for idx,axis in enumerate(extraAxes):
    axis.spines["right"].set_position(("axes",1+idx*.1))
    axis.set_frame_on(True)
    axis.patch.set_visible(False)
  fig.subplots_adjust(left=.075,right=1-len(extraAxes)*.075)

  #OPTIMIZATION STUFF:
  #Try setting the data.append stuff to be a variable

  start=time.clock()

  reachData=open(reachSetPath,'r')
  data=[[] for i in xrange(len(varPlotTuple)+1)]
  modeData=[[] for i in xrange(modeListLen)]
  modeTextList=[]
  plotUnsafeSet=0
  mode=0
  modeDataPrinted=0
  dataLen=len(data)-1
  for line in reachData:
    if line.rstrip():
      dataLine=line.rstrip().split()
      if dataLine[0]=="%":
        continue
      elif len(dataLine)==1:
        mode=int(dataLine[0])-1
      else:
        for i in xrange(dataLen):
          data[i].append(float(dataLine[varPlotTuple[i]]))
        modeData[mode].append(float(dataLine[varPlotTuple[0]]))
  print("Reading: "+str(time.clock()-start))

  start=time.clock()

  #Don't need to zip up the colors here
  for idx,axis in enumerate(axes):
    points=list(zip(data[0],data[idx+1]))

    xMin=min(data[0])
    xMax=max(data[0])
    yMin=min(data[idx+1])
    yMax=max(data[idx+1])

    partition=64

    xDiff=(xMax-xMin)/partition
    yDiff=(yMax-yMin)/partition
    if xDiff<yDiff:
      yN=int((yMax-yMin)/xDiff)
      yDiff=(yMax-yMin)/yN
    else:
      xN=int((xMax-xMin)/yDiff)
      xDiff=(xMax-xMin)/xN

    xN=int((xMax-xMin)/xDiff)
    yN=int((yMax-yMin)/yDiff)

    #Make this only happens once with the modeDataPrinted variable
    if not modeDataPrinted:
      comb=[]
      linesDict={}
      if varPlotTuple[0]==0:
        for i in xrange(1,modeListLen+1):
          for subset in combinations(range(modeListLen),i):
            comb.append(subset)
      comb.reverse()

      for c in comb:
        if len(c)>=2:
          intersect=set.intersection(*[set(modeData[idx]) for idx in c])
          if intersect:
            key=max(intersect)
            if key not in linesDict:
              linesDict[key]=c
        else:
          m=c[0]
          section=modeData[m]
          remainder=[]
          for n in xrange(modeListLen):
            if not m==n:
              remainder+=modeData[n]
          xVal=max(list(set(section)-set(remainder)))
          linesDict[xVal]=c

      for key,value in linesDict.items():
        text="Modes "
        for v in value:
          text+=str(v)+", "
        text=text[:-2]
        ax.axvline(x=key,ls='--',c='k')
        modeText=ax.text(x=key,y=yMin+(yMax-yMin)/2,s=text,rotation=90,ha="right",va="center")
        modeTextList.append(modeText)

      modeDataPrinted=1

    rectList=[]
    rectData=[[0 for x in xrange(0,xN+1)] for y in xrange(0,yN+1)]
    xPrevIndexLow,xPrevIndexHigh,yPrevIndexLow,yPrevIndexHigh=-1,-1,-1,-1
    for p in xrange(0,len(points),2):
      xIndexLow=int((points[p][0]-xMin)/xDiff)
      yIndexLow=int((points[p][1]-yMin)/yDiff)
      xIndexHigh=int((points[p+1][0]-xMin)/xDiff)
      yIndexHigh=int((points[p+1][1]-yMin)/yDiff)

      if xIndexLow==xPrevIndexLow and xIndexHigh==xPrevIndexHigh and \
         yIndexLow==yPrevIndexLow and yIndexHigh==yPrevIndexHigh:
        continue

      xPrevIndexLow=xIndexLow
      xPrevIndexHigh=xIndexHigh
      yPrevIndexLow=yIndexLow
      yPrevIndexHigh=yIndexHigh

      for y in xrange(yIndexLow,yIndexHigh+1):
        for x in xrange(xIndexLow,xIndexHigh+1):
          if rectData[y][x]==0:
            rectData[y][x]=1

    for y in xrange(0,yN+1):
      xLeft,xPrev=None,0
      yCoorLow=yMin+y*yDiff
      yCoorHigh=yCoorLow+yDiff
      for x in xrange(0,xN+1):
        cur=rectData[y][x]
        if not xLeft==None and (not cur==xPrev or (x==xN and cur)):
          xCoorLow=xMin+xLeft*xDiff
          xCoorHigh=xMin+x*xDiff 
          rectList.append(py.Polygon([(xCoorLow,yCoorLow),(xCoorLow,yCoorHigh),(xCoorHigh,yCoorHigh),
                          (xCoorHigh,yCoorLow)],facecolor=colors[idx],alpha=.5,linewidth=0))
          xLeft=None
        if not cur==xPrev and not cur==0 and xLeft==None:
          xLeft=x
        xPrev=cur

    if plotUnsafeSet and len(unsafeSetPoints)>2:
      rectList.append(py.Polygon(unsafeSetPoints,color='r'))

    rectCol=PatchCollection(rectList,match_original=True)
    axis.add_collection(rectCol)
    axis.set_ylim(yMin,yMax)
    axis.set_xlim(xMin,xMax)
    axis.tick_params(axis='y',colors=colors[idx])

  end=time.clock()
  print("Plotting: "+str(end-start))
  for axis in axes:
    axis.axis("off")
  for modeText in modeTextList:
    modeText.set_size(4)
  py.savefig("./../wd/temp.png")
  for idx,axis in enumerate(axes):
    axis.axis("on")
  for modeText in modeTextList:
    modeText.set_size("medium")
  return fig

def plotMultipleModes(reachSetPath,unsafeSet,varList,modeListLen,varPlotTuple):
  fig,ax=py.subplots()
  fig.set_size_inches(1,1)

  colors=('b','g','r','c','m','y')

  start=time.clock()

  reachData=open(reachSetPath,'r')
  data=[[],[]]
  modeData=[[[],[]] for m in xrange(modeListLen)]
  print(modeData)
  plotUnsafeSet=0
  mode=0
  dataLen=len(data)-1
  for line in reachData:
    if line.rstrip():
      dataLine=line.rstrip().split()
      if dataLine[0]=="%":
        continue
      elif len(dataLine)==1:
        mode=int(dataLine[0])-1
      else:
        modeData[mode][0].append(float(dataLine[varPlotTuple[0]]))
        modeData[mode][1].append(float(dataLine[varPlotTuple[1]]))
        data[0].append(float(dataLine[varPlotTuple[0]]))
        data[1].append(float(dataLine[varPlotTuple[1]]))
  end=time.clock()
  print("Reading: "+str(end-start))

  start=time.clock()

  xMin=min(data[0])
  xMax=max(data[0])
  yMin=min(data[1])
  yMax=max(data[1])

  comb=[]
  linesDict={}
  if varPlotTuple[0]==0:
    for i in xrange(1,modeListLen+1):
      for subset in combinations(range(modeListLen),i):
        comb.append(subset)
  comb.reverse()

  for c in comb:
    if len(c)>=2:
      itersect=set.intersection(*[set(modeData[idx][0]) for idx in c])
      if intersect:
        xVal=max(intersect)
        if xVal not in linesDict:
          linesDict[xVal]=c
    else:
      m=c[0]
      section=modeData[m][0]
      remainder=[]
      for n in xrange(modeListLen):
        if not m==n:
          remainder+=modeData[n][0]
      xVal=max(list(set(section)-set(remainder)))
      linesDict[xVal]=c

  for key,value in linesDict.items():
    text="Modes "
    for v in value:
      text+=str(v)+", "
    text=text[:-2]
    ax.axvline(x=key,ls='--',c='k')
    ax.text(x=key,y=yMin+(yMax-yMin)/2,s=text,rotation=90,ha="right",va="center")

  corners=[(xMin,yMin),(xMin,yMax),(xMax,yMax),(xMax,yMin)]
  print(xMin,xMax,yMin,yMax)
  generatePPL(unsafeSet,(xMin,xMax,yMin,yMax),varList,varPlotTuple)
  pointsFile=open("../wd/unsafeSetPoints.dat",'r')
  unsafeSetPoints=[]
  for line in pointsFile:
    vertex=[]
    for point in line.rsplit():
      num=point.split("/")
      vertex.append(float(num[0])/float(num[1]))
    unsafeSetPoints.append(tuple(vertex))
  unsafeSetPoints=convexHull(unsafeSetPoints)

  for p in unsafeSetPoints:
    if p in corners:
      continue
    else:
      plotUnsafeSet=1
      break

  print("UnsafeSet: "+str(time.clock()-start))

  partition=64

  xDiff=(xMax-xMin)/partition
  yDiff=(yMax-yMin)/partition
  if xDiff<yDiff:
    yN=int((yMax-yMin)/xDiff)
    yDiff=(yMax-yMin)/yN
  else:
    xN=int((xMax-xMin)/yDiff)
    xDiff=(xMax-xMin)/xN

  xN=int((xMax-xMin)/xDiff)
  yN=int((yMax-yMin)/yDiff)

  for idx,modeCol in enumerate(zip(modeData,colors)):
    if len(modeCol[0][0])==0:
      continue

    points=list(zip(modeCol[0][0],modeCol[0][1]))
    rectList=[]
    rectData=[[0 for x in xrange(0,xN+1)] for y in xrange(0,yN+1)]
    xPrevIndexLow,xPrevIndexHigh,yPrevIndexLow,yPrevIndexHigh=-1,-1,-1,-1
    for p in xrange(0,len(points),2):
      xIndexLow=int((points[p][0]-xMin)/xDiff)
      yIndexLow=int((points[p][1]-yMin)/yDiff)
      xIndexHigh=int((points[p+1][0]-xMin)/xDiff)
      yIndexHigh=int((points[p+1][1]-yMin)/yDiff)

      if xIndexLow==xPrevIndexLow and xIndexHigh==xPrevIndexHigh and \
         yIndexLow==yPrevIndexLow and yIndexHigh==yPrevIndexHigh:
        continue

      xPrevIndexLow=xIndexLow
      xPrevIndexHigh=xIndexHigh
      yPrevIndexLow=yIndexLow
      yPrevIndexHigh=yIndexHigh

      for y in xrange(yIndexLow,yIndexHigh+1):
        for x in xrange(xIndexLow,xIndexHigh+1):
          if rectData[y][x]==0:
            rectData[y][x]=idx+1

    for y in xrange(0,yN+1):
      xLeft,xPrev=None,0
      yCoorLow=yMin+y*yDiff
      yCoorHigh=yCoorLow+yDiff
      for x in xrange(0,xN+1):
        cur=rectData[y][x]
        if not xLeft==None and (not cur==xPrev or (x==xN and cur)):
          xCoorLow=xMin+xLeft*xDiff
          xCoorHigh=xMin+x*xDiff 
          rectList.append(py.Polygon([(xCoorLow,yCoorLow),(xCoorLow,yCoorHigh),(xCoorHigh,yCoorHigh),
                          (xCoorHigh,yCoorLow)],facecolor=modeCol[1][cur-1],alpha=.5,linewidth=0))
          xLeft=None
        if not cur==xPrev and not cur==0 and xLeft==None:
          xLeft=x
        xPrev=cur

    if plotUnsafeSet and len(unsafeSetPoints)>2:
      rectList.append(py.Polygon(unsafeSetPoints,color='r'))

    rectCol=PatchCollection(rectList,match_original=True)
    ax.add_collection(rectCol)

  end=time.clock()
  print("Plotting: "+str(end-start))
  ax.set_ylim(yMin,yMax)
  ax.set_xlim(xMin,xMax)
  ax.axis("off")
  py.savefig("./../wd/temp.png")
  ax.axis("on")
  return fig

"""
  generatePPL
  Description: generates a C++ file which gets the convex hull of the projection of the unsafe set
  Inputs: unsafeSet - parsed unsafe set
          bounds - the min,max values of the variables being plotted
          varList - list of variables in the mode
          varPlotTuple - two variables selected for plotting
  Outputs: none
  Return: none
"""
def generatePPL(unsafeSet,bounds,varList,varPlotTuple): 
  aMatrix,bMatrix,eqMatrix=unsafeSet
  eqTuple=(">=","<=")

  genPPL=open("../wd/reduceDimen.cpp",'w')
  genPPL.write("#include <ppl.hh>\n#include <iostream>\n#include <fstream>\n\n"+
               "using namespace std;\n\n")
  genPPL.write("void Parma_Polyhedra_Library::initialize();\n\n")
  genPPL.write("int main()\n{\n")

  genPPL.write("\tofstream filePoints;\n")
  genPPL.write("\tfilePoints.open(\"unsafeSetPoints.dat\");\n")

  remVars=[]
  for i,v in enumerate(varList):
    if not i in varPlotTuple:
      remVars.append(v)

  for i,v in enumerate(varList):
    genPPL.write("\tVariable "+v+"("+str(i)+");\n")

  genPPL.write("\n\tVariables_Set vars;\n")

  for r in remVars:
    genPPL.write("\tvars.insert("+r+");\n\n")

  genPPL.write("\tConstraint_System cs;\n")
  for i,ineq in enumerate(aMatrix):
    ineqString=""
    for j,coeff in enumerate(ineq):
      ineqString+=str(coeff)+"*"+varList[j+1]
      if not j==len(ineq)-1:
        ineqString+="+"
    ineqString+=eqMatrix[i][0]+str(bMatrix[i][0])
    genPPL.write("\tcs.insert("+ineqString+");\n")
  for i,b in enumerate(bounds):
    mult=10**checkDecimals(b)
    genPPL.write("\tcs.insert("+str(mult)+"*"+varList[varPlotTuple[i/2]]+eqTuple[i%2]+
                 str(b*mult)+");\n")

  genPPL.write("\tNNC_Polyhedron poly(cs);\n")
  genPPL.write("\tpoly.remove_space_dimensions(vars);\n\n")

  genPPL.write("\tGenerator_System gs=poly.minimized_generators();\n")
  genPPL.write("\tGenerator_System::const_iterator i;\n\n")

  genPPL.write("\tfor(i=gs.begin();i!=gs.end();++i)\n\t{\n")
  genPPL.write("\t\tif((*i).is_point())\n\t\t{\n")
  genPPL.write("\t\t\tdouble divisor=mpz_get_d((*i).divisor().get_mpz_t());\n")
  
  genPPL.write("\t\t\tint dim=int((*i).space_dimension());\n")
  genPPL.write("\t\t\tfor(int j=0;j<dim;j++)\n\t\t\t{\n")
  genPPL.write("\t\t\t\tdouble dividend=mpz_get_d((*i).coefficient(Variable(j)).get_mpz_t());\n")
  genPPL.write("\t\t\t\tfilePoints<<dividend<<\"/\"<<divisor<<\" \";\n\t\t\t}\n")
  genPPL.write("\t\t\tfilePoints<<endl;\n\t\t}\n\t}\n")

  genPPL.write("\treturn 0;\n}")

  genPPL.close()

  arguments=["g++","-o","../wd/reduceDimen","../wd/reduceDimen.cpp","-lppl"]
  subp=subprocess.Popen(arguments)
  while subp.poll()==None:
    continue
  arguments=["./reduceDimen"]
  subp=subprocess.Popen(arguments,cwd="../wd/")
  while subp.poll()==None:
    continue

"""
  checkDecimals
  Decription: checks if the inputs contains any decimals and get the number of decimals
  Inputs: value - inputs to be checked for decimals
  Outputs: none
  Return: returns the number of decimals in the inputs
"""
def checkDecimals(value):
  valueStr=str(value)
  if '.' not in valueStr:
    return 0
  else:
    return len(valueStr.split('.')[1])
  
"""
  convexHull
  Description: returns the convex hull given a set of points 
  Inputs: points - set of points from which to create the convex hull
  Ouputs: none
  Return: returns generated convex hull
"""
def convexHull(points):
  points=sorted(set(points))
  if len(points)<=1:
    return points

  def cross(o,a,b):
    return(a[0]-o[0])*(b[1]-o[1])-(a[1]-o[1])*(b[0]-o[0])

  #Building lower hull
  lower=[]
  for p in points:
    while len(lower)>=2 and cross(lower[-2],lower[-1],p)<=0:
      lower.pop()
    lower.append(p)

  #Building upper hull
  upper=[]
  for p in reversed(points):
    while len(upper)>=2 and cross(upper[-2],upper[-1],p)<=0:
      upper.pop()
    upper.append(p)

  return lower[:-1]+upper[:-1]

