import numpy as np
import scipy as sp
from scipy.integrate import odeint

def func(x, t):
    dx_0 = 1 + x[0] * x[0] * x[1] - 2.5 * x[0];
    dx_1 = 1.5 * x[0] - x[0] * x[0] * x[1];
    return dx_0, dx_1

init = 1.93, 1.06
t = np.linspace(0, 10, 1001)
sol = odeint(func, init, t)

for i,s in enumerate(sol):
    print("%.2f %.9f %.9f" % (t[i], s[0], s[1]))
