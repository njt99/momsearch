#!/usr/bin/python

try :
    sys
except :
    import sys

extra_path = ['/Applications/SnapPy.app/Contents/Resources/lib/python27.zip', \
 '/Applications/SnapPy.app/Contents/Resources/lib/python2.7', \
 '/Applications/SnapPy.app/Contents/Resources/lib/python2.7/plat-darwin', \
 '/Applications/SnapPy.app/Contents/Resources/lib/python2.7/plat-mac', \
 '/Applications/SnapPy.app/Contents/Resources/lib/python2.7/plat-mac/lib-scriptpackages', \
 '/Applications/SnapPy.app/Contents/Resources/lib/python2.7/lib-tk', \
 '/Applications/SnapPy.app/Contents/Resources/lib/python2.7/lib-old', \
 '/Applications/SnapPy.app/Contents/Resources/lib/python2.7/lib-dynload', \
 '/Applications/SnapPy.app/Contents/Resources/lib/python2.7/site-packages.zip', \
 '/Applications/SnapPy.app/Contents/Resources/lib/python2.7/site-packages', \
 '/Applications/SnapPy.app/Contents/Resources/lib/python2.7/IPython/extensions', \
 '/System/Library/Frameworks/Python.framework/Versions/2.7/lib/python27.zip', \
 '/System/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7', \
 '/System/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/plat-darwin', \
 '/System/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/plat-mac', \
 '/System/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/plat-mac/lib-scriptpackages', \
 '/System/Library/Frameworks/Python.framework/Versions/2.7/Extras/lib/python', \
 '/System/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/lib-tk', \
 '/System/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/lib-old', \
 '/System/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/lib-dynload', \
 '/Users/yarmola/Library/Python/2.7/lib/python/site-packages', \
 '/System/Library/Frameworks/Python.framework/Versions/2.7/Extras/lib/python/PyObjC', \
 '/Library/Python/2.7/site-packages'] 

for d in extra_path :
    if d not in sys.path :
        sys.path.append(d)

if 'os' not in sys.modules :
    import os
if 'glob' not in sys.modules :
    import glob
if 'pprint' not in sys.modules : 
    from pprint import *
if 'cmath' not in sys.modules : 
    from cmath import *
if 'numpy' not in sys.modules : 
    from numpy import floor, ceil, dot
 
scale = map(lambda x : 8 * pow(2, x/6.), range(0,-6,-1))
COMP_ERR = pow(2.,-10)

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
        return  1./abs(c)
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

def get_nearest_translate(c, p, m, n=1.) :
    assert imag(n) == 0.
    diff = p - c
    # The pojection of diff to the real axis along lattice
    # We assume tha n is real (along real axis)
    m_len = imag(diff) / imag(m)
    m_pow = round(m_len)
    n_len = (real(diff) - m_pow * real(m)) / real(n)
    n_pow = round(n_len)
    shift = - m_pow * m - n_pow * n
    translate = shift + p
    return {'translate' : translate, 'shift' : shift, 'm_pow' :  m_pow, 'n_pow' :  n_pow}

def translate_to_center(center_ball, horoballs, triang) :
    center = center_ball['center']
    shifted_horoballs = []
    shifted_triang = []
    for ball in horoballs :
        shifted_ball = dict(ball)
        shifted_ball['center'] = ball['center'] - center
        shifted_horoballs.append(shifted_ball)
    for edge in triang :
        shifted_edge = dict(edge)
        eps = edge['endpoints']
        shifted_edge['endpoints'] = (eps[0] - center, eps[1] - center)
        shifted_triang.append(shifted_edge)
    return {'horoballs' : shifted_horoballs, 'triangulation' :  shifted_triang}

def move_horoballs_to_domain(balls, trans) :
    m = trans[0]
    n = trans[1]
    # We assume that imag(m) >= 0, imag(n) == 0, real(n) > 0
    assert imag(m) > -COMP_ERR
    assert imag(n) == 0.
    assert real(n) > 0.
    new_balls = []
    for ball in balls :
        new_ball = dict(ball)
        # This is actually the nearest center, we need to move it to the domain
        domain_center = get_nearest_translate(0., ball['center'], m, n)['translate']
        real_proj = real(domain_center - m * (imag(domain_center) / imag(m)))
        if real_proj < 0. :
            domain_center += n 
        if imag(domain_center) < 0. :
            domain_center += m
        new_ball['center'] = domain_center
        new_balls.append(new_ball)
    return new_balls

def reverse_edge(edge) :
    reved_edge = dict(edge)
    reved_edge['endpoints'] = edge['endpoints'][::-1]
    reved_edge['indices'] = edge['indices'][::-1] 
    return reved_edge

def get_ball_edges(ball, triang, trans, reorient_edges = False) :
    center = ball['center']
    m = trans[0]
    n = trans[1]
    edges = []
    for edge in triang :
        eps = edge['endpoints']
        for i in range(2) :
            ep = eps[i]
            trans_data = get_nearest_translate(center, ep, m, n)
            if abs(trans_data['translate'] - center) < COMP_ERR :
                shift = trans_data['shift']
                shifted_edge = dict(edge)
                shifted_edge['endpoints'] = (eps[0] + shift, eps[1] + shift)
                if reorient_edges and i == 1 :
                    shifted_edge = reverse_edge(shifted_edge)
                edges.append(shifted_edge)
                continue
    return edges

def edge_angle(edge) :
    eps = edge['endpoints']
    direction = eps[1] - eps[0]
    unit = direction / abs(direction)
    return imag(log(-unit))

def get_edge_vect(edge) :
    eps = edge['endpoints']
    return eps[1] - eps[0]

# Returns edges cyclically orderes from the positive x-axis counter clockwise
def cyclically_order_edges(edges) :
    return sorted(edges, key=edge_angle)

# Invrse of edge under e0 + 1/(z-e0) where e0 is the left endpoint 
# with the correctly modified index
# TODO: Warning: we assume the horoball radius is 0.5
def inv_edge(edge) :
    eps = edge['endpoints']
    direction = eps[1] - eps[0]
    inv_dir = 1. / direction
    new_edge = dict(edge)
    new_edge['endpoints'] = (eps[0], eps[0] + inv_dir)
    inds = edge['indices']
    new_edge['indices'] = (inds[0], inds[2], inds[1])
    return new_edge

# This is out first SnapPy command
if 'snappy' not in sys.modules : 
    from snappy import *

def get_params_from_manifold(mfld) :
    mfld_hp = mfld.high_precision()
    cusp = mfld_hp.cusp_neighborhood()

    # We must set the displacement to get a maximal picture
    cusp.set_displacement(cusp.max_reach())
    hbls = cusp.horoballs(1)
    triang = cusp.triangulation()
    trans = cusp.translations()

    # We pick a 'zero' ball for our diagram
    zero_ball = hbls[0]
    centered = translate_to_center(zero_ball, hbls, triang)

    # We don't need the originals after centering
    hbls = centered['horoballs']
    triang = centered['triangulation']

    # Move the horoballs to the fundamental domain
    hbls = move_horoballs_to_domain(hbls, trans)

    # Get the cyclically ordered star of the new zero ball
    zero_ball = hbls[0]
    zero_star = get_ball_edges(zero_ball, triang, trans, reorient_edges=True)
    zero_star = cyclically_order_edges(zero_star)

    # We need to find the G ball. Most of the time will will only
    # have one G ball to choose from, but othertimes there will be more.
    # The edge index of a ball is the index of the edge going to infinity
    zero_ball_edge_idx = zero_star[0]['indices'][0]
    for i in range(1,len(hbls)) :
        G_ball = hbls[i]
        G_star = get_ball_edges(G_ball, triang, trans, reorient_edges=True)
        G_star = cyclically_order_edges(G_star)
        G_ball_edge_idx = G_star[0]['indices'][0]
        if G_ball_edge_idx == zero_ball_edge_idx :
            break

    # We have found a G ball, now we must find the loxodromic rotation
    first_edge = zero_star[0]
    second_edge = zero_star[1]
    inv_first_edge = inv_edge(first_edge)
    inv_second_edge = inv_edge(second_edge)

    # We match by the edge indices for a pair. Note that 1/z is an
    # isometry in H^3 and will switch the cyclic order of edges.
    # We take the first two edges around zero and find their image
    # by indices and then compute the angle
    lox = 0.
    inv_first_idx = inv_first_edge['indices']
    inv_second_idx = inv_second_edge['indices']
    G_first_match = {}
    G_second_match = {}
    match_count = 0 # Want to make sure this is a robust method

    for i in range(len(G_star)) :
        prev = G_star[i-1]
        curr = G_star[i]
        if prev['indices'] == inv_second_idx and curr['indices'] == inv_first_idx :
            match_count += 1
            G_first_match = curr
            G_second_match = prev

    if match_count == 0 :
        raise Exception('Could not find an edge match! Something is definitely broken.')
    if match_count > 1 :
        raise Exception('Too may edge matches! Something is definitely broken.')

    # If we are here, everything should be great
    lox = 1./(get_edge_vect(first_edge)*get_edge_vect(G_first_match))

    # Rescale for out parameter normalizaion. We assume that the complex
    # parameter is shorter than the real parameter (TODO: Does SnapPy always assume the opposite?)
    m = trans[0]
    n = trans[1]
    parabolic = G_ball['center']
    lox_sqrt = sqrt(lox)

    params = {}
    if abs(m) < abs(n) :
        params['lattice'] = (n / m).conjugate()
        params['lox_sqrt'] = (m * sqrt(lox)).conjugate()
        params['parabolic'] = (G_ball['center'] / m).conjugate()
    else :
        params['lattice'] = m / n
        params['lox_sqrt'] = n * sqrt(lox)
        params['parabolic'] = G_ball['center'] / n

    return params
