#!/usr/bin/python

import os, sys, glob, re
from Tkinter import *
from pprint import *
from cmath import *
from numpy import floor, ceil, dot, sqrt
from tkFileDialog import *

def dir(p,q) :
    v = (q[0]-p[0],q[1]-p[1])
    norm = sqrt(v[0]**2 + v[1]**2)
    return (v[0]/norm, v[1]/norm)

def rev(string) :
    return string[::-1]

def inv_gen(char) :
    if char.islower() :
        return char.upper()
    else :
        return char.lower()

def count_dict(items) :
    counts = dict()
    for i in items:
        counts[i] = counts.get(i, 0) + 1
    return counts

def gen_total_power(word,gen) :
    gen_count = 0
    for char in word :
        if char == gen or char == inv_gen(gen) :
            gen_count += 1
    return gen_count

def gen_signed_power(word,gen) :
    gen_count = 0
    for char in word :
        if char == gen : 
            gen_count += 1
        elif char == inv_gen(gen) :
            gen_count -= 1
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
    return new_word 

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

def get_sequence_of_balls(word) :
    last_g_idx = max(word.rfind('g'),word.rfind('G'))
    w = rotate_word(word, last_g_idx)
    print 'Word is {}\n'.format(w)
    seq = []
    lattice = ''
    prev_g = w[-1]
    for gen in w :
        if (gen != 'g' and gen != 'G') :
            lattice += gen 
        else :
            seq.append((inv_gen(prev_g), lattice + gen))
            prev_g = gen
            lattice = ''
    return seq

class word_diagram(Tk) :

    def __init__(self,parent):
        Tk.__init__(self,parent)
        self.parent = parent
        self.initialize()

    def setup_interface(self) :
        self.init_canvas_width = 800.
        self.init_canvas_height = 800.
        self.point_rad = 10.
        self.init_dist = 50.

        self.canvas = Canvas(self, width=self.init_canvas_width, height=self.init_canvas_height)
        self.canvas.pack(side='left', fill='both', expand=False)

        self.selected_objects = set([])

        self.factor = self.init_canvas_width/(3 * self.max_dist)
        self.origin = [self.init_canvas_width/2.,self.init_canvas_width/2.]

    def initialize(self) :
        if len(sys.argv) < 2 :
            print 'Usage: draw_word word'
            sys.exit(2)

        self.word = sys.argv[1]
        self.edges = get_sequence_of_balls(self.word)
        self.push_int_out = False        

        print sys.argv

        if len(sys.argv) == 3 and sys.argv[2] == 'p' :
            self.push_int_out = True

        print self.edges

        self.max_dist = max([1] + map(lambda x : gen_total_power(x[1],'M'), self.edges) + map(lambda x : gen_total_power(x[1],'N'), self.edges)) 
        self.g_coords = {'g' : (0.,0.), 'G' : (0.5,0.2)}

        self.setup_interface()
        self.draw_lattice()
        self.draw_diagram()

    def ball_coord(self, w) :
        assert(gen_total_power(w,'g') == 1 and (w[-1] == 'g' or w[-1] == 'G'))
        g_shift = self.g_coords[w[-1]]
        r_w = reduce_lattice(w)
        return (gen_signed_power(r_w,'M') + g_shift[0], gen_signed_power(r_w,'N') + g_shift[1])

    def get_canvas_point(self, lat_p, shift = (0.,0.)) :
        return tuple([self.origin[i] + (-1)**i * self.factor * (lat_p[i]+shift[i]) for i in range(0,2)])

    def draw_edge(self, lat_start, lat_end, init_dir = (1.,0.), shift = (0.,0.), fill_style = 'green') :
        i_s = tuple([self.point_rad * init_dir[i] / self.factor for i in range(0,2)])
        i_e = tuple([self.init_dist * init_dir[i] / self.factor for i in range(0,2)])
        if self.push_int_out :
            edge_dir = dir(i_e, lat_end)
        else :
            edge_dir = (0.,0.)
        e = tuple([lat_end[i] - self.point_rad * edge_dir[i] / self.factor for i in range(0,2)])

        i_shift = tuple([shift[i]+lat_start[i] for i in range(0,2)])
        init_start = self.get_canvas_point(i_s, shift = i_shift)
        init_end = self.get_canvas_point(i_e, shift = i_shift)
        end = self.get_canvas_point(e, shift = shift)

        self.canvas.create_line(init_start[0],init_start[1],init_end[0],init_end[1],fill=fill_style)
        self.canvas.create_line(init_end[0],init_end[1],end[0],end[1],fill=fill_style)
        return list(edge_dir)

    def draw_point(self, p, fill_style = 'black') :
        self.canvas.create_oval(p[0]-self.point_rad,p[1]-self.point_rad, p[0]+self.point_rad, p[1]+self.point_rad,fill=fill_style)

    def draw_lattice(self) :
        points_to_style = {self.g_coords['g'] : '', self.g_coords['G'] : 'black'}
        size = 4 * self.max_dist
        for v in points_to_style :
            style = points_to_style[v]
            for x in range(-size, size+1) : 
                for y in range(-size, size+1) :
                    sft = (x,y) 
                    p = self.get_canvas_point(v, shift = sft) 
                    self.draw_point(p, fill_style = style)

    def draw_diagram(self) :
        size = 4 * self.max_dist
        lat_start = self.ball_coord(self.edges[-1][0])
        lat_end = self.ball_coord(self.edges[-1][1])
        if self.push_int_out :
            prev_dir = dir(lat_start, lat_end) 
        else :
            prev_dir = (0.,0.)
        for edge in self.edges :
            lat_start = self.ball_coord(edge[0])
            lat_end = self.ball_coord(edge[1])
            for x in range(-size, size+1) : 
                for y in range(-size, size+1) : 
                    dir_loc = self.draw_edge(lat_start, lat_end, init_dir = prev_dir, shift = (x,y))
            if self.push_int_out :
                prev_dir = dir_loc
            else :
                prev_dir = (0.,0.)

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
