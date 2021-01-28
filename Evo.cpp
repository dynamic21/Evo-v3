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

class blueprintNode
{
public:
    int numberOfConnections;
    double mutationRate;     // the chance that this structureNode will mutate
    vector<int> connections; // list of connections to other structureNodes
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
    double mutationAmplitude; // how much a mutation can change the existing value
    double bias;              // the bias added if node is active
    vector<double> weights;   // list of connection weights

    void initialize(blueprintNode givenblueprintNode)
    {
        mutationAmplitude = defaultMutationAmplitude;
        bias = 0;
        for (int i = 0; i < givenblueprintNode.numberOfConnections; i++)
        {
            weights.push_back(0);
        }
    }

    void mutate(int givenNumberOfConnections)
    {
        bias += (randDouble() * 2 - 1) * mutationAmplitude;
        for (int i = 0; i < givenNumberOfConnections; i++)
        {
            weights[i] += (randDouble() * 2 - 1) * mutationAmplitude;
        }
    }
};

class agent
{
public:
    int numberOfNodes;
    vector<dataNode> dataNodes;                   // holds all weights and biases
    vector<blueprintNode> *pointerBlueprintNodes; // hold reference of structure to save space

    void initialize(vector<blueprintNode> *givenPointerBlueprintNodes)
    {
        numberOfNodes = numberOfInputNodes + numberOfOutputNodes;
        pointerBlueprintNodes = givenPointerBlueprintNodes;
        vector<blueprintNode> &blueprintNodes = *pointerBlueprintNodes;
        for (int i = 0; i < numberOfNodes; i++)
        {
            dataNode newDataNode;
            newDataNode.initialize(blueprintNodes[i]);
        }
    }

    void mutate()
    {
        for (int i = 0; i < numberOfNodes; i++)
        {
            vector<blueprintNode> &blueprintNodes = *pointerBlueprintNodes;
            dataNodes[i].mutate(blueprintNodes[i].numberOfConnections);
        }
    }
};

class specie
{
public:
    int numberOfNodes;                    // used for the number of blueprintNodes as well as agents a specie will have
    vector<blueprintNode> blueprintNodes; // the structure of the architecture
    vector<agent> agents;                 // agents in the specie

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
            blueprintNode newblueprintNode;
            if (i < numberOfInputNodes)
            {
                newblueprintNode.initialize(outputConnections);
            }
            else
            {
                newblueprintNode.initialize(inputConnections);
            }
            blueprintNodes.push_back(newblueprintNode);
        }
        for (int i = 0; i < numberOfNodes; i++)
        {
            agent newAgent;
            newAgent.initialize(&blueprintNodes);
            newAgent.mutate();
            agents.push_back(newAgent);
        }
    }
};

class world
{
public:
    int numberOfSpecies;
    vector<specie> species;

    void info()
    {
        for (int i = 0; i < numberOfSpecies; i++)
        {
            //
        }
    }

    void initialize()
    {
        numberOfSpecies = numberOfInputNodes + numberOfOutputNodes;
        for (int i = 0; i < numberOfSpecies; i++)
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
