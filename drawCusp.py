#!/usr/bin/python

import os, sys, glob, pprint
from numpy import *

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
        return h / ((c*z + d)*(c*z + d).conjugate())
    else :
        return 1 / (h * c * c.conjugate())

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

box = sys.argv[1]
depth = int(sys.argv[2])
params = get_center(box)
G = get_G(params)
g = get_g(params)
M = get_T(params,1,0)
N = get_T(params,0,1)
elements = { 'g' : g, 'G' : G, 'GG' : dot(G,G), 'MN' : dot(M,N), 'M' : M, 'N' : N }
cusp_height = max_horo_height(G)
I = [[1.,0.],[0.,1.]]
horoballs = { 0 : { '' : { 'center' : 0, 'height' : cusp_height, 'gamma' : I }}}

# Print box paramenters and group elements
print 'Parameters:'
pprint.pprint(params,width=1)
print 'Elemnts:'
pprint.pprint(elements,width=2)

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
            valid = ['M', 'N', 'GG']
        else :
            valid = ['g', 'G']        
 
        # Apply G and g if possible
        for h_char in valid :
            print 'Loop with element: {0}'.format(h_char)
            h = elements[h_char] 
            new_height = horo_image_height(h, center, height)
            h_center = mobius(h, center)
            print 'H: {0}'.format(h)
            print 'Lattice: {0}'.format(lattice)
            print 'Center: {0}'.format(center)
            print 'H Center: {0}'.format(h_center)
            # The pojection of h_center to the real axis along lattice
            # Recall that we assume -0.5 < real(lattice) < 0.5 and imag(lattice) > 0
            M_len = imag(h_center) / imag(lattice)
            N_len = real(h_center) - M_len * real(lattice)
            print 'M length: {0} and N length: {1}'.format(M_len, N_len)
            # Adjustments we make to land inside the fundamental domain 
            M_pow = -floor(M_len) if abs(M_len) > 1. else 0 
            N_pow = -floor(N_len) if abs(N_len) > 1. else 0
            print 'M power: {0} and N power: {1}'.format(M_pow, N_pow)
            # Keep track of the word and it's representative
            new_word = h_char + word
            new_gamma = dot(h,gamma)
            print 'Gamma: {0}'.format(gamma)
            print 'H Gamma: {0}'.format(new_gamma)
            M_word = 'M'*M_pow if M_pow > 0 else 'm'*M_pow 
            new_word = M_word + new_word
            N_word = 'N'*N_pow if N_pow > 0 else 'n'*N_pow 
            new_word = N_word + new_word
            T = get_T(params, M_pow, N_pow)
            new_gamma = dot(T, new_gamma)
            new_center = mobius(T, h_center)

            print 'New word: {0}'.format(new_word)
            print 'New center: {0}'.format(new_center)
            print 'New gamma: {0}'.format(new_gamma)

            horoballs[d+1][new_word] = { 'center' : new_center, 'height' : new_height, 'gamma' : new_gamma }
            print horoballs
    d += 1

print horoballs 
