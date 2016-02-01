def foo(node):
    if node.children == []:
        return str(node.value)
    elif len(node.children) == 2:
        if node.type == 'Logical':
            return foo(node.children[0]) + ' ' + node.value + ' ' + foo(node.children[1])
        else:
            return '(' + foo(node.children[0]) + node.value + foo(node.children[1]) + ')'
    elif len(node.children) == 1:
        return node.value + foo(node.children[0])
    
def negate(node):
    if node.value == 'or':
        node.value = 'and'
    elif node.value == 'and':
        node.value = 'or'
    elif node.value == '>':
        node.value = '<='
    elif node.value == '<':
        node.value = '>='
    elif node.value == '>=' or node.value == '=>':
        node.value = '<'
    elif node.value == '<=' or node.value == '=<':
        node.value = '>'
    elif node.value == '==':
        node.value = '!='
    if len(node.children) > 0:
        for i in node.children:
            negate(i)
            
def deps(node):
    if len(node.children) == 0 and node.type == 'Identifier' and type(node.value).__name__ == 'str':
        return [node.value]
    elif len(node.children) == 2:
        return deps(node.children[0]) + deps(node.children[1])
    elif len(node.children) == 1:
        return deps(node.children[0])
    else:
        return []