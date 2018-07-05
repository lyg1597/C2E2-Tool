import numpy as np
import time
import sympy

from bokeh.io import save, export_png
from bokeh.plotting import figure

from frontend.mod.constants import *
from frontend.mod.session import Session


def plotGraph(data_filepath, unsafe_set, var_list, mode_list,
    var_index_list, time_step, time_horizon, title, filename, x_var, 
    y_var_list, plotter_version):
    """
    File Format for input files:
        Line 0: Property Name
        Line 1: Simulated/Verified
        Line 2: Mode Names
        Line 3: Variable Names
        Line 4: Time Step
        Line 5: Time Horizon
        Line 6: K Value
        Line 7: Simulator
        Line 8: Refinement Strategy
        Line 9: Initial Set
        Line 10: Unsafe Set
    """

    print("WARNING: CALLING BETA PLOTGRAPH")

    file_object = open(data_filepath, 'r')

    variables = []
    for i in range(len(var_index_list)):
        variables.append([])
    lines = file_object.readlines()

    # Lines 0 - 11 contain property information
    for i in range(11, len(lines)):
        points = lines[i].split()
        if points[0] == '%':
            continue
        for j, k in enumerate(var_index_list):
            variables[j].append(float(points[k]))        

    property_name = lines[0].strip()
    sim_ver = lines[1].split().strip()
    mode_names = lines[2].split()
    variable_names = lines[3].split()
    time_step = float(lines[4].strip())
    time_horizon = float(lines[5].strip())
    k_value = float(lines[6].strip())
    simulator = lines[7].strip()
    refinement_strategy = lines[8].strip()
    initial_set = lines[9].strip()
    unsafe_set = lines[10].strip()

    if lines[1].split()[0] == SIMULATED:
        plot_line(variables, var_list, var_index_list, title, filename)
        return
    else:
        plot_quad(variables, var_list, var_index_list, title, filename)
        return

def plot_graph(plot_data, var_list, var_index_list, title, filename):
    
    print("Under Construction")

def generate_box_annotation_regions(var_list, var_index_list, unsafe_set_str):

    box_regions = []
    for eq_str in unsafe_set_str.split('&&'):
        eq = sympy.sympify(eq_str)
        if str(eq.lhs) not in var_list:
            continue
        if str(eq.lhs) == var_list:
            continue
            

def plot_line(variables, var_list, var_index_list, title, filename):

    bokeh_plot = figure()

    for i in range(1, len(variables)):
        x_axis = []
        y_axis = []
        for j in range(0, len(variables[0]), 2):
            x_axis.append(variables[0][j])
            y_axis.append(variables[i][j])

        bokeh_plot.line(x_axis, y_axis, line_width=2, color=PLOT_COLORS[i-1])

    filedir = '../work-dir/plotresult/'
    save(bokeh_plot, filename=filedir+filename+'.html', title=title)
    export_png(bokeh_plot, filename=filedir+filename+'.png')

def plot_quad(variables, var_list, var_index_list, title, filename):

    bokeh_plot = figure()

    for i in range(1, len(variables)):
        top = []
        bottom = []
        left = []
        right = []
        for j in range(1, len(variables[0]), 2):
            # Left/Right side defined by time values, always 0th column
            left.append(variables[0][j-1])
            right.append(variables[0][j])
            # Top/Bottom defined by max/min of variable values, ith column
            top.append(max(variables[i][j-1], variables[i][j]))
            bottom.append(min(variables[i][j-1], variables[i][j]))
        
        bokeh_plot.quad(top=top, bottom=bottom, left=left, right=right,
            color=PLOT_COLORS[i-1])

    filedir = '../work-dir/plotresult/'
    save(bokeh_plot, filename=filedir+filename+'.html', title=title)
    export_png(bokeh_plot, filename=filedir+filename+'.png')