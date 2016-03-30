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
from itertools import combinations_with_replacement
if 'snappy' not in sys.modules : 
    from snappy import *

RR = RealField(200)
CC = ComplexField(200)
II = CC(1j)
COMP_ERR = pow(2,-RR(100))
BIG_COMP_ERR = pow(2,-RR(20))

def get_cusp_volumes(mfld, verbose = True) :
    ans = {'manifold' : mfld, 'name' : '{}'.format(mfld), 'cusp_volumes' : {}}
    try :
        cusp_nbd = mfld.cusp_neighborhood()
        for c_idx in range(cusp_nbd.num_cusps()) :
            for idx in range(cusp_nbd.num_cusps()) :
                cusp_nbd.set_displacement(-4.0, which_cusp = idx) # 4 is random here
            assert cusp_nbd.stopper(c_idx) == c_idx
            reach = cusp_nbd.reach(which_cusp = c_idx)
            cusp_nbd.set_displacement(reach, which_cusp = c_idx)
            assert abs(cusp_nbd.get_displacement(which_cusp = c_idx) - reach) < BIG_COMP_ERR
            ans['cusp_volumes'][c_idx] = cusp_nbd.volume(which_cusp = c_idx)
            if verbose :
                print '{0} cusp {1} has cusp volume {2}'.format(ans['name'],c_idx,cusp_nbd.volume(which_cusp = c_idx))
    except :
        print 'Error: could not build cusp neighborhood for {}'.format(mfld)
    return ans

def get_cusp_volumes_by_name(name, verbose = True) :
    mfld = ManifoldHP(name)
    return get_cusp_volumes(mfld, verbose = verbose)
    
def farey(n, asc=True):
    """Python function to print the nth Farey sequence, either ascending or descending. Code Source : Wikipedia"""
    seq = []
    if asc :
        a, b, c, d = 0, 1,  1 , n
    else :
        a, b, c, d = 1, 1, n-1, n
    seq.append((a,b))
    while (asc and c <= n) or (not asc and a > 0):
        k = int((n + b)/d)
        a, b, c, d = c, d, k*c - a, k*d - b
        seq.append((a,b))
    return seq

def farey_slopes(depth) :
    f_seq = farey(depth)
    slopes = set()
    for (a,b) in f_seq :
        slopes.add((a,b))
        slopes.add((-a,b))
        slopes.add((b,a))
        slopes.add((-b,a))
    return list(slopes)

def find_dehn_fillings(mfld, farey_depth = 7, unique = False) :
    cusp_nbd = mfld.cusp_neighborhood()
    vol_dict = get_cusp_volumes(mfld, verbose = False)
    cusps = range(cusp_nbd.num_cusps())
    identities = set()
    big_dehn_fill = []
    for c_idx in cusps :
        c_vol = vol_dict['cusp_volumes'][c_idx]
        slopes = farey_slopes(farey_depth)
        slopes.append((0,0)) # For "empty" fillings of some cusps
        cusps_to_fill = copy(cusps)
        cusps_to_fill.remove(c_idx)
        for slope_tuples in combinations_with_replacement(slopes, len(cusps_to_fill)) :
            K = copy(mfld)
            new_idx = 0
            for f_idx in range(len(cusps_to_fill)) :
                c = cusps_to_fill[f_idx]
                s = slope_tuples[f_idx]
                K.dehn_fill(s, c)
                if c == cusps[f_idx] and s == (0,0) :
                    new_idx += 1 
            K_cusp_vols = get_cusp_volumes(K, verbose = False)['cusp_volumes']
            if  len(K_cusp_vols) > 0 and K_cusp_vols[new_idx] - c_vol > COMP_ERR :
                # We found a filling that goes up
                K_ident = K.identify()
                if len(K_ident) == 0 :
                    print 'Could not identify filling {}'.format(K)
                K_ident_set = set(K_ident)
                if unique and len(K_ident_set & identities) > 0 :
                    continue
                else :
                    identities.update(K_ident_set) 
                big_dehn_fill.append({ 'parent' : mfld, 'child' : K, 'child_ident' : K_ident,\
                                       'parent_cusp_vol' : c_vol, 'child_cusp_vol' : K_cusp_vols[new_idx],\
                                       'parent_cusp_index' : c_idx, 'child_cusp_index' : new_idx,\
                                       'cusps_filled' : cusps_to_fill, 'filling_slopes' : slope_tuples})
    return big_dehn_fill
