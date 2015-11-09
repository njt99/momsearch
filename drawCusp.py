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
        center[p] += (2*int(direction) - 1) * size[p]
        pos += 1
    
    params = {}
    params['lattice'] = scale[3]*center[3] + scale[0]*center[0]*1j
    params['lox_sqrt'] = scale[4]*center[4] + scale[1]*center[1]*1j
    params['parabolic'] = scale[5]*center[5] + scale[2]*center[2]*1j
   
    params['center'] = center
    params['size'] = size

    params['lattice_jet'] = { 'f' : scale[3]*center[3] + scale[0]*center[0]*1j, 'df0' : scale[3]*size[3] + scale[0]*size[0]*1j, 'df1' : 0., 'df2' : 0. }
    params['lox_sqrt_jet'] = { 'f' :  scale[4]*center[4] + scale[1]*center[1]*1j, 'df0' : 0., 'df1' : scale[4]*size[4] + scale[1]*size[1]*1j, 'df2' : 0. }
    params['parabolic_jet'] = { 'f' :  scale[5]*center[5] + scale[2]*center[2]*1j, 'df0' : 0., 'df1' : 0., 'df2' : scale[5]*size[5] + scale[2]*size[2]*1j }
    params['error'] = 0.

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
    min_params['lattice'] = m[3] + m[0]*1j
    min_params['lox_sqrt'] = m[4] + m[1]*1j
    min_params['parabolic'] = m[5] + m[2]*1j

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

def get_area(params) :
    abs_lox_sqrt = min_jet_parameter(params,'lox_sqrt_jet')
    min_lattice = min_parameter(params,'lattice')
    return abs_lox_sqrt * abs_lox_sqrt * min_lattice

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

def quad_sol(a,b,c) :
    d = b * b - 4. * a * c
    sq_d = sqrt(d)
    return ((-b - sq_d)/(2*a), (-b + sq_d)/(2*a))

class cusp(Tk) :

    def __init__(self,parent):
        Tk.__init__(self,parent)
        self.parent = parent
        self.initialize()

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
        
        self.canvas_width = 640
        self.canvas_height = 800
        self.point_rad = 5
        lattice = self.params['lattice']
        self.factor = 600/(abs(lattice)+1)

        print 'Factor {0}'.format(self.factor)
        print 'Lattice {0}'.format(lattice)

        self.canvas = Canvas(self, width=self.canvas_width, height=self.canvas_height)
        self.canvas.pack()

    # Given a height cut off, we have a maximum radius of horoball centers worth considering
    # for images that under G,g will map into the lattice domain.
    def set_horo_center_cutoff(self) :
        self.horo_center_cutoff = abs(self.params['parabolic']) + real(sqrt(self.cusp_height/self.height_cutoff))/abs(self.params['lox_sqrt'])

    def build_elements(self) :
        G = get_G(self.params)
        g = get_g(self.params)
        M = get_T(self.params,1,0)
        N = get_T(self.params,0,1)
        self.elements = { 'g' : g, 'G' : G, 'GG' : dot(G,G), 'MN' : dot(M,N), 'M' : M, 'N' : N, 'Ng' : dot(N,g), 'Mg' : dot(M,g), 'NMg' : dot(M,dot(N,g)) }
        self.cusp_height = max_horo_height(G)
 
    def print_elements(self) :
        # Print box paramenters and group elements
        print 'Parameters:'
        pprint(self.params,width=1)
        print 'Elemnts:'
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
                elif first == '' :
                    valid = ['g', 'Ng', 'Mg', 'NMg', 'G']
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
                    M_pow = -floor(M_len) if M_len != 1. else 0 
                    N_pow = -floor(N_len) if N_len != 1. else 0

                    # Keep track of the word and it's representative
                    h_word = h_char + word
                    new_height = horo_image_height_inf(h_gamma, height)
                    if new_height > 2*self.cusp_height :
                        sys.stderr.write('Possible giant horoball with word {0} of height {1} with center {2}\nElement:\n'.format(h_word,new_height,h_center))
                        sys.stderr.write(pformat(new_gamma,width=2)+'\n')
                    else :
                        M_word = 'M'*M_pow if M_pow > 0 else 'm'*M_pow 
                        new_word = M_word + h_word
                        N_word = 'N'*N_pow if N_pow > 0 else 'n'*N_pow 
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
                                if m != 0 and n != 0 :
                                    shift_M_pow = M_pow + m
                                    shift_N_pow = N_pow + n
                                    T = get_T(self.params, M_pow + m, N_pow + n)
                                    shift_center = mobius(T, h_center)
                                    # Make sure the horoball images in next depth are not too small
                                    if abs(shift_center) < r :
                                        shift_gamma = dot(T, h_gamma)
                                        M_word = 'M'*shift_M_pow if shift_M_pow > 0 else 'm'*shift_M_pow 
                                        shift_word = M_word + h_word
                                        N_word = 'N'*shift_N_pow if shift_N_pow > 0 else 'n'*shift_N_pow 
                                        shift_word = N_word + shift_word
                                        self.horoballs[d+1][shift_word] = { 'center' : shift_center,
                                                                            'height' : new_height,
                                                                            'gamma' : shift_gamma,
                                                                            'word' : shift_word,
                                                                            'canvas_id' : 0 }

            d += 1
        pprint(self.horoballs,width=2)

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
                        M_word = 'M'*M_pow if M_pow > 0 else 'm'*M_pow 
                        new_word = M_word + new_word
                        N_word = 'N'*N_pow if N_pow > 0 else 'n'*N_pow 
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
        self.object_dict[object_id] = ball

    def draw_cusp(self) :
        lattice = self.params['lattice']
        # Pick an origin. TODO: Make this more robust
        self.origin = [0.,0.]
        self.origin[0] = self.canvas_width/4 if real(lattice) > -0.2 else self.canvas_width/2.5
        self.origin[1] = self.factor*(imag(lattice)+0.5)
        print 'Origin {0}'.format(self.origin)

        # Draw the lattice domain lines and points
        lattice_points = [(1.,0.), (real(lattice) + 1, imag(lattice)),(real(lattice), imag(lattice)), (0.,0.)]
        v_coord = copy(self.origin)
        prev_coord = copy(self.origin) 
        for v in lattice_points :
            v_coord[0] = self.origin[0]+factor*v[0]
            v_coord[1] = self.origin[1]-factor*v[1]
            self.canvas.create_oval(v_coord[0]-point_rad,v_coord[1]-point_rad, v_coord[0]+point_rad, v_coord[1]+point_rad,fill='black')
            self.canvas.create_line(prev_coord[0],prev_coord[1],v_coord[0],v_coord[1])
            prev_coord = copy(v_coord)

        ball_count = 0
        for d in self.horoballs :
            depth_level = horoballs[d]
            for word in depth_level :
                ball = depth_level[word]
                height = ball['height']
                if height > height_cutoff :
                    self.draw_ball(ball)
                    ball_count += 1
        print 'Ball count: {0}'.format(ball_count)

if __name__ == "__main__":
    app = cusp(None)
    app.title(app.box)
    app.mainloop()
