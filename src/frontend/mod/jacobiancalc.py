from sympy import symbols,Matrix,cos,sin,tan,ln,exp,log

def removeDelete(grid):
    ret = []
    loop = len(grid)
    temp = []
    for i in range (loop):
        for j in range (loop):
            if grid[i][j] != 'd':
                temp.append(grid[i][j])
        if temp != []:
            ret.append(temp)
            temp = []
    return ret

def deletevar(varlist,deleteindex):
    retlist = []
    for i in range (len(deleteindex)):
        varlist[deleteindex[i]] = 'd'
    for i in range (len(varlist)):
        if varlist[i]!= "d":
            retlist.append(varlist[i])
    return retlist




def mark(matrix,num,size):
    for i in range (size):
        for j in range (size):
            if i==num or j == num:
                matrix[i][j] = 'd'


def jacobian(difvar,diffun,loop):
    dvl = [x.strip() for x in difvar.split(',')]
    
    #print (diffun)
    new_diffun = []
    for fun in diffun:
        fun = fun.replace('pow','Pow')
        new_diffun.append(fun)
    diffun = new_diffun

    dfl = diffun

    varlen = len(dvl)
    funlen = len(dfl)
    

    mat_y = [[y] for y in dvl]
    mat_x = [[x] for x in dfl]

    Y = Matrix(mat_y)
    X = Matrix(mat_x)

    jac = X.jacobian(Y)
    Ljac = jac.tolist()
    print (Ljac)
   
    deleterow = []
    deletecol = []
    for i in range (varlen):
        deleteflag = 1
        for j in range (varlen):
            if Ljac[i][j] != 0:
                deleteflag = 0
        if deleteflag ==1:
            deleterow.append(i)

    for j in range (varlen):
        deleteflag = 1
        for i in range (varlen):
            if Ljac[i][j]!=0:
                deleteflag = 0
        if deleteflag ==1:
            deletecol.append(j)

    delete_element=[]
    if len(deletecol)!=0 and len(deleterow)!=0:
        delete_element = list(set(deletecol).intersection(deleterow))
        dvl = deletevar(dvl,delete_element)
        for i in range (len(delete_element)):
            mark(Ljac,delete_element[i],varlen)
        Ljac = removeDelete(Ljac)
    
    #print(Ljac)
    funlen -= len(delete_element)
    varlen -= len(delete_element)

    for i in range (funlen):
        for j in range (varlen):
            Ljac[i][j] = "Entry"+"="+str(Ljac[i][j])

    naturestirng = ""
    naturestirng+=str(len(dvl))
    naturestirng+='\n'
    for i in range (len(dvl)):
        naturestirng+=dvl[i]
        naturestirng+='\n'
    #nodelist =[]
    naturestirng+=str(funlen*varlen)
    naturestirng+='\n'

    for i in range (funlen):
        for j in range (varlen):
            naturestirng+=Ljac[i][j]
            naturestirng+='\n'
    filename = ""
    filename +="../work-dir/jacobiannature"
    filename += str(loop+1)
    filename +=".txt"
    savefile=open(filename,'w')
    savefile.write(naturestirng)
    savefile.close()
    
    
    ComputeLDFstring = ""
    if loop == 0:
        ComputeLDFstring += "from math import *\n"
        ComputeLDFstring += "import numpy as np\n"
        ComputeLDFstring += "import numpy.linalg as la\n"
        #ComputeLDFstring += "import matplotlib.pyplot as plt\n"
        ComputeLDFstring += "import sys\n"
        ComputeLDFstring += "import time\n"
        ComputeLDFstring += "from sympy import Derivative\n"
        ComputeLDFstring += "def jcalc(listvalue,curstate):\n"
        ComputeLDFstring += "    ret = []\n"
    ComputeLDFstring += "    if curstate =="
    ComputeLDFstring += str(loop+1)
    ComputeLDFstring +=":\n"
    for i in range (len(dvl)):
            tempstring = "        "+dvl[i] + "= listvalue[" + str(i) + "]\n"
            ComputeLDFstring += tempstring
    for i in range (funlen):
        for j in range (varlen):
            tempstring="        "+Ljac[i][j]+"\n"
            ComputeLDFstring+=tempstring
            ComputeLDFstring+="        ret.append(Entry)\n"
    ComputeLDFstring +="        return ret\n"
    filename = ""
    filename +="../work-dir/ComputeLDF.py"
    if loop ==0:
        savefile=open(filename,'w')
        savefile.write(ComputeLDFstring)
    else:
        savefile= open(filename, "a")
        savefile.write(ComputeLDFstring)
    savefile.close()

    return delete_element

def createCDFfunction(delete_element):
    ComputeLDFstring = ""
    ComputeLDFstring += "\n"
    with open("../work-dir/ThinVarProp",'r') as thinvarfile:
        thinprop = thinvarfile.readlines()
    thinvarfile.close()
    print("thinprop: "+str(thinprop))
    thinvarlist = []
    for i in range(len(thinprop)):
        if thinprop[i][0] == '1':
            thinvarlist.append(i)
    print(thinvarlist)
    for i in range (len(delete_element)):
        ComputeLDFstring +="if int(state) == "
        ComputeLDFstring += str(i+1)
        ComputeLDFstring += ":\n"
        ComputeLDFstring += "    notbloating=["
        for j in range (len(delete_element[i])):
            ComputeLDFstring+=str(delete_element[i][j])
            if j!= len(delete_element[i])-1:
                ComputeLDFstring +=","
        ComputeLDFstring+="]\n" 

        ComputeLDFstring += "    thin_indicate=["
        emptyflag = 1
        for j in range (len(thinvarlist)):
            if thinvarlist[j] not in delete_element[i]:
                if emptyflag == 0:
                    ComputeLDFstring +=","
                ComputeLDFstring+=str(thinvarlist[j])
                emptyflag = 0
        ComputeLDFstring+="]\n"

        ComputeLDFstring += "    bloating=["
        emptyflag = 1
        for j in range (len(thinprop)):
            if j not in delete_element[i]:
                if emptyflag == 0:
                    ComputeLDFstring +=","
                ComputeLDFstring+=str(j)
                emptyflag = 0
        ComputeLDFstring+="]\n"

        ComputeLDFstring += "    not_thin=["
        emptyflag = 1
        for j in range (len(thinprop)):
            if (j not in thinvarlist) and (j not in delete_element[i]):
                if emptyflag == 0:
                    ComputeLDFstring +=","
                ComputeLDFstring+=str(j)
                emptyflag = 0
        ComputeLDFstring+="]\n"

    ComputeLDFstring += "thin_Jac_ind = []\n"

    ComputeLDFstring += "for not_thin_ele in (not_thin):\n"
    ComputeLDFstring += "    new_non_ele = not_thin_ele\n"
    ComputeLDFstring += "    for not_bloat_ele in notbloating:\n"
    ComputeLDFstring += "        if not_bloat_ele < not_thin_ele:\n"
    ComputeLDFstring += "            new_non_ele-=1\n"
    ComputeLDFstring += "    thin_Jac_ind.append(new_non_ele)\n"

    ComputeLDFstring += "\n"
    ComputeLDFstring += "for i in range (len(thin_indicate)):\n"
    ComputeLDFstring += "   thin_indicate[i]+=1\n"

    ComputeLDFstring += "for i in range (len(bloating)):\n"
    ComputeLDFstring += "   bloating[i]+=1\n"

    ComputeLDFstring += "for i in range (len(not_thin)):\n"
    ComputeLDFstring += "   not_thin[i]+=1\n"


    ComputeLDFstring += "\nfor i in range (len(notbloating)):\n"
    ComputeLDFstring += "   #delta.pop(notbloating[i])\n"
    ComputeLDFstring += "   notbloating[i]+=1\n"

    ComputeLDFstring += "\n"
    ComputeLDFstring += "f = open('../work-dir/SimuOutput', 'r')\n"
    ComputeLDFstring += "x = f.readlines()\n"
    ComputeLDFstring += "f.close()\n"

    ComputeLDFstring += "start = time.time()\n"

    ComputeLDFstring += "numofvar = len(x[0].rstrip().split())\n"
    ComputeLDFstring += "Simulation_data = [[] for i in range(len(x))]\n"
    ComputeLDFstring += "curline = 0\n"
    ComputeLDFstring += "for line in x:\n"
    ComputeLDFstring += "    if line.rstrip():\n"
    ComputeLDFstring += "        dataLine = line.rstrip().split()\n"
    ComputeLDFstring += "    for i in range (numofvar):\n"
    ComputeLDFstring += "        Simulation_data[curline].append(float(dataLine[i]))\n"
    ComputeLDFstring += "    curline+=1\n"
    ComputeLDFstring += "Simulation_data = np.array(Simulation_data)\n"
    ComputeLDFstring += "CT_step = min(CT_step,len(Simulation_data))\n"
    ComputeLDFstring += "blowting = np.zeros((int(len(x)),len(not_thin)))\n"
    ComputeLDFstring += "deltacopy = list(delta)\n"

    ComputeLDFstring += "blowting[0,:] = [delta[tmp] for tmp in [tmp_list-1 for tmp_list in not_thin]]\n"
    ComputeLDFstring += "numofvar-=(1+len(notbloating))\n"


    ComputeLDFstring += "Reach_tube = np.zeros((len(Simulation_data),len(Simulation_data[0])))\n"
    ComputeLDFstring += "if numofvar>0:\n"
    ComputeLDFstring += "   for i in range (0,len(Simulation_data),CT_step):\n"
    ComputeLDFstring += "       mean_array = np.mean(Simulation_data[i:i+CT_step,bloating],axis=0)\n"
    ComputeLDFstring += "       if len(mean_array)!=numofvar:\n"
    ComputeLDFstring += "           sys.exit('Error in calculating the mean matrix for coordinated transformation')\n"
    ComputeLDFstring += "       Jacobian_CT = jcalc(mean_array,int(state))\n"
    ComputeLDFstring += "       Jacobian_CT = np.reshape(Jacobian_CT,(numofvar,numofvar))\n"
    ComputeLDFstring += "       Jacobian_CT = Jacobian_CT[:,thin_Jac_ind]\n"
    ComputeLDFstring += "       Jacobian_CT = Jacobian_CT[thin_Jac_ind,:]\n"
    ComputeLDFstring += "       Eigenvalues,CT_matrix = la.eig(Jacobian_CT)\n"
    ComputeLDFstring += "       Local_Lipschitz = la.norm(Jacobian_CT)\n"
    ComputeLDFstring += "       for j in range (i,min(i+CT_step,len(x)),2):\n"
    ComputeLDFstring += "           if j+1 > len(Simulation_data):\n"
    ComputeLDFstring += "               sys.exit('Error accessing the last rectangle')\n"
    ComputeLDFstring += "           Delta_time = Simulation_data[j+1,0] - Simulation_data[j,0]\n"
    ComputeLDFstring += "           Simulation_box = Simulation_data[j:j+2,bloating]\n"
    ComputeLDFstring += "           Current_Jacobian = jcalc(np.mean(Simulation_box,axis=0),int(state))\n"
    ComputeLDFstring += "           Current_Jacobian = np.reshape(Current_Jacobian,(numofvar,numofvar))\n"
    ComputeLDFstring += "           Current_Jacobian = Current_Jacobian[:,thin_Jac_ind]\n"
    ComputeLDFstring += "           Current_Jacobian = Current_Jacobian[thin_Jac_ind,:]\n"
    ComputeLDFstring += "           Current_Jordan = np.dot(la.inv(CT_matrix),np.dot(Current_Jacobian,CT_matrix))\n"
    ComputeLDFstring += "           Current_lambda = max(la.eigvalsh(np.transpose(Current_Jordan)+Current_Jordan))/2\n"
    ComputeLDFstring += "           Disturb_matrix = np.reshape(jcalc((np.amax(Simulation_box,axis=0)+max(blowting[j,:])/2),int(state)),(numofvar,numofvar))-np.reshape(jcalc((np.amin(Simulation_box,axis=0)-max(blowting[j,:])/2),int(state)),(numofvar,numofvar))\n"
    ComputeLDFstring += "           Disturbance = la.norm(Disturb_matrix,ord=2)*exp(Local_Lipschitz*Delta_time)\n"
    ComputeLDFstring += "           Current_lambda = Current_lambda + Disturbance\n"
    ComputeLDFstring += "           if j == 0:\n"
    ComputeLDFstring += "               blowting[1,:] = blowting[0,:]*exp(Current_lambda * Delta_time)\n"
    ComputeLDFstring += "           else:\n"
    ComputeLDFstring += "               blowting[j,:] = blowting[j-1,:]\n"
    ComputeLDFstring += "               blowting[j+1,:] = blowting[j,:] * exp(Current_lambda * Delta_time)\n"
    ComputeLDFstring += "       for cnt in range (min(CT_step,len(Simulation_data))-i):\n"
    ComputeLDFstring += "           blowting[i+cnt,:] = blowting[i+cnt,:] * la.cond(CT_matrix)\n"


    ComputeLDFstring += "   for i in range (0,len(Simulation_data),2):\n"
    ComputeLDFstring += "       Reach_tube[i,not_thin] = np.amin(Simulation_data[i:i+2,not_thin],axis=0)-blowting[i,:]\n"
    ComputeLDFstring += "       Reach_tube[i+1,not_thin] = np.amax(Simulation_data[i:i+2,not_thin],axis=0)+blowting[i,:]\n"

    ComputeLDFstring += "       Reach_tube[i,notbloating] = Simulation_data[i,notbloating] - [deltacopy[tmp] for tmp in [tmp_list-1 for tmp_list in notbloating]]\n"
    ComputeLDFstring += "       Reach_tube[i+1,notbloating] = Simulation_data[i+1,notbloating] + [deltacopy[tmp] for tmp in [tmp_list-1 for tmp_list in notbloating]]\n"

    ComputeLDFstring += "       Reach_tube[i,thin_indicate] = Simulation_data[i,thin_indicate]\n"
    ComputeLDFstring += "       Reach_tube[i+1,thin_indicate] = Simulation_data[i+1,thin_indicate]\n"

    ComputeLDFstring += "for i in range (0,len(Simulation_data),2):\n"
    ComputeLDFstring += "   Reach_tube[i,0] = Simulation_data[i,0]\n"
    ComputeLDFstring += "   Reach_tube[i+1,0] = Simulation_data[i+1,0]\n"



    ComputeLDFstring += "\n"
    ComputeLDFstring += "end = time.time()\n"
    ComputeLDFstring += "print('ComputeLDF Time: ' + str(end-start) + 's')\n"
    ComputeLDFstring += "print('Final Bloating Size:')\n"
    ComputeLDFstring += "print(blowting[-1,:])\n"
    ComputeLDFstring += "f = open('../work-dir/reachtube.dat', 'w')\n"
    ComputeLDFstring += "bloatstring = ''\n"
    ComputeLDFstring += "bloatstring += state\n"
    ComputeLDFstring += "bloatstring += '\\n'"
    ComputeLDFstring += "\n"
    ComputeLDFstring += "for i in range (Reach_tube.shape[0]):\n"
    ComputeLDFstring += "    for j in range (Reach_tube.shape[1]):\n"
    ComputeLDFstring += "        bloatstring+=str(Reach_tube[i][j])\n"
    ComputeLDFstring += "        bloatstring+=' '\n"
    ComputeLDFstring += "    bloatstring+='\\n'\n"
    ComputeLDFstring += "f.write(bloatstring)\n"
    ComputeLDFstring += "f.close()\n"
  
    filename = ""
    filename +="../work-dir/ComputeLDF.py"
    savefile=open(filename,'a')
    savefile.write(ComputeLDFstring)
    savefile.close()
    
    

