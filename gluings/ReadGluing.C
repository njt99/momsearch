#include "ReadGluing.H"

static string PolyStrs[] = { "(4):", "(33):", "(5):", "(34):", "(333):",
			     "(6):", "(35):", "(44):", "(334):", 
				  "(3333):" };
static const int NumTets[] = { 4, 6, 5, 7, 9, 6, 8, 8, 10, 12 };
static const int NUM_TYPES = 10;

bool ConvertLine( string line, PolyhedraTypes & p_type, Matching & gluing )
{
  string::size_type pos, end_pos, length;
  PolyhedraTypes poly_type = _no_type;
  Matching gluing_data;
  int type_index;
	
  // Read the first word in 'line' to determine the polyhedra type.
  pos = line.find(' ');
  if( pos == string::npos ) return false;
  for( type_index=0; type_index<NUM_TYPES; type_index++ ){
    if( line.substr(0, pos) == PolyStrs[type_index] ){
      poly_type = (PolyhedraTypes) type_index;
      break;
    }
  }
  if( type_index == NUM_TYPES ) return false;
	
  // Now read the rest of the words in 'line' to determine the gluing data.
  pos++;
  length = line.length();
  while( pos < length ){
    if( line[pos] == ' ' ){
      pos++;
      continue;
    }
    end_pos = line.find(' ', pos+1);
    gluing_data.push_back(atoi(line.substr(pos, end_pos-pos).c_str()));
    if( end_pos == string::npos ){
      pos = length;
    } else {
      pos = end_pos+1;
    }
  }
	
  // Check the validity of the gluing data.
  if( (int) gluing_data.size() != 2*NumTets[type_index] ) return false;
  for( int i=0; i < (int) gluing_data.size(); i++ ){
    if( gluing_data[i] < 0 ||
	gluing_data[i] >= 2*NumTets[type_index] ||
	gluing_data[i] == i ||
	gluing_data[gluing_data[i]] != i ){
      return false;
    }
  }
	
  // At this point we know everything is valid.
  p_type = poly_type;
  gluing = gluing_data;
  return true;
}

bool CreateTriangulation( PolyhedraTypes poly_type,
			  const Matching & gluing_data,
			  TriangulationData & data )
{
  int num_tets;

  // First, create the TetrahedronData array.
  num_tets = NumTets[poly_type];
  data.num_tetrahedra = num_tets;
  data.tetrahedron_data = new TetrahedronData[num_tets];
  
  // Define the gluing permutations for the "left" and "right"
  // faces; this information is constant.
  for( int i=0; i<num_tets; i++ ){
    data.tetrahedron_data[i].gluing[0][0] = 1;
    data.tetrahedron_data[i].gluing[0][1] = 0;
    data.tetrahedron_data[i].gluing[0][2] = 2;
    data.tetrahedron_data[i].gluing[0][3] = 3;
    data.tetrahedron_data[i].gluing[1][0] = 1;
    data.tetrahedron_data[i].gluing[1][1] = 0;
    data.tetrahedron_data[i].gluing[1][2] = 2;
    data.tetrahedron_data[i].gluing[1][3] = 3;
  }

  // Define the "neighbour" data for the left and right faces. This
  // depends on poly_type, but not on the gluing data.
  switch( poly_type ){
  case _4:
    for( int i=0; i<4; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+1) % 4;
      data.tetrahedron_data[i].neighbor_index[1] = (i+3) % 4;
    }
    break;

  case _33:
    for( int i=0; i<3; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+1) % 3;
      data.tetrahedron_data[i].neighbor_index[1] = (i+2) % 3;
    }
    for( int i=3; i<6; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+1) % 3 + 3;
      data.tetrahedron_data[i].neighbor_index[1] = (i+2) % 3 + 3;
    }
    break;

  case _5:
    for( int i=0; i<5; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+1) % 5;
      data.tetrahedron_data[i].neighbor_index[1] = (i+4) % 5;
    }
    break;

  case _34:
    for( int i=0; i<3; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+1) % 3;
      data.tetrahedron_data[i].neighbor_index[1] = (i+2) % 3;
    }
    for( int i=3; i<7; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+2) % 4 + 3;
      data.tetrahedron_data[i].neighbor_index[1] = i % 4 + 3;
    }
    break;

  case _333:
    for( int i=0; i<3; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+1) % 3;
      data.tetrahedron_data[i].neighbor_index[1] = (i+2) % 3;
    }
    for( int i=3; i<6; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+1) % 3 + 3;
      data.tetrahedron_data[i].neighbor_index[1] = (i+2) % 3 + 3;
    }
    for( int i=6; i<9; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+1) % 3 + 6;
      data.tetrahedron_data[i].neighbor_index[1] = (i+2) % 3 + 6;
    }
    break;

  case _6:
    for( int i=0; i<6; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+1) % 6;
      data.tetrahedron_data[i].neighbor_index[1] = (i+5) % 6;
    }
    break;

  case _35:
    for( int i=0; i<3; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+1) % 3;
      data.tetrahedron_data[i].neighbor_index[1] = (i+2) % 3;
    }
    for( int i=3; i<8; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+3) % 5 + 3;
      data.tetrahedron_data[i].neighbor_index[1] = (i+1) % 5 + 3;
    }
    break;

  case _44:
    for( int i=0; i<4; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+1) % 4;
      data.tetrahedron_data[i].neighbor_index[1] = (i+3) % 4;
    }
    for( int i=4; i<8; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+1) % 4 + 4;
      data.tetrahedron_data[i].neighbor_index[1] = (i+3) % 4 + 4;
    }
    break;

  case _334:
    for( int i=0; i<3; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+1) % 3;
      data.tetrahedron_data[i].neighbor_index[1] = (i+2) % 3;
    }
    for( int i=3; i<6; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+1) % 3 + 3;
      data.tetrahedron_data[i].neighbor_index[1] = (i+2) % 3 + 3;
    }
    for( int i=6; i<10; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+3) % 4 + 6;
      data.tetrahedron_data[i].neighbor_index[1] = (i+1) % 4 + 6;
    }
    break;

  case _3333:
    for( int i=0; i<3; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+1) % 3;
      data.tetrahedron_data[i].neighbor_index[1] = (i+2) % 3;
    }
    for( int i=3; i<6; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+1) % 3 + 3;
      data.tetrahedron_data[i].neighbor_index[1] = (i+2) % 3 + 3;
    }
    for( int i=6; i<9; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+1) % 3 + 6;
      data.tetrahedron_data[i].neighbor_index[1] = (i+2) % 3 + 6;
    }
    for( int i=9; i<12; i++ ){
      data.tetrahedron_data[i].neighbor_index[0] = (i+1) % 3 + 9;
      data.tetrahedron_data[i].neighbor_index[1] = (i+2) % 3 + 9;
    }
    break;

  case _no_type:
    return false;
  }

  // Next, the "top" and "bottom" faces. These are determined
  // by gluing_data but not by poly_type.
  // For the "top" faces:
  for( int i=0; i<num_tets; i++ ){
    // If (*match)[i] = k, that means that the 2-face
    // (i.e. the "top" face) of simplex i is glued to a face of
    // simplex (k mod num_tets). If k<num_tets, then the gluing is
    // to another "top" face, otherwise it's to a "bottom" face.
    
    data.tetrahedron_data[i].neighbor_index[2] = gluing_data[i]%num_tets;
    if( gluing_data[i] < num_tets ){
      data.tetrahedron_data[i].gluing[2][0] = 1;
      data.tetrahedron_data[i].gluing[2][1] = 0;
      data.tetrahedron_data[i].gluing[2][2] = 2;
      data.tetrahedron_data[i].gluing[2][3] = 3;
    } else {
      data.tetrahedron_data[i].gluing[2][0] = 0;
      data.tetrahedron_data[i].gluing[2][1] = 1;
      data.tetrahedron_data[i].gluing[2][2] = 3;
      data.tetrahedron_data[i].gluing[2][3] = 2;
    }
  }

  // The "bottom" faces are handled similarly.
  for( int i=0; i<num_tets; i++ ){
    data.tetrahedron_data[i].neighbor_index[3] =
      gluing_data[i+num_tets] % num_tets;
    if( gluing_data[i+num_tets] < num_tets ){
      data.tetrahedron_data[i].gluing[3][0] = 0;
      data.tetrahedron_data[i].gluing[3][1] = 1;
      data.tetrahedron_data[i].gluing[3][2] = 3;
      data.tetrahedron_data[i].gluing[3][3] = 2;
    } else {
      data.tetrahedron_data[i].gluing[3][0] = 1;
      data.tetrahedron_data[i].gluing[3][1] = 0;
      data.tetrahedron_data[i].gluing[3][2] = 2;
      data.tetrahedron_data[i].gluing[3][3] = 3;
    }
  }

  data.name = "GLUING";
  data.solution_type = not_attempted;
  data.volume = 0;
  data.orientability = unknown_orientability;
  data.CS_value_is_known = false;
  data.CS_value = 0;
  data.num_or_cusps = 0;
  data.num_nonor_cusps = 0;
  data.cusp_data = 0;

  return true;
}
