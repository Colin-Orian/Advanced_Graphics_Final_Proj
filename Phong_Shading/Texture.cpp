/********************************
 *
 *           texture.cpp
 *
 *  Simple texture loader
 *
 *****************************/
#define _CRT_SECURE_NO_WARNINGS

#include <FreeImage.h>
#include "texture.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
struct Texture *loadTexture(const char *filename, const char *fileFormat) {
	int i, j;
	int k;
	FIBITMAP *bitmap;
	BYTE *bits;
	unsigned char *data;
	struct Texture *result;
	int size;
	if (fileFormat == "PNG") {
		bitmap = FreeImage_Load(FIF_PNG, filename, PNG_DEFAULT);
	}
	else {
		bitmap = FreeImage_Load(FIF_JPEG, filename, JPEG_DEFAULT);
	}
	
	result = new Texture;

	result->width = FreeImage_GetWidth(bitmap);
	result->height = FreeImage_GetHeight(bitmap);
	result->depth = FreeImage_GetBPP(bitmap) / 8;
	size = result->width * result->height * result->depth;
	result->size = size;
	data = (unsigned char*)malloc(size);
	result->data = data;

	k = 0;
	for (j = 0; j < result->height; j++) {
		bits = FreeImage_GetScanLine(bitmap, j);
		for (i = 0; i < result->width; i++) {
			data[k++] = bits[FI_RGBA_RED];
			data[k++] = bits[FI_RGBA_GREEN];
			data[k++] = bits[FI_RGBA_BLUE];
			bits += 3;
		}
	}

	return(result);

}

struct Cube *loadCube(const char *basename) {
	char filename[256];
	const char *extensions[] = { "posx.jpg", "negx.jpg", "posz.jpg", "negz.jpg", "posy.jpg", "negy.jpg" };
	struct Cube *result;
	struct Texture *tex;
	int i;

	result = new Cube();
	strcpy(filename, basename);
	strcat(filename, "/");
	strcat(filename, extensions[0]);

	tex = loadTexture(filename, "jpg");
	result->width = tex->width;
	result->height = tex->height;
	result->depth = tex->depth;
	result->data[0] = tex->data;
	for (i = 1; i < 6; i++) {
		strcpy(filename, basename);
		strcat(filename, "/");
		strcat(filename, extensions[i]);
		tex = loadTexture(filename, "jpg");
		result->data[i] = tex->data;
	}

	return(result);

}

void createFramebufferTexture(GLuint * tex, unsigned int WIDTH, unsigned int HEIGHT, GLenum type, GLenum target){
	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, type, WIDTH, HEIGHT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, target, *tex, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void createTexture(GLuint* texLoc, Texture texture) {
	glGenTextures(1, texLoc);
	glBindTexture(GL_TEXTURE_2D, *texLoc);
	glActiveTexture(GL_TEXTURE0);
	
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, texture.width, texture.height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture.width, texture.height, GL_RGB,
		GL_UNSIGNED_BYTE, texture.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
