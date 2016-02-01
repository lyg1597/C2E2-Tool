from node import *
from tree import show_tree
import itertools

def IsHierarchical(root):
    '''This function checks if the tree rooted at 'root' is a hierarchical stateflow diagram or not. If
    the system is hierarchical then we invoke functions to flatten out the hierarchical state machine'''
    state_pos = []
    for i in root.children[1].children:
        if i.type == 'SFState':
            for j in i.children:
                if j.type == 'position':
                    state_pos += [[round(k) for k in j.value]]
    state_pos = sorted(state_pos, key=lambda x:x[2]*x[3], reverse=True)
    length = len(state_pos)
    for i in range(length):
        for j in sorted(range(i+1,length),reverse=True):
            if IsContained(state_pos[i],state_pos[j]):
                return True
    return False
    
def IsContained(b1,b2):
    '''This function takes two boxes [x,y,w,h] where (x,y) is the coordinate of the top-left and (w,h) is the width and height of the box. The return value is 
    true if b1 is completely contained within b2 or the other way around, false otherwise'''
    if (b1[2]*b1[3] > b2[2]*b2[3]):
        if(b1[1] < b2[1] < b1[1]+b1[3] and b1[0] < b2[0] < b1[0]+b1[2] and b1[1] < b2[1]+b2[3] < b1[1]+b1[3] and b1[0] < b2[0]+b2[2] < b1[0]+b1[2]):
            return True
        else:
            return False
    else:
        if(b2[1] < b1[1] < b2[1]+b2[3] and b2[0] < b1[0] < b2[0]+b2[2] and b2[1] < b1[1]+b1[3] < b2[1]+b2[3] and b2[0] < b1[0]+b1[2] < b2[0]+b2[2]):
            return True
        else:
            return False

def IsContainedOneWay(b1,b2):
    '''Returns true if b2 is contained in b1'''
    if(b1[1] < b2[1] < b1[1]+b1[3] and b1[0] < b2[0] < b1[0]+b1[2] and b1[1] < b2[1]+b2[3] < b1[1]+b1[3] and b1[0] < b2[0]+b2[2] < b1[0]+b1[2]):
        return True
    else:
        return False
    
def FindParent(state,state_list):
    for i in state_list:
        if not i[0] == state[0]:
            if IsContainedOneWay(i[3],state[3]):
                return i
    return None

def FindChildren(state,state_list):
    children=[]
    for i in state_list:
        if FindParent(i,state_list) != None:
            if FindParent(i,state_list)[0] == state[0]:
                children+=[i]
    return children

def RemoveHierarchy(root):
    state_list = []
    for i in root.children[1].children:
        if i.type == 'SFChart':
            root_decomp = 'OR_STATE'
            for j in i.children:
                if j.type == 'decomposition':
                    if j.value == 'SET_CHART':
                        root_decomp = 'AND_STATE'
                    elif j.value == 'CLUSTER_CHART':
                        root_decomp = 'OR_STATE'
                    break
        if i.type == 'SFState':
            decomp = 'OR_STATE'
            for j in i.children:
                if j.type == 'id':
                    id = j.value
                elif j.type == 'position':
                    pos = [round(k) for k in j.value]
                elif j.type == 'labelString':
                    name = j.children[0].value
                elif j.type == 'decomposition':
                    if j.value == 'SET_STATE':
                        decomp = 'AND_STATE'
                    elif j.value == 'CLUSTER_STATE':
                        decomp = 'OR_STATE'
            state_list += [[id,name,decomp,pos]]
    state_list = sorted(state_list, key=lambda x:x[3][2]*x[3][3], reverse=False)
    htree = Node('-1',[],['-1','Root',root_decomp])
    for i in state_list:
        if FindParent(i,state_list) == None:
            htree.children += [Node(str(i[0]),[],i)]
    for i in htree.children:
        complete_tree(i,state_list)
#    show_tree(htree)
    system_modes = generate_modes(htree)
    id = 1
    state_modes = []
    for i in system_modes:
        components = [int(y) for y in i[3:].split('.')]
        name = ''
        for j in components:
            name+=findModeById(j,root).children[1].children[0].value+'.'    
        name = name[:-1]
        
        idrow = Node('id',[],id)
        id+=1
        
        during = Node('During',[],'')
        label = name+'\\ndu:'
        for j in components:
            try:
                tmp = findModeById(j,root)
                label += tmp.children[1].value.split('du:')[1]
                during.children += tmp.children[1].children[2].children
            except:
                pass
        lstring = Node('labelString',[Node('Mode',[],name),Node('Entry'),during,Node('Exit'),Node('Bind')],label)
        htree_value = Node('OriginalTree',[],i)
        state_modes += [Node('SFState',[idrow,lstring,htree_value],'')]


    root.children[1].children = filter(lambda a:a.type != 'SFState', root.children[1].children)
    root.children[1].children = [root.children[1].children[0]] + state_modes + root.children[1].children[1:]
    
    transitions = filter(lambda a:a.type == 'SFTransition', root.children[1].children)
    root.children[1].children = filter(lambda a:a.type != 'SFTransition', root.children[1].children)
    
    new_transitions = []
    tid = 1
    for i in transitions:
        dest = '-1'
        for j in i.children:
            if j.type == 'Source Block':
                source = j.children[0].value
                new_sources = findNewTreeModes(source,root)
            if j.type == 'Dest Block':
                dest = j.children[0].value
                new_dest = findNewTreeModes(dest,root)
        if not dest == '-1':
            sd = [new_sources,new_dest]
            sd_pairs = list(itertools.product(*sd))
            print sd_pairs
            for j in sd_pairs:
                new_transitions.append(changeSourceDest(i,j[0],j[1],tid))
                tid+=1
    
    root.children[1].children.extend(new_transitions)    
    
    return root

def changeSourceDest(blk,new_src,new_dest,new_id):
    for i in blk.children:
        if i.type == 'id':
            i.value = new_id
        if i.type == 'Source Block':
            i.children[0].value = new_src
        if i.type == 'Dest Block':
            i.children[0].value = new_dest
    return blk

def findNewTreeModes(id,root):
    new_ids = []
    for i in root.children[1].children:
        if i.type == 'SFState':
            components = [int(y) for y in i.children[2].value[3:].split('.')]
            if id in components:
                new_ids.append(i.children[0].value)
    return new_ids

def findModeById(id,root):
    for i in root.children[1].children:
        if i.type == 'SFState':
            if i.children[0].value == id:
                return i
    return None
        
def generate_modes(node):
    modes = []
    if node.children==[]:
        return [node.type]
    else:
        if node.value[2]=='AND_STATE':
            tmp_modes = []
            for i in node.children:
                tmp_modes += [generate_modes(i)]
            modes = map('.'.join, itertools.product(*tmp_modes))
            modes2 = []
            for i in modes:
                modes2 += [node.type+'.'+i]
            return modes2
        elif node.value[2]=='OR_STATE':
            for i in node.children:
                modes += generate_modes(i)
            modes2 = []
            for i in modes:
                modes2 += [node.type+"."+i]
            return modes2
        else:
            print "Error generating modes from hierarchies. State "+str(node.type)+" is neither an AND state nor an OR state"
            return []
    
def complete_tree(node,state_list):
    children = FindChildren(node.value,state_list)
    if children == []:
        return True
    else:
        for j in children:
            node.children+=[Node(str(j[0]),[],j)]
        for j in node.children:
            complete_tree(j,state_list)
        return True
