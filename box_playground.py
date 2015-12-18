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

import os
import glob
from sage.all import * 

scale_factor = 8 
scale = map(lambda x : scale_factor * pow(2, x/6.), range(0,-6,-1))
COMP_ERR = pow(2.,-30)

def g_depth(word) :
    g_count = 0
    for char in word :
        if char == 'g' or char == 'G' :
            g_count += 1
    return g_count

def real(x) :
    return x.real()

def imag(x) :
    return x.imag()

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
    sq_d = d.sqrt()
    return ((-b - sq_d)/(2*a), (-b + sq_d)/(2*a))

def floor(x) :
    int_x = int(x)
    if int_x > x :
        return int_x - 1
    else :
        return int_x

def get_domain_translate(p, m, n = 1.) :
    assert imag(m) > -COMP_ERR
    assert imag(n) == 0.
    assert real(n) > COMP_ERR
    # We check that we have minimal translation for m
    assert abs(abs(m) - min([abs(m), abs(m-n), abs(n+m)])) < COMP_ERR 
    # We assume that imag(m) >= 0, imag(n) == 0, real(n) > 0
    m_len = imag(p) / imag(m)
    m_pow = -floor(m_len + COMP_ERR)
    n_len = (real(p) - m_len * real(m)) / real(n)
    n_pow = -floor(n_len + COMP_ERR)
    shift = m_pow * m + n_pow * n
    domain_p = p + shift
    return {'translate' : domain_p, 'shift' : shift, 'm_pow' :  m_pow, 'n_pow' :  n_pow}

def get_least_translate(p, m, n = 1.) :
    # We make assertions about m,n in get_domain_translate
    domain_translate = get_domain_translate(p, m, n)
    near_by = [domain_translate]
    for t in [(-1.,0.),(0.,-1.),(-1.,-1.)] :
        near_by_translate = dict(domain_translate)
        near_by_translate['translate'] += m * t[0] + n * t[1]
        near_by_translate['shift'] += m * t[0] + n * t[1]
        near_by_translate['m_pow'] = t[0]
        near_by_translate['n_pow'] = t[1]
        near_by.append(near_by_translate)
    return min(near_by, key = lambda x : abs(x['translate'])) 

def get_nearest_translate(c, p, m, n=1.) :
    # We make assertions about m,n in get_domain_translate
    diff = p - c
    nearest_translate = get_least_translate(diff, m, n)
    nearest_translate['translate'] += c
    return nearest_translate 

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
    # We make assertions about m,n in get_domain_translate
    m = trans[0]
    n = trans[1]
    new_balls = []
    for ball in balls :
        new_ball = dict(ball)
        new_ball['center'] = get_domain_translate(ball['center'], m, n)['translate']
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
    return imag((-unit).log()) # We are using SnappyHP numbers here, so we call native methods

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

# Apply 1/z to the list. Note, the order must be reversed
def inv_edge_list(edges) :
    inv_edges = [inv_edge(edge) for edge in edges]
    inv_edges = inv_edges[::-1]
    return inv_edges

def find_shift_index(of_list, in_list, key) :
    len_of = len(of_list)
    len_in = len(in_list)
    found = []
    found_count = 0
    if len_of != len_in :
        return {'found' : found, 'count' : found_count}
    for i in range(len_in) :
        for j in range(len_of) :
            if of_list[j][key] != in_list[(i+j)%len_in][key] :
                break
            if j + 1 == len_of :
                found.append(i)
                found_count += 1
                break
    return {'found' : found, 'count' : found_count}

def append_census_params_to_file(params, file_name) :
    with open(file_name,'a') as fp :
        fp.write("M name={}\n".format(params['manifold']))
        fp.write("M vol={}\n".format(params['manifold_volume']))
        fp.write("M cusp_area={}\n".format(params['cusp_area']))
        fp.write("M flips={}\n".format(params['flips']))
        fp.write("M is_special={}\n".format(params['is_special']))
        fp.write("M lattice_might_be_norm_one={}\n".format(params['lattice_might_be_norm_one']))
        fp.write("M box={}\n".format(params['box_code']))
        fp.write("lattice = {0} + {1} I norm={2}\n".format(real(params['lattice']),imag(params['lattice']),abs(params['lattice'])))
        fp.write("lox_sqrt = {0} + {1} I norm={2}\n".format(real(params['lox_sqrt']),imag(params['lox_sqrt']),abs(params['lox_sqrt'])))
        fp.write("parabolic = {0} + {1} I norm={2}\n".format(real(params['parabolic']),imag(params['parabolic']),abs(params['parabolic'])))
    fp.close()

# This is out first SnapPy command
if 'snappy' not in sys.modules : 
    from snappy import *

def validate_params(params) :
    # We require that the loxodromic square root have norm >= 1. Since
    # the height of the maximal cusp neightborhood is 1/|sq_lox|, we need
    # 1/|sq_lox| <= 1 because a cusp with minimal horo-boundary translation
    # length 1 is always embedded (Jorgensen's ineq, see Meyerhoff's
    # "A LOWER BOUND FOR THE VOLUME OF HYPERBOLIC 3-MANIFOLDS" for standard proof)
    lox_sqrt = params['lox_sqrt']
    try :
        assert norm(lox_sqrt) >= 1.0
    except :
        print 'The maximal cusp is too small?!'
        print params
        return        

    # We require that -0.5 <= Re(lattice) <= 0.5
    lattice = params['lattice']
    while abs(real(lattice)) > 0.5 :
        if real(lattice) > 0 :
            lattice -= 1.0
        else :
            lattice += 1.0 

    # We must have the lattice length >= 1
    try :
        assert norm(lattice) >= 1.0
    except :
        print 'The lattice is too small?!'
        print params
        return        

    # We require that the parabolic element have
    parabolic = params['parabolic']
    # parabolic imag part <= 0.5 * (lattice imag part) 
    while abs(imag(parabolic)) > 0.5 * abs(imag(lattice)) :
        if imag(parabolic) > 0. :
            if imag(lattice) > 0. :
                parabolic -= lattice
            else :
                parabolic += lattice
        else :
            if imag(lattice) > 0. :
                parabolic += lattice
            else :
                parabolic -= lattice
    # parabolic real part <= 0.5
    while abs(real(parabolic)) > 0.5 :
        if real(parabolic) > 0. :
            parabolic -= 1.0
        else :
            parabolic += 1.0

    # It remains fix positivity/sign coniditons    
    # We record the parameter flips we have done in case we ever need to reference
    parameter_flips = []
    # parabolic real part >= 0 TODO in TestCollection this is > 0 
    if real(parabolic) < 0. :
        parabolic = -parabolic
        lattice = -lattice
        parameter_flips.append('RP')
    # parabolic imag part >= 0 TODO in TestCollection this is > 0
    if imag(parabolic) < 0. :
        parabolic = parabolic.conjugate()
        lattice = lattice.conjugate()
        lox_sqrt = lox_sqrt.conjugate()
        parameter_flips.append('IP')
    # lattice imag part >= 0 TODO in TestCollection this is > 0 
    if imag(lattice) < 0. :
        lattice = -lattice    
        parameter_flips.append('IL')
    # lox_sqrt imag part >= 0 TODO in TestCollection this is > 0 
    if imag(lox_sqrt) < 0. :
        lox_sqrt = -lox_sqrt
        parameter_flips.append('IS')
    # We keep the SPECIAL convention tag used by NJT
    params['is_special'] = False
    for param in [lattice, lox_sqrt, parabolic] :
        n = abs(param)
        s = scale_factor * n
        comp = abs(s - floor(s))
        if comp < COMP_ERR :
            params['is_special'] = True
            break
    # We should now have the bounds
    # 0. |lox_sqrt| >= 1 (horoball size)
    # 1. Im(lox_sqrt) >= 0 (only lox_sqrt^2 matters)
    # 2. -1/2 <= Re(lattice) <= 1/2 (reduction modulo N)
    # 3. Im(lattice) >= 0 (complex conjugate symmetry)
    # 4. 0 <= Im(parabolic) <= Im(lattice)/2 (reduction modulo M, flipping sign of lattice)
    # 5. 0 <= Re(parabolic) <= 1/2 (reduction modulo N, flipping sign of lattice)
    params['lattice'] = lattice
    params['lox_sqrt'] = lox_sqrt
    params['parabolic'] = parabolic
    params['flips'] =  parameter_flips

def get_box_code(validated_params, depth=120) :
    lattice = validated_params['lattice']
    lox_sqrt = validated_params['lox_sqrt']
    parabolic = validated_params['parabolic']
    coord = [0.]*6
    coord[0] = imag(lattice) / scale[0] 
    coord[1] = imag(lox_sqrt) / scale[1] 
    coord[2] = imag(parabolic) / scale[2] 
    coord[3] = real(lattice) / scale[3] 
    coord[4] = real(lox_sqrt) / scale[4] 
    coord[5] = real(parabolic) / scale[5] 
    code_list = []
    for i in range(0, depth) :
        n = i % 6
        if coord[n] > 0. :
            code_list.append('1')
            coord[n] = 2. * coord[n] - 1.
        else :
            code_list.append('0')
            coord[n] = 2. * coord[n] + 1.
    box_code = ''.join(code_list)
    return box_code

def get_params_from_manifold(mfld, census_out_file = None, cusp_idx = 0) :
    mfld_hp = mfld.high_precision()
    cusp_nbd = mfld_hp.cusp_neighborhood()

    # We must set the displacement to get a maximal picture for the specified cusp
    # We first set all other cusps to be super small. This seems to be necessary as
    # set_displacement will fail if other cusps are in the way. We also want the horoballs
    # form other cusps to not be tall
    for idx in range(cusp_nbd.num_cusps()) :
        cusp_nbd.set_displacement(-128.0,idx) # 128 is random here
    # We make sure that out cusp is its own stopper
    assert cusp_nbd.stopper(cusp_idx) == cusp_idx
    # We set the displacement
    cusp_nbd.set_displacement(cusp_nbd.reach(cusp_idx),cusp_idx)
    # Validate that the displacement was set successfully
    assert cusp_nbd.get_displacement(cusp_idx) == cusp_nbd.reach(cusp_idx)

    hbls = cusp_nbd.horoballs(1., high_precision=True)
    triang = cusp_nbd.triangulation(high_precision=True)
    trans = cusp_nbd.translations(cusp_idx)

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
    inv_zero_star = inv_edge_list(zero_star)

    found_dict = find_shift_index(inv_zero_star, G_star, 'indices')
    found_count = found_dict['count']
    found = found_dict['found']

    if found_count == 0 :
        raise Exception('Could not find an edge match! Something is definitely broken for manifold {0} and cusp {1}'.format(mfld.name(),cusp_idx))
    # If we found too many matches, this is too much symmetry. Something is wrong
    # with our algorithm or data
    if found_count > 2 :
        raise Exception('Too many edge matches! Something is broken for manifold {0}'.format(mfld.name()))
    if found_count == 2 :
        # We check that the two found vectors are antipodal. This happens in
        # highly symmetric cases.
        e1 = get_edge_vect(G_star[found[0]]) 
        e2 = get_edge_vect(G_star[found[1]])
        if abs(e2+e1) > COMP_ERR :
            raise Exception('Two found edges are not symmetric for manifold {0} and cusp {1}'.format(mfld.name(),cusp_idx))

    # We match the inverse zero star to the G_star cyclically
    zero_star_edge = zero_star[-1] # Since we searched via the inv of zero star
    G_star_edge = G_star[max(found)] # Use any of the at most two found matches

    lox = (get_edge_vect(zero_star_edge)*get_edge_vect(G_star_edge))**(-1)

    # Rescale for out parameter normalizaion. We assume that the complex
    # parameter is shorter than the real parameter (TODO: Does SnapPy always assume the opposite?)
    m = trans[0]
    n = trans[1]
    parabolic = G_ball['center']
    lox_sqrt = lox.sqrt()

    params = {}
    params['lattice_might_be_norm_one'] = False
    if abs(n) - abs(m) > COMP_ERR :
        params['lattice'] = (n / m).conjugate()
        params['lox_sqrt'] = (m * lox_sqrt).conjugate()
        params['parabolic'] = (G_ball['center'] / m).conjugate()
    elif abs(m) - abs(n) > COMP_ERR :
        params['lattice'] = m / n
        params['lox_sqrt'] = n * lox_sqrt
        params['parabolic'] = G_ball['center'] / n
    else :
        print 'Warning: translation lengths for {0} are really close for cusp {1}'.format(mfld.name(),cusp_idx)
        params['lattice_might_be_norm_one'] = True
        if abs(n) - abs(m) > 0. :
            params['lattice'] = (n / m).conjugate()
            params['lox_sqrt'] = (m * lox_sqrt).conjugate()
            params['parabolic'] = (G_ball['center'] / m).conjugate()
        else :
            params['lattice'] = m / n
            params['lox_sqrt'] = n * lox_sqrt
            params['parabolic'] = G_ball['center'] / n

    params['manifold'] = '{0}_{1}'.format(mfld.name(),cusp_idx)
    params['manifold_volume'] = mfld.volume()
    params['cusp_area'] = 2. * cusp_nbd.volume(cusp_idx)
    params['box_code'] = ''

    validate_params(params)

    if census_out_file :
        params['box_code'] = get_box_code(params)
        append_census_params_to_file(params, census_out_file)

    return params
