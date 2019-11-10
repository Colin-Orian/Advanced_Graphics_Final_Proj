#include "CreateGeometry.h"

void loadFile(std::string fileName, GLfloat * vertices, GLfloat * normals, GLuint * indices){
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	int nv;
	int nn;
	int ni;

	int i;
	struct _stat buf;
	std::string binName = fileName + ".bin";
	std::string objName = fileName + ".obj";

	int result = _stat(binName.c_str(), &buf);
	if (result == 0) {
		int fid = _open(binName.c_str(), _O_RDONLY | _O_BINARY);
		result = _read(fid, &nv, (sizeof vertices));
		result = _read(fid, &nn, (sizeof normals));
		result = _read(fid, &ni, (sizeof indices));

		printf("v: %d, n: %d i: %d\n", nv, nn, ni);
		vertices = new GLfloat[nv];
		result = _read(fid, vertices, nv*(sizeof GLfloat));
		normals = new GLfloat[nn];
		result = _read(fid, normals, nn*(sizeof GLfloat));
		indices = new GLuint[ni];
		result = _read(fid, indices, ni * sizeof(GLuint));
		_close(fid);
	}
	else {
		/*  Load the obj file */
		std::cout << "test" << std::endl;
		std::string err = tinyobj::LoadObj(shapes, materials, objName.c_str(), 0);

		if (!err.empty()) {
			std::cerr << err << std::endl;
		}

		/*  Retrieve the vertex coordinate data */

		nv = (int)shapes[0].mesh.positions.size();
		vertices = new GLfloat[nv];
		for (i = 0; i < nv; i++) {
			vertices[i] = shapes[0].mesh.positions[i];
		}

		/*  Retrieve the vertex normals */

		nn = (int)shapes[0].mesh.normals.size();
		normals = new GLfloat[nn];
		for (i = 0; i < nn; i++) {
			normals[i] = shapes[0].mesh.normals[i];
		}

		/*  Retrieve the triangle indices */
		ni = (int)shapes[0].mesh.indices.size();

		indices = new GLuint[ni];
		for (i = 0; i < ni; i++) {
			indices[i] = shapes[0].mesh.indices[i];
		}

		int fid = _open(binName.c_str(), _O_WRONLY | _O_BINARY | _O_CREAT, _S_IREAD | _S_IWRITE);
		result = _write(fid, &nv, (sizeof vertices));
		result = _write(fid, &nn, (sizeof normals));
		result = _write(fid, &ni, (sizeof indices));
		result = _write(fid, vertices, nv*(sizeof GLfloat));
		result = _write(fid, normals, nn*(sizeof GLfloat));
		result = _write(fid, indices, ni*(sizeof GLuint));
		_close(fid);
	}

	int vert = nv / 3;
	int nt = 2 * vert;
	GLfloat* tex = new GLfloat[nt];
	for (int i = 0; i < vert; i++) {
		GLfloat x = vertices[3 * i];
		GLfloat y = vertices[3 * i + 1];
		GLfloat z = vertices[3 * i + 2];
		double theta = atan2(x, z);
		double phi = atan2(y, sqrt(x*x + z * z));
		tex[2 * i] = fabs(theta) / M_PI;
		tex[2 * i + 1] = phi / M_PI;
	}

}

