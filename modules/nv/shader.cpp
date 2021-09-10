#include "debug.hpp"
#include "shader.hpp"

nv::shader::shader():
	vert_index(0),
	frag_index(0),
	comp_index(0)
{
}

void nv::shader::operator =(const std::string &filename)
{
	const auto n = this->list.size();
	this->list.resize(n + 1);
	this->list[n].load(filename);
}

void nv::shader::operator =(const char filename[])
{
	ASSERT(filename != nullptr)

	const auto n = this->list.size();
	this->list.resize(n + 1);
	this->list[n].load(filename);
}

uint32_t nv::shader::size() const
{
	return this->list.size();
}

void nv::shader::for_vertex_stage(const uint32_t n)
{
	this->vert_index = n;
	this->list[n].for_vertex_stage();
}

void nv::shader::for_fragment_stage(const uint32_t n)
{
	this->frag_index = n;
	this->list[n].for_fragment_stage();
}

void nv::shader::for_compute_stage(const uint32_t n)
{
	this->comp_index = n;
	this->list[n].for_compute_stage();
}

nv::shader::~shader()
{
}
