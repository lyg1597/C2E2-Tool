class Node:
    '''defines any node that could appear in the hybrid intermediate 
    representation tree.  For example, this could be an action, a transition,
    or a mode.  These attributes will be stored in type.  Value will hold a
    representation of this type.  If the the type can be expressed by a 
    a series of tokens (such as an inequality), then children will hold the
    types and values of these tokens.'''
    def __init__(self,type,children=None,value=None):
        self.type = type
        if children:
            self.children = children
        else:
            self.children = []
        self.value = value

    def equals(self, otherNode):
        '''This functions tests to see if two nodes are equal.  This means
        that they have the same structure and value'''
        equals = True

        if self.children is None and otherNode.children is None: 
            return self.value == otherNode.value
        
        if len(self.children) == len(otherNode.children):
            for x in range(0, len(self.children)):
                equals = equals and self.children[x].equals(otherNode.children[x])
            equals = equals and self.value == otherNode.value
        else:
            return False
        return equals
    
    def prints(self):
        
        print "Type is "+self.type+"\n"
        
        if self.children is None:
            print "ChildNone -"+printValue(self.type, self.value)+" \n"
        
        elif len(self.children) == 0:
            print "nochild -"+printValue(self.type, self.value)+" \n"
        
        elif len(self.children) > 0:
            print "children = "+str(len(self.children))+" value -"+printValue(self.type, self.value)+" \n"
            for j in self.children:
                j.prints()
                
def printValue(type,value):
    
    if value == None:
        return "None"
    if type == 'Logical':
        return str(value)
    elif type == 'Identifier':
        return str(value)
    elif type == 'Expression':
        return str(value)
    elif type == 'Relational':
        return str(value)
    elif type == 'Assignment':
        print "Coming to Assignment tab \n"
        return str(value)
    else: 
        return "NONE"