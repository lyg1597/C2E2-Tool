import pygtk
pygtk.require('2.0')

import gtk

def delete_event(widget,event,data=None):
    print "Window Closed"
    return False

treestore = gtk.TreeStore(str)

def destroy(widget, data=None):
    gtk.main_quit()

def print_tree(node):
    print node.type
    if node.value != None:
        print node.value
    for i in node.children:
        print_tree(i)

def build_tree(node,parent):
    if (node.value != None):
        if (node.value != ''):
            curr = treestore.append(parent,[(node.type,node.value)])
        else:
            curr = treestore.append(parent,[node.type])
    else:
        curr = treestore.append(parent,[node.type])
    try:
        for i in node.children:
            build_tree(i,curr)
    except:
        print "Type error. Skipping node "+str(node.children)

def qb2_callback(widget,data=None):
    print "HyTech clicked"

def qb3_callback(widget,data=None):
    print "UPPAAL clicked"

def show_tree(node):
    window = gtk.Window(gtk.WINDOW_TOPLEVEL)
    swindow = gtk.ScrolledWindow(hadjustment=None, vadjustment=None)
    swindow.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_ALWAYS)
    window.set_title("Parse Tree Viewer")
    window.set_size_request(500, 500)
    window.connect("delete_event", delete_event)
    window.connect("destroy",destroy)
    
    build_tree(node,None)
    
    treeview = gtk.TreeView(treestore)
    tvcolumn = gtk.TreeViewColumn('ParseTree')
    treeview.append_column(tvcolumn)
    cell = gtk.CellRendererText()
    tvcolumn.pack_start(cell, True)
    tvcolumn.add_attribute(cell, 'text', 0)
    swindow.add_with_viewport(treeview)
    
    qb = gtk.Button("Close")
    qb.connect("clicked", lambda w:gtk.main_quit())
    
    qb2 = gtk.Button("Generate HyTech")
    qb2.connect("clicked", qb2_callback)
    
    qb3 = gtk.Button("Generate UPPAAL")
    qb3.connect("clicked", qb3_callback)
    
    table = gtk.Table(10, 3, True)
    table.attach(swindow,0,3,0,9)
    
    button_table = gtk.Table(1,3,True)
    button_table.attach(qb,0,1,0,1,xpadding=5,ypadding=0)
    button_table.attach(qb2,1,2,0,1,xpadding=5,ypadding=0)
    button_table.attach(qb3,2,3,0,1,xpadding=5,ypadding=0)
    
    table.attach(button_table,0,3,9,10,xpadding=10,ypadding=10)

    
    window.add(table)
#    window.add(swindow)
#    window.add(button)
    window.show_all() 
    
    ##COMMENT OUT FOR QUICK DEBUGGING##
    gtk.main()
    
    ##ONLY FOR ENABLED FOR QUICK DEBUGGING##
#    print "DESTROYED WINDOW!"
#    window.destroy()

def clear_tree():
    treestore.clear()
