#!/usr/bin/python

import os, sys, glob
from numpy import *
from Tkinter import *
from pprint import *

scale = map(lambda x : 8 * pow(2, x/6.), range(0,-6,-1))

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
    if c != 0 :
        return  1/abs(c)
    else :
        return 0

def horo_image_height(M,z,h):
    # assert linalg.det(M) == 1.
    c = get_c(M)
    d = get_d(M)
    if c*z + d != 0 :
        return h / real((c*z + d)*(c*z + d).conjugate())
    else :
        return 1. / real(h * c * c.conjugate())

def get_center(box) :
    pos = 0
    size = [1.]*6
    center = [0.]*6

    for direction in box :
        p = pos % 6
        size[p] *= 0.5
        center[p] += (2*int(direction) - 1) * size[p]
        pos += 1
    
    params = {}
    params['lattice'] = scale[3]*center[3] + scale[0]*center[0]*1j
    params['lox_sqrt'] = scale[4]*center[4] + scale[1]*center[1]*1j
    params['parabolic'] = scale[5]*center[5] + scale[2]*center[2]*1j

    return params

def get_G(params) :
    l = params['lattice']
    s = params['lox_sqrt']
    p = params['parabolic']
    return [[p*s*1j, 1j/s], [s*1j, 0.]]

def get_g(params) :
    l = params['lattice']
    s = params['lox_sqrt']
    p = params['parabolic']
    return [[0., -1j/s], [-s*1j, p*s*1j]]

# Give parabolic element with M,N power counts
def get_T(params, M_pow, N_pow) :
    p = params['lattice']
    return [[1, p*M_pow + N_pow],[0,1]]

def get_first(word) :
    if len(word) > 0 :
        return word[0]
    else :
        return ''

if len(sys.argv) < 3 :
    print 'Usage: drawCusp box_code depth <height_cutoff>'
    sys.exit(2)

box = sys.argv[1]
depth = int(sys.argv[2])
height_cutoff = float(sys.argv[3]) if len(sys.argv) > 3 else 0.

params = get_center(box)
G = get_G(params)
g = get_g(params)
M = get_T(params,1,0)
N = get_T(params,0,1)
elements = { 'g' : g, 'G' : G, 'GG' : dot(G,G), 'MN' : dot(M,N), 'M' : M, 'N' : N }
cusp_height = max_horo_height(G)
I = [[1.,0.],[0.,1.]]
horoballs = { 0 : { '' : { 'center' : 0, 'height' : cusp_height, 'gamma' : I, 'word' : ''}}}

# Print box paramenters and group elements
print 'Parameters:'
pprint(params,width=1)
print 'Elemnts:'
pprint(elements,width=2)

# Generate new horoballs
d = 0
lattice = params['lattice']
while d < depth :
    horoballs[d+1] = {}
    for word,ball in horoballs[d].iteritems() :
        gamma = ball['gamma']
        center = ball['center']
        height = ball['height']

        first = get_first(word)
        if first == 'g' : 
            valid = ['g']
        elif first == 'G' :
            valid = ['G']
        elif first == '' :
            valid = ['M', 'N', 'GG', 'MN']
        else :
            valid = ['g', 'G']        
 
        # Apply G and g if possible
        for h_char in valid :
            h = elements[h_char] 
            new_height = horo_image_height(h, center, height)
            h_center = mobius(h, center)

            # The pojection of h_center to the real axis along lattice
            # Recall that we assume -0.5 < real(lattice) < 0.5 and imag(lattice) > 0
            M_len = imag(h_center) / imag(lattice)
            N_len = real(h_center) - M_len * real(lattice)

            # Adjustments we make to land inside the fundamental domain 
            M_pow = -floor(M_len) if M_len != 1. else 0 
            N_pow = -floor(N_len) if N_len != 1. else 0

            # Keep track of the word and it's representative
            new_word = h_char + word
            new_gamma = dot(h,gamma)
            if new_height > 2*cusp_height :
                sys.stderr.write('Possible giant horoball with word {0} of height {1} with center {2}\nElement:\n'.format(new_word,new_height,new_center))
                sys.stderr.write(pformat(new_gamma,width=2)+'\n')
            else :
                M_word = 'M'*M_pow if M_pow > 0 else 'm'*M_pow 
                new_word = M_word + new_word
                N_word = 'N'*N_pow if N_pow > 0 else 'n'*N_pow 
                new_word = N_word + new_word
                T = get_T(params, M_pow, N_pow)
                new_gamma = dot(T, new_gamma)
                new_center = mobius(T, h_center)

                horoballs[d+1][new_word] = { 'center' : new_center, 'height' : new_height, 'gamma' : new_gamma, 'word' : new_word }
    d += 1

#pprint(horoballs,width=2)

def draw_ball(ball, canvas, origin, factor, place_label=False) :
    center = ball['center']
    height = ball['height']
    
    x = origin[0] + factor*real(center)
    y = origin[1] - factor*imag(center) # Flip coordinate system 
    r = factor*height/2.
    #print '({0},{1},{2})'.format(x,y,r)

    canvas.create_oval(x-r,y-r,x+r,y+r)

    if place_label :
        word = ball['word']
        label_text = word if len(word) > 0 else 'I'
        canvas.create_text(x,y,text=label_text)

def add_tuples(v,w) :
    return tuple(map(sum,zip(v,w)))

def factor_tuple(v,factor) :
    return tuple(factor*x for x in v)

canvas_width = 640
canvas_height = 800
point_rad = 5
factor = 600/(abs(lattice)+1)

print 'Factor {0}'.format(factor)
print 'Lattice {0}'.format(lattice)

window = Tk()
canvas = Canvas(window, width=canvas_width, height=canvas_height)
canvas.pack()

# Pick an origin. TODO: Make this more robust
origin = [0.,0.]
origin[0] = canvas_width/4 if real(lattice) > -0.2 else canvas_width/2.5
origin[1] = factor*(imag(lattice)+0.5)
print 'Origin {0}'.format(origin)

# Draw the lattice domain lines and points
lattice_points = [(1.,0.), (real(lattice) + 1, imag(lattice)),(real(lattice), imag(lattice)), (0.,0.)]
v_coord = copy(origin)
prev_coord = copy(origin) 
for v in lattice_points :
    v_coord[0] = origin[0]+factor*v[0]
    v_coord[1] = origin[1]-factor*v[1]
    canvas.create_oval(v_coord[0]-point_rad,v_coord[1]-point_rad, v_coord[0]+point_rad, v_coord[1]+point_rad,fill='black')
    canvas.create_line(prev_coord[0],prev_coord[1],v_coord[0],v_coord[1])
    prev_coord = copy(v_coord)

ball_count = 0
for d in horoballs :
    depth_level = horoballs[d]
    for word in depth_level :
        ball = depth_level[word]
        height = ball['height']
        if height > height_cutoff :
            draw_ball(ball,canvas,origin,factor)
            ball_count += 1

print 'Ball count: {0}'.format(ball_count)
mainloop()



