#include "ReadGluing.h"
#include "snappea/SnapPea.h"
#include "snappea/unix_cusped_census.h"
#include <stdio.h>

vector<Triangulation*> targetManifolds;

void loadTargetManifolds()
{
	FILE* fp = fopen("targetManifolds", "r");
	char buf[1000];
	while (fgets(buf, sizeof(buf), fp)) {
		int idx = atoi(buf+1);
		char type = buf[0];
		int numTetrahedra = 0;
		switch(type) {
			case 'm': numTetrahedra = 5; break;
			case 's': numTetrahedra = 6; break;
			case 'v': numTetrahedra = 7; break;
		}
		if (numTetrahedra == 0) {
			fprintf(stderr, "can't parse manifold %s", buf);
			continue;
		}
		Triangulation* t = GetCuspedCensusManifold(numTetrahedra, oriented_manifold, idx);
		if (!t) {
			fprintf(stderr, "can't load %s", buf);
			continue;
		}
		targetManifolds.push_back(t);
	}
	fclose(fp);
}

int main()
{
	loadTargetManifolds();
	char buf[1000];
	PolyhedraTypes p_types;
	Matching gluing;
	TriangulationData td;
	Triangulation* manifold;
	while (fgets(buf, sizeof(buf), stdin)) {
		int n = strlen(buf);
		if (buf[n-1] == '\n')
			buf[n-1] = '\0';
		if (!ConvertLine(buf, p_types, gluing)) {
			fprintf(stderr, "can't read %s\n", buf);
			continue;
		}

		if (!CreateTriangulation(p_types, gluing, td)) {
			fprintf(stderr, "can't make triangulation for %s\n", buf);
			continue;
		}

		data_to_triangulation(&td, &manifold);
		int vPrecision;
		double v = volume(manifold, &vPrecision);

		int i;
		Boolean are_isometric;
		for (i = 0; i < targetManifolds.size(); ++i) {
			FuncResult result = compute_isometries(manifold, targetManifolds[i], &are_isometric, 0, 0);
			if (result == func_OK && are_isometric) {
				printf("GLUING[%s] = %s\n", buf, get_triangulation_name(targetManifolds[i]));
				break;
			}
		}
			
		if (i == targetManifolds.size())
			printf("volume = %f[%d] gluing '%s'\n", v, vPrecision, buf);
	}
}
