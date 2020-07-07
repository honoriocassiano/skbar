/*
 * framebuffer.h
 *
 *  Created on: 3 de jun de 2019
 *      Author: cassiano
 */

#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

namespace skbar {

class FrameBuffer {
public:

    enum class Mode {
        READ,
        WRITE,
        READ_WRITE
    };

	FrameBuffer(int width, int height, bool renderbufferTex = false);
	virtual ~FrameBuffer();

	void Bind(const Mode &mode);

	inline int Width() const {
		return width;
	}

	inline int Height() const {
		return height;
	}

	bool IsActive(const Mode &mode) const;

	void Unbind(const Mode &mode);

	inline FrameBuffer *GetCurrentDraw() {
		return drawFBO;
	}

	inline FrameBuffer *GetCurrentRead() {
		return readFBO;
	}

private:

	static FrameBuffer *readFBO; /**< current binded read framebuffer */
	static FrameBuffer *drawFBO; /**< current binded draw framebuffer */

	void Init();

	int width;
	int height;

	unsigned fbo;
	unsigned tex;
	unsigned depth;

	const bool renderbufferTex;
};

} /* namespace sbq */

#endif /* FRAMEBUFFER_H_ */
