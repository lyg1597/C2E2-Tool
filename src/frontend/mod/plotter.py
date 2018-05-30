import numpy as np
import time

from bokeh.io import save, export_png
from bokeh.plotting import figure


def plotGraph(reach_set_filepath, unsafe_set, var_list, mode_list,
    var_index_list, time_step, time_horizon, title, filename, x_var, 
    y_var_list, plotter_version):

    return bookeh_minimum(reach_set_filepath, var_list, var_index_list, title,
        filename)

def bookeh_minimum(reach_set_filepath, var_list, var_index_list, title, 
    filename):

    COLORS = ['blue', 'green', 'red', 'yellow', 'orange', 'cyan', 'pink']

    file_object = open(reach_set_filepath, 'r')
    bokeh_plot = figure()

    variables = []
    for i in range(len(var_index_list)):
        variables.append([])

    for line in iter(file_object):
        points = line.split()
        if points[0] == '%':
            continue
        j = 0
        for i in var_index_list:
            variables[j].append(float(points[i]))
            j += 1

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
            color=COLORS[i])

    filedir = '../work-dir/plotresult/'
    save(bokeh_plot, filename=filedir+filename+'.html', title=title)
    export_png(bokeh_plot, filename=filedir+filename+'.png')