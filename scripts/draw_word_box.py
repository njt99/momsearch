#!/usr/bin/python

import os, sys, glob, re
from Tkinter import *
from pprint import *
from cmath import *
from numpy import floor, ceil, dot, sqrt
from tkFileDialog import *
from itertools import izip, tee

def pairwise(iterable):
    "s -> (s0,s1), (s1,s2), (s2, s3), ..."
    a, b = tee(iterable)
    next(b, None)
    return izip(a, b)

def vect_dir(p,q) :
    assert(len(p) == len(q))
    length = len(p)
    v = [q[i] - p[i] for i in range(length)]
    norm = sqrt(sum([v[i]**2 for i in range(length)]))
    return tuple([v[i]/norm for i in range(length)])

def vect_add(p,q) :
    assert(len(p) == len(q))
    return tuple([p[i]+q[i] for i in range(len(p))])

def vect_scale(p,c) :
    return tuple([p[i]*c for i in range(len(p))])

def rev(string) :
    return string[::-1]

def vect_mid(p,q) :
    return vect_scale(vect_add(p,q),0.5)

def inv_gen(char) :
    if char.islower() :
        return char.upper()
    else :
        return char.lower()

def gen_total_power(word,gen) :
    gen_count = 0
    for char in word :
        if char == gen or char == inv_gen(gen) :
            gen_count += 1
    return gen_count

def reduce_lattice(word) :
    new_word = ''
    x = 0 # M power
    y = 0 # N power
    for gen in word :
        if gen == 'M' :
            x += 1 
        if gen == 'm' :
            x -= 1
        if gen == 'N' :
            y += 1 
        if gen == 'n' :
            y -= 1 
        if gen == 'g' or gen == 'G' :
            Ms = 'M'*x if x > 0 else 'm'*(-x)
            Ns = 'N'*y if y > 0 else 'n'*(-y)
            new_word += Ms + Ns + gen
            x = 0 # M power
            y = 0 # N power
    Ms = 'M'*x if x > 0 else 'm'*(-x)
    Ns = 'N'*y if y > 0 else 'n'*(-y)
    new_word += Ms + Ns
    return new_word 

# Truncate any lattice subword on the right end of a word
def max_ball_word(word) :
    last_g_idx = max(word.rfind('g'),word.rfind('G'))
    return word[0:last_g_idx+1]

def inverse(word) :
    new_word = ''
    for gen in rev(word) :
        new_word += inv_gen(gen)
    return reduce_lattice(new_word)

def rotate_word(word, idx_of_last) :
    length = len(word)
    assert(-length < idx_of_last + 1 and idx_of_last < length) 
    new_word = word[idx_of_last+1:length] + word[0:idx_of_last+1]
    return reduce_lattice(new_word)

# We generate a sequene of arcs connecting 
# horoballs assocaited to a word. Conjugate a relator word
# to the form w = (l_1 g_1)(l_2 g_2) ... (l_n g_n) where
# l_i is a word in M,N,m,n and g_i is either g or G.
# Let w_k = (l_1 g_1)(l_2 g_2) ... (l_k g_k) 
# The associated horoball necklace for w is
# B(w_1(\infty)), B(w_2(\infty)) ... B(w_n(\infty)) = B(\infty),
# where B(*) denotes the maximal horobal tandent at the given point.
# We consider the disc D given by gluing the ideal trinagles with
# vertices w_k(\infty), w_{k+1}(\infty), and \infty.
# The function below returns the piecewise geoesic arcs on 
# a small cusp neighborhood that arrise by intersecting with D.
def get_sequence_of_arcs(word) :
    last_g_idx = max(word.rfind('g'),word.rfind('G'))
    w = rotate_word(word, last_g_idx)
    print 'Rotated word is {}'.format(w)
    g_power = gen_total_power(w,'g')
    sub_word = ''
    sub_words = []
    seq = []
    lattice = ''
    prev_g = w[-1]
    for gen in w :
        if (gen != 'g' and gen != 'G') :
            lattice += gen 
        else :
            if 1 < gen_total_power(sub_word,'g') < g_power - 1 :
                seq.append((inv_gen(prev_g), max_ball_word(inverse(sub_word)), lattice + gen))
            else :
                seq.append((inv_gen(prev_g), lattice + gen))
            prev_g = gen
            sub_word += (lattice + gen)
            sub_words.append(sub_word)
            lattice = ''
    seq[0] = tuple([seq[0][0]] + rev(sub_words[1:-2]) + [seq[0][1]]) 
    return seq

#### Box and mobius code ####
def real(x) :
    return x.real

def imag(x) :
    return x.imag

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

# Center of image of inifnity horoball under M
def horo_center_inf(M) :
    c = get_c(M)
    assert c != 0.
    return get_a(M)/c

def max_horo_height(M):
    c = get_c(M)
    if c != 0. :
        return  1./abs(c)
    else :
        return 0.

def horo_image_height_inf(M,h):
    c = get_c(M)
    assert c != 0.
    d = get_d(M)
    return 1. / real(h * c * c.conjugate())

scale = map(lambda x : 8 * pow(2, x/6.), range(0,-6,-1))

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

    return params

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
    return [[1., p*float(N_pow) + float(M_pow)],[0.,1.]]

def get_gen(params) :
    G = get_G(params)
    g = get_g(params)
    M = get_T(params,1,0)
    m = get_T(params,-1,0)
    N = get_T(params,0,1)
    n = get_T(params,0,-1)
    return { 'g' : g, 'G' : G, 'M' : M, 'm' : m, 'N' : N, 'n' : n }

def get_mob_from_gen(word,elem) :
    w = [[1.,0.],[0.,1.]]
    g = get_g
    for gen in word :
        w = dot(w,elem[gen])
    return w

def get_mob_from_params(word,params) :
    return get_mob_from_gen(word, get_gen(params))


### Diagram drawing code ###
class word_diagram(Tk) :

    def __init__(self,parent):
        Tk.__init__(self,parent)
        self.parent = parent
        self.initialize()

    def setup_interface(self) :
        self.init_canvas_width = 800.
        self.init_canvas_height = 800.
        self.point_rad = 5.

        self.canvas = Canvas(self, width=self.init_canvas_width, height=self.init_canvas_height)
        self.canvas.pack(side='left', fill='both', expand=False)

        self.selected_objects = set([])

        self.factor = self.init_canvas_width/(3. * self.lattice_len)
        print self.factor
        self.origin = [self.init_canvas_width/2.,self.init_canvas_width/2.]

    def initialize(self) :
        if len(sys.argv) < 2 :
            print 'Usage: draw_word_box word box'
            sys.exit(2)

        self.word = sys.argv[1]
        self.box = sys.argv[2]
        self.g_power = gen_total_power(self.word, 'g') 

        self.build_box_data()
        self.edges = get_sequence_of_arcs(self.word)

        self.setup_interface()
        self.draw_lattice()
        self.draw_diagram()

    def build_box_data(self) :
        self.params = get_params(self.box)
        self.gen = get_gen(self.params)
        self.lattice_len = abs(self.params['lattice'])

    def ball_coord(self, w) :
        z = horo_center_inf(get_mob_from_gen(w, self.gen))
        return (real(z), imag(z))

    def get_canvas_point(self, z_coord, shift) :
        return tuple([self.origin[i] + (-1)**i * self.factor * (z_coord[i]+shift[i]) for i in range(0,2)])

    def draw_label(self, e1, e2, edge_label = '0', dir_label_shift = 3.0) :
        m = vect_mid(e1,e2)
        self.canvas.create_text(m[0],m[1], text = edge_label)
        d = vect_scale(vect_dir(e1,e2), self.point_rad)
        p1 = vect_add(m, vect_scale(d, dir_label_shift))
        p2 = vect_add(p1,(d[1]-d[0],-d[0]-d[1]))
        p3 = vect_add(p1,(-d[1]-d[0],d[0]-d[1]))
        self.canvas.create_polygon(p1[0],p1[1],p2[0],p2[1],p3[0],p3[1])

    def draw_edge(self, endpts, edge_label = 0, fill_style = 'black') :
        first = endpts[0]
        last = endpts[-1]
        for e1, e2 in pairwise(endpts) :
            if e1 == first :
                v = vect_scale(vect_dir(e1,e2), self.point_rad)
                e1 = vect_add(e1,v)
            if e2 == last :
                v = vect_scale(vect_dir(e2,e1), self.point_rad)
                e2 = vect_add(e2,v)
            self.canvas.create_line(e1[0],e1[1],e2[0],e2[1],fill=fill_style)
        # Labels
        l1 = str((edge_label % self.g_power)+1)
        l2 = str(((edge_label + 1) % self.g_power)+1)
        if len(endpts) > 2 :
            self.draw_label(endpts[0], endpts[1], edge_label = l1)
            self.draw_label(endpts[-2], endpts[-1], edge_label = l2)
        else :
            m = vect_mid(endpts[0],endpts[1])
            self.draw_label(endpts[0], m , edge_label = l1)
            self.draw_label(m, endpts[1], edge_label = l2)

    def draw_point(self, p, fill_style = 'black') :
        self.canvas.create_oval(p[0]-self.point_rad,p[1]-self.point_rad, p[0]+self.point_rad, p[1]+self.point_rad,fill=fill_style)

    def draw_lattice(self) :
        points_to_style = {(0.,0.) : 'black', self.ball_coord('G') : ''}
        size = 4
        L = self.ball_coord('Ng')
        for v in points_to_style :
            style = points_to_style[v]
            for x in range(-size, size+1) : 
                for y in range(-size, size+1) :
                    shift = (x + y*L[0], y*L[1]) 
                    p = self.get_canvas_point(v, shift) 
                    self.draw_point(p, fill_style = style)

    def draw_diagram(self) :
        size = 4
        L = self.ball_coord('Ng')
        self.edge_count = 0
        print self.params
        for edge in self.edges :
            print edge
            endpts = map(self.ball_coord, edge)
            print endpts
            for x in range(-size, size+1) : 
                for y in range(-size, size+1) : 
                    shift = (x + y*L[0], y*L[1]) 
                    coords =  map(lambda p : self.get_canvas_point(p, shift), endpts) 
                    self.draw_edge(coords, edge_label = self.edge_count)
            self.edge_count += 1

    def print_callback(self) :
        file_path = asksaveasfilename() 
        if file_path :
            self.canvas.postscript(file=file_path, colormode='color')

    def remove_from_canvas(self, canvas_id) :
        self.canvas.delete(canvas_id)
        del self.object_dict[canvas_id]

if __name__ == "__main__":
    app = word_diagram(None)
    app.title(app.word)
    app.mainloop()
