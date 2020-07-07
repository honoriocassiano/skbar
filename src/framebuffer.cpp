/*
 * framebuffer.cpp
 *
 *  Created on: 3 de jun de 2019
 *      Author: cassiano
 */

#include "framebuffer.h"

#include <GL/glew.h>
#include <GL/gl.h>

namespace skbar {

FrameBuffer * FrameBuffer::readFBO = nullptr;
FrameBuffer * FrameBuffer::drawFBO = nullptr;

FrameBuffer::FrameBuffer(int w, int h, bool rbt) :
		renderbufferTex(rbt), width(w), height(h) {

	glGenFramebuffers(1, &fbo);
	glGenRenderbuffers(1, &depth);

	if (renderbufferTex) {
		glGenRenderbuffers(1, &tex);
	} else {
		glGenTextures(1, &tex);
	}

	Init();
}

FrameBuffer::~FrameBuffer() {
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &depth);

	if (renderbufferTex) {
		glDeleteRenderbuffers(1, &tex);
	} else {
		glDeleteTextures(1, &tex);
	}
}

void FrameBuffer::Bind(const Mode &mode) {
	if ((mode == Mode::READ) || (mode == Mode::READ_WRITE)) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		readFBO = this;
	}

	if ((mode == Mode::WRITE) || (mode == Mode::READ_WRITE)) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		drawFBO = this;
	}
}

void FrameBuffer::Unbind(const Mode &mode) {
	if ((mode == Mode::READ) || (mode == Mode::READ_WRITE)) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		readFBO = nullptr;
	}

	if ((mode == Mode::WRITE) || (mode == Mode::READ_WRITE)) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		drawFBO = nullptr;
	}
}

bool FrameBuffer::IsActive(const Mode &mode) const {
	// Check if a framebuffer was attached outside the class
	int drawId, readId;

	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawId);
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readId);

	bool ret = true;

	if ((mode == Mode::READ) || (mode == Mode::READ_WRITE)) {
		ret = ret && (readFBO == this)
				&& (static_cast<unsigned>(readId) == fbo);
	}

	if ((mode == Mode::WRITE) || (mode == Mode::READ_WRITE)) {
		ret = ret && (drawFBO == this)
				&& (static_cast<unsigned>(drawId) == fbo);
	}

	return ret;
}

void FrameBuffer::Init() {

	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Initialize texture
	if (!renderbufferTex) {
		glBindTexture(GL_TEXTURE_2D, tex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
		GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, tex, 0);

		glBindTexture(GL_TEXTURE_2D, 0);
	} else {
		glBindRenderbuffer(GL_RENDERBUFFER, tex);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA32F, width, height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_RENDERBUFFER, tex);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	// Initialize depth buffer
	glBindRenderbuffer(GL_RENDERBUFFER, depth);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
	GL_RENDERBUFFER, depth);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} /* namespace sbq */
