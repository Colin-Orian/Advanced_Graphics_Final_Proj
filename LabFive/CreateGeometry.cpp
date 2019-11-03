#include "CreateGeometry.h"
Mesh loadFile(GLuint program, std::string fileName) {
	
	Mesh mesh;
	std::vector<GLfloat> vertVector;
	GLfloat* vertices;
	GLfloat* normals;
	GLuint* indices;
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
			return Mesh();
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
		GLfloat y = vertices[3 * i+1];
		GLfloat z = vertices[3 * i+2];
		double theta = atan2(x, z);
		double phi = atan2(y, sqrt(x*x + z * z));
		tex[2 * i] = fabs(theta) / M_PI;
		tex[2 * i + 1] = phi / M_PI;
	}

	
	mesh = Mesh(vertices, normals, tex, indices, nv, nn, ni, nt);
	mesh.sendToGPU(program);
	return mesh;
}

GLfloat* getCubeVert() {
	GLfloat vertices[8 * 3] = {
	-1.0, -1.0, -1.0,		//0
	-1.0, -1.0, 1.0,		//1
	-1.0, 1.0, -1.0,		//2
	-1.0, 1.0, 1.0, 		//3
	1.0, -1.0, -1.0,		//4
	1.0, -1.0, 1.0,		//5
	1.0, 1.0, -1.0,		//6
	1.0, 1.0, 1.0,			//7
	};

	return vertices;
}

GLfloat* getCubeNorm() {
	GLfloat normals[8 * 3] = {
	-1.0, -1.0, -1.0,			//0
	-1.0, -1.0, 1.0,			//1
	-1.0, 1.0, -1.0,			//2
	-1.0, 1.0, 1.0,				//3
	1.0, -1.0, -1.0,
	1.0, -1.0, 1.0,
	1.0, 1.0, -1.0,
	1.0, 1.0, 1.0,
	};

	return normals;
}

GLuint* getCubeIndicies() {
	GLuint indexes[36] = { 0, 1, 3, 0, 2, 3,
	0, 4, 5, 0, 1, 5,
	2, 6, 7, 2, 3, 7,
	0, 4, 6, 0, 2, 6,
	1, 5, 7, 1, 3, 7,
	4, 5, 7, 4, 6, 7 };
	return indexes;
}
Mesh createCube(GLuint program) {
	GLfloat vertices[8 * 3] = {
	-1.0, -1.0, -1.0,		//0
	-1.0, -1.0, 1.0,		//1
	-1.0, 1.0, -1.0,		//2
	-1.0, 1.0, 1.0, 		//3
	1.0, -1.0, -1.0,		//4
	1.0, -1.0, 1.0,		//5
	1.0, 1.0, -1.0,		//6
	1.0, 1.0, 1.0,			//7
	};

	

	GLfloat normals[8 * 3] = {
	-1.0, -1.0, -1.0,			//0
	-1.0, -1.0, 1.0,			//1
	-1.0, 1.0, -1.0,			//2
	-1.0, 1.0, 1.0,				//3
	1.0, -1.0, -1.0,
	1.0, -1.0, 1.0, 
	1.0, 1.0, -1.0,
	1.0, 1.0, 1.0,
	};

	GLfloat tex[8 * 2] = {
		-1.0, -1.0,			//0
		-1.0, -1.0,			//1
		-1.0, 1.0,			//2
		-1.0, 1.0,				//3
		1.0, -1.0,			//4
		1.0, -1.0,				//5
		1.0, 1.0,				//6
		1.0, 1.0,				//7
	};

	GLuint indexes[36] = { 0, 1, 3, 0, 2, 3,
		0, 4, 5, 0, 1, 5,
		2, 6, 7, 2, 3, 7,
		0, 4, 6, 0, 2, 6,
		1, 5, 7, 1, 3, 7,
		4, 5, 7, 4, 6, 7 };

	Mesh mesh = Mesh(vertices, normals, tex, indexes, 8 * 3, 8 * 3, 36, 8 * 2);
	mesh.sendToGPU(program);
	return mesh;
}
/*
Mesh createQuad(GLuint program) {
	GLfloat *vertices;
	vertices = new GLfloat[12];
	for (int i = 0; i < 12; i++) {
		vertices[i] = -4.0f;
	}

	-1, -1, -2, bot left
	1, -1, -2, bot right
	-1, 1, -2, top left
	1, 1, -2 top right

	vertices[3] = 4.0f;
	vertices[7] = 4.0f;
	vertices[9] = 4.0f;
	vertices[10] = 4.0f;
	for (int i = 0; i < 12; i++) {

		if (i % 3 == 2) {
			vertices[i] = -2.0f;
		}
		
	}
	GLfloat *normals;
	normals = new GLfloat[12];

	//Calculate the normals. The normals of a plane will all be indetical
	glm::vec3 forwardDir = glm::vec3(vertices[0], vertices[1], vertices[2]) 
							- glm::vec3(vertices[3], vertices[4], vertices[5]);
	glm::vec3 sideDir =glm::vec3(vertices[0], vertices[1], vertices[2])
							- glm::vec3(vertices[6], vertices[7], vertices[8]);
	glm::vec3 norm = glm::cross(forwardDir, sideDir);
	for (int i = 0; i < 12; i++) {
		if (i % 3 == 0) {
			normals[i] = norm.x;
		}
		else if (i % 3 == 1) {
			normals[i] = norm.y;
		}
		else {
			normals[i] = norm.z;
		}
	}
	GLuint *indices;
	indices = new GLuint[6];
	indices[0] = 0;
	indices[1] = 3;
	indices[2] = 2;

	indices[3] = 0;
	indices[4] = 1;
	indices[5] = 3;
	int nv = 12;
	int nn = 12;
	int ni = 6;

	Mesh mesh = Mesh(vertices, normals, indices, nv, nn, ni);
	mesh.sendToGPU(program);
	return mesh;
}*/