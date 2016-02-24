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

#verifLog.setLevel()

F1 = logging.FileHandler('../wd/.c2e2verification.log')
F1.setLevel(logging.DEBUG)
verifLog.setLevel(logging.DEBUG)
F1.setFormatter(logging.Formatter('%(asctime)s - %(levelname)s - %(message)s'))
verifLog.addHandler(F1)

verifLog.info('Launching the logger')
Global_Linear = 0
Global_Refine = 0
Global_Simulation =0
#verifLog.basicConfig(filename='../wd/.c2e2verification.log', level=verifLog.DEBUG, format='%(asctime)s %(message)s')  


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
    
    #verifLog.info('Started!')
    
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
    Global_Linear = 0
    global Global_Simulation
    Global_Simulation = 0

    fileName,fileExtension=os.path.splitext(os.path.basename(fileChoosen))
    self.set_title("C2E2: "+fileName)
    propList=[]
    typeInput = 0
    paramList = []
    if fileExtension==".hyxml":
      hybridRep,propList=hyirXML(fileChoosen)
      typeInput = 1
      for prop in propList:
          if prop.paramData[0] != 0 and prop.paramData[1] != 0 and prop.paramData[2] != 0 and prop.paramData[3] != 0 :
              paramList = [str(prop.paramData[0]),str(prop.paramData[1]),str(prop.paramData[2]),str(prop.paramData[3])]

      
    else:
      typeInput = 2  
      model=open(fileChoosen,"r")
      rawModel=model.read()
      x=re.search(r"Stateflow {",rawModel)
      sf_data=rawModel[x.start():]
      sf_tree=extract_sf(sf_data)
      if IsHierarchical(sf_tree):
        sf_tree=RemoveHierarchy(sf_tree)
      hybridRep=hyirMdl(sf_tree,fileChoosen)
      # self.hybridRep.remove_nd()
      # Processing the annotations now.
      for annotString in hybridRep.annotationsRaw:
          print("annotation find")
          annotMode = 0; K = 2000; gamma = 0; annotType = 1;
          splits = string.split(annotString, ';')
          for substrings in splits:
              match = re.search('mode=.*',substrings)
              if not match == None:
                  matchedString = match.group(0)
                  modeString = matchedString[5:]
                  modeIndex=0;
                  for mode in hybridRep.automata[0].modes:
                      modeIndex+=1
                      if mode.name == modeString:
                          annotMode = modeIndex
                          #print " annot mode is " + str(annotMode)
#                  print "split and searched"
#                  print match.group(0);
              
              match = re.search('k=.*',substrings)
              if not match == None:
                  Kstring = match.group(0)[2:]
                  K = float(Kstring)

#                  print " K string is " + Kstring + " k is "+ str(K)
              
              match = re.search('gamma=.*',substrings)
              if not match == None:
                  GammaString = match.group(0)[6:]
                  gamma = float(GammaString) 

#                  print " gamma string is " + GammaString + " k is "+ str(gamma)
              
              match = re.search('type=.*',substrings)
              if not match == None:
                  TypeString = match.group(0)[5:]
                  if TypeString == "exponential" :
                      annotType = 1
                  if TypeString == "linear" :
                      annotType = 2
                  if TypeString == "contraction" :
                      annotType = 3        
#              print "splitted"
#              print substrings
              
          newAnnot = [annotMode,K,gamma,annotType]
          hybridRep.annotationsParsed += [newAnnot]
          
          print hybridRep.annotationsParsed 
          
          print " sequence "
          print annotString

      if hybridRep.annotationsRaw == []:
        print ("No annotation find")
        annotMode = 1; K = 2000; gamma = 0; annotType = 1;
        #newAnnot = [annotMode,K,gamma,annotType]
        modenum = hybridRep.modesnumber()
        for i in range (modenum):
          newAnnot = [annotMode,K,gamma,annotType]
          hybridRep.annotationsParsed += [newAnnot]
          annotMode+=1


    dupHybridRep = hybridRep
    verifLog.info('Model is \n' + hybridRep.convertToXML([]))
    dupHybridRep.printGuardsResets()
    dupHybridRep.printInvariants()
    #dupHybridRep.convertToCAPD("simulator")
    #generate default Simulator
    st = 'constant'
    path = '../Hylink/simulator.cpp'
    gen_simulator(path, dupHybridRep, step_type=st)

    parseTree,vList,mList=hybridRep.display_system("System")
    
    if typeInput == 1 :
        self.modelNotebook=ModelNotebook(parseTree,hybridRep,propList,vList,mList,paramList)
    if typeInput == 2 :
        self.modelNotebook=ModelNotebook(parseTree,hybridRep,propList,vList,mList,None)

    #self.ModelNotebook.propertiesFrame.disableAllButtons()
    arguments1 = ['mv', 'guardGen.cpp', 'Invcheck.cpp', 'simulator.cpp', '../wd/']
    subp1 = subprocess.Popen(arguments1)
    subp1.wait()

  
    arguments = ['sh', './compileAndExecute', "0"]
    subp = subprocess.Popen(arguments,cwd="../wd/")
    subp.wait()


    self.fileLabel.hide()
    self.windowVBox.pack_start(self.modelNotebook)
    #print("displayed")
    

    
#    arguments1 = ['mv', 'guardGen.cpp', 'Invcheck.cpp', 'simulator.cpp', '../wd/']
#    subp1 = subprocess.Popen(arguments1); 
#    
#    arguments = ['sh', './compileAndExecute']
#    subp = subprocess.Popen(arguments,cwd="../wd/")
#
#
#
#    
#    while subp.poll() == None:
#      if not subp.poll()==None:
#        break
    

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
      verifLog.info(' File opened ' + fileName)
      self.loadFileChoosen(fileName)
      self.fileOpened=True

    #FIXME

   
    openDialog.destroy()
    
    
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
      if not value and self.fileSaved=="" or value:
        saveDialog=gtk.FileChooserDialog("Save File",self,gtk.FILE_CHOOSER_ACTION_SAVE,(gtk.STOCK_CANCEL,gtk.RESPONSE_CANCEL,gtk.STOCK_SAVE,gtk.RESPONSE_OK)) 
        filter=gtk.FileFilter()
        filter.set_name("Models")
        filter.add_pattern("*.hyxml")
        saveDialog.add_filter(filter)
        saveDialog.set_select_multiple(False)
        response=saveDialog.run()
        if response==gtk.RESPONSE_OK:
          self.fileSaved=saveDialog.get_filename()
          if not ".hyxml" in self.fileSaved:
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
    self.propertyListLen=0
    self.editListLen=0
    self.varList=varList
    self.modeList=modeList
    if paramsData == None or paramsData == []:
      self.paramData=[["Partitioning:","0.2",1],["Time-step:","0.01",1],["Time horizon:","10.0",1],
                    ["Taylor model order:","10",1]]
    if paramsData != None and paramsData != []:
      self.paramData=[["Partitioning:",paramsData[0],1],["Time-step:",paramsData[1],1],["Time horizon:",paramsData[2],1],
                    ["Taylor model order:",paramsData[3],1]]
    self.parseTree=parseTree
    self.verifyingPlotting=[False]
    self.hybridRep=hybridRep

    for prop in propList:
      self.propertyList.append([prop])
      self.propertyListLen+=1

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
      if i==len(self.paramData)-1:
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
    linearbutton.connect("toggled", self.linarcallback, "Linear Model")
    paramHBox1.pack_start(linearbutton,True,True,2)

    SimulationButton = gtk.CheckButton("Simulation")
    SimulationButton.connect("toggled", self.simulationcallback, "Simulation")
    paramHBox1.pack_start(SimulationButton,True,True,2)

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


    #linearbutton.show()

    self.add(self.paramVBox)


  def changed_refine_cb(self,combobox):
    index = combobox.get_active()
    global Global_Refine
    Global_Refine = index


  def changed_cb(self,combobox):
    index = combobox.get_active()
    if index != self.prevSimuIndex:
      self.prevSimuIndex = index
      if index == 2:
        self.hybridRep.convertToCAPD("simulator")
        arguments1 = ['mv','simulator.cpp', '../wd/']
        subp1 = subprocess.Popen(arguments1)
        subp1.wait()
        arguments = ['sh', './compileSimulator', "2"]
        subp = subprocess.Popen(arguments,cwd="../wd/")
        subp.wait()
      if index == 0:
        st = 'constant'
        path = '../Hylink/simulator.cpp'
        gen_simulator(path, self.hybridRep, step_type=st)
        arguments1 = ['mv','simulator.cpp', '../wd/']
        subp1 = subprocess.Popen(arguments1)
        subp1.wait()
        arguments = ['sh', './compileSimulator', "0"]
        subp = subprocess.Popen(arguments,cwd="../wd/")
        subp.wait()

      if index == 1:
        st = 'adaptive'
        path = '../Hylink/simulator.cpp'
        gen_simulator(path, self.hybridRep, step_type=st)
        arguments1 = ['mv','simulator.cpp', '../wd/']
        subp1 = subprocess.Popen(arguments1)
        subp1.wait()
        arguments = ['sh', './compileSimulator', "1"]
        subp = subprocess.Popen(arguments,cwd="../wd/")
        subp.wait()


  def linarcallback(self, widget, data):
    global Global_Linear
    Global_Linear = 1- Global_Linear

  def simulationcallback(self, widget, data):
    global Global_Simulation
    Global_Simulation = 1- Global_Simulation


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
    propTable.attach(self.verifyAbortBtn,0,3,2,3,gtk.FILL|gtk.EXPAND,0)
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

  def enableAllButtons(self):
#    self.verifyingPlotting[0]=not True
#    self.rendererToggle.set_activatable(True)
    self.columnToggle.set_property("clickable",True)
#    self.propToggleAll.set_sensitive(True)
    self.addBtn.set_sensitive(True)
    self.editBtn.set_sensitive(True)
    self.copyBtn.set_sensitive(True)
    self.removeBtn.set_sensitive(True)
    self.verifyAbortBtn.set_sensitive(True)

  def enableWidgets(self,val):
    self.verifyingPlotting[0]=not val
    self.rendererToggle.set_activatable(val)
    self.columnToggle.set_property("clickable",val)
    self.propToggleAll.set_sensitive(val)
    self.addBtn.set_sensitive(val)
    self.editBtn.set_sensitive(val)
    self.copyBtn.set_sensitive(val)
    self.removeBtn.set_sensitive(val)
    if val:
      self.verifyAbortBtn.set_label("Verify")
    else:
      self.verifyAbortBtn.set_label("Abort")
  
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
        if verified=="Verified" or verified=="Verified*":
          prop=self.propertyList.get_value(propIter,0)
          if prop.tabChild==None:
            unsafeSet=prop.unsafeSetParsed
            reachSetPath=prop.reachSetPath
            plotWindow=PlotWindow(["time"]+self.varList,self.modeList,unsafeSet,reachSetPath,
                                  prop.paramData[1],prop.paramData[2],self.verifyingPlotting)
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
#       print str(x) + " and " + str(y) + " OK! "
#       if rowData == None:
#         print "rowData is none"
#       if not(rowData == None):
#           print rowData[1].get_title()
      if (prop.status=="Verified*") and ((rowData == None) or not(rowData[1].get_title()=="Result")):
        toolTip.set_text("Partitioning: %s\nTime-step: %s\nTime horizon: %s\nTaylor order: %d"%(prop.paramData[0],prop.paramData[1],prop.paramData[2],prop.paramData[3]))
        return True
      if (prop.status=="Verified" or prop.status=="Verified*") and not (rowData == None) and rowData[1].get_title() == "Result":
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

  """
    verifyAbortBtnCallback
    Description: handles the verification of the selected properties in the list
    Inputs: btn - button clicked
    Outputs: none
    Return: none
  """
  #Change this to be verifyAbort instead
  def verifyAbortBtnCallback(self,btn):
    #print(Global_Linear)
    for param in self.paramData:
      if param[2]==0:
        return

    if btn.get_label()=="Verify" and self.editListLen > 0:

      #self.showGenerationMessage()

      os.system("./DeleteRE.sh")
      
      #self.enableAllButtons()
      #while gtk.events_pending():
      #  gtk.main_iteration()

      self.enableWidgets(False)
      for prop in self.propertyList:
        prop=prop[0]
        if prop.checked and prop.status=="Not verified":

          pathString="../wd/ReachSet"+prop.name
          
          verifLog.info(' Verification started - property name - ' + prop.name + ' - ')
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
          global Global_Simulation
          c2e2String+= "simuflag = \""+str(Global_Simulation)+"\"\n"

          
          verifLog.info(' partiotion - ' + delta + ' - time-step - ' + tstep + ' - time-horizon - ' + thoriz + ' - ')
          
          numInitEqns = len(initialIneqs)
          c2e2String+= "init-eqns=\""+str(numInitEqns)+"\"\n"
          c2e2String+= "init-matrix=["
          commaCount = 0;
          for j in range(0,numInitEqns):
              multiplicity = 1;
              rowMatrix = initialMatrix[j]
              rowDir = initialIneqs[j][0]
              
              if rowDir == '>=':
                  multiplicity = -1
                  
              for number in rowMatrix:
                  if commaCount == 0:
                      c2e2String += str(multiplicity*number)
                      commaCount+=1
                  else:
                      c2e2String += ","+str(multiplicity*number)
          
          c2e2String += "]\n"
          c2e2String += "init-b=["
          commaCount = 0
          for j in range(0,numInitEqns):
              multiplicity = 1;
              number = initialB[j][0]
              rowDir = initialIneqs[j][0]
              
              if rowDir == '>=':
                  multiplicity = -1
              
              if commaCount == 0:
                  c2e2String += str(multiplicity*number)
                  commaCount+=1
              else:
                  c2e2String += ","+str(multiplicity*number)
          
          c2e2String += "]\n"        
              
          numUnsafeEqns = len(unsafeIneqs)
          c2e2String+= "unsafe-eqns=\""+str(numUnsafeEqns)+"\"\n"
          c2e2String+= "unsafe-matrix=["
          commaCount = 0;
          for j in range(0,numUnsafeEqns):
              multiplicity = 1;
              rowMatrix = unsafeMatrix[j]
              rowDir = unsafeIneqs[j][0]
              
              if rowDir == '>=':
                  multiplicity = -1
                  
              for number in rowMatrix:
                  if commaCount == 0:
                      c2e2String += str(multiplicity*number)
                      commaCount+=1
                  else:
                      c2e2String += ","+str(multiplicity*number)
          
          c2e2String += "]\n"
          c2e2String += "unsafe-b=["
          commaCount = 0
          for j in range(0,numUnsafeEqns):
              multiplicity = 1;
              number = unsafeB[j][0]
              rowDir = unsafeIneqs[j][0]
              
              if rowDir == '>=':
                  multiplicity = -1
              
              if commaCount == 0:
                  c2e2String += str(multiplicity*number)
                  commaCount+=1
              else:
                  c2e2String += ","+str(multiplicity*number)
          
          c2e2String += "]\n"
       

          mode_num  = 1
          if len(self.hybridRep.annotationsParsed) == len(self.modeList) :
              for array in self.hybridRep.annotationsParsed :
                  c2e2String+= "annotation-mode=\""+str(array[0])+"\"\n"
                  if array[3] == 1 or array[3] == 3 :
                      c2e2String+= "annotation-type=\"contraction\"\n"
                  if array[3] == 2 :
                      c2e2String+= "annotation-type=\"linear\"\n"
                  c2e2String+= "annotation=\'dx1^2 + dx2^2\'\n"
                  c2e2String+= "beta=\'dx1^2 + dx2^2\'\n"
                  if (Global_Linear):
                      filename = ""
                      filename +="../wd/jacobiannature"
                      filename += str(mode_num)
                      filename +=".txt"
                      fid = open(filename,'r').read().split('\n')
                      numofvar = int(fid[0])
                      #print numofvar
                      listofvar = []
                      for i in range (numofvar):
                        listofvar.append(fid[i+1])
                      #rint listofvar
                      equationpos = 1+numofvar
                      numofeq = int(fid[equationpos])
                      equationpos+=1
                      listofeq = []
                      for i in range (numofeq):
                        listofeq.append(fid[equationpos+i])
                      #print listofeq
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
                      #print(codestring)
                      exec(codestring)
                      Constant_Jacobian = jcalc(listofvar,np.ones((1,numofvar))[0])
                      Constant_Jacobian = np.reshape(Constant_Jacobian,(numofvar,numofvar))
                      gamma_rate = la.eigvals(Constant_Jacobian).real
                      array[2] = max(gamma_rate)
                      if abs(max(gamma_rate)) < 0.00001:
                        array[2] = 0                   
                      array[1] = la.norm(Constant_Jacobian)
                  c2e2String+= "k=\""+str(array[1])+"\"\n"
                  c2e2String+= "gamma=\""+str(array[2])+"\"\n"
                  c2e2String+= "is_linear=\""+str(Global_Linear)+"\"\n"
                  mode_num = mode_num+1
          
          else:
              i = 0
              while i < len(self.modeList) :
                  c2e2String+= "annotation-mode=\""+str(i+1)+"\"\n"
                  c2e2String+= "annotation-type=\"contraction\"\n"
                  c2e2String+= "annotation=\'dx1^2 + dx2^2\'\n"
                  c2e2String+= "beta=\'dx1^2 + dx2^2\'\n"
                  c2e2String+= "k=\"1.1\"\n"
                  c2e2String+= "gamma=\"0.0\"\n"
                  i+=1   
        

            
          c2e2String+= "visualize all to ReachSet"+prop.name+"\n"
          
          filenamestring = "ReachSet"+prop.name
          os.system("rm ../wd/"+filenamestring)
          writer = open("Configuration-C2E2","w")
          writer.write(c2e2String)
          writer.close()
          
          arguments1 = ['mv', 'Configuration-C2E2', '../wd/']
          subp1 = subprocess.Popen(arguments1); 
    
          arguments = ['sh', './ExecuteC2E2']
          self.subp = subprocess.Popen(arguments,cwd="../wd/",preexec_fn=os.setsid)

          #Might need a variable here to check if the process was aborted
          while self.subp.poll() == None:
              prop.status="Verifying"
              self.propertyList.row_changed(prop.index,self.propertyList.get_iter(prop.index))
              while gtk.events_pending():
                gtk.main_iteration()
              time.sleep(0.5) 
              if not self.subp.poll()==None:
                break

              prop.status="Verifying."
              self.propertyList.row_changed(prop.index,self.propertyList.get_iter(prop.index))
              while gtk.events_pending():
                gtk.main_iteration()
              time.sleep(0.5) 
              if not self.subp.poll()==None:
                break

              prop.status="Verifying.."
              self.propertyList.row_changed(prop.index,self.propertyList.get_iter(prop.index))
              while gtk.events_pending():
                gtk.main_iteration()
              time.sleep(0.5) 
              if not self.subp.poll()==None:
                break

              prop.status="Verifying..."
              self.propertyList.row_changed(prop.index,self.propertyList.get_iter(prop.index))
              while gtk.events_pending():
                gtk.main_iteration()
              time.sleep(0.5)
              if not self.subp.poll()==None:
                break

    
          checkfilename = "../wd/ReachSet"+prop.name

          self.subp=None
          if self.abortedVerifying==False:
            prop.status="Verified"
            with open("../wd/Result.dat",'r') as f:
              res=f.readline()
              print "This is the result read " + res
              res = int(res)
              print "This is the result read " + str(res)
              if res<0:
                prop.verifResult="Unsafe"
              elif res==0:
                prop.verifResult="Unknown"
              else:
                if os.stat(checkfilename).st_size == 0:
                  prop.verifResult="ERROR"
                else:
                  prop.verifResult="Safe"
            prop.newTabPixbuf=gtk.gdk.pixbuf_new_from_file("./Resources/newTab.png").scale_simple(16,16,
                                                           gtk.gdk.INTERP_HYPER)
            prop.reachSetPath=pathString
            
            verifLog.info(' Verification Result Returned - ' + prop.verifResult + ' - ')
            
            self.propertyList.row_changed(prop.index,self.propertyList.get_iter(prop.index))
          else:
            self.abortedVerifying=False
            prop.status="Not verified"
            
            verifLog.info(' Verification Aborted ')

            self.propertyList.row_changed(prop.index,self.propertyList.get_iter(prop.index))
            print("Breaking")
            break;

      self.enableWidgets(True)
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
      verifLog.info(' Verification completely aborted ')
    
if __name__=="__main__":
  c2e2=Main()
  c2e2.main()

