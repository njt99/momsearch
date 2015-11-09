#!/usr/bin/python

import os, sys, glob
from pprint import *
from cmath import *
from numpy import floor, ceil, dot
 
scale = map(lambda x : 8 * pow(2, x/6.), range(0,-6,-1))

def g_depth(word) :
    g_count = 0
    for char in word :
        if char == 'g' or char == 'G' :
            g_count += 1
    return g_count

def real(x) :
    return x.real

def imag(x) :
    return x.imag

def norm(x) :
    return real(x*x.conjugate())    

def vert_hyp_dist(y2,y1) :
    return abs(real(log(y2/y1)))

# Rotate 180 around geodesic with radius r = |c2-c1| with an end point at c2 and top point over c1
# The c1 horoball rotates to a horoball at infinity of height r^2/h1
def dist_btw_balls(c1=0,h1=1,c2=1,h2=1) :
    r_sqrd= norm(c2-c1)
    return real(log(r_sqrd/(h1*h2)))

def get_a(M):
    return M[0][0]

def get_b(M):
    return M[0][1]

def get_c(M):
    return M[1][0]

def get_d(M):
    return M[1][1]

def mobius(M,z) :
    return (get_a(M)*z + get_b(M)) / (get_c(M)*z + get_d(M))  

def max_horo_height(M):
    # assert linalg.det(M) == 1.
    c = get_c(M)
    if c != 0. :
        return  1/abs(c)
    else :
        return 0

# Center of image of inifnity horoball under M
def horo_center_inf(M) :
    c = get_c(M)
    assert c != 0.
    return get_a(M)/c

# Height of image of infinity horoball under M
def horo_image_height_inf(M,h):
    c = get_c(M)
    assert c != 0.
    d = get_d(M)
    return 1. / real(h * c * c.conjugate())

def horo_image_height(M,z,h):
    # assert linalg.det(M) == 1.
    c = get_c(M)
    d = get_d(M)
    if c*z + d != 0. :
        return h / norm(c*z + d)
    else : # We really should be here. This is height of infinity horoball
        return 1. / real(h * c * c.conjugate())

def get_params(box) :
    pos = 0
    size = [1.]*6
    center = [0.]*6

    for direction in box :
        p = pos % 6
        size[p] *= 0.5
        center[p] += float((2*int(direction) - 1)) * size[p]
        pos += 1
    
    params = {}
    params['lattice'] = scale[3]*center[3] + scale[0]*center[0]*1.j
    params['lox_sqrt'] = scale[4]*center[4] + scale[1]*center[1]*1.j
    params['parabolic'] = scale[5]*center[5] + scale[2]*center[2]*1.j
   
    params['center'] = center
    params['size'] = size

    params['lattice_jet'] = { 'f' : scale[3]*center[3] + scale[0]*center[0]*1.j, 'df0' : scale[3]*size[3] + scale[0]*size[0]*1.j, 'df1' : 0., 'df2' : 0., 'error' : 0. }
    params['lox_sqrt_jet'] = { 'f' :  scale[4]*center[4] + scale[1]*center[1]*1.j, 'df0' : 0., 'df1' : scale[4]*size[4] + scale[1]*size[1]*1.j, 'df2' : 0., 'error' : 0. }
    params['parabolic_jet'] = { 'f' :  scale[5]*center[5] + scale[2]*center[2]*1.j, 'df0' : 0., 'df1' : 0., 'df2' : scale[5]*size[5] + scale[2]*size[2]*1.j, 'error' : 0. }

    return params

def min_parameters(params) :
    center = params['center']
    size = params['size']
    m = [0.]*6
    # Get values as close to zero in box as we can
    for i in range(5) :
        if center[i] < 0 :
            m[i] = scale[i]*(center[i]+size[i])
        else :
            m[i] = scale[i]*(center[i]-size[i])

    min_params = {}
    min_params['lattice'] = m[3] + m[0]*1.j
    min_params['lox_sqrt'] = m[4] + m[1]*1.j
    min_params['parabolic'] = m[5] + m[2]*1.j

    return min_params

def max_parameters(params) :
    center = params['center']
    size = params['size']
    m = [0.]*6
    # Get values as close to zero in box as we can
    for i in range(5) :
        if center[i] < 0 :
            m[i] = scale[i]*(center[i]-size[i])
        else :
            m[i] = scale[i]*(center[i]+size[i])

    max_params = {}
    max_params['lattice'] = m[3] + m[0]*1.j
    max_params['lox_sqrt'] = m[4] + m[1]*1.j
    max_params['parabolic'] = m[5] + m[2]*1.j

    return max_params

def min_parameter(params, key) :
    return min_parameters(params)[key]

def max_parameter(params, key) :
    return max_parameters(params)[key]

def jet_size(jet) :
    return abs(jet['df0']) + abs(jet['df1']) + abs(jet['df2'])

def jet_min_abs(jet) :
    v = abs(jet['f']) - jet_size(jet) - jet['error']
    return max(v,0.)

def min_jet_parameter(params,key) :
    return jet_min_abs(params[key]) 

def jet_max_abs(jet) :
    v = abs(jet['f']) + jet_size(jet) + jet['error']
    return max(v,0.)

def max_jet_parameter(params,key) :
    return jet_max_abs(params[key]) 

def get_min_area(params) :
    abs_lox_sqrt = min_jet_parameter(params,'lox_sqrt_jet')
    min_lattice = min_parameter(params,'lattice')
    return abs_lox_sqrt * abs_lox_sqrt * imag(min_lattice)

def get_max_area(params) :
    abs_lox_sqrt = max_jet_parameter(params,'lox_sqrt_jet')
    max_lattice = max_parameter(params,'lattice')
    return abs_lox_sqrt * abs_lox_sqrt * imag(max_lattice)

def get_G(params) :
    l = params['lattice']
    s = params['lox_sqrt']
    p = params['parabolic']
    return [[p*s*1.j, 1.j/s], [s*1.j, 0.]]

def get_g(params) :
    l = params['lattice']
    s = params['lox_sqrt']
    p = params['parabolic']
    return [[0., -1.j/s], [-s*1.j, p*s*1.j]]

# Give parabolic element with M,N power counts
def get_T(params, M_pow, N_pow) :
    p = params['lattice']
    return [[1., p*float(M_pow) + float(N_pow)],[0.,1.]]

def get_first(word) :
    if len(word) > 0 :
        return word[0]
    else :
        return ''

def quad_sol(a,b,c) :
    d = b * b - 4. * a * c
    sq_d = sqrt(d)
    return ((-b - sq_d)/(2*a), (-b + sq_d)/(2*a))
