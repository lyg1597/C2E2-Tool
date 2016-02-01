import pygtk
pygtk.require('2.0')
import gtk,gobject,re

import logging

plotterLog2 = logging.getLogger('c2e2VerificationLog')


"""
  PlotDialog
  Description: dialog where user can select various options for the plots to be displayed
"""
class PlotDialog(gtk.Dialog):
  """
    __init__
    Description: initializes the contents of the dialog along with any data
    Inputs: parent - the application window
            butType - button that opened the dialog, add/edit
            plotList - list of the plots and associated ata
            plotListLen - length of the plot list
            varList - list of variables in the model
    Outputs: plotList
             removePlotList: list of the plots whose icon has to be removed
             editPlotList: list of plots whose data was editted and plot display windows have to be 
                           closed
    Return: none
  """
  def __init__(self,parent,butType,plotList,plotListLen,varList,removePlotList,editPlotList):
    gtk.Dialog.__init__(self)
    self.butType=butType
    self.plotList=plotList
    self.plotListLen=plotListLen
    self.varList=varList
    self.removePlotList=removePlotList
    self.editPlotList=editPlotList
    self.existingPlotNames=[]
    self.newPlotNames=[]

    if butType=="Add":
      self.plotTitle=self.butType+" Plot"
      self.plotList.append([PlotDatum(self.plotListLen)])
      self.newPlotNames.append(["",0,self.plotListLen])
      for plot in self.plotList:
        self.existingPlotNames.append(plot[0].name)
    else:
      for plot in self.plotList:
        plot=plot[0]
        if plot.checked:
          self.newPlotNames.append([plot.name,1,plot.index])
        else:
          self.existingPlotNames.append(plot.name)
      if len(self.newPlotNames)>1:
        self.plotTitle=self.butType+" Plots"
      else:
        self.plotTitle=self.butType+" Plot"

    self.set_title(self.plotTitle)
    self.set_size_request(320,280)
    self.set_transient_for(parent)
    self.set_position(gtk.WIN_POS_CENTER_ON_PARENT)

    dialogSW=gtk.ScrolledWindow()
    dialogSW.set_policy(gtk.POLICY_NEVER,gtk.POLICY_ALWAYS)
    dialogVBox=gtk.VBox(False,0)

    self.widgets={}
    for index,plot in enumerate(self.newPlotNames):
      if not index==0:
        dialogVBox.pack_start(gtk.HSeparator(),False,False,5)

      plot=self.plotList[plot[2]][0]
      vbox=gtk.VBox(False,5)

      self.widgets[index]={}
      self.widgets[index]["nameEntry"]=NameEntry()
      self.widgets[index]["plotTitle"]=gtk.Entry()
      self.widgets[index]["xAxisLabel"]=gtk.Entry()
      self.widgets[index]["yAxisLabel"]=[gtk.Entry() for label in plot.yLabelList]
      #self.widgets[index]["modeToggle"]=gtk.CheckButton()
      #self.widgets[index]["partitionEntry"]=NumberEntry()
      #self.widgets[index]["partitionValid"]=1
      self.widgets[index]["axesDiff"]=1
      self.widgets[index]["axesVars"]=[]
      self.widgets[index]["axesImageList"]=[]
      axesVars=self.widgets[index]["axesVars"]
      axesImageList=self.widgets[index]["axesImageList"]

      row=gtk.HBox(False,0)
      row.pack_start(gtk.Label("Plot name: "),False,False,0)
      row.pack_start(self.widgets[index]["nameEntry"],True,True,0)
      self.widgets[index]["nameEntry"].set_text(plot.name)
      checkImage=gtk.Image()
      row.pack_start(checkImage,False,False,5)
      if self.newPlotNames[index][1]:
        checkImage.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)
      else:
        checkImage.set_from_stock(gtk.STOCK_CANCEL,gtk.ICON_SIZE_MENU)
      self.widgets[index]["nameEntry"].connect("changed",self.checkPlotName,row.get_children()[2],
                                               index)
      vbox.pack_start(row,False,False)

      row=gtk.HBox(False,0)
      row.pack_start(gtk.Label("x-axis variable: "),False,False,0)
      xAxisCB=gtk.combo_box_new_text()
      row.pack_start(xAxisCB,True,True,0)
      for v in self.varList:
        xAxisCB.append_text(v)
      xAxisCB.set_active(plot.xVarIndex)
      entry=self.widgets[index]["xAxisLabel"]
      xAxisCB.connect("changed",self.checkAxes,0,index,entry)
      axesVars.append(plot.xVarIndex)
      if self.butType=="Add":
        plot.xLabel=self.varList[plot.xVarIndex]
      checkImage=gtk.Image()
      row.pack_start(checkImage,False,False,5)
      checkImage.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)
      axesImageList.append(checkImage)
      vbox.pack_start(row,False,False)

      for idx,var in enumerate(plot.yVarIndexList):
        row=gtk.HBox(False,0)
        row.pack_start(gtk.Label("y-axis variable: "),False,False,0)
        yAxisCB=gtk.combo_box_new_text()
        for v in self.varList:
          yAxisCB.append_text(v)
        row.pack_start(yAxisCB,True,True,0)
        yAxisCB.set_active(var)
        entry=self.widgets[index]["yAxisLabel"][idx]
        yAxisCB.connect("changed",self.checkAxes,len(axesVars),index,entry)
        axesVars.append(var)
        if self.butType=="Add":
          plot.yLabelList[idx]=self.varList[var]
        checkImage=gtk.Image()
        row.pack_start(checkImage,False,False,5)
        checkImage.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)
        axesImageList.append(checkImage)
        vbox.pack_start(row,False,False)

      row=gtk.HBox(False,0)
      addRowBtn=gtk.Button()
      if len(axesVars)<len(self.varList):
        addRowBtn.set_sensitive(True)
      else:
        addRowBtn.set_sensitive(False)
      addRowBtn.set_image(gtk.image_new_from_stock(gtk.STOCK_ADD,gtk.ICON_SIZE_MENU))
      remRowBtn=gtk.Button()
      remRowBtn.set_sensitive(False)
      if len(axesVars)<=2:
        remRowBtn.set_sensitive(False)
      else:
        remRowBtn.set_sensitive(True)
      remRowBtn.set_image(gtk.image_new_from_stock(gtk.STOCK_REMOVE,gtk.ICON_SIZE_MENU))
      addRowBtn.connect("clicked",self.addRowBtnCallback,remRowBtn,vbox,index)
      remRowBtn.connect("clicked",self.remRowBtnCallback,addRowBtn,vbox,index)
      row.pack_end(remRowBtn,False,False,5)
      row.pack_end(addRowBtn,False,False,5)
      row.pack_end(gtk.Label("Add y-axis:"),False,False,0)
      vbox.pack_start(row,False,False)

      '''
      row=gtk.HBox(False,0)
      checkImage=gtk.Image()
      checkImage.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)
      row.pack_end(checkImage,False,False,5)
      modeToggle=self.widgets[index]["modeToggle"]
      row.pack_end(modeToggle,False,False,0)
      modeToggle.set_active(plot.dispMode)
      row.pack_end(gtk.Label("Plot with modes: "),False,False,0)
      vbox.pack_start(row,False,False)

      
      row=gtk.HBox(False,0)
      checkImage=gtk.Image()
      checkImage.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)
      row.pack_end(checkImage,False,False,5)
      #partitionEntry=self.widgets[index]["partitionEntry"]
      #partitionEntry.set_text(str(plot.partitions))
      #row.pack_end(partitionEntry,False,False,0)
      #partitionEntry.connect("changed",self.checkPartition,checkImage,index)
      #row.pack_end(gtk.Label("Number of partitions: "),False,False,0)
      vbox.pack_start(row,False,False)
      '''

      row=gtk.HBox(False,0)
      row.pack_start(gtk.Label("Plot title: "),False,False,0)
      row.pack_start(self.widgets[index]["plotTitle"],True,True,0)
      self.widgets[index]["plotTitle"].set_text(plot.plotTitle)
      checkImage=gtk.Image()
      row.pack_start(checkImage,False,False,5)
      checkImage.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)
      vbox.pack_start(row,False,False)

      row=gtk.HBox(False,0)
      row.pack_start(gtk.Label("x-axis label: "),False,False,0)
      row.pack_start(self.widgets[index]["xAxisLabel"],True,True)
      self.widgets[index]["xAxisLabel"].set_text(plot.xLabel)
      checkImage=gtk.Image()
      row.pack_start(checkImage,False,False,5)
      checkImage.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)
      vbox.pack_start(row,False,False)

      for idx,label in enumerate(plot.yLabelList):
        row=gtk.HBox(False,0)
        row.pack_start(gtk.Label("y-axis label: "),False,False,0)
        row.pack_start(self.widgets[index]["yAxisLabel"][idx],True,True)
        self.widgets[index]["yAxisLabel"][idx].set_text(label)
        checkImage=gtk.Image()
        row.pack_start(checkImage,False,False,5)
        checkImage.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)
        vbox.pack_start(row,False,False)

      dialogVBox.pack_start(vbox,True,True,0)

    saveBtn=gtk.Button("Save")
    saveBtn.connect("clicked",self.saveBtnCallback)
    cancelBtn=gtk.Button("Cancel")
    cancelBtn.connect("clicked",self.cancelBtnCallback)

    dialogSW.add_with_viewport(dialogVBox)
    dialogSW.get_child().set_shadow_type(gtk.SHADOW_NONE)
    self.vbox.pack_start(dialogSW,True,True,0)
    self.action_area.pack_start(saveBtn)
    self.action_area.pack_start(cancelBtn)
    self.connect("delete-event",self.closeDialog)
    self.show_all()

  """
    closeDialog
    Description: handles the closing of the dialog by sending a cancel response
    Inputs: widget - widget to be closed
            event - event triggering the closing
    Outputs: none
    Return: none
  """
  def closeDialog(self,widget,event):
    if self.butType=="Add":
      self.plotList.remove(self.plotList.iter_nth_child(None,self.plotListLen))
    self.response(gtk.RESPONSE_CANCEL)

  #Fix this appropriately
  """
    checkAxes
    Description: checks whether the use has selected the same two axes or not
    Inputs: cb1 - the x axis combo box
            cb2 - the y axis combo box
            image1 - image next to the x axis combo box
            image2 - image nex to the y axis combo box
    Outputs: none
    Return: none
  """
  def checkAxes(self,cb,cbIndex,dicIndex,entry):
    axesVars=self.widgets[dicIndex]["axesVars"]
    axesImageList=self.widgets[dicIndex]["axesImageList"]
    axesVars[cbIndex]=cb.get_active()
    entry.set_text(self.varList[axesVars[cbIndex]])
    self.widgets[dicIndex]["axesDiff"]=1
    for idx,v in enumerate(axesVars):
      if axesVars.count(v)>1:
        axesImageList[idx].set_from_stock(gtk.STOCK_CANCEL,gtk.ICON_SIZE_MENU)
        self.widgets[dicIndex]["axesDiff"]=0
      else:
        axesImageList[idx].set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)

  """
    checkPlotName
    Description: checks that all the plots names differ from one another
    Inputs: entry - name entry currently being editted
            image - image next to the name entry
            index - the index of the plot whose name is currently being editted
    Outputs: none
    Return: none
  """
  def checkPlotName(self,entry,image,index):
    newName=entry.get_text()
    namePattern=re.compile("([_a-zA-Z][_a-zA-Z0-9]*)$")
    match=namePattern.match(newName)

    if match==None:
      self.newPlotNames[index][0]=newName
      self.newPlotNames[index][1]=0
      image.set_from_stock(gtk.STOCK_CANCEL,gtk.ICON_SIZE_MENU)

    edittedPlotNames=[name[0] for idx,name in enumerate(self.newPlotNames) if not idx==index]
    if not newName=="" and not newName in self.existingPlotNames and not newName in edittedPlotNames:
      self.newPlotNames[index][0]=newName
      self.newPlotNames[index][1]=1
      image.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)
    else:
      self.newPlotNames[index][0]=newName
      self.newPlotNames[index][1]=0
      image.set_from_stock(gtk.STOCK_CANCEL,gtk.ICON_SIZE_MENU)

  """
    checkPartition
  """
  def checkPartition(self,entry,image,index):
    if entry.get_text().isdigit():
      image.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)
      self.widgets[index]["partitionValid"]=1
    else:
      image.set_from_stock(gtk.STOCK_CANCEL,gtk.ICON_SIZE_MENU)
      self.widgets[index]["partitionValid"]=0

  #Fix this description
  """
    addRowBtnCallback
    Description: adds a new row to the dialog for new y-axis var
    Inputs: addBtn - the button to add a row
            remBtn - the button to remove a row
            vbox - the VBox into which the row should be packed into
    Outputs: none
    Returns: none
  """
  def addRowBtnCallback(self,addBtn,remBtn,vbox,index):
    row=gtk.HBox(False,0)
    row.pack_start(gtk.Label("y-axis label: "),False,False,0)
    addYLabel=gtk.Entry()
    row.pack_start(addYLabel,True,True)
    self.widgets[index]["yAxisLabel"].append(addYLabel)
    checkImage=gtk.Image()
    row.pack_start(checkImage,False,False,5)
    checkImage.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)
    row.show_all()
    vbox.pack_start(row,False,False)
    vbox.reorder_child(row,-1)

    row=gtk.HBox(False,0)
    row.pack_start(gtk.Label("y-axis variable: "),False,False,0)
    addYAxis=gtk.combo_box_new_text()
    row.pack_start(addYAxis,True,True)
    for v in self.varList:
      addYAxis.append_text(v)
    axesVars=self.widgets[index]["axesVars"]
    untakenVar=list(set([x for x in xrange(len(self.varList))])-set(axesVars))[0]
    addYAxis.set_active(untakenVar)
    addYAxis.connect("changed",self.checkAxes,len(axesVars),index,addYLabel)
    axesVars.append(untakenVar)
    addYLabel.set_text(self.varList[untakenVar])
    checkImage=gtk.Image()
    row.pack_start(checkImage,False,False,5)
    checkImage.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)
    self.widgets[index]["axesImageList"].append(checkImage)
    row.show_all()
    vbox.pack_start(row)
    vbox.reorder_child(row,len(axesVars))

    if len(axesVars)>=len(self.varList) or len(axesVars)>=5:
      addBtn.set_sensitive(False)
    remBtn.set_sensitive(True)

  #Fix this description
  """
    remRowBtnCallback
    Description: removes a row from the dialog if there are more than one y-axis var rows
    Inputs: remBtn - the button to remove a row
            addBtn - the button to add a row
            vbox = the VBox into which the row should be packed into
    Outputs: none
    Returns: none
  """
  def remRowBtnCallback(self,remBtn,addBtn,vbox,index):
    axesVars=self.widgets[index]["axesVars"]
    vbox.remove(vbox.get_children()[len(axesVars)])
    vbox.remove(vbox.get_children()[-1])
    axesVars.pop()
    self.widgets[index]["axesImageList"].pop()
    self.widgets[index]["yAxisLabel"].pop()
    if len(axesVars)<=2:
      remBtn.set_sensitive(False)
    addBtn.set_sensitive(True)
    axesImageList=self.widgets[index]["axesImageList"]
    for idx,v in enumerate(axesVars):
      if axesVars.count(v)>1:
        axesImageList[idx].set_from_stock(gtk.STOCK_CANCEL,gtk.ICON_SIZE_MENU)
        self.widgets[index]["axesDiff"]=0
      else:
        axesImageList[idx].set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)


    
  """
    saveBtnCallback
    Description: checks that all the entries are valid and saves the changes to the plot list
    Inputs: btn - button that was clicked
    Outputs: none
    Return: none
  """
  def saveBtnCallback(self,btn):
    for idx,new in enumerate(self.newPlotNames):
      if self.widgets[idx]["axesDiff"]==0 or new[1]==0:
        return

    for index,new in enumerate(self.newPlotNames):
      plot=self.plotList[new[2]][0]
      plotTitle=self.widgets[index]["plotTitle"].get_text()
      xAxisLabel=self.widgets[index]["xAxisLabel"].get_text()
      yAxisLabel=[label.get_text() for label in self.widgets[index]["yAxisLabel"]]
      xAxis=self.widgets[index]["axesVars"][0]
      yAxis=self.widgets[index]["axesVars"][1:]
      dispMode=True
      #partitions=int(self.widgets[index]["partitionEntry"].get_text())
      #print("Number of partitions: "+str(partitions))

      if plot.status=="Plotted" and (not plot.xVarIndex==xAxis or not plot.yVarIndexList==yAxis or \
         not plot.partitions==partitions or not plot.dispMode==dispMode):
        plot.status="Not plotted"
        self.removePlotList.append((plot.index,plot.iconDatum))
      elif plot.status=="Plotted" and (not plot.name==new[0] or not plot.plotTitle==plotTitle or \
           not plot.xLabel==xAxisLabel or not plot.yLabelList==yAxisLabel):
        self.editPlotList.append((new[0],plot.iconDatum))

      plot.name=new[0]
      plot.plotTitle=plotTitle
      plot.xLabel=xAxisLabel
      plot.yLabelList=yAxisLabel
      plot.xVarIndex=xAxis
      plot.yVarIndexList=yAxis
      plot.dispMode=dispMode
      #plot.partitions=partitions

      plot.printData()
      
      plotterLog2.info(' New Plotting Saved ')
      plotterLog2.info(' Name - ' + plot.name + ' - title - ' + plot.plotTitle + ' - status - ' +  plot.status + ' - modes - ' + str(plot.dispMode) + ' - partitions - ' + str(plot.partitions) + ' - ')
      plotterLog2.info(' xaxis - ' + plot.xLabel + ' - yaxis - ' + str(plot.yLabelList) + ' -  xvarIndex - ' + str(plot.xVarIndex) + ' - yvarIndex - ' + str(plot.yVarIndexList) + ' - ')      

    self.response(gtk.RESPONSE_OK)

  """
    cancelBtnCallback
    Description: cancels the addition/editting of plots and closes the dialog
    Inputs: btn - cancel button that was pressed
    Outputs: none
    Return: none
  """
  def cancelBtnCallback(self,btn):
    if self.butType=="Add":
      self.plotList.remove(self.plotList.iter_nth_child(None,self.plotListLen))
    self.response(gtk.RESPONSE_CANCEL)

"""
  PlotDatum
  Description: a class which contains all the necessary information regarding a specific plot
"""
class PlotDatum():
  def __init__(self,plotListLen):
    self.checked=False
    self.name=""
    self.status="Not plotted"
    self.plotTitle=""
    self.xLabel=""
    self.yLabelList=[""]
    self.xVarIndex=0
    self.yVarIndexList=[1]
    self.index=plotListLen
    self.iconDatum=None
    self.dispMode=1
    self.partitions=128

  def copyDatum(self,plotDatum,plotListNames):
    copyNum=0
    self.name=plotDatum.name+"Copy%s"%(copyNum)
    while self.name in plotListNames:
      copyNum+=1
      self.name=plotDatum.name+"Copy%s"%(copyNum)

    self.plotTitle=plotDatum.plotTitle
    self.xLabel=plotDatum.xLabel
    self.yLabelList=plotDatum.yLabelList
    self.xVarIndex=plotDatum.xVarIndex
    self.yVarIndexList=plotDatum.yVarIndexList
    self.dispMode=plotDatum.dispMode
    self.partitions=plotDatum.partitions

  def printData(self):
    print(self.checked)
    print(self.name)
    print(self.status)
    print(self.plotTitle)
    print(self.xLabel)
    print(self.yLabelList)
    print(self.xVarIndex)
    print(self.yVarIndexList)
    print(self.index)
    print(self.iconDatum)
    print(self.dispMode)
    print(self.partitions)

"""
  NameEntry
  Description: a subclasses of gtk.Entry that limits a user to only input valid names
"""
class NameEntry(gtk.Entry):
  """
    __init__
    Description: initializes the entry and the regex pattern
    Inputs: none
    Outputs: none
    Return: none
  """
  def __init__(self):
    gtk.Entry.__init__(self)
    self.connect("changed",self.validateInput)
    self.namePattern=re.compile("(?:[_a-zA-Z])(?:[_a-zA-Z0-9]*)")

  """
    validateInput
    Description: checks to see if the input is valid and keep the changes if it is
    Inputs: entry - entry being editted
    Outputs: none
    Return: none
  """
  def validateInput(self,entry):
    text=entry.get_text()
    validInput=self.namePattern.findall(text)
    if len(validInput):
      entry.set_text(validInput[0])
    else:
      entry.set_text("")
gobject.type_register(NameEntry)

"""
  NumberEntry
"""
class NumberEntry(gtk.Entry):
  def __init__(self):
    gtk.Entry.__init__(self)
    self.connect("changed",self.validateInput)
    self.numInput=re.compile("\d*")
    self.set_width_chars(5)

  def validateInput(self,entry):
    text=entry.get_text()
    validInput=self.numInput.findall(text)
    if len(validInput):
      entry.set_text(validInput[0])
    else:
      entry.set_text("")
gobject.type_register(NumberEntry)
