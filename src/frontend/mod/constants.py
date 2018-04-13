# Events
OPEN_EVENT = '<<Open>>'
CLOSE_EVENT = '<<Close>>'

# Property types
SAFETY = 'Safety'
ODEINT_FIX = 'ODEINT: fixed step'
ODEINT_ADP = 'ODEINT: adaptive step'
CAPD = 'CAPD'
DEF_STRAT = 'Default Strategy'
USR_STRAT = 'User Strategy'

#File types
MDL_FILE = '.mdl'
HYXML_FILE = '.hyxml'

# Resources
CROSS_IMG = 'frontend/res/cross.png'
TICK_IMG = 'frontend/res/tick.png'

#Status
Simulated = "Simulated"
Verified = "Verified"
Plotted = "Plotted"

#Result

# Variable Types / LMB 12/21/2017 / Options for variable type
REAL = 'Real'
INTEGER = 'Integer'
VARIABLE_TYPES = [ REAL, INTEGER ]

# Variable Scopes / LMB 3/2/2018 / Options for variable scope
LOCAL = 'LOCAL_DATA'
INPUT = 'INPUT_DATA'
OUTPUT = 'OUTPUT_DATA'
VARIABLE_SCOPES = {LOCAL, INPUT, OUTPUT}

# Contexts / LMB 12/21/2017 / Used to determine right-click menus and popup entry/edit menus in the ModelTab Treeview
AUTOMATON = 'Automaton'
VARIABLES = 'Variables'
THINVARIABLES = 'Thin Variables'
MODES = 'Modes'
TRANSITIONS = 'Transitions'


# Actions / LMB 1/2/2018 / Used to determine right-click menus and popup entry/edit
ADD = 'Add'
EDIT = 'Edit' 
DELETE = 'Delete'

# Save Dialog options / LMB 3/26/2018 / Save Dialog popup options
SAVE = 'Save'
SAVE_AS = 'Save As'
DISCARD = 'Discard'

# Save Contexts / LMB 4/9/2018 / Determines what we're saving - the XML or the TreeView Model
MODEL = 'Model'
EDITOR = 'Editor'

# Save Options / LMB 4/2/2018 / Save Options
SAVE_OPT = {
    'defaultextension': '.hyxml',
    'filetypes': [('HyXML files', '.hyxml')],
    'title': 'Save File' }