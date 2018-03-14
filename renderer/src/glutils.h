#ifndef CHECKERROR_H
#define CHECKERROR_H

#include <memory>

#include <GLES3/gl3.h>
#include <logger/logger.h>

#define CHECK_GL_ERROR(instruction, msg, retValue...) \
	while (glGetError() != GL_NO_ERROR) \
		; \
	instruction; \
	if (glGetError() != GL_NO_ERROR) { \
		LOG_ERROR(msg); \
		return retValue; \
	}

using GLuintPtr = std::shared_ptr<GLuint>;

#endif // CHECKERROR_H
