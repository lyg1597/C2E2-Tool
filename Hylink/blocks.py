class BlockType:
    def __init__(self,type,params):
        if type=='Constant':
            self.bclass = Constant(params)
        elif type=='Gain':
            self.bclass = Gain(params)
        elif type=='Product':
            self.bclass = Product(params)
        elif type=='Sum':
            self.bclass = Sum(params)
        elif type=='Trigonometry':
            self.bclass = Trigonometry(params)
        elif type=='Switch':
            self.bclass = Switch(params)
        elif type=='RelationalOperator':
            self.bclass = RelationalOperator(params)
        else:
            self.bclass = None
        
    def get_func(self):
        if self.bclass == None:
            return None
        else:
            return self.bclass.func

class Constant:
    def __init__(self,value):
        self.value = value
        self.func = self.value

class Integrator:
    def __init__(self,init):
        self.init = init
        
class Gain:
    def __init__(self,gain):
        self.gain = gain
        self.func = self.gain+'*([1])'
        
class Product:
    def __init__(self,inputs):
        self.inputs = inputs
        self.func = ''
        try:
            n = int(inputs)
            for i in range(1,n):
                self.func = self.func+'(['+str(i)+'])*' 
            self.func = self.func+'(['+str(n)+'])'
        except:
            for i in range(0,len(inputs)):
                if inputs[i]=='*':
                    if i==len(inputs)-1:
                        self.func = self.func+'(['+str(i+1)+'])'
                    else:
                        self.func = self.func+'(['+str(i+1)+'])*'
                elif inputs[i]=='/':
                    if i==len(inputs)-1:
                        self.func = self.func+'(1/(['+str(i+1)+']))'
                    else:
                        self.func = self.func+'(1/(['+str(i+1)+']))*'

class Sum:
    def __init__(self,signs):
        self.signs = signs
        self.func = ''
        try:
            n = int(signs)
            for i in range(1,n):
                self.func = self.func+'(['+str(i)+'])+' 
            self.func = self.func+'(['+str(n)+'])'
        except:
            for i in range(0,len(signs)):
                if i==0:
                    if signs[i]=='+':
                        self.func='([1])'
                    else:
                        self.func='-([1])'
                else:
                    self.func = self.func+signs[i]+'(['+str(i+1)+'])'
                    
class Trigonometry:
    def __init__(self,function):
        self.function = function
        self.func = function+'([1])'
                    
class Switch:
    def __init__(self,position):
        self.position = position
        if position == '0':
            self.func = '[1]'
        elif position == '1':
            self.func = '[3]'
        else:
            self.func = ''
            
class RelationalOperator:
    def __init__(self,operator):
        self.operator = operator
        self.func = '([1])'+operator+'([2])'        