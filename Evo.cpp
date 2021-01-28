#include <iostream>
#include <vector>
#include <ctime>
#define numberOfInputNodes 3
#define numberOfOutputNodes 3
#define defaultMutationRate 0.1
#define defaultMutationAmplitude 0.1

using namespace std;

double randDouble()
{
    return (double)rand() / RAND_MAX;
}

class structureNode
{
public:
    int numberOfConnections;
    double mutationRate;
    vector<int> connections;
    void initialize(vector<int> givenConnection)
    {
        numberOfConnections = givenConnection.size();
        for (int i = 0; i < numberOfConnections; i++)
        {
            mutationRate = defaultMutationRate;
            connections.push_back(givenConnection[i]);
        }
    }
};

class dataNode
{
public:
    vector<double> weights;

    void initialize(structureNode givenStructureNode)
    {
        for (int i = 0; i < givenStructureNode.numberOfConnections; i++)
        {
            weights.push_back(0);
        }
    }
};

class agent
{
public:
    int numberOfNodes;
    vector<dataNode> dataNodes;
    vector<structureNode> *structureNodes;

    void initialize(vector<structureNode> &givenStructureNodes)
    {
        vector<structureNode> &convertedStructureNodes = givenStructureNodes;
        numberOfNodes = numberOfInputNodes + numberOfOutputNodes;
        cout << givenStructureNodes << endl;
        structureNodes = givenStructureNodes;
        cout << structureNodes << endl;
        for (int i = 0; i < numberOfNodes; i++)
        {
            dataNode newDataNode;
            newDataNode.initialize(convertedStructureNodes[i]);
        }
    }
};

class specie
{
public:
    int numberOfNodes;
    vector<structureNode> structureNodes;
    vector<agent> agents;

    void initialize()
    {
        numberOfNodes = numberOfInputNodes + numberOfOutputNodes;
        vector<int> inputConnections;
        vector<int> outputConnections;
        for (int i = 0; i < numberOfInputNodes; i++)
        {
            inputConnections.push_back(i);
        }
        for (int i = numberOfInputNodes; i < numberOfNodes; i++)
        {
            outputConnections.push_back(i);
        }
        for (int i = 0; i < numberOfNodes; i++)
        {
            structureNode newStructureNode;
            if (i < numberOfInputNodes)
            {
                newStructureNode.initialize(outputConnections);
            }
            else
            {
                newStructureNode.initialize(inputConnections);
            }
            structureNodes.push_back(newStructureNode);
        }
        for (int i = 0; i < numberOfNodes; i++)
        {
            agent newAgent;
            newAgent.initialize(&structureNodes);
            agents.push_back(newAgent);
        }
    }
};

class world
{
public:
    vector<specie> species;
    void initialize()
    {
        for (int i = 0; i < numberOfInputNodes + numberOfOutputNodes; i++)
        {
            specie newSpecie;
            newSpecie.initialize();
        }
    }
};

int main()
{
    srand(time(NULL));
    randDouble();

    world newWorld;
    newWorld.initialize();

    int done;
    cout << "Enter Any Key To Exit: " << endl;
    cin >> done;
}
