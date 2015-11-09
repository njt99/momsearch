#!/usr/bin/python

import os, sys, glob
from Tkinter import *
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

def min_parameter(params, key) :
    return min_parameters(params)[key]

def jet_size(jet) :
    return abs(jet['df0']) + abs(jet['df1']) + abs(jet['df2'])

def jet_min_abs(jet) :
    v = abs(jet['f']) - jet_size(jet) - jet['error']
    return max(v,0.)

def min_jet_parameter(params,key) :
    return jet_min_abs(params[key]) 

def get_min_area(params) :
    abs_lox_sqrt = min_jet_parameter(params,'lox_sqrt_jet')
    min_lattice = min_parameter(params,'lattice')
    return abs_lox_sqrt * abs_lox_sqrt * imag(min_lattice)

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

class cusp(Tk) :

    def __init__(self,parent):
        Tk.__init__(self,parent)
        self.parent = parent
        self.initialize()

    def setup_interface(self) :
        self.init_frame_width = 1300.
        self.init_frame_height = 800.
        self.init_canvas_width = 640.
        self.init_canvas_height = 800.
        self.point_rad = 5.

        self.canvas = Canvas(self, width=self.init_canvas_width, height=self.init_canvas_height)
        self.canvas.pack(side='left', fill='both', expand=False)
        self.display = Frame(self, width=self.init_frame_width-self.init_canvas_width, height=self.init_frame_height, bg='gray')
        self.display.pack(side='left', fill='both', expand=True)
        self.canvas.bind("<ButtonRelease-1>", self.click_up)

        self.selected_objects = set([])

        self.display.columnconfigure(1, weight=1)

        self.box_label = Label(self.display,text='Box: ', justify='left', wraplength=400, bg='gray')
        self.box_value = StringVar()
        self.box_value_label = Label(self.display, textvariable=self.box_value, justify='left', wraplength=400, bg='gray')
        self.box_label.grid(row=0, column=0, sticky=N+W)
        self.box_value_label.grid(row=0, column=1, sticky=N+S+E+W)

        self.area_label = Label(self.display,text='Min Area: ', justify='left', wraplength=400, bg='gray')
        self.area_value = StringVar()
        self.area_value_label = Label(self.display, textvariable=self.area_value, justify='left', wraplength=400, bg='gray')
        self.area_label.grid(row=1, column=0, sticky=N+W)
        self.area_value_label.grid(row=1, column=1, sticky=N+S+E+W)

        self.cusp_height_label = Label(self.display,text='Cusp Height: ', justify='left', wraplength=400, bg='gray')
        self.cusp_height_value = StringVar()
        self.cusp_height_value_label = Label(self.display, textvariable=self.cusp_height_value, justify='left', wraplength=400, bg='gray')
        self.cusp_height_label.grid(row=2, column=0, sticky=N+W)
        self.cusp_height_value_label.grid(row=2, column=1, sticky=N+S+E+W)

        self.lat_label = Label(self.display,text='Lattice: ', justify='left', wraplength=400, bg='gray')
        self.lat_value = StringVar()
        self.lat_value_label = Label(self.display, textvariable=self.lat_value, justify='left', wraplength=400, bg='gray')
        self.lat_label.grid(row=3, column=0, sticky=N+W)
        self.lat_value_label.grid(row=3, column=1, sticky=N+S+E+W)

        self.lox_label = Label(self.display,text='Loxodromic Square Root: ', justify='left', wraplength=400, bg='gray')
        self.lox_value = StringVar()
        self.lox_value_label = Label(self.display, textvariable=self.lox_value, justify='left', wraplength=400, bg='gray')
        self.lox_label.grid(row=4, column=0, sticky=N+W)
        self.lox_value_label.grid(row=4, column=1, sticky=N+S+E+W)

        self.para_label = Label(self.display,text='Parabolic: ', justify='left', wraplength=400, bg='gray')
        self.para_value = StringVar()
        self.para_value_label = Label(self.display, textvariable=self.para_value, justify='left', wraplength=400, bg='gray')
        self.para_label.grid(row=5, column=0, sticky=N+W)
        self.para_value_label.grid(row=5, column=1, sticky=N+S+E+W)

        self.G_label = Label(self.display,text='G[z] = p + 1/(z s^2)', justify='left', wraplength=400, bg='gray')
        self.G_value = StringVar()
        self.G_value_label = Label(self.display, textvariable=self.G_value, justify='left', wraplength=400, bg='gray')
        self.G_label.grid(row=6, column=0, sticky=N+W)
        self.G_value_label.grid(row=6, column=1, sticky=N+S+E+W)

        self.b1_word_label = Label(self.display,text='Red horobal word: ', justify='left', wraplength=400, bg='gray')
        self.b1_word_value = StringVar()
        self.b1_word_value_label = Label(self.display, textvariable=self.b1_word_value, justify='left', wraplength=400, bg='gray')
        self.b1_word_label.grid(row=7, column=0, sticky=N+W)
        self.b1_word_value_label.grid(row=7, column=1, sticky=N+S+E+W)

        self.b1_center_label = Label(self.display,text='Red horoball center: ', justify='left', wraplength=400, bg='gray')
        self.b1_center_value = StringVar()
        self.b1_center_value_label = Label(self.display, textvariable=self.b1_center_value, justify='left', wraplength=400, bg='gray')
        self.b1_center_label.grid(row=8, column=0, sticky=N+W)
        self.b1_center_value_label.grid(row=8, column=1, sticky=N+S+E+W)

        self.b1_d_inf_label = Label(self.display,text='Red/Inf hyperbolic distance', justify='left', wraplength=400, bg='gray')
        self.b1_d_inf_value = StringVar()
        self.b1_d_inf_value_label = Label(self.display, textvariable=self.b1_d_inf_value, justify='left', wraplength=400, bg='gray')
        self.b1_d_inf_label.grid(row=9, column=0, sticky=N+W)
        self.b1_d_inf_value_label.grid(row=9, column=1, sticky=N+S+E+W)

        self.b1_rad_label = Label(self.display,text='Blue horoball radius', justify='left', wraplength=400, bg='gray')
        self.b1_rad_value = StringVar()
        self.b1_rad_value_label = Label(self.display, textvariable=self.b1_rad_value, justify='left', wraplength=400, bg='gray')
        self.b1_rad_label.grid(row=10, column=0, sticky=N+W)
        self.b1_rad_value_label.grid(row=10, column=1, sticky=N+S+E+W)

        self.b2_word_label = Label(self.display,text='Blue horobal word: ', justify='left', wraplength=400, bg='gray')
        self.b2_word_value = StringVar()
        self.b2_word_value_label = Label(self.display, textvariable=self.b2_word_value, justify='left', wraplength=400, bg='gray')
        self.b2_word_label.grid(row=11, column=0, sticky=N+W)
        self.b2_word_value_label.grid(row=11, column=1, sticky=N+S+E+W)

        self.b2_center_label = Label(self.display,text='Blue horoball center: ', justify='left', wraplength=400, bg='gray')
        self.b2_center_value = StringVar()
        self.b2_center_value_label = Label(self.display, textvariable=self.b2_center_value, justify='left', wraplength=400, bg='gray')
        self.b2_center_label.grid(row=12, column=0, sticky=N+W)
        self.b2_center_value_label.grid(row=12, column=1, sticky=N+S+E+W)

        self.b2_rad_label = Label(self.display,text='Blue horoball radius', justify='left', wraplength=400, bg='gray')
        self.b2_rad_value = StringVar()
        self.b2_rad_value_label = Label(self.display, textvariable=self.b2_rad_value, justify='left', wraplength=400, bg='gray')
        self.b2_rad_label.grid(row=13, column=0, sticky=N+W)
        self.b2_rad_value_label.grid(row=13, column=1, sticky=N+S+E+W)

        self.b2_d_inf_label = Label(self.display,text='Blue/Inf hyperbolic distance', justify='left', wraplength=400, bg='gray')
        self.b2_d_inf_value = StringVar()
        self.b2_d_inf_value_label = Label(self.display, textvariable=self.b2_d_inf_value, justify='left', wraplength=400, bg='gray')
        self.b2_d_inf_label.grid(row=14, column=0, sticky=N+W)
        self.b2_d_inf_value_label.grid(row=14, column=1, sticky=N+S+E+W)

        self.dist_label = Label(self.display,text='Red/Blue hyperbolic distance: ', justify='left', wraplength=400, bg='gray')
        self.dist_value = StringVar()
        self.dist_value_label = Label(self.display, textvariable=self.dist_value, justify='left', wraplength=400, bg='gray')
        self.dist_label.grid(row=15, column=0, sticky=N+W)
        self.dist_value_label.grid(row=15, column=1, sticky=N+S+E+W)

    def initialize(self) :
        if len(sys.argv) < 3 :
            print 'Usage: drawCusp box_code depth <height_cutoff>'
            sys.exit(2)

        self.box = sys.argv[1]
        self.depth = int(sys.argv[2])
        self.height_cutoff = float(sys.argv[3]) if len(sys.argv) > 3 else 0.001
        self.params = get_params(self.box)
        self.object_dict = {}
        self.build_elements()
        self.set_horo_center_cutoff()       
        self.print_elements()
        self.init_horoballs()
        
        self.setup_interface()
        self.setup_values()

        self.draw_cusp()

    def setup_values(self) :
        self.box_value.set(self.box)
        self.area_value.set('{0}'.format(get_min_area(self.params)))
        self.cusp_height_value.set('{0}'.format(self.cusp_height))
        self.lat_value.set('{0}'.format(self.params['lattice']))
        self.lox_value.set('{0}'.format(self.params['lox_sqrt']))
        self.para_value.set('{0}'.format(self.params['parabolic']))
        self.G_value.set(pformat(self.elements['G'],width=1))

    def display_ball_info(self, ball, color='red') :
        if color == 'red' :
            self.b1_word_value.set(ball['word'])    
            self.b1_rad_value.set('{0}'.format(ball['height']/2.))
            self.b1_center_value.set('{0}'.format(ball['center']))
            self.b1_d_inf_value.set('{0}'.format(vert_hyp_dist(self.cusp_height,ball['height'])))
        else :
            self.b2_word_value.set(ball['word'])    
            self.b2_rad_value.set('{0}'.format(ball['height']/2.))
            self.b2_center_value.set('{0}'.format(ball['center']))
            self.b2_d_inf_value.set('{0}'.format(vert_hyp_dist(self.cusp_height,ball['height'])))

    def clear_ball_info(self) :
        self.b1_word_value.set('')    
        self.b1_rad_value.set('')
        self.b1_center_value.set('')
        self.b1_d_inf_value.set('')
        self.b2_word_value.set('')    
        self.b2_rad_value.set('')
        self.b2_center_value.set('')
        self.b2_d_inf_value.set('')
        self.dist_value.set('')

    # Given a height cut off, we have a maximum radius of horoball centers worth considering
    # for images that under G,g will map into the lattice domain.
    def set_horo_center_cutoff(self) :
        self.horo_center_cutoff = abs(self.params['parabolic']) + real(sqrt(self.cusp_height/self.height_cutoff))/abs(self.params['lox_sqrt'])

    def build_elements(self) :
        G = get_G(self.params)
        g = get_g(self.params)
        M = get_T(self.params,1,0)
        N = get_T(self.params,0,1)
        self.elements = { 'g' : g, 'G' : G, 'NM' : dot(M,N), 'M' : M, 'N' : N, 'GG' : dot(G,G) }
        self.cusp_height = max_horo_height(G)
 
    def print_elements(self) :
        # Print box paramenters and group elements
        print 'Parameters:'
        pprint(self.params,width=1)
        print 'Elements:'
        pprint(self.elements,width=2)

    def init_horoballs(self) :
        I = [[1.,0.],[0.,1.]]
        self.horoballs = { 0 : { '' : { 'center' : float("inf"), 'height' : self.cusp_height, 'gamma' : I, 'word' : ''}}}
        # Generate new horoballs
        d = 0
        lattice = self.params['lattice']
        while d < self.depth :
            self.horoballs[d+1] = {}
            for word,ball in self.horoballs[d].iteritems() :
                gamma = ball['gamma']
                height = ball['height']
                first = get_first(word)
                if first == 'g' : 
                    valid = ['g']
                elif first == 'G' :
                    valid = ['G']
                else :
                    valid = ['g', 'G']        
         
                # Apply G and g if possible
                for h_char in valid :
                    h = self.elements[h_char] 
                    h_gamma = dot(h,gamma)
                    h_center = horo_center_inf(h_gamma)

                    # The pojection of h_center to the real axis along lattice
                    # Recall that we assume -0.5 < real(lattice) < 0.5 and imag(lattice) > 0
                    M_len = imag(h_center) / imag(lattice)
                    N_len = real(h_center) - M_len * real(lattice)

                    # Adjustments we make sure to land inside the fundamental domain 
                    if abs(M_len) > pow(2.,-10) :
                        M_pow = -floor(M_len) if abs(M_len - 1.) > pow(2.,-10) else 0 
                    else :
                        M_pow = 0
                    if abs(N_len) > pow(2.,-10) :
                        N_pow = -floor(N_len) if abs(N_len - 1.) > pow(2.,-10) else 0 
                    else :
                        N_pow = 0

                    # Keep track of the word and it's representative
                    h_word = h_char + word
                    new_height = horo_image_height_inf(h_gamma, self.cusp_height)
                    if new_height > 2*self.cusp_height :
                        sys.stderr.write('Possible giant horoball with word {0} of height {1} with center {2}\nElement:\n'.format(h_word,new_height,h_center))
                        sys.stderr.write(pformat(new_gamma,width=2)+'\n')
                    else :
                        M_word = 'M'*M_pow if M_pow > 0 else 'm'*(-M_pow) 
                        new_word = M_word + h_word
                        N_word = 'N'*N_pow if N_pow > 0 else 'n'*(-N_pow) 
                        new_word = N_word + new_word
                        T = get_T(self.params, M_pow, N_pow)
                        new_gamma = dot(T, h_gamma)
                        new_center = mobius(T, h_center)

                        self.horoballs[d+1][new_word] = { 'center' : new_center, 'height' : new_height, 'gamma' : new_gamma, 'word' : new_word, 'canvas_id' : 0 }
 
                        # Add translates that will be good for next depth
                        x = real(new_center)
                        y = imag(new_center)
                        r = self.horo_center_cutoff
                        horiz_range = quad_sol(1., 2.*x, norm(new_center) - r**2)
                        vert_range = quad_sol(norm(lattice), 2*(x*real(lattice) + y*imag(lattice)), norm(new_center) - r**2)
                        horiz_range = (int(ceil(real(horiz_range[0]))), int(floor(real(horiz_range[1]))))
                        vert_range = (int(ceil(real(vert_range[0]))), int(floor(real(vert_range[1]))))
                        for n in range(*horiz_range) :
                            for m in range(*vert_range) :
                                if m != 0 or n != 0 :
                                    shift_M_pow = M_pow + m
                                    shift_N_pow = N_pow + n
                                    T = get_T(self.params, shift_M_pow, shift_N_pow)
                                    shift_center = mobius(T, h_center)
                                    # Make sure the horoball images in next depth are not too small
                                    if abs(shift_center) < r :
                                        shift_gamma = dot(T, h_gamma)
                                        M_word = 'M'*shift_M_pow if shift_M_pow > 0 else 'm'*(-shift_M_pow) 
                                        shift_word = M_word + h_word
                                        N_word = 'N'*shift_N_pow if shift_N_pow > 0 else 'n'*(-shift_N_pow) 
                                        shift_word = N_word + shift_word

                                        self.horoballs[d+1][shift_word] = { 'center' : shift_center,
                                                                            'height' : new_height,
                                                                            'gamma' : shift_gamma,
                                                                            'word' : shift_word,
                                                                            'canvas_id' : 0 }

            d += 1
        #pprint(self.horoballs,width=2)

    # A simple generation algorthim. Generates lot of small horoballs
    def init_horoballs_orig(self) :
        I = [[1.,0.],[0.,1.]]
        self.horoballs = { 0 : { '' : { 'center' : 0, 'height' : cusp_height, 'gamma' : I, 'word' : ''}}}

        # Generate new horoballs
        d = 0
        lattice = self.params['lattice']
        while d < depth :
            self.horoballs[d+1] = {}
            for word,ball in self.horoballs[d].iteritems() :
                gamma = ball['gamma']
                center = ball['center']
                height = ball['height']

                first = get_first(word)
                if first == 'g' : 
                    valid = ['g']
                elif first == 'G' :
                    valid = ['G']
                elif first == '' :
                    valid = ['M', 'N', 'GG', 'NM']
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

                    # Adjustments we make sure to land inside the fundamental domain 
                    M_pow = -floor(M_len) if M_len != 1. else 0 
                    N_pow = -floor(N_len) if N_len != 1. else 0

                    # Keep track of the word and it's representative
                    new_word = h_char + word
                    new_gamma = dot(h,gamma)
                    if new_height > 2*cusp_height :
                        sys.stderr.write('Possible giant horoball with word {0} of height {1} with center {2}\nElement:\n'.format(new_word,new_height,new_center))
                        sys.stderr.write(pformat(new_gamma,width=2)+'\n')
                    else :
                        M_word = 'M'*M_pow if M_pow > 0 else 'm'*(-M_pow) 
                        new_word = M_word + new_word
                        N_word = 'N'*N_pow if N_pow > 0 else 'n'*(-N_pow) 
                        new_word = N_word + new_word
                        T = get_T(self.params, M_pow, N_pow)
                        new_gamma = dot(T, new_gamma)
                        new_center = mobius(T, h_center)

                        self.horoballs[d+1][new_word] = { 'center' : new_center, 'height' : new_height, 'gamma' : new_gamma, 'word' : new_word, 'canvas_id' : 0 }
            d += 1

    # TODO Allow depth increase and decrease in interface
    def modify_depth(self, new_depth) :
        delta = new_depth - self.depth
        if delta > 0 :
            return 0    
        elif delta < 0 : 
            return 1

    def deselect(self, object_id) :
        self.canvas.itemconfigure(object_id, fill='')
        try :
            self.selected_objects.remove(object_id)
        except :
            return        

    def display_distance(self, x, y) :
        dist = dist_btw_balls(c1=x['center'], h1=x['height'], c2=y['center'], h2=y['height'])
        self.dist_value.set('{0}'.format(dist))

    def select(self, object_id, color='red') :
        self.canvas.itemconfigure(object_id, fill=color)
        self.selected_objects.add(object_id)
      
    def click_up(self,event) :
        object_tuple = self.canvas.find_closest(event.x,event.y)
        object_id = object_tuple[0]
        if len(self.selected_objects) > 1 or object_id not in self.object_dict :
            for selected_id in list(self.selected_objects) :
                self.deselect(selected_id)
            self.clear_ball_info()
            return

        if len(self.selected_objects) > 0 :
            prev_id = next(iter(self.selected_objects))
            # deselect if we hit the same ball
            if prev_id == object_id :
                self.deselect(object_id)
                self.clear_ball_info()
                return
            self.select(object_id,color='blue')
            if prev_id in self.object_dict :
                first_ball = self.object_dict[prev_id]
                second_ball = self.object_dict[object_id]
                self.display_ball_info(second_ball, color='blue')
                self.display_distance(first_ball, second_ball)
        else :
            self.select(object_id)
            self.display_ball_info(self.object_dict[object_id])

    def remove_from_canvas(self, canvas_id) :
        self.canvas.delete(canvas_id)
        del self.object_dict[canvas_id]

    def draw_ball(self, ball) :
        center = ball['center']
        height = ball['height']
        
        x = self.origin[0] + self.factor*real(center)
        y = self.origin[1] - self.factor*imag(center) # Flip coordinate system 
        r = self.factor*height/2.
        #print '({0},{1},{2})'.format(x,y,r)

        object_id = self.canvas.create_oval(x-r,y-r,x+r,y+r)
        ball['canvas_id'] = object_id
        ball['selected'] = False
        self.object_dict[object_id] = ball

    def draw_cusp(self) :
        lattice = self.params['lattice']
        self.factor = self.init_canvas_width/(abs(lattice)+1)
        # Pick an origin. TODO: Make this more robust
        self.origin = [0.,0.]
        self.origin[0] = self.init_canvas_width/4.5 if real(lattice) > -0.2 else self.init_canvas_width/2.5
        self.origin[1] = self.factor*(imag(lattice)+0.7)
        print 'Origin {0}'.format(self.origin)

        # Draw the lattice domain lines and points
        lattice_points = [(1.,0.), (real(lattice) + 1, imag(lattice)),(real(lattice), imag(lattice)), (0.,0.)]
        v_coord = list(self.origin)
        prev_coord = list(self.origin) 
        for v in lattice_points :
            v_coord[0] = self.origin[0]+self.factor*v[0]
            v_coord[1] = self.origin[1]-self.factor*v[1]
            self.canvas.create_oval(v_coord[0]-self.point_rad,v_coord[1]-self.point_rad, v_coord[0]+self.point_rad, v_coord[1]+self.point_rad,fill='black')
            self.canvas.create_line(prev_coord[0],prev_coord[1],v_coord[0],v_coord[1])
            prev_coord = list(v_coord)

        ball_count = 0
        for d in self.horoballs :
            depth_level = self.horoballs[d]
            for word in depth_level :
                ball = depth_level[word]
                height = ball['height']
                if height > self.height_cutoff :
                    self.draw_ball(ball)
                    ball_count += 1
        print 'Ball count: {0}'.format(ball_count)

if __name__ == "__main__":
    app = cusp(None)
    app.title(app.box)
    app.mainloop()
