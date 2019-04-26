#include <iostream>
#include "RandomEngine.h"
#include "Evaluator.h"
#include "OpenCLHelper.h"
#include <algorithm>
#include <cmath>

using namespace std;

class TestEvaluator : public Evaluator {
public:
	TestEvaluator(int tPopulationSize, Genome& tSeed, GeneTracker* tGeneTracker) : Evaluator(
		tPopulationSize, tSeed, tGeneTracker) {}

private:
	float evaluateGenome(Genome* genome) override {
		float fitness = 4.0f;
		vector<vector<float>> testInputs = { {0, 0},
											{0, 1},
											{1, 0},
											{1, 1} };
		vector<float> expected = { 0, 1, 1, 0 };

		float sumSquareError = 0.0f;

		for (int i = 0; i < testInputs.size(); i++) {
			auto result = genome->evaluate(testInputs[i]);
			fitness -= std::pow((result[0] - expected[i]), 2.0);
		}


		return fitness;
	}

};

void testEvolution() {
	GeneTracker* geneTracker = new GeneTracker();

	Genome genome;
	genome.generatePhenotype(geneTracker);

	TestEvaluator testEvaluator(100, genome, geneTracker);

	for (int i = 0; i < 1000; i++) {
		testEvaluator.evaluate();

		auto alpha = testEvaluator.getAlpha();

		auto zero = alpha.evaluate({ 0.0f, 0.0f });

		auto one = alpha.evaluate({ 0.0f, 1.0f });

		auto two = alpha.evaluate({ 1.0f, 0.0f });

		auto three = alpha.evaluate({ 1.0f, 1.0f });


		std::cout << "============================" << endl;
		std::cout << "Generation: " << i << endl;
		std::cout << "Innovations: " << geneTracker->getCurrent() << endl;
		std::cout << "Species: " << testEvaluator.speciesCount() << endl;
		std::cout << "High Score: " << testEvaluator.highScore() << endl;
		std::cout << "Dominant Result: " << zero.front() << " " <<
			one.front() << " " << two.front() << " " << three.front()
			<< endl;
		std::cout << "============================" << endl;

		if (i % 20 == 0)
		{
			testEvaluator.getAlpha().print();
		}

	}


	testEvaluator.getAlpha().print();
}


void testOpenCL()
{
	cl::Program program = CreateProgram("kernels/NumericalReduction.cl");
	auto context = program.getInfo<CL_PROGRAM_CONTEXT>();
	auto devices = context.getInfo<CL_CONTEXT_DEVICES>();
	auto device = devices.front();

	std::vector<int> vec(1024);

	for (int i = 0; i < vec.size(); i++)
	{
		vec[i] = i;
	}

	cl::Kernel kernel(program, "NumericalReduction");
	cl_int err = 0;

	auto workGroupSize = kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(device, &err);

	auto numWorkGroups = vec.size() / workGroupSize;

	cl::Buffer buf(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int) * vec.size(), vec.data());
	cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(int) * numWorkGroups);

	kernel.setArg(0, buf);
	kernel.setArg(1, sizeof(int) * workGroupSize, nullptr);
	kernel.setArg(2, outBuf);

	std::vector<int> outVec(numWorkGroups);

	cl::CommandQueue queue(context, device);
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(vec.size()), cl::NDRange(workGroupSize));
	queue.enqueueReadBuffer(outBuf, CL_TRUE, 0, sizeof(int) * outVec.size(), outVec.data());

	std::cin.get();
}

void testEvaluate() {
	Genome genome;

	auto n1 = Node(1, NodeType::INPUT);
	auto n2 = Node(2, NodeType::INPUT);
	auto n3 = Node(3, NodeType::INPUT);

	auto n4 = Node(4, NodeType::OUTPUT);
	auto n5 = Node(5, NodeType::HIDDEN);


	genome.addNode(n1);
	genome.addNode(n2);
	genome.addNode(n3);
	genome.addNode(n4);
	genome.addNode(n5);

	genome.addGene(Gene(1, n1.getId(), n4.getId(), 1.2f, true));
	genome.addGene(Gene(2, n2.getId(), n4.getId(), 0.8f, false));
	genome.addGene(Gene(3, n3.getId(), n4.getId(), 0.5f, true));
	genome.addGene(Gene(4, n2.getId(), n5.getId(), 0.8f, true));
	genome.addGene(Gene(5, n5.getId(), n4.getId(), 1.0f, true));
	genome.addGene(Gene(8, n1.getId(), n5.getId(), 1.0f, true));


	genome.print();


	auto output = genome.evaluate({ 1.0f, 1.0f, 1.0f });

	std::cout << "output: ";
	for (auto o : output) {
		std::cout << o << " ";
	}

	std::cout << endl;
}

void testOpenCLObjects()
{
	GeneTracker* geneTracker = new GeneTracker();

	Genome genome;
	genome.generatePhenotype(geneTracker);

	genome.print();



	cl::Program program = CreateProgram("kernels/Test.cl");
	auto context = program.getInfo<CL_PROGRAM_CONTEXT>();
	auto devices = context.getInfo<CL_CONTEXT_DEVICES>();
	auto device = devices.front();

	cl_int err = 0;


	std::vector<Node> nodes = { genome.getNode(1) };
	//auto nodes = genome.getNodes();

	cl::Buffer nodesBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(Node) * nodes.size(), nodes.data(), &err);
	cl::Kernel kernel(program, "Test", &err);
	kernel.setArg(0, nodesBuffer);

	cl::CommandQueue queue(context, device);
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(nodes.size()));
	queue.enqueueReadBuffer(nodesBuffer, CL_TRUE, 0, sizeof(Node)* nodes.size(), nodes.data());

	for (auto pair : nodes)
		std::cout << "id: " << pair.getId() << " type:" << pair.getType() << " value: " << pair.getValue() << endl;

}

int main() {

	//testOpenCLObjects();
	//testOpenCL();
	testEvolution();
	//testEvaluate();
	return 0;
}