from hyir import HyIR, Variable, Mode, Invariant, DAI, Transition, Guard, Action
import copy
from blocks import *
from hyir import *

class Block:
    def __init__(self,name,type,inputs,outputs,data,params,switch_pos=-1,):
        self.name = name
        self.type = type
        tmp = []
        for i in range(1,inputs+1):
            tmp += [('in'+str(i),'Null')] 
        for i in range(1,outputs+1):
            tmp += [('out'+str(i),'Null')]
        self.delta = dict(tmp)
        self.data = data
        self.params = params
        self.switch_pos = switch_pos
    
    def display(self):
        print "-----"
        print "Name: "+self.name
        print "Type: "+self.type
        if self.type == 'Switch':
            print "Position: "+str(self.switch_pos)
        print "Delta Function :"
        print self.delta
        print "Parameters:"
        for i in self.params:
            print i
#        print self.data
        if self.type == 'Switch':
            print 'Position:'+str(self.switch_pos)
        print "-----"
        
class Line:
    def __init__(self,src,dest,data):
        self.src = src
        self.dest = dest
        self.data = data
        
    def display(self):
        print "----"
        print "Source : " + str(self.src)
        print "Destination : " + str(self.dest)
        print "----"

def AssertInherentDelta(blocks,lines,VSet):
    vars = 1
    
    for i in blocks:
#        We don't consider subsystems for now
#        if i.type == 'Inport':
#            i.delta['out1'] = i.name.replace('input','').strip()
#            VSet += [i.delta['out1']]
  
        if i.type == 'Constant':
            for j in i.data[1:]:
                if j[0] == 'Value':
                    i.delta['out1'] = j[1]
                    break
            else:
                i.delta['out1'] = '1'
        if i.type == 'Reference':
            for j in i.data[1:]:
                if j[0] == 'MinVal':
                    min = eval(j[1])
                if j[0] == 'MaxVal':
                    max = eval(j[1])
            i.delta['out1'] = '['+str(min)+','+str(max)+']'
        
#        I don't know what "Merge" block is !!    
#        if i.type == 'Merge':
#            var = VSet.new()
#            for j in range(i.data[3][1][0]):
#                tmp = 'in'+str(j+1)
#                i.delta[tmp] = var
#            i.delta['out1'] = var
#            for j in lines:
#                for k in j.dest:
#                    if k[0] == i.name:
#                        for l in blocks:
#                            if l.name == j.src[0]:
#                                l.delta['out'+str(j.src[1])] = var            
    
        if i.type == 'Integrator':
            i.delta['out1'] = 'x'+str(vars)
            vars+=1
            if i.delta['out1'] not in VSet:
                VSet += [i.delta['out1']]
    
#    for i in blocks:
#        if i.type == 'Integrator' and i.delta['out1'] == 'Null':
#            for j in lines:
#                if j.src[0] == i.name:
#                    tmp = j.dest[0][0]
#            i.delta['out1'] = tmp
#            VSet += [tmp]

def isBlockComplete(block):
    for i in block.delta:
        if block.delta[i] == 'Null':
            return False
    else:
        return True
    
def findBlockByName(blocks,name):
    for i in blocks:
        if i.name == name:
            return i
    else:
        return None
    
def findLineBySrcPort(lines,src,port):
    for i in lines:
        if i.src == (src,port):
            return i
    else:
        return None

def AssertDelta(blocks,lines):
    for block in blocks:
        for i in block.delta:
            if i[:3] == 'out' and not block.delta[i] == 'Null':
                port = eval(i[-1:])
                line = findLineBySrcPort(lines,block.name,port)
                for j in line.dest:
                    if not j[0] == 'Scope' and not j[0] == 'XY Graph':
                        DestBlk = findBlockByName(blocks,j[0])
                        if DestBlk == None:
                            line.display()
                        if DestBlk.delta['in'+str(j[1])] == 'Null':
                            try:
                                value = eval(block.delta[i])
                            except:
                                value = block.delta[i]
                            DestBlk.delta['in'+str(j[1])] = str(value)
                            
def AssertFunctionality(blocks):
    for block in blocks:
        if block.type == 'Integrator':
            continue
        InputsReady = True
        for i in block.delta:
            if i[:2] == 'in' and block.delta[i] == 'Null':
                InputsReady = False
        if InputsReady:
            if block.type == 'Switch':
                x = BlockType(block.type,str(block.switch_pos))
            else:
                x = BlockType(block.type,block.params[0])
            func = x.get_func()
            for i in block.delta:
                if i[:2] == 'in':
                    func = func.replace('['+i[-1:]+']',block.delta[i])
                try:
                    block.delta['out1'] = eval(func)
                except:
                    block.delta['out1'] = func
            
def NullBlocks(blocks):
    for i in blocks:
        if i.type == 'Integrator':
            if i.delta['in1'] == 'Null':
                return True
    return False

def ResetBlocks(blocks):
    for i in blocks:
        for j in i.delta:
            i.delta[j] = 'Null'

def int2bin(n, count=24):
    return "".join([str((n >> y) & 1) for y in range(count-1, -1, -1)])

def negate(condn):
    if condn.count('<') > 0:
        return condn.replace('<','>=')
    elif condn.count('>') > 0:
        return condn.replace('>','<=')
    elif condn.count('<=') > 0:
        return condn.replace('<=','>')
    elif condn.count('>=') > 0:
        return condn.replace('>=','<')
    elif condn.count('==') > 0:
        return condn.replace('==','!=')
    elif condn.count('~=') > 0:
        return condn.replace('~=','==')

def get_dest(branch):
    bcnt = 0
    #Base Case:
    for i in branch[1:]:
        if i[0] == 'Branch':
            bcnt+=1
    if bcnt == 0:
        for i in branch[1:]:
            if i[0] == 'DstBlock':
                dblk = i[1]
            if i[0] == 'DstPort':
                dport = i[1]
        return [(dblk,dport)]
    else:
        dst = []
        for i in branch[1:]:
            if i[0] == 'Branch':
                dst+=get_dest(i)
        return dst

def parse_eqn(eq):
    return Node('NULL',[])

def DecimalToBinary(decimal, emptyList):
    if(decimal == 0):
        return 0;
    else:
        emptyList.append(decimal % 2);
        DecimalToBinary((int)(decimal / 2), emptyList);
        
#def BinaryList(x):
#    valueList = []
#    binary = []
#    for cnt in range(x):
#        DecimalToBinary(cnt+1, binary)
#        binary.reverse()
#        if len(binary) == x:
#            valueList.append(binary)
#        else:
#            y = x-len(binary)
#            for i in range(y):
#                binary.insert(0,0)
#            valueList.append(binary)
#        binary = []
#    return valueList

def BinaryList(x):
    if x==1:
        return ['0','1']
    tmp = BinaryList(x-1)
    return ['0'+v for v in tmp] + ['1'+v for v in tmp]


def sl_hyir(sl,file_name):
    VSet = []
    SwitchCount = 0;
    Blocks = []
    Lines = []
    RBlocks = []
    RLines = []
    p = ''
    p2 = []
    
    for i in sl:
        if i[0] == 'Block':
            RBlocks += [i]
            if i[1][1] == 'Switch':
                SwitchCount+=1
        elif i[0] == 'Line':
            RLines += [i]
            
    for i in RBlocks:
        if i[1][1] not in ['Scope','Reference']:
            name = i[2][1]
            typ = i[1][1]
            ports = [0,0]
            if i[1][1] == 'Switch':
                p = 'u2 >= Threshold'
                p2 = ['0']
                ports = [3,1]
                for j in i:
                    if j[0] == 'Criteria':
                        p = j[1]
                    if j[0] == 'Threshold':
                        p2 = [j[1]]
            elif i[1][1] == 'Constant':
                ports = [0,1]
                for j in i:
                    if j[0] == 'Value':
                        p = j[1]
                        break
                else:
                    p = '1'
            elif i[1][1] == 'RelationalOperator':
                ports = [2,1]
                for j in i:
                    if j[0] == 'Operator':
                        p = j[1]
                        break
                else:
                    p = '<='
#                elif i[1][1] == 'Switch':
#                    ports = [3,1]
            elif i[1][1] == 'Gain':
                ports = [1,1]
                for j in i:
                    if j[0] == 'Gain':
                        p = j[1]
                        break
                else:
                    p = '1'
            elif i[1][1] == 'Product' or i[1][1] == 'Sum':
                for j in i:
                    if j[0] == 'Inputs':
                        p = j[1]
                        break
                else:
                    p = '2'
            elif i[1][1] == 'Trigonometry':
                for j in i:
                    if j[0] == 'Operator':
                        p = j[1]
                        break
                else:
                    p = 'sin'
            elif i[1][1] == 'Integrator':
                for j in i:
                    if j[0] == 'InitialCondition':
                        p = j[1]
                        break
                else:
                    p = '0'
            for j in i[1:]:
                if j[0] == 'Ports':
                    ports = j[1]
            newblock = Block(name,typ,ports[0],ports[1],i,[p]+p2)
            Blocks += [newblock]
            
    
    for i in RLines:
        i = i.asList()
        if i[1][0] == 'Name':
            i.remove(i[1])
        if i[1][0] == 'Labels':
            i.remove(i[1])
        dest = []
        if i[1][0] == 'SrcBlock':
            src = (i[1][1],i[2][1])
        else:
            src = (i[2][1],i[3][1])
        if i[3][0] == 'DstBlock':
            dest += [(i[3][1],i[4][1])]
        elif i[4][0] == 'DstBlock':
            dest += [(i[4][1],i[5][1])]
        for j in i[2:]:
            if j[0] == 'Branch':
                dest += get_dest(j)
#                    if j[1][0] == 'DstBlock':
#                        dest += [(j[1][1],j[2][1])]
#                    elif j[2][0] == 'DstBlock':
#                        dest += [(j[2][1],j[3][1])]
        Lines += [Line(src,dest,i)]
    
    if SwitchCount == 0:
        
        AssertInherentDelta(Blocks,Lines,VSet)
        
        while NullBlocks(Blocks):
            AssertDelta(Blocks,Lines)
            AssertFunctionality(Blocks)
#    
        eqns = []
        for i in Blocks:
            if i.type == 'Integrator':
                eqns+=[i.delta['out1']+'_dot = '+i.delta['in1']]

        hybrid = HyIR()
        hybrid.file_name = file_name
        automaton = Automaton()
        hybrid.add_automaton(automaton)
        
        m = Mode()
        m.id = 1
        m.name = 'mode1'
        for i in eqns:
            m.add_dai(DAI(parse_eqn(i),i))
            
        inv = Invariant(Node("True",[]),"True")
        m.add_inv(inv)
            
        hybrid.automata[0].add_mode(m)
        
    else:
        id = 1
        hybrid = HyIR()
        hybrid.file_name = file_name
        automaton = Automaton()
        hybrid.add_automaton(automaton)
        
#        mode_cnt = pow(2,SwitchCount)
        counter = 1
        for i in BinaryList(SwitchCount):
            ResetBlocks(Blocks)
            ptr = 0
            for j in Blocks:
                if j.type == 'Switch':
                    j.switch_pos = i[ptr]
                    ptr+=1

            AssertInherentDelta(Blocks,Lines,VSet)

            while NullBlocks(Blocks):
                AssertDelta(Blocks,Lines)
                AssertFunctionality(Blocks)

#            for i in Blocks:
#                i.display()

            eqns = []
            invars = []
            ptr = 0
            for j in Blocks:
                if j.type == 'Integrator':
                    eqns+=[j.delta['out1']+'_dot = '+j.delta['in1']]
                if j.type == 'Switch':
                    if i[ptr] == '0':
                        invars+=[j.delta['in2']]
                    elif i[ptr] == '1':
                        invars+=['~('+j.delta['in2']+')']
                    ptr+=1
                
                
            
            m = Mode()
            m.id = id
            m.name = 'mode'+str(counter)
            for j in eqns:
                m.add_dai(DAI(parse_eqn(j),j))
                
            for j in invars:
                if not j=='Null' and not j=='~(Null)':
                    m.add_inv(Invariant([],j))
            
            hybrid.automata[0].add_mode(m)
            counter+=1
            id+=1
    
    id = 1
    for m in hybrid.automata[0].modes:
        for n in hybrid.automata[0].modes:
            if n.id==m.id:
                continue
            else:
                guard = ''
                flag=0
                for inv in n.invs:
                    if flag==0:
                        guard += inv.raw
                        flag=1
                    else:
                        guard += ' & '+inv.raw
                t = Transition(Guard('',guard),[])
                t.src = m.id
                t.dest = n.id
                t.id = id
                hybrid.automata[0].add_trans(t)
                id+=1
    
    for i in VSet:
        v = Variable()
        v.name = i
        v.scope = "Local"
        v.type = "Continuous"
        hybrid.add_var(v)
        
    return hybrid
            