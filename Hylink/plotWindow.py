import pygtk
pygtk.require('2.0')
import gtk,gobject,os,time,Queue,threading
import multiprocessing
from plotDialog import PlotDialog,PlotDatum
from plotter import plotGraph
from matplotlib.backends.backend_gtkagg import FigureCanvasGTKAgg as FigureCanvas
from matplotlib.backends.backend_gtkagg import NavigationToolbar2GTKAgg as NavigationToolbar

import logging

plotterLog = logging.getLogger('c2e2VerificationLog')


"""
  PlotWindow
  Description: creates the interface that allows the user to plot/interact with graphs
"""
class PlotWindow(gtk.Table):
  """
    __init__
    Description: initializes GUI, lists for data, and other variables
    Inputs: varList - list of the variables in the model
            unsafeSet - parsed unsafe set data
            reachSetPath - the file path of the reachSet data
    Outputs: none
    Return: none
  """
  def __init__(self,varList,modeList,unsafeSet,reachSetPath,timeStep,timeHoriz,verifyingPlotting):
    gtk.Table.__init__(self,1,10,True)
    
    plotterLog.info(' Started the plotter ')
    #plotList: (checked,name,status,plotTitle,xLabel,yLabelList,xVarIndex,yVarIndexList,index,
    #           iconDatum)
    self.plotList=gtk.ListStore(gobject.TYPE_PYOBJECT)
    self.plotListFilter=self.plotList.filter_new()
    self.plotListFilter.set_visible_func(self.showRow)

    #iconList: (iconImage,iconName,figure,dispWindow)
    self.iconList=gtk.ListStore(gtk.gdk.Pixbuf,gobject.TYPE_STRING,gobject.TYPE_PYOBJECT,
                                gobject.TYPE_PYOBJECT)
    self.varList=varList
    self.modeList=modeList
    self.unsafeSet=unsafeSet
    self.reachSetPath=reachSetPath
    self.timeStep=timeStep
    self.timeHoriz=timeHoriz
    self.plotListLen=0
    self.editListLen=0

    if self.varList==None:
      self.varList=["t","x","y"]

    self.connect("check-resize",self.windowResized)

    self.plotView=PlotView(self.plotList,self.iconList)
    self.plotFrame=PlotFrame(self.plotList,self.plotListFilter,self.plotListLen,self.editListLen,
                             self.iconList,self.varList,self.modeList,self.reachSetPath,
                             self.unsafeSet,self.timeStep,self.timeHoriz,verifyingPlotting)

    self.set_col_spacing(6,5)
    self.attach(self.plotView,0,7,0,1)
    self.attach(self.plotFrame,7,10,0,1)

    self.show_all()
    self.plotFrame.showToggleAll()

  """
    windowResized: Checks if the window has been resized and updates the icon view to display 
                   correctly
    Inputs: window - window that is being resized
    Outputs: none
    Return: none
  """
  def windowResized(self,window):
    newWindowSize=self.get_size()
    if not newWindowSize==self.size:
      self.iconView.set_model(None)
      self.iconView.set_model(self.iconList)
      self.size=newWindowSize

  """
    showRow
    Description: prevents a plot from being displayed while it is being added
    Inputs: model - tree model
            iter - tree iter pointing to the row in the model
    Outputs: none
    Return: none
  """
  def showRow(self,model,iter):
    plot=model.get_value(iter,0)
    if not plot==None and not plot.name=="":
      return True 
    else:
      return False

"""
  PlotFrame
  Description: creates the right side portion of the interface where the user create new plots and
               select various options for the plots
"""
class PlotFrame(gtk.Frame):
  """
    __init__
    Description: initializes GUI
    Inputs: plotList - list of the plots and associated data
            plotListFilter - filter for the plot list
            plotListLen - length of the plot list
            editListLen - number of plots that user has selected
            iconList - list of icons currently displayed in the icon view
            varList - list of variables in the model
            reachSetPath - the file path of the reachSet data
            unsafeSet - parsed unsafe set data
    Outputs: plotList,plotListLen,editListLen,iconList
    Return: None
  """
  def __init__(self,plotList,plotListFilter,plotListLen,editListLen,iconList,varList,modeList,
               reachSetPath,unsafeSet,timeStep,timeHoriz,verifyingPlotting):
    gtk.Frame.__init__(self,"Plotter")
    self.plotList=plotList
    self.plotListFilter=plotListFilter
    self.plotListLen=plotListLen
    self.editListLen=editListLen
    self.iconList=iconList
    self.varList=varList
    self.modeList=modeList
    self.reachSetPath=reachSetPath
    self.unsafeSet=unsafeSet
    self.timeStep=timeStep
    self.timeHoriz=timeHoriz
    self.pStop=None
    self.abortedPlotting=False
    self.verifyingPlotting=verifyingPlotting

    self.initPlotFrame()

  """
    initPlotFrame
    Description: initializes the list where user can create/select/modify/remove plots
    Inputs: none
    Outputs: none
    Return: none
  """
  def initPlotFrame(self):
    plotTable=gtk.Table(4,1,False)
    plotHBox=gtk.HBox(True,0)

    self.addBtn=gtk.Button("Add")
    self.addBtn.connect("clicked",self.addEditBtnCallback)
    plotHBox.pack_start(self.addBtn,True,True,0)

    self.editBtn=gtk.Button("Edit")
    self.editBtn.connect("clicked",self.addEditBtnCallback)
    plotHBox.pack_start(self.editBtn,True,True,0)

    self.copyBtn=gtk.Button("Copy")
    self.copyBtn.connect("clicked",self.copyBtnCallback)
    plotHBox.pack_start(self.copyBtn,True,True,0)

    self.removeBtn=gtk.Button("Remove")
    self.removeBtn.connect("clicked",self.removeBtnCallback)
    plotHBox.pack_start(self.removeBtn,True,True,0)

    listSW=gtk.ScrolledWindow()
    listSW.set_policy(gtk.POLICY_AUTOMATIC,gtk.POLICY_AUTOMATIC)
    self.listView=gtk.TreeView(self.plotListFilter)
    listSelection=self.listView.get_selection()
    listSelection.set_mode(gtk.SELECTION_NONE)
    listSW.add(self.listView)

    self.rendererToggle=gtk.CellRendererToggle()
    self.rendererToggle.set_property("activatable",True)
    self.rendererToggle.connect("toggled",self.plotToggleCallback)

    self.plotToggleAll=gtk.CheckButton()
    self.plotToggleAll.connect("toggled",self.plotToggleAllCallback)
    self.plotToggleAll.set_sensitive(False)
    self.columnToggle=gtk.TreeViewColumn("Checked",self.rendererToggle)
    self.columnToggle.set_cell_data_func(self.rendererToggle,self.setToggleColumn)
    self.columnToggle.set_property("clickable",True)
    self.columnToggle.connect("clicked",self.toggleToggleAll,self.plotToggleAll)
    self.columnToggle.set_widget(self.plotToggleAll)
    self.columnToggle.set_sizing(gtk.TREE_VIEW_COLUMN_AUTOSIZE)
    self.listView.append_column(self.columnToggle)

    rendererText=gtk.CellRendererText()
    columnText=gtk.TreeViewColumn("Plots",rendererText)
    columnText.set_cell_data_func(rendererText,self.setTextColumn,"name")
    columnText.set_sizing(gtk.TREE_VIEW_COLUMN_AUTOSIZE)
    self.listView.append_column(columnText)

    rendererText=gtk.CellRendererText()
    columnText=gtk.TreeViewColumn("Status",rendererText)
    columnText.set_cell_data_func(rendererText,self.setTextColumn,"status")
    columnText.set_sizing(gtk.TREE_VIEW_COLUMN_AUTOSIZE)
    self.listView.append_column(columnText)

    plotTable.attach(listSW,0,1,0,1)
    plotTable.attach(plotHBox,0,1,1,2,gtk.FILL|gtk.EXPAND,0)

    self.plotBtn=gtk.Button("Plot")
    self.plotBtn.connect("clicked",self.plotBtnCallback)
    plotTable.attach(self.plotBtn,0,1,2,3,gtk.FILL,0)

    self.plotStatus=gtk.Label("Status: Ready")
    self.plotStatus.set_alignment(0,.5)
    plotTable.attach(self.plotStatus,0,1,3,4,gtk.FILL,0)

    self.add(plotTable)

  """
    setToggleColumn
  """
  def setToggleColumn(self,column,cell,model,iter):
    plot=model.get_value(iter,0)
    cell.set_property("active",plot.checked)
    
  """
    setTextColumn
  """
  def setTextColumn(self,column,cell,model,iter,textType):
    plot=model.get_value(iter,0)
    if textType=="name":
      cell.set_property("text",plot.name)
    else:
      cell.set_property("text",plot.status)

  """
    toggleToggleAll
    Description: controls the click even of the toggle column; 
    Inputs: column - column whose header was clicked
            togBtn - toggle button displayed in the header
    Outputs: none
    Return: None
    Note: program can't focus on the toggle button in the header so need to go through the header
          click events instead; bug in PyGTK
  """
  def toggleToggleAll(self,column,togBtn):
    if togBtn.get_sensitive()==True:
      if togBtn.get_active()==True:
        togBtn.set_focus_on_click(True)
        togBtn.set_active(False)
      else:
        togBtn.set_focus_on_click(True)
        togBtn.set_active(True)
    
  """
    showToggleAll
    Description: shows the toggle button in the header
    Inputs: none
    Outputs: none
    Return: none
    Note: the toggle button in the header won't appear unless it is called specifically shown 
          after the whole window is shown; bug in PyGTK
  """
  def showToggleAll(self):
    self.plotToggleAll.show()

  def enableWidgets(self,val):
    self.verifyingPlotting[0]= not val
    self.rendererToggle.set_activatable(val)
    self.columnToggle.set_property("clickable",val)
    self.plotToggleAll.set_sensitive(val)
    self.addBtn.set_sensitive(val) 
    self.editBtn.set_sensitive(val) 
    self.copyBtn.set_sensitive(val)
    self.removeBtn.set_sensitive(val) 
    if val:
      self.plotBtn.set_label("Plot")
    else:
      self.plotBtn.set_label("Abort")


  """
    plotToggleCallback
    Description: controls the toggling of toggle buttons in the list view
    Inputs: cell - cell in the list view
            path - path of the toggle toggled in the list
    Outputs: none
    Return: none
  """
  def plotToggleCallback(self,cell,path):
    plot=self.plotList[path][0]
    plot.checked=not plot.checked
    if plot.checked:
      self.editListLen+=1
      if self.editListLen==self.plotListLen:
        self.plotToggleAll.set_active(True)
    else:
      self.editListLen-=1
      self.plotToggleAll.set_active(False)

  """
    plotToggleAllCallback
    Description: control the toggling of the toggle button in the column header
    Inputs: togBtn - toggle button being toggled
    Outputs: none
    Return: none
    Note: need to refilter the plot so that the changes are visible in the list
  """
  def plotToggleAllCallback(self,togBtn):
    if togBtn.get_active():
      for p in self.plotList:
        p[0].checked=True
      self.editListLen=self.plotListLen
    else:
      if self.editListLen==self.plotListLen:
        for p in self.plotList:
          p[0].checked=False
        self.editListLen=0
    self.plotListFilter.refilter()
    
  """
    addEditBtnCallback
    Description: handles the adding/editting of plots in the list view
    Inputs: btn - button being clicked
    Outputs: none
    Return: none
    Note: need to refilter the list so that the plot will actually be displayed in the list
  """
  def addEditBtnCallback(self,btn):
    if btn.get_label()=="Add" or btn.get_label()=="Edit" and self.editListLen>0:
      removePlotList=[]
      editPlotList=[]
      plotDialog=PlotDialog(self.get_toplevel(),btn.get_label(),self.plotList,self.plotListLen,
                            self.varList,removePlotList,editPlotList)
      response=plotDialog.run()
      if response==gtk.RESPONSE_OK:
        if btn.get_label()=="Add":
          self.plotListLen+=1
          self.plotToggleAll.set_active(False)
          self.plotToggleAll.set_sensitive(True)
        else:
          for r in removePlotList:
            remIter=r[1].iter
            dispWindow=self.iconList.get_value(remIter,3)
            if not dispWindow==None:
              dispWindow.destroy()
            self.iconList.remove(remIter)
            self.plotList.get_value(self.plotList.iter_nth_child(None,r[0]),0).iconDatum=None
          for e in editPlotList:
            editIter=e[1].iter
            dispWindow=self.iconList.get_value(editIter,3)
            if not dispWindow==None:
              dispWindow.destroy()
            self.iconList.set_value(editIter,1,e[0])

      plotDialog.destroy()
      self.plotListFilter.refilter()

  """
    copyBtnCallback
  """
  def copyBtnCallback(self,btn):
    if self.editListLen<=0:
      return

    plotListNames=[]
    plotListIndices=[]
    for plot in self.plotList:
      plotListNames.append(plot[0].name)
      if plot[0].checked:
        plotListIndices.append(plot[0].index)

    for idx in plotListIndices:
      plot=self.plotList.get_value(self.plotList.iter_nth_child(None,idx),0)
      newPlot=PlotDatum(self.plotListLen)
      newPlot.copyDatum(plot,plotListNames)
      self.plotList.append([newPlot])
      self.plotListLen+=1
      self.plotToggleAll.set_active(False)

  """
    removeBtnCallback
    Description: handles the removing of plots in the list view
    Inputs: btn - button being clicked
    Outputs: none
    Return: none
  """
  def removeBtnCallback(self,btn):
    if self.editListLen>0:
      removePlotList=[]
      for plot in self.plotList:
        plot=plot[0]
        if plot.checked:
          plotIter=self.plotList.iter_nth_child(None,plot.index)
          removePlotList.append([plotIter,plot.iconDatum]) 

      for rem in reversed(removePlotList):
        if self.plotList.get_value(rem[0],0).status=="Plotted":
          remIter=rem[1].iter
          dispWindow=self.iconList.get_value(remIter,3)
          if not dispWindow==None:
            dispWindow.destroy()
          self.iconList.remove(remIter)
        self.plotList.remove(rem[0])
      self.plotListLen=len(self.plotList)
      self.editListLen=0
      self.plotToggleAll.set_active(False)
      if self.plotListLen==0:
        self.plotToggleAll.set_sensitive(False)

      reindex=0
      for plot in self.plotList:
        plot[0].index=reindex
        reindex+=1

  """
    plotBtnCallback
    Description: handles the plotting of the plots in the list view
    Inputs: btn - button being clicked
    Outputs: none
    Return: none
  """
  def plotBtnCallback(self,btn):
    if self.editListLen<=0:
      return

    if btn.get_label()=="Plot":
      self.enableWidgets(False)
            
      for plot in self.plotList:  
        plot=plot[0]
        plotterLog.info(' Started Plotting - ' + plot.name + ' - ')

        if plot.checked and plot.status=="Not plotted":
          taskQueue=Queue.Queue()
          figureQueue=Queue.Queue()

          #Fix this part of the graph
          progress=[0]

          axes=tuple([plot.xVarIndex]+plot.yVarIndexList)
          self.pStop=threading.Event()
          taskQueue.put([plotGraph,(self.pStop,self.reachSetPath,self.unsafeSet,self.varList,self.modeList,
                        axes,plot.dispMode,self.timeStep,self.timeHoriz,self.plotStatus,plot.plotTitle,plot.name,plot.xLabel,plot.yLabelList)])

          p=threading.Thread(target=self.worker,args=(taskQueue,figureQueue))
          p.start()
          while p.is_alive():
            plot.status="Plotting"
            self.plotList.row_changed(plot.index,self.plotList.get_iter(plot.index))
            while gtk.events_pending():
              gtk.main_iteration()
            time.sleep(0.5)
            if not p.is_alive():
              break

            plot.status="Plotting."
            self.plotList.row_changed(plot.index,self.plotList.get_iter(plot.index))
            while gtk.events_pending():
              gtk.main_iteration()
            time.sleep(0.5)
            if not p.is_alive():
              break

            plot.status="Plotting.."
            self.plotList.row_changed(plot.index,self.plotList.get_iter(plot.index))
            while gtk.events_pending():
              gtk.main_iteration()
            time.sleep(0.5)
            if not p.is_alive():
              break

            plot.status="Plotting..."
            self.plotList.row_changed(plot.index,self.plotList.get_iter(plot.index))
            while gtk.events_pending():
              gtk.main_iteration()
            time.sleep(0.5)
            if not p.is_alive():
              break

          if self.abortedPlotting==False:
            pixbuf=gtk.gdk.pixbuf_new_from_file("./../wd/plotresult/"+plot.name+".png")
            scaled_buf = pixbuf.scale_simple(150,150,gtk.gdk.INTERP_BILINEAR)
            iconIndex=len(self.iconList)
            self.iconList.insert(iconIndex,[scaled_buf,plot.name,figureQueue.get(),None])
            plot.status="Plotted"
            
            plotterLog.info(' Finished Plotting ')
            
            plot.iconDatum=self.iconList[iconIndex]
            self.plotList.row_changed(plot.index,self.plotList.get_iter(plot.index))
          else:
            self.abortedPlotting=False
            plot.status="Not plotted"
            
            plotterLog.info(' Aborted Plotting ')
            
            self.plotList.row_changed(plot.index,self.plotList.get_iter(plot.index))
            print("Breaking")
            break;

      self.enableWidgets(True)

    else:
      self.pStop.set()
      self.abortedPlotting=True

    #I can set the label after the end of the if statement
    self.plotStatus.set_label("Status: Ready")
    while gtk.events_pending():
      gtk.main_iteration()

  """
    worker
  """
  def worker(self,inputQueue,outputQueue):
    func,args=inputQueue.get()
    figure=func(*args)
    if self.abortedPlotting==False:
      outputQueue.put(figure)

"""
  PlotView
  Description: initializes the plot view that holds the icons of the various plots the user selected
               in the plot list
"""
class PlotView(gtk.ScrolledWindow):
  """
    __init__
    Description: initialized the GUI
    Inputs: plotList - list of plots and associated data
            iconList - list of the icons
    Outputs: none
    Return: none
  """
  def __init__(self,plotList,iconList):
    gtk.ScrolledWindow.__init__(self)
    self.set_policy(gtk.POLICY_NEVER,gtk.POLICY_AUTOMATIC)
    self.plotList=plotList
    self.iconList=iconList
    self.initPlotView() 

  """
    initPlotView
    Description: initializes the icon view
    Inputs: none
    Outputs: none
    Return: none
  """
  def initPlotView(self):
    self.iconView=gtk.IconView()

    self.iconView.set_model(self.iconList)
    self.iconView.set_pixbuf_column(0)
    self.iconView.set_text_column(1)
    self.iconView.connect("item-activated",self.iconCallback,)

    self.add_with_viewport(self.iconView)
    self.get_child().set_shadow_type(gtk.SHADOW_NONE)
    self.iconView.show_all()
  
  """
    iconCallback
    Description: displays the plot shown in the icon in a separate window
    Inputs: view - icon that was clicked
            path - path to the icon
    Outputs: none
    Return: none
  """
  def iconCallback(self,view,path):
    print(path)
    iconItem=self.iconList[path[0]]
    plotIndex=-1
    for i,plot in enumerate(self.plotList):
      if iconItem in plot:
        plotIndex=i
    dispWindow=DispWindow(self.get_toplevel(),iconItem[2],self.plotList[plotIndex][0])
    self.iconList.set_value(self.iconList.get_iter(path[0]),3,dispWindow)
    dispWindow.main()

"""
  DispWindow
  Description: window that displays a plot along with its axis
"""
class DispWindow(gtk.Window):
  """
    __init__
    Description: initializes the window and displays the plot
    Inputs: parentWindow - the main window of the whole program
            figure - matplotlib figure to display in the window
            plot - the plot list item with axes information
    Outputs: none
    Return: none
  """
  def __init__(self,parentWindow,figure,plot):
    gtk.Window.__init__(self)
    #self.set_transient_for(parentWindow)
    self.set_keep_above(False)
    self.set_position(gtk.WIN_POS_CENTER)
    self.set_size_request(600,480)
    self.set_title("C2E2 "+figure)
    self.connect("destroy",gtk.main_quit)
    self.set_destroy_with_parent(True)
    #windowVBox=gtk.VBox()

    colors=('b','g','r','c','m')
    img = gtk.image_new_from_file("./../wd/plotresult/"+figure+".png")

    '''
    figure.suptitle(plot.plotTitle)
    numAxes=len(figure.get_axes())
    for idx,axis in enumerate(figure.get_axes()):
      if idx==0:
        axis.set_xlabel(plot.xLabel)
      if numAxes==1:
        axis.set_ylabel(plot.yLabelList[idx])
      else:
        axis.set_ylabel(plot.yLabelList[idx],color=colors[idx])

    canvas=FigureCanvas(figure)
    toolbar=NavigationToolbar(canvas,self)
    windowVBox.pack_start(canvas)
    windowVBox.pack_start(toolbar,False,False,0)
    '''

    self.add(img)
    self.show_all()

  """
    main
    Description: runs and shows the window
    Inputs: none
    Outputs: none
    Return: none
  """
  def main(self):
    gtk.main()
