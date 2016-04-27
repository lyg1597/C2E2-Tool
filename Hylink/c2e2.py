import pygtk
pygtk.require('2.0')
import gtk,gobject,re,os,subprocess,string,time,psutil
import logging
import os
from plotWindow import PlotWindow
from hierarchy import IsHierarchical,RemoveHierarchy
from hyir import hyirMdl,hyirXML
from propDialog import PropDialog,PropertyDatum
from SFParse import extract_sf
import numpy as np
import numpy.linalg as la
from propDialog import *

#FIXME
from genSimulator import *

verifLog = logging.getLogger('c2e2VerificationLog') 
F1 = logging.FileHandler('../wd/.c2e2verification.log', mode='w')
F1.setLevel(logging.DEBUG)
verifLog.setLevel(logging.DEBUG)
F1.setFormatter(logging.Formatter('%(asctime)s - %(levelname)s - %(message)s'))
verifLog.addHandler(F1)

verifLog.info('Launching the logger')
Global_Linear = 0
Global_Refine = 0
Global_Simulator = 0

"""
  Main
  Description: the main window of the program
"""
class Main(gtk.Window):
  """
    __init__
    Description: initializes the GUI along with lists of data and other variables
    Inputs: none
    Outputs: none
    Return: none
  """
  def __init__(self):
    gtk.Window.__init__(self)
        
    self.fileSaved=""
    self.fileOpened=False

    self.set_position(gtk.WIN_POS_CENTER)
    self.set_size_request(1000,700)
    self.set_title("C2E2")
    self.connect("destroy",gtk.main_quit)

    self.windowVBox=gtk.VBox(False,1)
    self.windowVBox.pack_start(self.initMenuBar(),False,False,0)

    self.fileLabel=gtk.Label()
    self.fileLabel.set_use_markup(True)
    self.fileLabel.set_markup('<span size="xx-large" weight="light">Open File in File->Open</span>')
    self.windowVBox.pack_start(self.fileLabel)
     
    self.add(self.windowVBox)
    self.show_all()
    self.set_focus(None)

  """
    initMenuBar
    Description: initializes the menu bar of the program
    Inputs: none
    Outputs: none
    Return: none
  """
  def initMenuBar(self):
    menuItems= (("/_File",None,None, 0,"<Branch>"),
                ("/File/_Open","<control>O",self.openFileCallback,0,None),
                ("/File/_Save","<control>S",self.saveFileCallback,0,None),
                ("/File/_Save As","<control><shift>S",self.saveFileCallback,1,None),
                ("/File/_Quit","<control>Q",self.quitProgramCallback,0,None),
                ("/_Help",None,None,0,"<Branch>"))

    accelGroup=gtk.AccelGroup()
    itemFactory=gtk.ItemFactory(gtk.MenuBar,"<menu>",accelGroup)
    itemFactory.create_items(menuItems)
    self.add_accel_group(accelGroup)
    menuBar=itemFactory.get_widget("<menu>")

    menuVBox=gtk.VBox(False,1)
    menuVBox.pack_start(menuBar,False,False,0)
    return menuVBox

  """
    openFileCallback
    Description: creates an open file dialog with which the user can select a model to open
    Inputs: value - value provided by the callback
            widget - widget that triggered the callback
    Outputs: none
    Return: none
  """
  def openFileCallback(self,value,widget):
    openDialog=gtk.FileChooserDialog("Open File",self,gtk.FILE_CHOOSER_ACTION_OPEN,(gtk.STOCK_CANCEL,gtk.RESPONSE_CANCEL,gtk.STOCK_OPEN,gtk.RESPONSE_OK))
    filter=gtk.FileFilter()
    filter.set_name("Models")
    filter.add_pattern("*.mdl")
    filter.add_pattern("*.hyxml")
    openDialog.add_filter(filter)
    openDialog.set_select_multiple(False)
    response=openDialog.run()
    if response==gtk.RESPONSE_OK:
      fileName = openDialog.get_filename()
      verifLog.info('File opened: ' + fileName)
      self.loadFileChoosen(fileName)
      self.fileOpened=True
    openDialog.destroy()

  """
    loadFileChosen
    Description: loads the selected file and displays the model
    Inputs: fileChosen - the path to the chosenFile
    Outputs: none
    Return: none
  """
  def loadFileChoosen(self,fileChoosen):
    os.system("./DeleteEX.sh")
    if self.fileOpened:
      self.modelNotebook.destroy()
    
    global Global_Linear
    global Global_Simulator
    Global_Linear = 0
    Global_Simulator = 0

    fileName, extension = os.path.splitext(os.path.basename(fileChoosen))
    self.set_title("C2E2: " + fileName)
    propList=[]
    paramList = None
    if extension=='.hyxml':
      hyir, propList = hyirXML(fileChoosen)
      for prop in propList:
          if all(param!=0 for param in prop.paramData):
            paramList = [str(param) for param in prop.paramData]
    elif extension=='.mdl':
      model=open(fileChoosen,"r")
      rawModel=model.read()
      x=re.search(r"Stateflow {",rawModel)
      sf_data=rawModel[x.start():]
      sf_tree=extract_sf(sf_data)
      if IsHierarchical(sf_tree):
        sf_tree=RemoveHierarchy(sf_tree)
      hyir = hyirMdl(sf_tree, fileChoosen)

    verifLog.info('Model is \n' + hyir.convertToXML([]))
    hyir.printHybridSimGuardsInvariants();
    hyir.printBloatedSimGuardsInvariants()
    
    st = 'constant'
    path = '../wd/simulator.cpp'
    gen_simulator(path, hyir, step_type=st)

    parseTree,vList,mList = hyir.display_system()
    self.modelNotebook=ModelNotebook(parseTree,hyir,propList,vList,mList,paramList)

    #self.ModelNotebook.propertiesFrame.disableAllButtons()
    # arguments = ['mv', 'bloatedSimGI.cpp', 'hybridSimGI.cpp', 'simulator.cpp', '../wd/']
    # subp = subprocess.Popen(arguments)
    # subp.wait()

    arguments = ['sh', './compileAndExecute', "0"]
    subp = subprocess.Popen(arguments,cwd="../wd/")
    subp.wait()

    self.fileLabel.hide()
    self.windowVBox.pack_start(self.modelNotebook)
    
  """
    saveFileCallback
    Description: creates an save file dialog with which the user can save a model in the hyxml format
    Inputs: value - value provided by the callback
            widget - widget that triggered the callback
    Outputs: none
    Return: none
  """
  def saveFileCallback(self,value,widget):
    if self.fileOpened:
      if (value==0 and self.fileSaved=='') or value==1:
        saveDialog=gtk.FileChooserDialog("Save File",self,gtk.FILE_CHOOSER_ACTION_SAVE,(gtk.STOCK_CANCEL,gtk.RESPONSE_CANCEL,gtk.STOCK_SAVE,gtk.RESPONSE_OK)) 
        filter=gtk.FileFilter()
        filter.set_name("Models")
        filter.add_pattern("*.hyxml")
        saveDialog.add_filter(filter)
        saveDialog.set_select_multiple(False)
        response=saveDialog.run()
        if response==gtk.RESPONSE_OK:
          self.fileSaved=saveDialog.get_filename()
          if not self.fileSaved.endswith(".hyxml"):
          # if not ".hyxml" in self.fileSaved:
            self.fileSaved+=".hyxml"
          self.modelNotebook.saveModel(self.fileSaved)
        saveDialog.destroy()
      else:
        self.modelNotebook.saveModel(self.fileSaved)

  def closeModelNotebook(self):
    self.modelNotebook.destroy()
    self.fileSaved = ""
    self.fileLabel.show()

  """
    quitProgramCallback
    Description: closes down the entire program
    Inputs: value - value passed in by the callback
            widget - widget that triggered the callback
    Outputs: none
    Return: none
  """
  def quitProgramCallback(self,value,widget):
    gtk.main_quit()

  """
    main
    Description: runs and shows the window
    Inputs: none
    Outputs: none
    Return: none
  """
  def main(self):
    gobject.threads_init()
    gtk.main()

#Comment this sectionI
class ModelNotebook(gtk.Notebook):
  def __init__(self,parseTree,hybridRep,propList,varList,modeList,paramsData):
    gtk.Notebook.__init__(self)
    self.propertyList=gtk.ListStore(gobject.TYPE_PYOBJECT)
    self.propertyListFilter=self.propertyList.filter_new()
    self.propertyListFilter.set_visible_func(self.showRow)
    self.editListLen=0
    self.varList=varList
    self.modeList=modeList
    self.paramData=[["Partitioning:","0.2",1], \
                    ["Time-step:","0.01",1], \
                    ["Time horizon:","10.0",1], \
                    ["Taylor model order:","10",1], \
                    ["K value: ", "2000", 1]]
    if paramsData:
      for i,param in enumerate(paramsData):
        self.paramData[i][1]=param
    self.parseTree=parseTree
    self.verifyingPlotting=[False]
    self.hybridRep=hybridRep

    self.propertyListLen = len(propList)
    for prop in propList:
      self.propertyList.append([prop])

    self.modelTable=gtk.Table(1,10,True)
    self.modelTable.set_col_spacing(6,5)
    self.modelVBox=gtk.VBox(False,0)
    self.modelVBox.pack_start(self.parseTree)
    self.modelTable.attach(self.modelVBox,0,7,0,1)
    
    self.propertiesFrame=PropertiesFrame(self,self.propertyList,self.propertyListFilter,
                                         self.propertyListLen,self.editListLen,self.varList,
                                         self.modeList,self.paramData,self.hybridRep,self.verifyingPlotting)
    self.parameterFrame=ParameterFrame(self.paramData,self.propertyList,self.propertiesFrame.listView,
                                       self.propertiesFrame.rendererStatus,self.hybridRep)

    self.infoVBox=gtk.VBox(False,0)
    self.infoVBox.pack_start(self.parameterFrame,False,False,0)
    self.infoVBox.pack_start(self.propertiesFrame,True,True,0)
    self.modelTable.attach(self.infoVBox,7,10,0,1)

    self.tab=gtk.HBox()
    self.tab.pack_start(gtk.Label("Model"))
    closeBtn=gtk.Button()
    closeBtn.set_image(gtk.image_new_from_stock(gtk.STOCK_CLOSE,gtk.ICON_SIZE_MENU))
    closeBtn.set_relief(gtk.RELIEF_NONE)
    closeBtn.connect("clicked",self.closeModelNotebookCallback)
    self.tab.pack_end(closeBtn,False,False)
    self.tab.show_all()

    self.connect("switch-page",self.switchTabsCallback,self.verifyingPlotting)
    self.append_page(self.modelTable,self.tab)
    self.show_all()
    self.propertiesFrame.showToggleAll()

  def switchTabsCallback(self,notebook,page,pageNum,verifyingPlotting):
    if verifyingPlotting[0]==True:
      notebook.stop_emission("switch_page")

  """
    showRow
    Description: prevents property from being displayed while it is being added
    Inputs: model - tree model
            iter - a tree iter pointing to the row in the model
    Outputs: none
    Return: none
  """
  def showRow(self,model,iter):
    plot=model.get_value(iter,0)
    if not plot==None and not plot.name=="":
      return True
    else:
      return False

  def saveModel(self,fileSave):
    saveFile=open(fileSave,'w')
    savedModelString = self.hybridRep.convertToXML(self.propertyList)
    verifLog.info(' Saved Model is \n'+savedModelString)
    saveFile.write(savedModelString)
    saveFile.close()

  def closeModelNotebookCallback(self,btn):
    parent=self.get_toplevel()
    warningDialog=gtk.MessageDialog(parent,gtk.DIALOG_DESTROY_WITH_PARENT,gtk.MESSAGE_WARNING,
                                    gtk.BUTTONS_OK_CANCEL,
                                    "Are you sure want to close the model (unsaved work will be lost)?")
    response=warningDialog.run()
    if response==gtk.RESPONSE_OK:
      parent.closeModelNotebook()
    warningDialog.destroy()

"""
  ParameterFrame
  Description: initializes the upper right side of the interface where the user can input parameters
  Inputs: none
  Outputs: none
  Return: none
"""
class ParameterFrame(gtk.Frame):
  """
    __init__
    Description: initalizes the parameter interface and data
    Input: paramData - data associated with the various parameters
    Outputs: none
    Return: none
  """
  def __init__(self,paramData,propertyList,listView,rendererStatus,hybridRep):
    self.prevSimuIndex =0
    gtk.Frame.__init__(self,"Parameters")
    self.paramData=paramData
    self.propertyList=propertyList
    self.listView=listView
    self.rendererStatus=rendererStatus
    self.initParameterFrame()
    self.hybridRep = hybridRep
 
  """
    initParameterFrame
    Description: initializes the contents of the frame
    Inputs: none
    Outputs: none
    Return: none
  """
  def initParameterFrame(self):
    self.paramVBox=gtk.VBox(False,0)

    for i in xrange(1,len(self.paramData)):
      paramHBox=gtk.HBox(False,0)
      label=gtk.Label(self.paramData[i][0])
      label.set_alignment(0,0)
      paramHBox.pack_start(label,True,True,0)
      if i==3: #Taylor model order must be integer
        entry=NumberEntry()
      else:
        entry=FloatEntry()
      entry.set_width_chars(10)
      entry.set_text(self.paramData[i][1])
      paramHBox.pack_start(entry,False,False,0)
      checkImage=gtk.Image()
      checkImage.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU) 
      paramHBox.pack_start(checkImage,False,False,5)
      entry.connect("changed",self.entryCallback,i,checkImage)
      self.paramVBox.pack_start(paramHBox,True,True,0)

    paramHBox1 = gtk.HBox(False,0)
    linearbutton = gtk.CheckButton("Linear Model")
    linearbutton.connect("toggled", self.linearCallback, "Linear Model")
    paramHBox1.pack_start(linearbutton,True,True,2)

    self.paramVBox.pack_start(paramHBox1,True,True,0)

    combobox = gtk.combo_box_new_text()
    combobox.connect('changed', self.changed_cb)
    self.paramVBox.pack_start(combobox,True,True,2)
    combobox.append_text('ODEINT:FIXED')
    combobox.append_text('ODEINT:ADAPTIVE')
    combobox.append_text('CAPD')
    combobox.set_active(0)

    refinecombobox = gtk.combo_box_new_text()
    refinecombobox.connect("changed", self.changed_refine_cb)
    self.paramVBox.pack_start(refinecombobox,True,True,2)
    refinecombobox.append_text('DEFAULT REFINE STRATEGY')
    refinecombobox.append_text('USER DEFINE STRATEGY')
    refinecombobox.set_active(0)

    self.add(self.paramVBox)

  def changed_refine_cb(self,combobox):
    index = combobox.get_active()
    global Global_Refine
    Global_Refine = index

  def changed_cb(self,combobox):
    index = combobox.get_active()
    if index != self.prevSimuIndex:
      self.prevSimuIndex = index
      if index == 0:
        st = 'constant'
        path = '../Hylink/simulator.cpp'
        gen_simulator(path, self.hybridRep, step_type=st)
      if index == 1:
        st = 'adaptive'
        path = '../Hylink/simulator.cpp'
        gen_simulator(path, self.hybridRep, step_type=st)
      if index == 2:
        self.hybridRep.convertToCAPD("simulator")
      
      arguments = ['mv','simulator.cpp', '../wd/']
      subp = subprocess.Popen(arguments)
      subp.wait()

      arguments = ['sh', './compileSimulator', str(index)]
      subp = subprocess.Popen(arguments,cwd="../wd/")
      subp.wait()
      # if index == 0:
      #   st = 'constant'
      #   path = '../Hylink/simulator.cpp'
      #   gen_simulator(path, self.hybridRep, step_type=st)
      #   arguments1 = ['mv','simulator.cpp', '../wd/']
      #   subp1 = subprocess.Popen(arguments1)
      #   subp1.wait()
      #   arguments = ['sh', './compileSimulator', "0"]
      #   subp = subprocess.Popen(arguments,cwd="../wd/")
      #   subp.wait()
      # if index == 1:
      #   st = 'adaptive'
      #   path = '../Hylink/simulator.cpp'
      #   gen_simulator(path, self.hybridRep, step_type=st)
      #   arguments1 = ['mv','simulator.cpp', '../wd/']
      #   subp1 = subprocess.Popen(arguments1)
      #   subp1.wait()
      #   arguments = ['sh', './compileSimulator', "1"]
      #   subp = subprocess.Popen(arguments,cwd="../wd/")
      #   subp.wait()
      # if index == 2:
      #   self.hybridRep.convertToCAPD("simulator")
      #   arguments1 = ['mv','simulator.cpp', '../wd/']
      #   subp1 = subprocess.Popen(arguments1)
      #   subp1.wait()
      #   arguments = ['sh', './compileSimulator', "2"]
      #   subp = subprocess.Popen(arguments,cwd="../wd/")
      #   subp.wait()

    global Global_Simulator
    Global_Simulator = index    


  def linearCallback(self, widget, data):
    global Global_Linear
    Global_Linear = 1- Global_Linear

  """
    entryCallback
    Description: informs the user whether the input in the entry is valid or not
    Inputs: entry - entry currently being editted
            index - the index of the entry being editted
            image - image associated with the entry
    Outputs: none
    Return: none
  """
  def entryCallback(self,entry,index,image):
    self.paramData[index][1]=entry.get_text()
    try:
      float(self.paramData[index][1])
      image.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)
      self.paramData[index][2]=1
      for prop in self.propertyList:
        prop=prop[0]
        value=float(entry.get_text())
        if prop.status=="Verified" and not value==prop.paramData[index]:
          prop.status="Verified*"
          self.propertyList.row_changed(prop.index,self.propertyList.get_iter(prop.index))
          toolTip=gtk.Tooltip()
          path=self.propertyList.get_path(self.propertyList.iter_nth_child(None,prop.index))
          column=self.listView.get_column(2)
          self.listView.set_tooltip_cell(toolTip,path,column,self.rendererStatus)
        elif prop.status=="Verified*" and value==prop.paramData[index]:
          prop.status="Verified"
          self.propertyList.row_changed(prop.index,self.propertyList.get_iter(prop.index))
        if prop.status=="Simulated" and not value==prop.paramData[index]:
          prop.status="Simulated*"
          self.propertyList.row_changed(prop.index,self.propertyList.get_iter(prop.index))
          toolTip=gtk.Tooltip()
          path=self.propertyList.get_path(self.propertyList.iter_nth_child(None,prop.index))
          column=self.listView.get_column(2)
          self.listView.set_tooltip_cell(toolTip,path,column,self.rendererStatus)
        elif prop.status=="Simulated*" and value==prop.paramData[index]:
          prop.status="Simulated"
          self.propertyList.row_changed(prop.index,self.propertyList.get_iter(prop.index))          
    except ValueError:
      image.set_from_stock(gtk.STOCK_CANCEL,gtk.ICON_SIZE_MENU)
      self.paramData[index][2]=0

"""
  FloatEntry
  Description: a subclass of gtk.Entry that limits user to only input valid number for the parameters
"""
class FloatEntry(gtk.Entry):
  """
    __init__
    Description: initializes the entry and the regex pattern
  """
  def __init__(self):
    gtk.Entry.__init__(self)
    self.connect("changed",self.validateInput)
    self.numInput=re.compile("[-+]?(?:\d*\.?\d*)(?:[Ee][-+]?\d*)?")

  """
    validateInput
    Description: checks to see if the inputs is valid and keeps the changes if it is
    Inputs: none
    Outputs: none
    Return: none
  """
  def validateInput(self,entry):
    text=entry.get_text()
    validInput=self.numInput.findall(text)
    if len(validInput):
      entry.set_text(validInput[0])
    else:
      entry.set_text("")

gobject.type_register(FloatEntry)

"""
  NumberEntry
  Description: a subclass of gtk.Entry that limits user to only input valid number for the parameters
"""
class NumberEntry(gtk.Entry):
  """
    __init__
    Description: initializes the entry and the regex pattern
  """
  def __init__(self):
    gtk.Entry.__init__(self)
    self.connect("changed",self.validateInput)
    self.numInput=re.compile("\d*")

  """
    validateInput
    Description: checks to see if the inputs is valid and keeps the changes if it is
    Inputs: none
    Outputs: none
    Return: none
  """
  def validateInput(self,entry):
    text=entry.get_text()
    validInput=self.numInput.findall(text)
    if len(validInput):
      entry.set_text(validInput[0])
    else:
      entry.set_text("")

gobject.type_register(NumberEntry)

"""
  PropertiesFrame
  Description: initializes the lower right side of the interface which allows user to 
               create/modify/remove properties
"""
class PropertiesFrame(gtk.Frame):
  """
    __init__
    Description: initializes all the contents of the frame along with list and data
    Inputs: notebook - notebook to allow various tabs in the program
            propertyList - list of properties and associated data
            propertyListFilter - filter for the property list
            propertyListLen - length of the property list
            editListLen - number of properties selected by the user
            varList - list of variables of the model
    Outputs: notebook,propertyList,propertyListLen
    Return: none
  """
  def __init__(self,notebook,propertyList,propertyListFilter,propertyListLen,editListLen,varList,
               modeList,paramData,hybridRep,verifyingPlotting):
    gtk.Frame.__init__(self,"Properties")
    self.notebook=notebook
    self.propertyList=propertyList
    self.propertyListFilter=propertyListFilter
    self.propertyListLen=propertyListLen
    self.editListLen=editListLen
    self.varList=varList
    self.modeList=modeList
    self.paramData=paramData
    self.hybridRep=hybridRep
    self.subp=None
    self.abortedVerifying=False
    self.verifyingPlotting=verifyingPlotting
    self.initPropertiesFrame()

  """
    initPropertiesFrame
    Description: initializes the contents of the frame
    Inputs: none
    Outputs: none
    Return: none
  """
  def initPropertiesFrame(self):
    propTable=gtk.Table(3,3,False) 
    propHBox=gtk.HBox(True,0)
    propHBox1 = gtk.HBox(True,0)

    self.addBtn=gtk.Button("Add")
    self.addBtn.connect("clicked",self.addEditBtnCallback)
    propHBox.pack_start(self.addBtn,True,True,0)

    self.editBtn=gtk.Button("Edit")
    self.editBtn.connect("clicked",self.addEditBtnCallback)
    propHBox.pack_start(self.editBtn,True,True,0)

    self.copyBtn=gtk.Button("Copy")
    self.copyBtn.connect("clicked",self.copyBtnCallback)
    propHBox.pack_start(self.copyBtn,True,True,0)

    self.removeBtn=gtk.Button("Remove")
    self.removeBtn.connect("clicked",self.removeBtnCallback)
    propHBox.pack_start(self.removeBtn,True,True,0)

    self.verifyAbortBtn=gtk.Button("Verify")
    self.verifyAbortBtn.connect("clicked",self.verifyAbortBtnCallback)
    propHBox1.pack_start(self.verifyAbortBtn,True,True,0)

    self.simulateAbortBtn = gtk.Button("Simulate")
    self.simulateAbortBtn.connect("clicked",self.simulateAbortBtnCallback)
    propHBox1.pack_start(self.simulateAbortBtn,True,True,0)

    listSW=gtk.ScrolledWindow()
    listSW.set_policy(gtk.POLICY_AUTOMATIC,gtk.POLICY_AUTOMATIC)
    self.listView=gtk.TreeView(self.propertyListFilter)
    self.listView.connect("button-press-event",self.rowCallback)
    self.listView.connect("query-tooltip",self.queryToolTip)
    self.listView.set_has_tooltip(True)
    listSelection=self.listView.get_selection()
    listSelection.set_mode(gtk.SELECTION_NONE)
    listSW.add(self.listView)

    self.rendererToggle=gtk.CellRendererToggle()
    self.rendererToggle.set_property("activatable",True)
    self.rendererToggle.connect("toggled",self.propToggleCallback)

    self.propToggleAll=gtk.CheckButton()
    self.propToggleAll.connect("toggled",self.propToggleAllCallback)
    if self.propertyListLen>0:
      self.propToggleAll.set_sensitive(True)
    else:
      self.propToggleAll.set_sensitive(False)
    self.propToggleAll.set_active(False)
    self.columnToggle=gtk.TreeViewColumn("Checked",self.rendererToggle)
    self.columnToggle.set_cell_data_func(self.rendererToggle,self.setToggleColumn)
    self.columnToggle.set_property("clickable",True)
    self.columnToggle.connect("clicked",self.toggleToggleAll,self.propToggleAll)
    self.columnToggle.set_widget(self.propToggleAll)
    self.columnToggle.set_sizing(gtk.TREE_VIEW_COLUMN_AUTOSIZE)
    self.listView.append_column(self.columnToggle)

    rendererText=gtk.CellRendererText()
    columnText=gtk.TreeViewColumn("Properties",rendererText)
    columnText.set_cell_data_func(rendererText,self.setTextColumn,"name")
    columnText.set_sizing(gtk.TREE_VIEW_COLUMN_AUTOSIZE)
    self.listView.append_column(columnText)

    self.rendererStatus=gtk.CellRendererText()
    columnText=gtk.TreeViewColumn("Status         ",self.rendererStatus)
    columnText.set_cell_data_func(self.rendererStatus,self.setTextColumn,"status")
    columnText.set_sizing(gtk.TREE_VIEW_COLUMN_AUTOSIZE)
    self.listView.append_column(columnText)

    rendererText=gtk.CellRendererText()
    rendererPixbuf=gtk.CellRendererPixbuf()
    columnPixbufText=gtk.TreeViewColumn("Result")
    columnPixbufText.pack_start(rendererText,True)
    columnPixbufText.pack_start(rendererPixbuf,False)
    columnPixbufText.set_cell_data_func(rendererText,self.setTextColumn,"result")
    columnPixbufText.set_cell_data_func(rendererPixbuf,self.setPixbufColumn)
    columnPixbufText.set_sizing(gtk.TREE_VIEW_COLUMN_AUTOSIZE)
    self.listView.append_column(columnPixbufText)

    propTable.attach(listSW,0,3,0,1)
    propTable.attach(propHBox,0,3,1,2,gtk.FILL|gtk.EXPAND,0)
    propTable.attach(propHBox1,0,3,2,3,gtk.FILL|gtk.EXPAND,0)
    self.add(propTable)

  """
    setToggleColumn
  """
  def setToggleColumn(self,column,cell,model,iter):
    prop=model.get_value(iter,0)
    cell.set_property("active",prop.checked)

  """
    setTextColumn
  """
  def setTextColumn(self,column,cell,model,iter,textType):
    prop=model.get_value(iter,0)
    if textType=="name":
      cell.set_property("text",prop.name)
    elif textType=="status":
      cell.set_property("text",prop.status)
    else:
      cell.set_property("text",prop.verifResult)
  
  def toggleToggleAll(self,column,togBtn):
    if togBtn.get_sensitive()==True:
      if togBtn.get_active()==True:
        togBtn.set_focus_on_click(True)
        togBtn.set_active(False)
      else:
        togBtn.set_focus_on_click(True)
        togBtn.set_active(True)

  """
    setPixbufColumn
  """
  def setPixbufColumn(self,column,cell,model,iter):
    prop=model.get_value(iter,0)
    cell.set_property("pixbuf",prop.newTabPixbuf)

  """
    showToggleAll
    Description: shows the toggle button in the header
    Inputs: none
    Outputs: none
    Return: none
    Note: the toggle button in the header won't appear unless it is specifically shown
          after the whole window is shown; bug in PyGTK
  """
  def showToggleAll(self):
    self.propToggleAll.show()

  """
    enableWidgets
    Description: enables the widgets in the frame only after the model has been opened
    Inputs: none
    Outputs: none
    Return: none
  """

  def showGenerationMessage(self):
#    self.verifyingPlotting[0]=not False    
#    self.rendererToggle.set_activatable(False)
    self.columnToggle.set_property("clickable",False)
#    self.propToggleAll.set_sensitive(False)
    self.addBtn.set_sensitive(False)
    self.editBtn.set_sensitive(False)
    self.copyBtn.set_sensitive(False)
    self.removeBtn.set_sensitive(False)
    self.verifyAbortBtn.set_sensitive(False)
    self.verifyAbortBtn.set_label("Generating Files")
    print "The generation message should be shown!"

  def enableWidgets(self,val,flag,task_str):
    self.verifyingPlotting[0]=not val
    self.rendererToggle.set_activatable(val)
    self.columnToggle.set_property("clickable",val)
    self.propToggleAll.set_sensitive(val)
    self.addBtn.set_sensitive(val)
    self.editBtn.set_sensitive(val)
    self.copyBtn.set_sensitive(val)
    self.removeBtn.set_sensitive(val)
    if flag==0:
      currBtn = self.verifyAbortBtn
      otherBtn = self.simulateAbortBtn
    elif flag==1:
      currBtn = self.simulateAbortBtn
      otherBtn = self.verifyAbortBtn
    otherBtn.set_sensitive(val)
    if val:
      currBtn.set_label(task_str)
    else:
      currBtn.set_label("Abort")

  """
    propToggleCallback
    Description: controls the toggling of the toggle button s int the list view
    Inputs: cell - cell in the list view
            path - path of the toggle toggled in the list
    Outputs: none
    Return: none
  """
  def propToggleCallback(self,cell,path):
    plot=self.propertyList[path][0]
    plot.checked=not plot.checked
    if plot.checked:
      self.editListLen+=1
      if self.editListLen==self.propertyListLen:
        self.propToggleAll.set_active(True)
    else:
      self.editListLen-=1
      self.propToggleAll.set_active(False)

  """
    propToggleAllCallback
  """
  def propToggleAllCallback(self,togBtn):
    if togBtn.get_active():
      for p in self.propertyList:
        p[0].checked=True
      self.editListLen=self.propertyListLen
    else:
      if self.editListLen==self.propertyListLen:
        for p in self.propertyList:
          p[0].checked=False
        self.editListLen=0
    self.propertyListFilter.refilter()

  """
    rowCallback
    Description: allows the user the list and opens a new in the notebook if the property has been
                 verified
    Inputs: widget - widget involved in the callback
            event - event triggering the callback
    Outputs: none
    Return: none
  """
  def rowCallback(self,widget,event):
    if event.button==1 and event.type==gtk.gdk.BUTTON_PRESS:
      rowData=self.listView.get_path_at_pos(int(event.x),int(event.y))
#       print "While Pressing"
#       print str(event.x) + " and " + str(event.y) + " OK!"
      if not rowData==None and rowData[1].get_title()=="Result":
        propIter=self.propertyList.get_iter(rowData[0])
        verified=self.propertyList.get_value(propIter,0).status
        if verified=="Verified" or verified=="Verified*" or verified=="Simulated" or verified=="Simulated*":
          prop=self.propertyList.get_value(propIter,0)
          if prop.tabChild==None:
            unsafeSet=prop.unsafeSetParsed
            reachSetPath=prop.reachSetPath
            #print prop.simulator
            if prop.simulator==1:
              ploterversion = 2 
              #print ploterversion
            elif prop.simulation == 1:
              ploterversion = 3
            else:
              ploterversion = 1
              #print ploterversion
            plotWindow=PlotWindow(["time"]+self.varList,self.modeList,unsafeSet,reachSetPath,
                                  prop.paramData[1],prop.paramData[2],self.verifyingPlotting, ploterversion)
            tab=gtk.HBox()
            tab.pack_start(gtk.Label(prop.name),True,True)
            closeBtn=gtk.Button()
            closeBtn.set_image(gtk.image_new_from_stock(gtk.STOCK_CLOSE,gtk.ICON_SIZE_MENU))
            closeBtn.set_relief(gtk.RELIEF_NONE)
            closeBtn.connect("clicked",self.closePlotTabCallback,propIter,plotWindow)
            tab.pack_end(closeBtn,False,False)
            tab.show_all()

            self.notebook.append_page(plotWindow,tab)
            prop.tabChild=plotWindow
            self.notebook.set_current_page(self.notebook.page_num(plotWindow))
          else:
            self.notebook.set_current_page(self.notebook.page_num(prop.tabChild))

  """
    queryToolTip
  """
  def queryToolTip(self,widget,x,y,keyboardMode,toolTip):
    if not widget.get_tooltip_context(x,y,keyboardMode):
      return False
    else:
      model,path,iter=widget.get_tooltip_context(x,y,keyboardMode)
      prop=self.propertyList[path][0]
      rowData = self.listView.get_path_at_pos(int(x),int(y)-20)

      if (prop.status=="Verified*") and ((rowData == None) or not(rowData[1].get_title()=="Result")):
        toolTip.set_text("Partitioning: %s\nTime-step: %s\nTime horizon: %s\nTaylor order: %d"%(prop.paramData[0],prop.paramData[1],prop.paramData[2],prop.paramData[3]))
        return True
      if (prop.status=="Verified" or prop.status=="Verified*") and not (rowData == None) and rowData[1].get_title() == "Result":
        toolTip.set_text("Plot")
        return True
      if (prop.status=="Simulated*") and ((rowData == None) or not(rowData[1].get_title()=="Result")):
        toolTip.set_text("Partitioning: %s\nTime-step: %s\nTime horizon: %s\nTaylor order: %d"%(prop.paramData[0],prop.paramData[1],prop.paramData[2],prop.paramData[3]))
        return True
      if (prop.status=="Simulated" or prop.status=="Simulated*") and not (rowData == None) and rowData[1].get_title() == "Result":
        toolTip.set_text("Plot")
        return True
      else:
        return False

  """
    closePlotTabCallback
    Description: closes a tab in the notebook and update the property
    Inputs: btn - button clicked
            iter - property list iter
            child - the widget contained within the page of the notebook
    Outputs: none
    Return: none
  """
  def closePlotTabCallback(self,btn,iter,child):
    parent=self.get_toplevel()
    warningDialog=gtk.MessageDialog(parent,gtk.DIALOG_DESTROY_WITH_PARENT,gtk.MESSAGE_WARNING,
                                    gtk.BUTTONS_OK_CANCEL,
                                    "Are you sure you want to close this plot tab (all unsaved plots will be lost)?")
    response=warningDialog.run()
    if response==gtk.RESPONSE_OK:
      self.notebook.remove_page(self.notebook.page_num(child))
      self.notebook.queue_draw_area(0,0,-1,-1)
      self.propertyList.get_value(iter,0).tabChild=None
    warningDialog.destroy()


  """
    addEditBtnCallback
    Description: allows the user to add/edit the properties in the property list
    Inputs: btn - one of two buttons clicked, add/edit
    Outputs: none
    Return: none
  """
  def addEditBtnCallback(self,btn):
    if btn.get_label()=="Add" or btn.get_label()=="Edit" and self.editListLen>0:
      removePropList=[]
      editPropList=[]
      propDialog=PropDialog(self.get_toplevel(),btn.get_label(),self.propertyList,
                            self.propertyListLen,self.varList,self.modeList,removePropList,
                            editPropList)
      response=propDialog.run()
      if response==gtk.RESPONSE_OK:
        if btn.get_label()=="Add":
          self.propertyListLen=len(self.propertyList)
          self.propToggleAll.set_active(False)
          self.propToggleAll.set_sensitive(True)
        else:
          for r in removePropList:
            prop=self.propertyList.get_value(self.propertyList.iter_nth_child(None,r),0)
            if not prop.tabChild==None:
              self.notebook.remove_page(self.notebook.page_num(prop.tabChild))
              prop.tabChild=None
          for e in editPropList:
            prop=self.propertyList.get_value(self.propertyList.iter_nth_child(None,e[1]),0)
            if not prop.tabChild==None:
              self.notebook.get_tab_label(prop.tabChild).get_children()[0].set_text(e[0])
            if prop.status=="Verified":
              newPath="ReachSet"+prop.name
              arguments=["mv",prop.reachSetPath,newPath]
              subprocess.Popen(arguments,cwd="../wd")
              prop.reachSetPath=newPath
      propDialog.destroy()
      self.propertyListFilter.refilter()

  """
    copyBtnCallback
  """
  def copyBtnCallback(self,btn):
    if self.editListLen<=0:
      return

    propertyListNames=[]
    propertyListIndices=[]
    for prop in self.propertyList:
      propertyListNames.append(prop[0].name)
      if prop[0].checked:
        propertyListIndices.append(prop[0].index)

    for idx in propertyListIndices:
      prop=self.propertyList.get_value(self.propertyList.iter_nth_child(None,idx),0)
      newProp=PropertyDatum(self.propertyListLen)
      newProp.copyDatum(prop,propertyListNames)
      newProp.printData()
      self.propertyList.append([newProp])
      self.propertyListLen+=1
      self.propToggleAll.set_active(False)

  """
    removeBtnCallback
    Description: remove a property from the list
    Inputs: btn - button clicked
    Outputs: none
    Return: none
  """
  def removeBtnCallback(self,btn):
    if self.editListLen>0:
      removePropList=[]
      for prop in self.propertyList:
        if prop[0].checked:
          propIter=self.propertyList.iter_nth_child(None,prop[0].index)
          removePropList.append(propIter)
      for rem in reversed(removePropList):
        prop=self.propertyList.get_value(rem,0);
        if not prop.tabChild==None:
          self.notebook.remove_page(self.notebook.page_num(prop.tabChild))
        self.propertyList.remove(rem)
      self.propertyListLen=len(self.propertyList)
      self.editListLen=0
      self.propToggleAll.set_active(False)
      if self.propertyListLen==0:
        self.propToggleAll.set_sensitive(False)

      reindex=0
      for prop in self.propertyList:
        prop[0].index=reindex
        reindex+=1

  def verifyAbortBtnCallback(self, btn):
    self.executeBtnCallback(btn,0,"Verify","Verifying","Verified")

  def simulateAbortBtnCallback(self, btn):
    self.executeBtnCallback(btn,1,"Simulate","Simulating","Simulated")

  def executeBtnCallback(self,btn,flag,task_str,status_str,completed_str):
    for param in self.paramData:
      if param[2]==0:
        return
    if btn.get_label()==task_str and self.editListLen > 0:
      os.system("./DeleteRE.sh")
      self.enableWidgets(False, flag, task_str)
      for prop in self.propertyList:
        prop=prop[0]
        if prop.checked and prop.status=='Not verified':
          global Global_Simulator
          prop.simulator = Global_Simulator
          prop.simulation  = flag

          pathString="../wd/ReachSet"+prop.name
          
          verifLog.info(task_str+' started - property name - ' + prop.name + ' - ')
          verifLog.info(' Property - initial set - ' + prop.initialSetStr + ' - unsafe set - ' + prop.unsafeSetStr + ' - ')
          
          prop.reachSetPath=pathString
 
          # Here we will print the entire file where the file is automatically generated
          c2e2String = ""
          c2e2String+= "dimensions=\""+str(len(self.varList))+"\"\n"
          c2e2String+= "modes=\""+str(len(self.modeList))+"\"\n"
          c2e2String+= "simulator=\"simu\"\n"
          
          initialSet = prop.initialSetParsed
          unsafeSet = prop.unsafeSetParsed
          
          initialModeString = initialSet[0]
          initialMatrix = initialSet[1]
          initialB = initialSet[2]
          initialIneqs = initialSet[3]
          
          unsafeMatrix = unsafeSet[0]
          unsafeB = unsafeSet[1]
          unsafeIneqs = unsafeSet[2]
          initModeC2E2Rep = -1;
          modesList = self.hybridRep.automata[0].modes
          modeI = 0
          for modeIndex in modesList:
              modeI += 1
              if(initialModeString == modeIndex.name):
                  initModeC2E2Rep = modeI;
          
          c2e2String+= "init-mode=\""+str(initModeC2E2Rep)+"\"\n"
          print(self.paramData)
          taylor = self.paramData[3][1]
          relerr = "0.000000001"
          abserr = "0.0000000001"
          thoriz = self.paramData[2][1]
          tstep = self.paramData[1][1]
          global Global_Refine
          refine = str(Global_Refine)
          delta = "0"
          prop.paramData[0]=float(delta)
          prop.paramData[1]=float(tstep)
          prop.paramData[2]=float(thoriz)
          prop.paramData[3]=float(taylor)
          c2e2String+= "refine=\""+refine+"\"\n"
          c2e2String+= "time-step=\""+tstep+"\"\n"
          c2e2String+= "abs-error=\""+abserr+"\"\n"
          c2e2String+= "rel-error=\""+relerr+"\"\n"
          c2e2String+= "time-horizon=\""+thoriz+"\"\n"
          c2e2String+= 'simuflag = "'+str(flag)+'"'

          verifLog.info(' Partition - ' + delta + ' - Time step - ' + tstep + ' - Time horizon - ' + thoriz + ' - ')
          
          c2e2String+= "init-eqns=\""+str(len(initialIneqs))+"\"\n"
          c2e2String+= "init-matrix=["
          c2e2String+= self.getMatrixStr(initialMatrix, initialIneqs)
          c2e2String+= "]\n"
          c2e2String+= "init-b=["
          c2e2String+= self.getMatrixStr(initialB, initialIneqs)
          c2e2String+= "]\n"        
          c2e2String+= "unsafe-eqns=\""+str(len(unsafeIneqs))+"\"\n"
          c2e2String+= "unsafe-matrix=["
          c2e2String+= self.getMatrixStr(unsafeMatrix, unsafeIneqs)
          c2e2String+= "]\n"
          c2e2String+= "unsafe-b=["
          c2e2String+= self.getMatrixStr(unsafeB, unsafeIneqs)
          c2e2String+= "]\n"
       
          for mode_num in range(1, len(self.modeList)+1):
            c2e2String+= "annotation-mode=\""+str(mode_num)+"\"\n"
            c2e2String+= "annotation-type=\"contraction\"\n"    
            # if array[3] == 1 or array[3] == 3 :
            #     c2e2String+= "annotation-type=\"contraction\"\n"
            # if array[3] == 2 :
            #     c2e2String+= "annotation-type=\"linear\"\n"
            c2e2String+= "annotation=\'dx1^2 + dx2^2\'\n"
            c2e2String+= "beta=\'dx1^2 + dx2^2\'\n"
            if (Global_Linear):
                filename = ""
                filename +="../wd/jacobiannature"
                filename += str(mode_num)
                filename +=".txt"
                fid = open(filename,'r').read().split('\n')
                numofvar = int(fid[0])
                listofvar = []
                for i in range (numofvar):
                  listofvar.append(fid[i+1])
                equationpos = 1+numofvar
                numofeq = int(fid[equationpos])
                equationpos+=1
                listofeq = []
                for i in range (numofeq):
                  listofeq.append(fid[equationpos+i])
                codestring = "def jcalc("
                codestring += "listofvar, "
                codestring += "listvalue"
                codestring+='):\n'
                codestring+=" for i in range (len(listofvar)):\n"
                codestring+="   temp = listofvar[i]\n"
                codestring+="   rightside = '='+str(listvalue[i])\n"
                codestring+="   exec(temp+rightside)\n"
                codestring+=" ret = []\n"
                for i in range (numofeq):
                  codestring+=" "
                  codestring+=listofeq[i]
                  codestring+='\n'
                  codestring+=' ret.append(Entry)\n'
                codestring+=' return ret'
                exec(codestring)
                Constant_Jacobian = jcalc(listofvar,np.ones((1,numofvar))[0])
                Constant_Jacobian = np.reshape(Constant_Jacobian,(numofvar,numofvar))
                gamma_rate = la.eigvals(Constant_Jacobian).real
                gamma = max(gamma_rate)
                if abs(max(gamma_rate)) < 0.00001:
                  gamma = 0                   
                k = la.norm(Constant_Jacobian)
            else:
                gamma = 0
                k = self.paramData[4][1]
            c2e2String+= "k=\""+str(k)+"\"\n"
            c2e2String+= "gamma=\""+str(gamma)+"\"\n"
            c2e2String+= "is_linear=\""+str(Global_Linear)+"\"\n"

          c2e2String+= "visualize all to ReachSet"+prop.name+"\n"
          
          filenamestring = "ReachSet"+prop.name
          os.system("rm ../wd/"+filenamestring)
          writer = open("../wd/Configuration-C2E2","w")
          writer.write(c2e2String)
          writer.close()
          
          arguments = ['sh', './ExecuteC2E2']
          self.subp = subprocess.Popen(arguments,cwd="../wd/",preexec_fn=os.setsid)

          #Might need a variable here to check if the process was aborted
          while self.subp.poll() == None:
            for i in range(4):
              prop.status = status_str + '.'*i
              self.propertyList.row_changed(prop.index,self.propertyList.get_iter(prop.index))
              while gtk.events_pending():
                gtk.main_iteration()
              time.sleep(0.5) 
              if self.subp.poll():
                break
      
          checkfilename = "../wd/ReachSet"+prop.name

          self.subp=None
          if self.abortedVerifying==False:
            prop.status=completed_str
            if os.stat(checkfilename).st_size == 0:
              prop.verifResult = "ERROR"
            with open("../wd/Result.dat",'r') as f:
              res = int(f.readline())
              if res<0:
                prop.verifResult = "Unsafe"
              elif res==0:
                prop.verifResult = "Unknown"
              else:
                prop.verifResult = "Safe"

            prop.newTabPixbuf=gtk.gdk.pixbuf_new_from_file("./Resources/newTab.png").scale_simple(16,16,gtk.gdk.INTERP_HYPER)
            prop.reachSetPath=pathString
            
            verifLog.info(task_str+' Result Returned - ' + prop.verifResult + ' - ')
            
            self.propertyList.row_changed(prop.index,self.propertyList.get_iter(prop.index))
          else:
            self.abortedVerifying=False
            prop.status='Not verified'
            verifLog.info(task_str+ ' Aborted ')
            self.propertyList.row_changed(prop.index,self.propertyList.get_iter(prop.index))
            print("Breaking")
            break

      self.enableWidgets(True, flag, task_str)
      self.notebook.set_current_page(self.notebook.get_current_page())

    else:
      #Note that I will have race conditions here regardless unless I have a lock
      if not self.subp==None:
        print("Here!",self.subp.pid)
        parent=psutil.Process(self.subp.pid)
        for child in parent.children(recursive=True):
          child.kill()
        parent.kill()
      self.abortedVerifying=True
      verifLog.info(task_str+' completely aborted ')
  
  def getMatrixStr(self, row_matrix, eq_matrix):
    matrix = []
    for row, eq in zip(row_matrix, eq_matrix):
      if eq[0]=='>=': row = [str(-x) for x in row]
      else: row = [str(x) for x in row]
      matrix.extend(row)
    return ','.join(matrix)

if __name__=="__main__":
  c2e2=Main()
  c2e2.main()

