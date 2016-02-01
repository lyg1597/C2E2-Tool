## Identifying Hierarchical State Machines ##
#This file has functions to handle hierarchical state machines with one level of
#hierarchy. The function split_machines takes a Stateflow parse tree object and returns
#a list of stateflow parse trees, one for each sub machine.

import itertools
import copy

def sub(s1,s2):
    if (((s1[1][2]+s1[1][0]) > s2[1][0] > s1[1][0]) and ((s1[1][3]+s1[1][1]) > s2[1][1] > s1[1][1]) and (((s1[1][2]+s1[1][0]) > (s2[1][2]+s2[1][0]) > s1[1][0]) and ((s1[1][3]+s1[1][1]) > (s2[1][3]+s2[1][1]) > s1[1][1]))):
        return True
    else:
        return False

def split_machines(sf_tree):
    sf_list = []
    st = []
    for i in sf_tree.children:
        if i.type == 'SFChartObj':
            for j in i.children:
                if j.type == 'SFChart':
                    for k in j.children:
                        if k.type == 'decomposition':
                            if k.value == 'SET_CHART':
                                hsm = True
                            else:
                                hsm = False
                if j.type == 'SFState':
                    st = st + [[j.children[0].value,j.children[2].value,[]]]
                            
    if hsm:
        print 'Hierarchical State Machine identified'
        for i in itertools.combinations(st,2):
            print "Checking " + str(i[0][0]) + " and " + str(i[1][0])
            if sub(i[0],i[1]):
                i[0][2] = i[0][2] + [i[1][0]]
                print "Added " + str(i[1][0]) +" to " + str(i[0][0])
            elif sub(i[1],i[0]):
                i[1][2] = i[1][2] + [i[0][0]]
                print "Added " + str(i[0][0]) +" to " + str(i[1][0])
                                    
        for i in st:
            if i[2] != []:
                removeset = []
                print '***\nCreating copy for super state ' + str(i[0])
                tmp = copy.deepcopy(sf_tree)
                for j in tmp.children[1].children:
                    if j.type == 'SFState':
                        print 'Checking ' + str(j.children[0].value) + ' (' + str(j.children[1].children[0].value) + ')'
                        if j.children[0].value == i[0] or j.children[0].value not in i[2]:
                            print 'Removing ' + str(j.children[0].value) 
                            removeset = removeset + [j]
                            if j.children[0].value == i[0]:
                                SupStName = j.children[1].children[0].value
                    elif j.type == 'SFTransition':
                        if j.children[5].children[0].value not in i[2]:
                            removeset = removeset + [j]
                tmp.children[1].children = [x for x in tmp.children[1].children if x not in removeset]
                tmp.children[0].value = SupStName
                sf_list = sf_list + [tmp]
        print '---------------------------------------'
        return sf_list
    else:
        print '---------------------------------------'
        return [sf_tree]