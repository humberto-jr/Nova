#include "pipeline.hpp"
#include "shader.hpp"
#include "debug.hpp"

nv::pipeline::pipeline()
{
	this->interface.add(this->viewport);
	this->interface.add(this->rasterizer);
	this->interface.add(this->multisampling);
	this->interface.add(this->blending);
	this->interface.add(this->stencil);
	this->interface.add(this->layout);
}

void nv::pipeline::use(const nv::shader &s, const uint32_t index)
{
	ASSERT(index < s.list.size())

	this->interface.add(s.list[index]);
}

nv::pipeline::~pipeline()
{
}
