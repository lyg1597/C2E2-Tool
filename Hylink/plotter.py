import matplotlib,gobject
matplotlib.use("AGG")
import pylab as py
import time,subprocess 
from math import floor as floor
from matplotlib.collections import PatchCollection
from itertools import combinations,izip_longest

import Gnuplot
from numpy import *
import pylab as py
from matplotlib.collections import PatchCollection
import numpy as np
import time
from math import floor as floor

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
#Add a thread event here that will be checked
def plotGraph(threadEvent,reachSetPath,unsafeSet,varList,modeList,varPlotTuple,dispMode,
              timeStep,timeHoriz,plotStatus,title,filename,xindex,yindexlist):
  '''
  print(reachSetPath)
  print(unsafeSet)
  print(varList)
  print(modeList)
  print(varPlotTuple)
  print(dispMode)
  print(timeStep)
  print(timeHoriz)
  print(plotStatus)
  print(title)
  '''
  
  
  if len(varPlotTuple)>2:
    return plotMultipleVars(threadEvent,reachSetPath,varList,modeList,varPlotTuple,dispMode,timeStep,timeHoriz,
                            plotStatus,title,filename,xindex,yindexlist)
  else:
    return plotMultipleModes(threadEvent,reachSetPath,unsafeSet,varList,modeList,varPlotTuple,dispMode,
                             timeStep,timeHoriz,plotStatus,title,filename,xindex,yindexlist)
  


def plotMultipleVars(threadEvent,reachSetPath,varlist,modelist,varPlotTuple,dispMode,timeStep,timeHoriz,plotStatus,title,filename,xindex,yindexlist):
  colors=('blue','green','red','yellow','black','brown','orange','cyan','pink','magenta')
  plotStatus.set_label("Status: Reading Data")
  numberofstep = timeHoriz/timeStep

  #opne the file, and create 3D list to hold upper and lower bound data
  #the list looks like upperbound[mode][var][time_step]
  reachData=open(reachSetPath,'r')
  upperbound = [[[-float("Inf") for w in xrange(int(numberofstep)+1)] for i in xrange(len(varlist))] for j in xrange(len(modelist))]
  lowerbound = [[[float("Inf") for w in xrange(int(numberofstep)+1)] for i in xrange(len(varlist))] for j in xrange(len(modelist))]
 
  mode=0
  dataLen=len(varlist)
  linecounter=0

  #store the result to upperbound or lowerbound in order
  #if there are few values for same time-step(because of multiple execution)
  #check the old value and over-write old value if needed
  for line in reachData:
    if line.rstrip():
      dataLine=line.rstrip().split()
      if dataLine[0]=="%":
        mode=int(dataLine[2])-1
        linecounter=0
      
      else:
        if linecounter%2 ==0:
          timeindex = int(float(dataLine[0])/timeStep+0.5)
          for i in xrange(dataLen):
            if i == 0:
              lowerbound[mode][0][timeindex]=dataLine[0]
            else:
              if float(dataLine[i]) < float(lowerbound[mode][i][timeindex]):
                lowerbound[mode][i][timeindex] = dataLine[i]
        else:
          timeindex = int(float(dataLine[0])/timeStep+0.5)
          #print timeindex
          for i in xrange(dataLen):
            if i == 0:
              upperbound[mode][0][timeindex]=dataLine[0]
            else:
              if float(dataLine[i]) > float(upperbound[mode][i][timeindex]):
                upperbound[mode][i][timeindex] = dataLine[i]

        linecounter+=1

  reachData.close()

  #set the plot x,y max and min variable
  xmin = float("Inf")
  ymin = float("Inf")
  xmax = -float("Inf")
  ymax = -float("Inf")


  #generate plot string, and store string into configuration file
  g=Gnuplot.Gnuplot()
  plotstring =""
  plotstring += 'set term png transparent truecolor size 600,480  enhanced font "Helvetica,15"'
  plotstring += "\n"
  plotstring += 'set output "./../wd/plotresult/'
  plotstring += filename
  plotstring += '.png"'
  plotstring += "\n"
  plotstring += "set xlabel "
  plotstring += '"'
  plotstring += xindex
  plotstring += '"'
  plotstring += "\n"
  plotstring += "set ylabel "
  plotstring += '"'
  for i in range (len(yindexlist)):
    plotstring+=yindexlist[i]
    if i < (len(yindexlist)-1):
      plotstring+=' & '
  plotstring += '"'
  plotstring += '\n'
  #print(plotstring)
  plotstring += 'set grid'
  plotstring += '\n'
  plotstring += 'set title '
  plotstring += '"'
  plotstring += title
  plotstring += '"'
  plotstring += "\n"
  plotstring += "set style rect fill  transparent solid 0.50 noborder\n"
  #plotstring+='plot '
  blocknum = 1
  for i in range(len(modelist)):
    for j in range (int(numberofstep)):
      for k in range (len(varPlotTuple)-1):
        if float(lowerbound[i][varPlotTuple[0]][j])<float(xmin):
          xmin=float(lowerbound[i][varPlotTuple[0]][j])
        if float(lowerbound[i][varPlotTuple[k+1]][j])<float(ymin):
          ymin=float(lowerbound[i][varPlotTuple[k+1]][j])
        if float(upperbound[i][varPlotTuple[0]][j+1])>float(xmax):
          xmax=float(upperbound[i][varPlotTuple[0]][j+1])
        if float(upperbound[i][varPlotTuple[k+1]][j+1])>float(ymax):
          ymax=float(upperbound[i][varPlotTuple[k+1]][j+1])

  Min_display_range = (float(ymax)-float(ymin))/200

  # store the upper and lower bound to the plot configuration file
  for i in range(len(modelist)):
    for j in range (int(numberofstep)):
      for k in range (len(varPlotTuple)-1):
        if (lowerbound[i][0][j])!=float("Inf"):
          plotstring+= 'set object '
          plotstring+= str(blocknum)
          plotstring+=' rect from '
          if (float(upperbound[i][varPlotTuple[k+1]][j+1])-float(lowerbound[i][varPlotTuple[k+1]][j])) > Min_display_range:
            plotstring+=lowerbound[i][varPlotTuple[0]][j]+','+lowerbound[i][varPlotTuple[k+1]][j]+' to '+upperbound[i][varPlotTuple[0]][j+1]+','+upperbound[i][varPlotTuple[k+1]][j+1]+' fc rgb '
          else:
            plotstring+=lowerbound[i][varPlotTuple[0]][j]+','+lowerbound[i][varPlotTuple[k+1]][j]+' to '+upperbound[i][varPlotTuple[0]][j+1]+','+str(float(lowerbound[i][varPlotTuple[k+1]][j])+Min_display_range)+' fc rgb '

          plotstring+='"'
          plotstring+=colors[k]
          plotstring+='"'
          plotstring+='\n'
          blocknum+=1

        


  #print xmin,ymin,xmax,ymax
  plotstring+= "set key center bottom Left title '"
  for i in range (len(varPlotTuple)-1):
    plotstring+= varlist[varPlotTuple[i+1]]+':'+colors[i]+' '
  #print varlist
  #print varPlotTuple
    
  plotstring+="'\n"
  #set up the uppper and lower boundary for our output image
  plotstring+="plot ["
  plotstring+=str(xmin)
  plotstring+=':'
  plotstring+=str(xmax)
  plotstring+='] ['
  plotstring+=str(ymin)
  plotstring+=':'
  plotstring+=str(ymax)
  plotstring+='] '
  plotstring+='NaN notitle'

  saveFile = open('form.gp','w')
  saveFile.write(plotstring)
  saveFile.close()
  #let gnuplot load configuration file and plot.
  g('load "form.gp"')

  plotStatus.set_label("Status: Ready")
  return filename

def plotMultipleModes(threadEvent,reachSetPath,unsafeset,varlist,modelist,varPlotTuple,dispMode,
                      timeStep,timeHoriz,plotStatus,title,filename,xindex,yindexlist):
  colors=('blue','green','red','yellow','brown','orange','cyan','pink','magenta')
  plotStatus.set_label("Status: Reading Data")

  #opne the file, and create 3D list to hold upper and lower bound data
  #the list looks like upperbound[mode][var][time_step]
  #we also create 3D list for unsafe execution for plot multiple modes
  numberofstep = timeHoriz/timeStep
  reachData=open(reachSetPath,'r')
  upperbound = [[[-float("Inf") for w in xrange(int(numberofstep)+1)] for i in xrange(len(varlist))] for j in xrange(len(modelist))]
  lowerbound = [[[float("Inf") for w in xrange(int(numberofstep)+1)] for i in xrange(len(varlist))] for j in xrange(len(modelist))]
  unsafeupperbound = [[[-float("Inf") for w in xrange(int(numberofstep)+1)] for i in xrange(len(varlist))] for j in xrange(len(modelist))]
  unsafelowerbound = [[[-float("Inf") for w in xrange(int(numberofstep)+1)] for i in xrange(len(varlist))] for j in xrange(len(modelist))]

  mode=0
  dataLen=len(varlist)
  linecounter=0

  #check if this excution is unsafe
  unsafeflag = 0
  
  xmin = float("Inf")
  ymin = float("Inf")
  xmax = -float("Inf")
  ymax = -float("Inf")
 
  #store the result to upperbound or lowerbound in order
  #if there are few values for same time-step(because of multiple execution)
  #check the old value and over-write old value if needed
  #if it is unsafe excution, just store it since there will be only one unsafe execution.
  for line in reachData:
    if line.rstrip():
      dataLine=line.rstrip().split()
      if dataLine[0]=="%":
        mode=int(dataLine[2])-1
        linecounter=0
        #print(len(dataLine))
        if len(dataLine)==4:
          print("unsafe")
          unsafeflag=1

      
      else:
        if linecounter%2 ==0:
          timeindex = int(float(dataLine[0])/timeStep+0.5)
          for i in xrange(dataLen):
            if i == 0:
              lowerbound[mode][0][timeindex]=dataLine[0]
              if unsafeflag ==1:
                #print("store unsafe lower")
                unsafelowerbound[mode][0][timeindex]=dataLine[0]
            else:
              if unsafeflag==1:
                  unsafelowerbound[mode][i][timeindex]=dataLine[i]
              if float(dataLine[i]) < float(lowerbound[mode][i][timeindex]):
                lowerbound[mode][i][timeindex] = dataLine[i]
                
        else:
          timeindex = int(float(dataLine[0])/timeStep+0.5)
          #print timeindex
          for i in xrange(dataLen):
            if i == 0:
              upperbound[mode][0][timeindex]=dataLine[0]
              if unsafeflag==1:
                unsafeupperbound[mode][0][timeindex]=dataLine[0]

            else:
              if unsafeflag==1:
                unsafeupperbound[mode][i][timeindex]=dataLine[i]
              if float(dataLine[i]) > float(upperbound[mode][i][timeindex]):
                upperbound[mode][i][timeindex] = dataLine[i]

        linecounter+=1

  reachData.close()

  #set the plot x,y max and min variable


 #set the boundary for unsafe set
  unsafe_x_bot_bound = "graph 0"
  unsafe_y_bot_bound = "graph 0"
  unsafe_x_top_bound = "graph 1"
  unsafe_y_top_bound = "graph 1"
  numofunsafevar = len(varlist)-1
  for i in range (len(unsafeset[0])):
    #continue
    pos = -1
    for j in range (numofunsafevar):
      if unsafeset[0][i][j] >0:
        pos = j
    #print pos
    dif = 1/unsafeset[0][i][pos]
    right = dif*unsafeset[1][i][0]
    #print right
    symbol = unsafeset[2][i][0]
    varnum = pos+1
    print varnum
    exist = 0
    x_or_y = -1
    for j in range (2):
      if varPlotTuple[j] == varnum:
        exist = 1
        x_or_y = j
    #print x_or_y

    if exist == 0:
      print("will not draw unsafe")
      continue
    if exist == 1:
      if symbol==">" or symbol==">=":
        if x_or_y == 0:
          unsafe_x_bot_bound = "first " + str(right)
        if x_or_y == 1:
          unsafe_y_bot_bound = "first " + str(right)
      if symbol=="<" or symbol=="<=":
        if x_or_y == 0:
          unsafe_x_top_bound = "first " + str(right)
        if x_or_y == 1:
          unsafe_y_top_bound = "first " + str(right)

  unsafestring = "set object rectangle from "+unsafe_x_bot_bound+', '+unsafe_y_bot_bound+" to "+unsafe_x_top_bound+', '+unsafe_y_top_bound+" fillcolor rgb 'pink'\n"
  
  #generate plot string, and store string into configuration file
  g=Gnuplot.Gnuplot()
  plotstring =""
  plotstring += 'set term png transparent truecolor size 600,480  enhanced font "Helvetica,15"'
  plotstring += "\n"
  plotstring += 'set output "./../wd/plotresult/'
  plotstring += filename
  plotstring += '.png"'
  plotstring += "\n"
  plotstring += "set xlabel "
  plotstring += '"'
  plotstring += xindex
  plotstring += '"'
  plotstring += "\n"
  plotstring += "set ylabel "
  plotstring += '"'
  plotstring += yindexlist[0]
  plotstring += '"'
  plotstring += '\n'
  #print(plotstring)
  plotstring += 'set grid'
  plotstring += '\n'
  plotstring += 'set title '
  plotstring += '"'
  plotstring += title
  plotstring += '"'
  plotstring += "\n"
  plotstring += "set style rect fill  transparent solid 0.50 noborder\n"
  #plotstring+='plot '
  blocknum = 1
  for i in range(len(modelist)):
    for j in range (int(numberofstep)):
      if (lowerbound[i][0][j])!=float("Inf"):
        if float(lowerbound[i][varPlotTuple[0]][j])<float(xmin):
          xmin = lowerbound[i][varPlotTuple[0]][j]
        if float(lowerbound[i][varPlotTuple[1]][j])<float(ymin):
          ymin = lowerbound[i][varPlotTuple[1]][j]
        if float(upperbound[i][varPlotTuple[0]][j+1])>float(xmax):
          xmax = upperbound[i][varPlotTuple[0]][j+1]
        if float(upperbound[i][varPlotTuple[1]][j+1])>float(ymax):
          ymax = upperbound[i][varPlotTuple[1]][j+1]
  Min_display_range = (float(ymax)-float(ymin))/200

  for i in range(len(modelist)):
    for j in range (int(numberofstep)):
      if (lowerbound[i][0][j])!=float("Inf"):
    
        plotstring+= 'set object '
        plotstring+= str(blocknum)
        plotstring+=' rect from '
        if (float(upperbound[i][varPlotTuple[1]][j+1]) - float(lowerbound[i][varPlotTuple[1]][j]))>Min_display_range:
          plotstring+=lowerbound[i][varPlotTuple[0]][j]+','+lowerbound[i][varPlotTuple[1]][j]+' to '+upperbound[i][varPlotTuple[0]][j+1]+','+upperbound[i][varPlotTuple[1]][j+1]+' fc rgb '
        else:
          plotstring+=lowerbound[i][varPlotTuple[0]][j]+','+lowerbound[i][varPlotTuple[1]][j]+' to '+upperbound[i][varPlotTuple[0]][j+1]+','+str(float(lowerbound[i][varPlotTuple[1]][j])+Min_display_range)+' fc rgb '
        plotstring+='"'
        plotstring+=colors[i]
        plotstring+='"'
        plotstring+="\n"
        blocknum+=1
  for i in range(len(modelist)):
    for j in range (int(numberofstep)):
      if (unsafelowerbound[i][0][j])!=float("-Inf"):
    
        plotstring+= 'set object '
        plotstring+= str(blocknum)
        plotstring+=' rect from '

        plotstring+=unsafelowerbound[i][varPlotTuple[0]][j]+','+unsafelowerbound[i][varPlotTuple[1]][j]+' to '+unsafeupperbound[i][varPlotTuple[0]][j+1]+','+unsafeupperbound[i][varPlotTuple[1]][j+1]+' fc rgb "black"\n'
        blocknum+=1


  if unsafe_y_bot_bound != "graph 0" or unsafe_y_bot_bound != "graph 0" or unsafe_x_top_bound != "graph 1" or unsafe_y_top_bound!="graph 1":
    plotstring+= unsafestring
    plotstring+= "set key center bottom Left title '"
    for i in range (len(modelist)):
      plotstring+= modelist[i]+':'+colors[i]+' '
    plotstring+='Unsafe:pink'
    plotstring+="' font 'Helvetica, 10'"
    plotstring+="\n"

  else:
    plotstring+= "set key center bottom Left title '"
    for i in range (len(modelist)):
      plotstring+= modelist[i]+':'+colors[i]+' '
    plotstring+="'\n"

  plotstring+="plot ["
  plotstring+=str(xmin)
  plotstring+=':'
  plotstring+=str(xmax)
  plotstring+='] ['
  plotstring+=str(ymin)
  plotstring+=':'
  plotstring+=str(ymax)
  plotstring+='] '
  plotstring+='NaN notitle'

  saveFile = open('form.gp','w')
  saveFile.write(plotstring)
  saveFile.close()
  #let gnuplot load configuration file and plot.

  g('load "form.gp"')
  plotStatus.set_label("Status: Ready")
  return filename


"""
  getDecimals
"""
def getDecimals(value):
  decimals=str(float(value)).split('.')[1]
  if decimals=='0':
    return 0
  return len(decimals)

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
def generatePPL(threadEvent,unsafeSet,bounds,varList,varPlotTuple): 
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

  arguments=["g++","-o","../wd/reduceDimen","../wd/reduceDimen.cpp","-lppl", "-lgmp"]
  subp=subprocess.Popen(arguments)
  while subp.poll()==None:
    if threadEvent.isSet():
      subp.kill()
      return
    continue
  arguments=["./reduceDimen"]
  subp=subprocess.Popen(arguments,cwd="../wd/")
  while subp.poll()==None:
    if threadEvent.isSet():
      subp.kill()
      return
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

