import ply.yacc as yacc
import ply.lex as lex
#from tree import print_tree, show_tree
from frontend.mod.ModeParse import lstring2
from frontend.mod.EdgeParse import lstring3
from frontend.mod.node import Node

import logging
logging.basicConfig(
    level = logging.DEBUG,
    filename = "sfparselog.txt",
    filemode = "w",
    format = "%(filename)10s:%(lineno)4d:%(message)s"
)
log = logging.getLogger()

class LSLexer:
	tokens = ('STATEFLOW',
	          'LFLBR',
	          'RFLBR',
	          'LSQBR',
	          'RSQBR',
	          'MACHINE',
	          'TARGET',
	          'CHART',
	          'STATE',
	          'TRANSITION',
	          'DATA',
	          'INSTANCE',
	          'ID',
	          'INTEGER',
	          'FLOAT',
	          'NAME',
	          'CREATED',
	          'ISLIBRARY',
	          'FIRSTTARGET',
	          'SFVERSION',
	          'STRING',
	          'WINDOWPOSITION',
	          'VIEWLIMITS',
	          'LABELSTRING',
	          'POSITION',
	          'FONTSIZE',
	          'LABELPOSITION',
	          'SSIDNUMBER',
	          'LINKNODE',
	          'DESCRIPTION',
	          'ZOOMFACTOR',
	          'SCREEN',
	          'TREENODE',
	          'FIRSTTRANSITION',
	          'VIEWOBJ',
	          'VISIBLE',
	          'X1',
	          'Y1',
	          'SSIDHIGHWATERMARK',
	          'DECOMPOSITION',
	          'FIRSTDATA',
	          'SUBVIEWS',
	          'DECVAL',
	          'UPDATEMETHOD',
	          'CHARTFILENUMBER',
	          'USERSPECIFIEDSTATETRANSITIONEXECUTIONORDER',
	          'DISABLEIMPLICITCASTING',
	          'SUBVIEWER',
	          'TYPE',
	          'SRC',
	          'DST',
	          'INTERSECTION',
	          'MIDPOINT',
	          'DATALIMITS',
	          'DRAWSTYLE',
	          'SLIDE',
	          'EXECUTIONORDER',
	          'SCOPE',
	          'PROPS',
	          'DATATYPE',
	          'PRIMITIVE',
	          'ISSIGNED',
	          'WORDLENGTH',
	          'UDATEVAL',
	          'TYPEVAL',
	          'PRIMVAL',
	          'SCPVAL',
	          'UMVAL',
	          'DSTYVAL',
	          'PLANTMODELINGINFO',
	          'MIDPOINTSTICKY',
	          'FRAME',
	          'FRMVAL',
	          'SANGVAL',
	          'STAMPANGLE',
	          'SMVAL',
	          'STATEMACHINETYPE',
	          'MODE',
	          'STICKY',
	          'MODEVAL',
	          'STICKYVAL',
	          'METHOD',
	          'MTDVAL',
	          'ARROWSIZE',
	          'FIRSTTARGET2',
	          'RUNTIMECHECK',
	          'DEBUG',
	          'SUPPORTVARIABLESIZING',
	          'CODEFLAGS',
	          'CHECKSUMOLD',
	          'SFDEMOCHECKSUM',
	          'GROUPED',
	          'SUPERSTATE',
	          'NOTEBOX',
	          'ISNOTEBOX'
	)
	
	t_SFDEMOCHECKSUM = r'sfDemoChecksum'
	t_CODEFLAGS = r'codeFlags'
	t_CHECKSUMOLD = r'checksumOld'
	t_SUPPORTVARIABLESIZING = r'supportVariableSizing'
	t_LFLBR = r'\{'
	t_RFLBR = r'\}'
	t_LSQBR = r'\['
	t_RSQBR = r'\]'
	t_ARROWSIZE = r'arrowSize'
	t_STATEFLOW = r'Stateflow'
	t_MACHINE = r'machine'
	t_STATE = r'state'
	t_TARGET = r'target'
	t_TRANSITION = r'transition'
	t_DATA = r'data'
	t_INSTANCE = r'instance'
	t_ID = r'id'
	t_NAME = r'name'
	t_CREATED = r'created'
	t_ISLIBRARY = r'isLibrary'
	t_FIRSTTARGET = r'firstTarget'
	t_FIRSTTARGET2 = r'firsttarget'
	t_SFVERSION = r'sfVersion'
	t_WINDOWPOSITION = r'windowPosition'
	t_VIEWLIMITS = r'viewLimits'
	t_LABELSTRING = r'labelString'
	t_POSITION = r'position'
	t_FONTSIZE = r'fontSize'
	t_LABELPOSITION = r'labelPosition'
	t_DESCRIPTION = r'description'
	t_ZOOMFACTOR = r'zoomFactor'
	t_SCREEN = r'screen'
	t_TREENODE = r'treeNode'
	t_FIRSTTRANSITION = r'firstTransition'
	t_VIEWOBJ = r'viewObj'
	t_VISIBLE = r'visible'
	t_X1 = r'x1'
	t_Y1 = r'y1'
	t_SSIDHIGHWATERMARK = r'ssIdHighWaterMark'
	t_DECOMPOSITION = r'decomposition'
	t_FIRSTDATA = r'firstData'
	t_SUBVIEWS = r'subviewS'
	t_DECVAL = r'CLUSTER_CHART | CLUSTER_STATE | SET_CHART | SET_STATE'
	t_UMVAL = r'CONTINUOUS | INHERITED | DISCRETE'
	t_TYPEVAL = r'OR_STATE | AND_STATE | GROUP_STATE'
	t_DSTYVAL = r'SMART'
	t_PRIMVAL = r'SF_DOUBLE_TYPE | SF_BOOLEAN_TYPE'
	t_UDATEVAL = r'SF_CONTINUOUS_TIME_DATA'
	t_SCPVAL = r'LOCAL_DATA | OUTPUT_DATA | INPUT_DATA'
	t_SANGVAL = r'NaN'
	t_FRMVAL = r'SF_FRAME_NO | SF_FRAME_INHERITED'
	t_SMVAL = r'MEALY_MACHINE | MOORE_MACHINE'
	t_STICKYVAL = r'BOTH_STICK | SRC_STICK'
	t_MODEVAL = r'MIRROR_SLIDE'
	t_MTDVAL = r'SF_INHERITED_TYPE'
	t_UPDATEMETHOD = r'updateMethod'
	t_CHARTFILENUMBER = r'chartFileNumber'
	t_USERSPECIFIEDSTATETRANSITIONEXECUTIONORDER = r'userSpecifiedStateTransitionExecutionOrder'
	t_DISABLEIMPLICITCASTING = r'disableImplicitCasting'
	t_CHART = r'chart'
	t_SUBVIEWER = r'subviewer'
	t_SSIDNUMBER = r'ssIdNumber'
	t_METHOD = r'method'
	t_TYPE = r'type'
	t_SRC = r'src'
	t_DST = r'dst'
	t_INTERSECTION = r'intersection'
	t_MIDPOINT = r'midPoint'
	t_LINKNODE = r'linkNode'
	t_DATALIMITS = r'dataLimits'
	t_DRAWSTYLE = r'drawStyle'
	t_SLIDE = r'slide'
	t_EXECUTIONORDER = r'executionOrder'
	t_SCOPE = r'scope'
	t_PROPS = r'props'
	t_DATATYPE = r'dataType'
	t_PRIMITIVE = r'primitive'
	t_ISSIGNED = r'isSigned'
	t_WORDLENGTH = r'wordLength'
	t_PLANTMODELINGINFO = r'plantModelingInfo'
	t_MIDPOINTSTICKY = r'midPointSticky'
	t_FRAME = r'frame'
	t_STAMPANGLE = r'stampAngle'
	t_STATEMACHINETYPE = r'stateMachineType'
	t_STICKY = r'sticky'
	t_MODE = r'mode'
	t_RUNTIMECHECK = r'runTimeCheck'
	t_DEBUG = r'debug'
	t_GROUPED = r'GROUPED'
	t_SUPERSTATE = r'superState'
	t_NOTEBOX = r'noteBox'
	t_ISNOTEBOX = r'isNoteBox'

	def t_FLOAT(self,t):
		r'[-+]?[0-9]*\.[0-9]*'
		t.value = float(t.value)
		return t
	
	def t_STRING(self,t):
		r'"[^"]*"'
		t.value = t.value[1:-1]
		return t
	
	def t_INTEGER(self,t):
		r'[-+]?[0-9]+'
		t.value = int(t.value)
		return t
	
	def t_error(self,t):
		t.lexer.skip(1)
		
	def build(self,**kwargs):
		#self.lexer = lex.lex(module=self, **kwargs)
		self.lexer=lex.lex(module=self,debug=True,debuglog=log)
		return self.lexer

class LSParser:

	tokens = LSLexer().tokens

	def p_Sflow(self,p):
		'Sflow : STATEFLOW LFLBR SFSpec RFLBR'
		p[0] = Node("Sflow",p[3])	

	def p_SFSpec(self,p):
		'''SFSpec : SFMachine SFChartObj SFChartObj2'''
		p[0] = [p[1],p[2]]+p[3]

	def p_empty(self,p):
		'empty :'
		pass
	
	def p_SFChartObj2(self,p):
		'''SFChartObj2 : SFChartObj SFChartObj2
					   | SFTarget'''
		if (len(p) == 3):
			p[0] = [p[1]]+p[2]
		elif (len(p) == 2):
			p[0] = [p[1]]  
	
	def p_SFChartObj(self,p):
		'SFChartObj	: SFChart SFChart2'
		p[0] = Node("SFChartObj",[p[1]]+p[2])
		
	def p_SFChart2(self,p):
		'''SFChart2 : SFState SFChart2
		            | SFTransition SFChart2
					| SFData SFChart2
					| SFAnnot SFChart2
					| SFInstance'''
		if (len(p) == 3):
			p[0] = [p[1]]+p[2]
		elif (len(p) == 2):
			p[0] = [p[1]]
					
	def p_SFState2(self,p):
		'''SFState2 : SFTransition SFState2
		            | SFTransition2'''
		if (len(p) == 3):
			p[0] = [p[1]]+p[2]
		elif (len(p) == 2):
			p[0] = p[1]
					
	def p_SFTransition2(self,p):
		'''SFTransition2 : SFData SFTransition2
						 | SFInstance'''
		if (len(p) == 3):
			p[0] = [p[1]]+p[2]
		elif (len(p) == 2):
			p[0] = [p[1]]
	
	def p_SFTarget(self,p):
		'SFTarget : TARGET LFLBR TargetSpec RFLBR'
		p[0] = Node("SFTarget",p[3])
	
	def p_SFChart(self,p):
		'SFChart : CHART LFLBR ChartSpec RFLBR'
		p[0] = Node("SFChart",p[3])
		
	def p_SFState(self,p):
		'SFState : STATE LFLBR StateSpec RFLBR'
		p[0] = Node("SFState",p[3])

	def p_SFAnnot(self,p):
		'SFAnnot : STATE LFLBR AnnotSpec RFLBR'
		p[0] = Node("SFAnnot",p[3])
		
	def p_SFTransition(self,p):
		'SFTransition : TRANSITION LFLBR TransSpec RFLBR'
		p[0] = Node("SFTransition",p[3])
		
	def p_SFData(self,p):
		'SFData : DATA LFLBR DataSpec RFLBR'
		p[0] = Node("SFData",p[3])
		
	def p_SFInstance(self,p):
		'SFInstance : INSTANCE LFLBR InstSpec RFLBR'
		p[0] = Node("SFInstance",p[3])
	
	###################################
	### Machine Unit Specifications ###
	###################################
	
	def p_SFMachine(self,p):
		'SFMachine : MACHINE LFLBR MachSpec RFLBR'
		p[0] = Node("SFMachine",p[3])
	
	def p_MachSpec(self,p):
		'MachSpec : IDRow NameRow CreatedRow isLibRow firstTarRow debugBlk sfVerRow sfDemChkRow'
		p[0] = [p[1],p[2],p[3],p[4],p[5],p[6]]
	
	def p_sfDemChkRow(self,p):
		'''sfDemChkRow : SFDEMOCHECKSUM Array
						| empty'''
		if (len(p)==3):
			p[0] = Node(p[1],None,p[2])
		elif (len(p)==2):
			p[0] = Node("sfDemoChecksum",None,"None")		
	
	def p_debugBlk(self,p):
		'''debugBlk : DEBUG LFLBR runTimeCheckRow RFLBR
					| empty'''
		if (len(p)==5):
			p[0] = Node(p[1],[p[3]],'')
		else:
			p[0] = Node('debug',None,'')
	
	def p_runTimeCheckRow(self,p):
		'''runTimeCheckRow : RUNTIMECHECK Array
							| empty'''
		if (len(p)==3):
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node("runTimeCheck",None,'')
		
	def p_IDRow(self,p):
		'''IDRow : ID INTEGER
				| empty'''
		if (len(p)==3):
			p[0] = Node(p[1],None,p[2])
		elif (len(p)==2):
			p[0] = Node("id",None,'')
	
	def p_NameRow(self,p):
		'NameRow : NAME STRING'
		p[0] = Node(p[1],None,p[2])
	
	def p_CreatedRow(self,p):
		'CreatedRow : CREATED STRING'
		p[0] = Node(p[1],None,p[2])
	
	def p_isLibRow(self,p):
		'isLibRow : ISLIBRARY INTEGER'
		p[0] = Node(p[1],None,p[2])
		
	def p_firstTarRow(self,p):
		'firstTarRow : FIRSTTARGET INTEGER'
		p[0] = Node(p[1],None,p[2])
	
	def p_sfVerRow(self,p):
		'''sfVerRow : SFVERSION FLOAT
				    | SFVERSION INTEGER'''
		p[0] = Node(p[1],None,p[2])
		
	### End Machine Unit Specification ###
	
	
	###################################
	### Chart Unit Specifications #####
	###################################
	
        #FIXME
       
	def p_ChartSpec(self,p):
		'ChartSpec : IDRow NameRow WinPosRow viewLimRow zFacRow scrRow treeNodeRow fstTarRow fstTransRow viewObjRow visRow machRow sviewSCluster ssIdHWRow decompRow fstDataRow updateRow crfilnumRow usrSSTEORow disImpRow pmodelCluster smTypeRow suppVarSizingRow'
		p[0] = [p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],p[11],p[12],p[13],p[14],p[15],p[16],p[17],p[18],p[19],p[20],p[21],p[22]]
	
	def p_WinPosRow(self,p):
		'WinPosRow : WINDOWPOSITION Array'
		p[0] = Node(p[1],None,p[2])
	
	def p_smTypeRow(self,p):
		'''smTypeRow : STATEMACHINETYPE SMVAL
					| empty'''
		if len(p) == 3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('stateMachineType',None,'')
	
	def p_suppVarSizingRow(self,p):
		'''suppVarSizingRow : SUPPORTVARIABLESIZING INTEGER
						| empty'''
		if len(p) == 3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('supportVariableSizing',None,'')			
			
	def p_viewLimRow(self,p):
		'viewLimRow : VIEWLIMITS Array'
		p[0] = Node(p[1],None,p[2])
		
	def p_zFacRow(self,p):
		'''zFacRow : ZOOMFACTOR FLOAT
					| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('zoomFactor',None,'')
	
	def p_scrRow(self,p):
		'scrRow : SCREEN Array'
		p[0] = Node(p[1],None,p[2])
	
	def p_treeNodeRow(self,p):
		'treeNodeRow : TREENODE Array'
		p[0] = Node(p[1],None,p[2])
	
	def p_fstTransRow(self,p):
		'''fstTransRow : FIRSTTRANSITION INTEGER
						| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('firstTransition',None,'')
	
	def p_fstTarRow(self,p):
		'''fstTarRow : FIRSTTARGET2 INTEGER
						| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('firsttarget',None,'')	
	
	def p_viewObjRow(self,p):
		'viewObjRow : VIEWOBJ INTEGER'
		p[0] = Node(p[1],None,p[2])
	
	def p_visRow(self,p):
		'''visRow : VISIBLE INTEGER
				| empty'''
		if len(p) == 3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('visible',None,'')
		
	def p_machRow(self,p):
		'machRow : MACHINE INTEGER'
		p[0] = Node(p[1],None,p[2])
		
	def p_sviewSCluster(self,p):
		'''sviewSCluster : SUBVIEWS LFLBR x1Row y1Row zFacRow RFLBR
                                 | empty'''
		if len(p) == 7:
		    p[0] = Node("SubviewS",[p[3],p[4],p[5]])
		else:
		    p[0] = Node('SubviewS', None, '')
	
	def p_x1Row(self,p):
		'''x1Row : X1 FLOAT
				| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node("X1",None,"")
	
	def p_y1Row(self,p):
		'''y1Row : Y1 FLOAT
				| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node("Y1",None,"")
	
	def p_ssIdHWRow(self,p):
		'ssIdHWRow : SSIDHIGHWATERMARK INTEGER'
		p[0] = Node(p[1],None,p[2])
		
	def p_decompRow(self,p):
		'decompRow : DECOMPOSITION DECVAL'
		p[0] = Node(p[1],None,p[2])
	
	def p_fstDataRow(self,p):
		'''fstDataRow : FIRSTDATA INTEGER
						| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('firstData',None,'')
	
	def p_updateRow(self,p):
		'''updateRow : UPDATEMETHOD UMVAL
					| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('updateMethod',None,'')
	
	def p_crfilnumRow(self,p):
		'''crfilnumRow : CHARTFILENUMBER INTEGER
                               | empty'''
		if len(p) == 3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('chartFileNumber', None, '')
		
	def p_usrSSTEORow(self,p):
		'''usrSSTEORow : USERSPECIFIEDSTATETRANSITIONEXECUTIONORDER INTEGER
						| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('userSpecifiedStateTransitionExecutionOrder',None,'')
		
	def p_disImpRow(self,p):
		'''disImpRow : DISABLEIMPLICITCASTING INTEGER
					| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('disableImplicitCasting',None,'')
		
	def p_pmodelCluster(self,p):
		'''pmodelCluster : PLANTMODELINGINFO LFLBR RFLBR
						| empty'''
		if len(p)==4:
			p[0] = Node(p[1],None,"")
		else:
			p[0] = Node("plantModelingInfo",None,"") 
			
	### End Chart Unit Specification ##	
	
	###################################
	### State Unit Specifications #####
	###################################

	###################################
	### Annotation Specifications #####
	###################################

	
	def p_StateSpec(self,p):
		'StateSpec : IDRow StlStringRow posRow fSizeRow aSizeRow chartRow treeNodeRow fstTransRow subVwrRow ssIdRow typeRow decompRow execOrdRow'
		p[0] = [p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],p[11],p[12],p[13]]

#	def p_AnnotSpec(self,p):
#		'AnnotSpec : IDRow StlStringRow posRow fSizeRow aSizeRow chartRow treeNodeRow fstTransRow subVwrRow ssIdRow typeRow decompRow execOrdRow'
#		p[0] = [p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],p[11],p[12],p[13]]

	def p_AnnotSpec(self,p):
		'AnnotSpec : IDRow StlStringRow posRow chartRow treeNodeRow superStateRow isNoteBoxRow noteBoxRow subVwrRow ssIdRow typeRow decompRow'
		p[0] = [p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],p[11],p[12]]

	def p_StlStringRow(self,p):
		'StlStringRow : LABELSTRING STRING eString'
	#	print p[2]+p[3]
	#	p[0] = Node(p[1],None,p[2]+p[3])
#		print "strings are - P2 -" + p[2] + " - P3 is "+ p[3] + " ok? \n"
	
		if not p[2][0] == '@':
			#print "Coming to with @ sign \n and string is " + p[2] + p[3] + "-OK?"
			p[0] = Node(p[1],lstring2(p[2]+p[3]).children,p[2]+p[3])
		else:
			p[0] = Node(p[1],None,p[2]+p[3])
		
#	def p_cleanStringRow(self,p):
#		'cleanStringRow : LABELSTRING eString'
#		p[0] = Node(p[1],p[2])
		
	def p_eString(self,p):
		'''eString : STRING eString
						| empty'''
		if len(p)==3:
			p[0] = p[1]+p[2]
		else:
			p[0] = ''
			
	def p_posRow(self,p):
		'posRow : POSITION Array'
		p[0] = Node(p[1],None,p[2])
		
	def p_fSizeRow(self,p):
		'''fSizeRow : FONTSIZE INTEGER
					| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('fontSize',None,'')
	
	def p_chartRow(self,p):
		' chartRow : CHART INTEGER'
		p[0] = Node(p[1],None,p[2])

#superStateRow isNoteBoxRow noteBoxRow
	
	def p_superStateRow(self,p):
		'superStateRow : SUPERSTATE GROUPED'
		p[0] = Node('superState',None,'')

	def p_isNoteBoxRow(self,p):
		'isNoteBoxRow : ISNOTEBOX INTEGER'
		p[0] = Node('isNoteBox',None,p[2])
	
	def p_noteBoxRow(self,p):
		'''noteBoxRow : NOTEBOX Array
					| NOTEBOX'''
		if len(p)==3:
			p[0] = Node('noteBox',None,p[2])
		else:
			p[0]= Node('noteBox', None, None)
	
	def p_subVwrRow(self,p):
		' subVwrRow : SUBVIEWER INTEGER'
		p[0] = Node(p[1],None,p[2])
	
	def p_ssIdRow(self,p):
		' ssIdRow : SSIDNUMBER INTEGER'
		p[0] = Node(p[1],None,p[2])
	
	def p_typeRow(self,p):
		' typeRow : TYPE TYPEVAL'
		p[0] = Node(p[1],None,p[2])
	
	### End State Unit Specification ##
	
	###################################
	# Transition Unit Specifications ##
	###################################
	
	def p_TransSpec(self,p):
		'TransSpec : IDRow TrlStringRow lPosRow fSizeRow aSizeRow srcBlk dstBlk mPtRow chartRow lnkNodeRow dataLRow stpAngRow subVwrRow drStyleRow slBlk execOrdRow ssIdRow'
		p[0] = [p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],p[11],p[12],p[13]]+p[14]+p[15]+[p[16],p[17]]
	
	def p_TrlStringRow(self,p):
		'''TrlStringRow : LABELSTRING STRING
						| empty'''
		if len(p) == 3:
#			print "Label String is"
#			print p[2]
			p[0] = Node(p[1],lstring3(p[2]).children,p[2])
		else:
			p[0] = Node('labelString',None,'')
			
	def p_lPosRow(self,p):
		'lPosRow : LABELPOSITION Array'
		p[0] = Node(p[1],None,p[2])
		
	def p_aSizeRow(self,p):
		'''aSizeRow : ARROWSIZE FLOAT
					| ARROWSIZE INTEGER
					| empty'''
		if len(p) == 3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('arrowSize',None,'')			
					
	def p_srcBlk(self,p):
		'srcBlk : SRC LFLBR IDRow itrsecRow RFLBR'
		p[0] = Node("Source Block",[p[3]]+[p[4]])
	
	def p_dstBlk(self,p):
		'dstBlk : DST LFLBR IDRow itrsecRow RFLBR'
		p[0] = Node("Dest Block",[p[3],p[4]])
		
	def p_itrsecRow(self,p):
		'itrsecRow : INTERSECTION Array'
		p[0] = Node(p[1],None,p[2])
		
	def p_mPtRow(self,p):
		'mPtRow : MIDPOINT Array'
		p[0] = Node(p[1],None,p[2])
		
	def p_lnkNodeRow(self,p):
		'lnkNodeRow : LINKNODE Array'
		p[0] = Node(p[1],None,p[2])
		
	def p_dataLRow(self,p):
		'dataLRow : DATALIMITS Array'
		p[0] = Node(p[1],None,p[2])
		
	def p_stpAngRow(self,p):
		'''stpAngRow : STAMPANGLE SANGVAL
					| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('stampAngle',None,'') 
		
	def p_drStyleRow(self,p):
		'''drStyleRow : DRAWSTYLE DSTYVAL
					| empty'''
		if (len(p)==3):
			p[0] = [Node(p[1],None,p[2])]
		elif (len(p)==2):
			p[0] = []
		
	def p_slBlk(self,p):
		'''slBlk : SLIDE LFLBR modeRow stickyRow mPtSRow RFLBR
				| empty'''
		if (len(p)==7):
			p[0] = [Node("Slide Block",[p[3],p[4],p[5]])]
		elif (len(p)==2):
			p[0] = []
		
	def p_modeRow(self,p):
		'''modeRow : MODE MODEVAL
					| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('mode',None,'')
	
	def p_stickyRow(self,p):
		'''stickyRow : STICKY STICKYVAL
					| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('sticky',None,'')
		
	def p_mPtSRow(self,p):
		'''mPtSRow : MIDPOINTSTICKY INTEGER
					| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('midPointSticky',None,'')
		
	def p_execOrdRow(self,p):
		'''execOrdRow : EXECUTIONORDER INTEGER
					  | empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('executionOrder',None,'')
		
	### Transition Unit Specification ##
	
	###################################
	# Data Unit Specifications ########
	###################################
	
	def p_DataSpec(self,p):
		'DataSpec : IDRow ssIdRow NameRow lnkNodeRow scopeRow machRow propsBlk dTypeRow'
		p[0] = [p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8]]
	
	def p_scopeRow(self,p):
		'scopeRow : SCOPE SCPVAL'
		p[0] = Node(p[1],None,p[2])
		
	def p_propsBlk(self,p):
		'propsBlk : PROPS LFLBR propSpec RFLBR'
		p[0] = Node("props",p[3])
	
	def p_propSpec(self,p):
		'propSpec : udateRow typeBlk frmRow'
		p[0] = [p[1],p[2],p[3]]
	
	def p_frmRow(self,p):
		'''frmRow : FRAME FRMVAL
					| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('frame',None,'')
			
	def p_udateRow(self,p):
		'''udateRow : UPDATEMETHOD UDATEVAL
					| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('updateMethod',None,'')
		
	def p_dTypeRow(self,p):
		'dTypeRow : DATATYPE STRING'
		p[0] = Node(p[1],None,p[2])
		
	def p_typeBlk(self,p):
		'typeBlk : TYPE LFLBR mtdRow primRow isSgnRow wrdLenRow RFLBR'
		p[0] = Node("Type Block",[p[3],p[4],p[5],p[6]])
	
	def p_mtdRow(self,p):
		'''mtdRow : METHOD MTDVAL
					| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('method',None,'')
	
	def p_primRow(self,p):
		'primRow : PRIMITIVE PRIMVAL'
		p[0] = Node(p[1],None,p[2])
		
	def p_isSgnRow(self,p):
		'''isSgnRow : ISSIGNED INTEGER
					| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('isSigned',None,'')
	
	def p_wrdLenRow(self,p):
		'''wrdLenRow : WORDLENGTH STRING
					| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('wordLength',None,'')
	
	### Data Unit Specification ##
	
	
	####################################
	### Instance Unit  Specifications ##
	####################################
	
	def p_InstSpec(self,p):
		'InstSpec : IDRow NameRow machRow chartRow'
		p[0] = [p[1],p[2],p[3],p[4]]
	
	# End Instance Unit Specification ##
	
	####################################
	#### Target Unit Specifications ####
	####################################
	
	def p_TargetSpec(self,p):
		'TargetSpec : IDRow NameRow descRow codeFlagsRow machRow lnkNodeRow chkSumRow'
		p[0] = [p[1],p[2],p[3],p[4],p[5]]
	
	def p_descRow(self,p):
		'descRow : DESCRIPTION STRING'
		p[0] = Node(p[1],None,p[2])
		
	def p_codeFlagsRow(self,p):
		'''codeFlagsRow : CODEFLAGS STRING
						| empty'''
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('codeFlags',None,'')
	
	def p_chkSumRow(self,p):
		'''chkSumRow : CHECKSUMOLD Array
					| empty'''		
		if len(p)==3:
			p[0] = Node(p[1],None,p[2])
		else:
			p[0] = Node('checksumOld',None,'')
				
	# End Target Unit Specification ##
	
	def p_Array(self,p):
		'''Array : LSQBR FLOAT Array2
				 | LSQBR INTEGER Array2'''
		p[0] = [p[2]]+p[3]
		
	def p_Array2(self,p):
		'''Array2 : FLOAT Array2
				  | INTEGER Array2
				  | RSQBR'''
		if(len(p) == 3):
			p[0] = [p[1]]+p[2]
		if(len(p) == 2):
			p[0] = []
	
	def p_error(self,p):
		print ("Syntax Error in SFParse at " + str(p))
		yacc.errok()
		
	def build_and_parse(self,data,**kwargs):
		log = logging.getLogger()
		parser = yacc.yacc(module=self,debug = True, debuglog = log)
		result = parser.parse(data, debug=log, **kwargs)
		return result
		#parser = yacc.yacc(module=self)
		#result = parser.parse(data,**kwargs)
		return result
		
	
def extract_sf(data):
	'''Returns a tree object which is the parsed form of the input raw text data'''
	lxr = LSLexer().build()
	result = LSParser().build_and_parse(data, lexer=lxr)
	return result
