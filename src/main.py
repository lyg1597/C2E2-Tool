from frontend.gui.c2e2 import C2E2
from frontend.mod.constants import *
from frontend.mod.filehandler import FileHandler
from frontend.mod.session import Session, Property
from frontend.mod.simgen import *
from sys import argv

import sys


def getopts( argv ):
    """ Parse command line arguments ( expected "-name value" pairs ) """

    if( (len(argv) > 1) and (argv[1] == "--help" or argv[1] == "-help") ):
        printusage()
        sys.exit()

    opts = {}

    while( argv ):
        
        if( argv[0][0] == '-' ):  # Looking for "-name value" pair.
            opts[argv[0]] = argv[1]

        argv = argv[1:]  # Remove first element from argv

    return opts

def printusage():
    """ Prints usage instructions to user """

    print( "python3 main.py -f [path/to/file] -s [simulator] -a [simulate/verify]" )
    print( "  -f    Path to input file. REQUIRED." )
    print( "  -a    Simulate or Verify. Optional." )
    print( '            [default] "simulate" or "s" ' )
    print( '                      "verify" or "v" ' )
    print( "  -s    Simulator choice. Optional." )
    print( '            [default] "odeintfixed" or "of" for ODEINT: fixed step' )
    print( '                      "odeintadp" or "oa" for ODEINT: adaptive step' )
    print( "  -r    Refinement strategy. Optional." )
    print( '            [defualt] "default" or "d" for Default Strategy' )
    print( '                      "user" or "us" for User Strategy' )

    return

if __name__ == '__main__':

    args = getopts( argv )

    if( args ):

        # Read file into Session variables
        if( '-f' not in args ):
            print( "ERROR: File required" )
            sys.exit()
        FileHandler.open_file( args['-f'] )
        
        # Select Simulator ( Session defaults to ODEINT_FIX )
        if( ('-s' in args) and (args['-s'] == "odeintadp") ):
            Session.simulator = ODEINT_ADP

        # Select Refinement Strategy ( Session defualts to DEF_STRAT )
        if( ('-r' in args) and (args['-r'] == "user") ):
            Session.refine_strat = USR_STRAT
        
        HyIR.compose_all( Session.hybrid_automata )
        # Set simulator property - not sure how we'll handle this moving forward
        Session.cur_prop = Session.prop_list[0]
        Property.validate_cur_prop()

        # Simulate/Verify (Simulate is default)
        if( ('-a' in args) and (args['-a'] == "verify") ):
            result = verify()
            print( "\nVerification Results" )
        else:
            result = simulate()
            print( "\nSimulation Results" )

        print( "-----------------------" )
        print( "Property: ", Session.cur_prop.name )
        print( "Stauts:   ", Session.cur_prop.status )
        print( "Result:   ", Session.cur_prop.result )

    else:
        app = C2E2(None)
        app.mainloop()