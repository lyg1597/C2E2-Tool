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
              timeStep,timeHoriz,plotStatus,title,filename,xindex,yindexlist,plotterversion):

  # print (reachSetPath)
  # print (unsafeSet)
  # print (varList)
  # print (modeList)
  # print (varPlotTuple)
  # print (timeStep)
  # print (timeHoriz)
  # print (plotStatus)
  # print (title)
  # print (filename)
  # print (xindex)
  # print (yindexlist)
  if plotterversion == 1:
    if len(varPlotTuple)>2:
      return plotMultipleVars(threadEvent,reachSetPath,varList,modeList,varPlotTuple,dispMode,timeStep,timeHoriz,
                              plotStatus,title,filename,xindex,yindexlist)
    else:
      return plotMultipleModes(threadEvent,reachSetPath,unsafeSet,varList,modeList,varPlotTuple,dispMode,
                               timeStep,timeHoriz,plotStatus,title,filename,xindex,yindexlist)
  if plotterversion == 2:
    if len(varPlotTuple)>2:
      return plotMultipleVarsV2(threadEvent,reachSetPath,varList,modeList,varPlotTuple,dispMode,timeStep,timeHoriz,
                              plotStatus,title,filename,xindex,yindexlist)

    else:
      return plotMultipleModesV2(threadEvent,reachSetPath,unsafeSet,varList,modeList,varPlotTuple,dispMode,
                               timeStep,timeHoriz,plotStatus,title,filename,xindex,yindexlist)

  if plotterversion == 3:
    if len(varPlotTuple)>2:
      return plotMultipleVarsV2(threadEvent,reachSetPath,varList,modeList,varPlotTuple,dispMode,timeStep,timeHoriz,
                              plotStatus,title,filename,xindex,yindexlist)
    else:
      return plotMultipleModesV3(threadEvent,reachSetPath,unsafeSet,varList,modeList,varPlotTuple,dispMode,
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

  if unsafeflag==1:
    for i in range (len(modelist)):
      for j in range (int(numberofstep)):
        if (unsafelowerbound[i][0][j])!=float("Inf"):
          if float(unsafelowerbound[i][varPlotTuple[0]][j])<float(xmin):
            xmin = unsafelowerbound[i][varPlotTuple[0]][j]
          if float(unsafelowerbound[i][varPlotTuple[1]][j])<float(ymin):
            ymin = unsafelowerbound[i][varPlotTuple[1]][j]
          if float(unsafeupperbound[i][varPlotTuple[0]][j+1])>float(xmax):
            xmax = unsafeupperbound[i][varPlotTuple[0]][j+1]
          if float(unsafeupperbound[i][varPlotTuple[1]][j+1])>float(ymax):
            ymax = unsafeupperbound[i][varPlotTuple[1]][j+1]

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

def plotMultipleModesV2(threadEvent,reachSetPath,unsafeset,varlist,modelist,varPlotTuple,dispMode,
      timeStep,timeHoriz,plotStatus,title,filename,xindex,yindexlist):
  colors=('blue','green','red','yellow','brown','orange','cyan','pink','magenta')
  plotStatus.set_label("Status: Reading Data")
  reachData=open(reachSetPath,'r')
 
  upperbound = [[[]for i in xrange(len(varlist))] for j in xrange(len(modelist))]
  lowerbound = [[[]for i in xrange(len(varlist))] for j in xrange(len(modelist))]

  unsafeupperbound = [[[]for i in xrange(len(varlist))] for j in xrange(len(modelist))]
  unsafelowerbound = [[[]for i in xrange(len(varlist))] for j in xrange(len(modelist))]

  mode=0
  dataLen=len(varlist)
  linecounter=0

  #check if this excution is unsafe
  unsafeflag = 0
  
  xmin = float("Inf")
  ymin = float("Inf")
  xmax = -float("Inf")
  ymax = -float("Inf")
  
  print lowerbound
 
  for line in reachData:
    if line.rstrip():
      dataLine=line.rstrip().split()
      if dataLine[0]=="%":
        mode=int(dataLine[2])-1
        linecounter=0
        if len(dataLine)==4:
          unsafeflag=1
      else:
        if linecounter%2 == 0:
          for i in xrange (dataLen):
            if unsafeflag == 1:
              unsafelowerbound[mode][i].append(dataLine[i])
            else:
              #print dataLine[0]
              lowerbound[mode][i].append(dataLine[i])
        else:
          #print"UB"
          for i in xrange (dataLen):
            if unsafeflag == 1:
              unsafeupperbound[mode][i].append(dataLine[i])
            else:
              upperbound[mode][i].append(dataLine[i])

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
    for j in range (len(lowerbound[i][0])):
      if float(lowerbound[i][varPlotTuple[0]][j])<float(xmin):
        xmin = lowerbound[i][varPlotTuple[0]][j]
      if float(lowerbound[i][varPlotTuple[1]][j])<float(ymin):
        ymin = lowerbound[i][varPlotTuple[1]][j]
      if float(upperbound[i][varPlotTuple[0]][j])>float(xmax):
        xmax = upperbound[i][varPlotTuple[0]][j]
      if float(upperbound[i][varPlotTuple[1]][j])>float(ymax):
        ymax = upperbound[i][varPlotTuple[1]][j]

  if unsafeflag == 1:
    for i in range (len(modelist)):
      for j in range (len(unsafelowerbound[i][0])):

        if float(unsafelowerbound[i][varPlotTuple[0]][j])<float(xmin):
          xmin = unsafelowerbound[i][varPlotTuple[0]][j]
        if float(unsafelowerbound[i][varPlotTuple[1]][j])<float(ymin):
          ymin = unsafelowerbound[i][varPlotTuple[1]][j]
        if float(unsafeupperbound[i][varPlotTuple[0]][j])>float(xmax):
          xmax = unsafeupperbound[i][varPlotTuple[0]][j]
        if float(unsafeupperbound[i][varPlotTuple[1]][j])>float(ymax):
          ymax = unsafeupperbound[i][varPlotTuple[1]][j]    
              
  Min_display_range = (float(ymax)-float(ymin))/200

  for i in range(len(modelist)):
    for j in range (len(lowerbound[i][0])):
      plotstring+= 'set object '
      plotstring+= str(blocknum)
      plotstring+=' rect from '
      if (float(upperbound[i][varPlotTuple[1]][j]) - float(lowerbound[i][varPlotTuple[1]][j]))>Min_display_range:
        plotstring+=lowerbound[i][varPlotTuple[0]][j]+','+lowerbound[i][varPlotTuple[1]][j]+' to '+upperbound[i][varPlotTuple[0]][j]+','+upperbound[i][varPlotTuple[1]][j]+' fc rgb '
      else:
        plotstring+=lowerbound[i][varPlotTuple[0]][j]+','+lowerbound[i][varPlotTuple[1]][j]+' to '+upperbound[i][varPlotTuple[0]][j]+','+str(float(lowerbound[i][varPlotTuple[1]][j])+Min_display_range)+' fc rgb '
      plotstring+='"'
      plotstring+=colors[i]
      plotstring+='"'
      plotstring+="\n"
      blocknum+=1
  for i in range(len(modelist)):
    for j in range (len(unsafelowerbound[i][0])):
      plotstring+= 'set object '
      plotstring+= str(blocknum)
      plotstring+=' rect from '
      plotstring+=unsafelowerbound[i][varPlotTuple[0]][j]+','+unsafelowerbound[i][varPlotTuple[1]][j]+' to '+unsafeupperbound[i][varPlotTuple[0]][j]+','+unsafeupperbound[i][varPlotTuple[1]][j]+' fc rgb "black"\n'
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

def plotMultipleVarsV2(threadEvent,reachSetPath,varlist,modelist,varPlotTuple,dispMode,timeStep,timeHoriz,plotStatus,title,filename,xindex,yindexlist):
  colors=('blue','green','red','yellow','black','brown','orange','cyan','pink','magenta')
  plotStatus.set_label("Status: Reading Data")
  numberofstep = timeHoriz/timeStep

  #opne the file, and create 3D list to hold upper and lower bound data
  #the list looks like upperbound[mode][var][time_step]
  reachData=open(reachSetPath,'r')
  upperbound = [[[]for i in xrange(len(varlist))] for j in xrange(len(modelist))]
  lowerbound = [[[]for i in xrange(len(varlist))] for j in xrange(len(modelist))]
 
  mode=0
  dataLen=len(varlist)
  linecounter=0
  xmin = float("Inf")
  ymin = float("Inf")
  xmax = -float("Inf")
  ymax = -float("Inf")
  

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
          for i in xrange(dataLen):
            lowerbound[mode][i].append(dataLine[i])
        else:
          for i in xrange(dataLen):
           upperbound[mode][i].append(dataLine[i])

        linecounter+=1

  reachData.close()


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
    for k in range (len(varPlotTuple)-1):
      for j in range (len(lowerbound[i][k+1])):
        if float(lowerbound[i][varPlotTuple[0]][j])<float(xmin):
          xmin=float(lowerbound[i][varPlotTuple[0]][j])
        if float(lowerbound[i][varPlotTuple[k+1]][j])<float(ymin):
          ymin=float(lowerbound[i][varPlotTuple[k+1]][j])
        if float(upperbound[i][varPlotTuple[0]][j])>float(xmax):
          xmax=float(upperbound[i][varPlotTuple[0]][j])
        if float(upperbound[i][varPlotTuple[k+1]][j])>float(ymax):
          ymax=float(upperbound[i][varPlotTuple[k+1]][j])

  Min_display_range = (float(ymax)-float(ymin))/200

  # store the upper and lower bound to the plot configuration file
  for i in range(len(modelist)):
    for k in range (len(varPlotTuple)-1):
      for j in range (len(lowerbound[i][k+1])):
        plotstring+= 'set object '
        plotstring+= str(blocknum)
        plotstring+=' rect from '
        if (float(upperbound[i][varPlotTuple[k+1]][j])-float(lowerbound[i][varPlotTuple[k+1]][j])) > Min_display_range:
          plotstring+=lowerbound[i][varPlotTuple[0]][j]+','+lowerbound[i][varPlotTuple[k+1]][j]+' to '+upperbound[i][varPlotTuple[0]][j]+','+upperbound[i][varPlotTuple[k+1]][j]+' fc rgb '
        else:
          plotstring+=lowerbound[i][varPlotTuple[0]][j]+','+lowerbound[i][varPlotTuple[k+1]][j]+' to '+upperbound[i][varPlotTuple[0]][j]+','+str(float(lowerbound[i][varPlotTuple[k+1]][j])+Min_display_range)+' fc rgb '

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


def plotMultipleModesV3(threadEvent,reachSetPath,unsafeset,varlist,modelist,varPlotTuple,dispMode,
      timeStep,timeHoriz,plotStatus,title,filename,xindex,yindexlist):
  colors=('blue','green','red','yellow','brown','orange','cyan','pink','magenta')
  reachData=open(reachSetPath,'r')
 
  listofsafetube=[]
  listofunsafetube = []
  tubeorder = []

  mode=0
  dataLen=len(varlist)
  linecounter=0

  #check if this excution is unsafe
  unsafeflag = 0
  
  xmin = float("Inf")
  ymin = float("Inf")
  xmax = -float("Inf")
  ymax = -float("Inf")
  
  #print lowerbound
  trace = []

  for line in reachData:
    if line.rstrip():
      dataLine=line.rstrip().split()
      if dataLine[0]=="%":
        if len(trace)>0 and unsafeflag==1:
          listofunsafetube.append(trace)
        elif len(trace)>0:
          listofsafetube.append(trace)
        trace = []
        mode=int(dataLine[2])-1
        tubeorder.append(mode)
        linecounter=0
        if len(dataLine)==4:
          unsafeflag=1
          del tubeorder[-1]
      else:
        if linecounter%2 == 0:
          point = []
          for i in xrange (dataLen):
            point.append(dataLine[i])
          trace.append(point)
        linecounter+=1
  if unsafeflag==1:
    listofunsafetube.append(trace)
  else:
    listofsafetube.append(trace)
  
  #print tubeorder
  reachData.close()

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
  
  for i in range(len(listofsafetube)):
    for j in range (len(listofsafetube[i])):
      if float(listofsafetube[i][j][varPlotTuple[0]])<float(xmin):
        xmin = listofsafetube[i][j][varPlotTuple[0]]
      if float(listofsafetube[i][j][varPlotTuple[1]])<float(ymin):
        ymin = listofsafetube[i][j][varPlotTuple[1]]
      if float(listofsafetube[i][j][varPlotTuple[0]])>float(xmax):
        xmax = listofsafetube[i][j][varPlotTuple[0]]
      if float(listofsafetube[i][j][varPlotTuple[1]])>float(ymax):
        ymax = listofsafetube[i][j][varPlotTuple[1]]
 
  for i in range(len(listofunsafetube)):
    for j in range (len(listofunsafetube[i])):
      if float(listofunsafetube[i][j][varPlotTuple[0]])<float(xmin):
        xmin = listofunsafetube[i][j][varPlotTuple[0]]
      if float(listofunsafetube[i][j][varPlotTuple[1]])<float(ymin):
        ymin = listofunsafetube[i][j][varPlotTuple[1]]
      if float(listofunsafetube[i][j][varPlotTuple[0]])>float(xmax):
        xmax = listofunsafetube[i][j][varPlotTuple[0]]
      if float(listofunsafetube[i][j][varPlotTuple[1]])>float(ymax):
        ymax = listofunsafetube[i][j][varPlotTuple[1]] 

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
  
  for i in range (len(tubeorder)):
    if i == 0:
      plotstring+= 'plot "-" u '
    else:
      plotstring+=' "" u '

    plotstring+=str(varPlotTuple[0]+1)
    plotstring+=':'
    plotstring+=str(varPlotTuple[1]+1)

    plotstring+=" notitle smooth bezier lc rgb "
    plotstring+='"'
    plotstring+=colors[tubeorder[i]]
    plotstring+='"'

    if i!=len(tubeorder)-1:
      plotstring+=','

  if len(listofunsafetube)==0:
    plotstring+='\n'
  else:
    plotstring+=',"" u '
    plotstring+=str(varPlotTuple[0]+1)
    plotstring+=':'
    plotstring+=str(varPlotTuple[1]+1)
    plotstring+=' smooth bezier lc rgb "black"\n'

  for i in range(len(listofsafetube)):
    for j in range (len(listofsafetube[i])):
      for k in range (len(varlist)):
        plotstring+=str(listofsafetube[i][j][k])
        plotstring+=" "

      plotstring+='\n'

    plotstring+='e'
    plotstring+='\n'
  
  for i in range(len(listofunsafetube)):
    for j in range (len(listofunsafetube[i])):
      for k in range (len(varlist)):
        plotstring+=str(listofunsafetube[i][j][k])
        plotstring+=" "

      plotstring+='\n'
    plotstring+='e\n'

  saveFile = open('form1.gp','w')
  saveFile.write(plotstring)
  saveFile.close()

  g('load "form1.gp"')
  return filename