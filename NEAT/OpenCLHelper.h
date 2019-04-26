#include <string>
#include <CL/cl.hpp>
#include <fstream>
#pragma once

cl::Program CreateProgram(const std::string& file)
{
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	auto platform = platforms.front();
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);


	auto device = devices.front();
	auto vendor = device.getInfo<CL_DEVICE_VENDOR>();
	auto version = device.getInfo<CL_DEVICE_VERSION>();

	std::cout << "OpenCL Devices: " << devices.size() << " " << vendor << " " << version << std::endl;;

	std::ifstream kernelFile(file);
	std::string src(std::istreambuf_iterator<char>(kernelFile), (std::istreambuf_iterator<char>()));

	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));

	cl::Context context(devices);
	cl::Program program(context, sources);

	program.build("-cl-std=CL1.2");
	return program;
}