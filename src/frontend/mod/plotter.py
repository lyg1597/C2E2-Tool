import numpy as np
import time

from bokeh.io import save, export_png
from bokeh.plotting import figure

from frontend.mod.constants import *


def plotGraph(data_filepath, unsafe_set, var_list, mode_list,
    var_index_list, time_step, time_horizon, title, filename, x_var, 
    y_var_list, plotter_version):

    file_object = open(data_filepath, 'r')

    variables = []
    for i in range(len(var_index_list)):
        variables.append([])
    lines = file_object.readlines()

    for i in range(3, len(lines)):
        points = lines[i].split()
        if points[0] == '%':
            continue
        for j, k in enumerate(var_index_list):
            variables[j].append(float(points[k]))

    if lines[0].split()[0] == SIMULATED:
        bokeh_line(variables, var_list, var_index_list, title, filename)
        return
    else:
        bokeh_quad(variables, var_list, var_index_list, title, filename)
        return


def bokeh_line(variables, var_list, var_index_list, title, filename):

    print("***** bokeh_line entry point *****")
    print("variables: " + str(variables))
    print("var_list: " + str(var_list))
    print("var_index_list: " + str(var_index_list))
    print("title: " + str(title))
    print("filename: " + str(filename))

    bokeh_plot = figure()

    for i in range(1, len(variables)):
        x_axis = []
        y_axis = []
        for j in range(0, len(variables[0]), 2):
            x_axis.append(variables[0][j])
            y_axis.append(variables[i][j])

        print("x_axis " + str(i) + ": " + str(x_axis))
        print("y_axis " + str(i) + ": " + str(y_axis))
        bokeh_plot.line(x_axis, y_axis, line_width=2)

    filedir = '../work-dir/plotresult/'
    save(bokeh_plot, filename=filedir+filename+'.html', title=title)
    export_png(bokeh_plot, filename=filedir+filename+'.png')


def bokeh_quad(variables, var_list, var_index_list, title, filename):

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