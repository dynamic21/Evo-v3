#include <iostream>
#include <vector>
#include <ctime>
#define defaultNumberOfInputNodes 3
#define defaultNumberOfOutputNodes 3
#define defaultMutationRate 0.1
#define defaultMutationAmplitude 0.1
#define globalMutationFactor 0.01

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

    void info()
    {
        cout << "---|numberOfConnections: " << numberOfConnections << endl;
        cout << "---|mutationRate: " << mutationRate << endl;
        cout << "---|connections: ";
        for (int i = 0; i < numberOfConnections; i++)
        {
            cout << connections[i] << " ";
        }
        cout << endl;
    }

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
public:                       // there is no numberOfWeights int due to it existing in the blueprintNodes
    double mutationAmplitude; // how much a mutation can change the existing value
    double bias;              // the bias added if node is active
    vector<double> weights;   // list of connection weights

    void info()
    {
        cout << "-----|mutationAmplitude: " << mutationAmplitude << endl;
        cout << "-----|bias: " << bias << endl;
        cout << "-----|weights: ";
        for (int i = 0; i < weights.size(); i++)
        {
            cout << weights[i] << " ";
        }
        cout << endl;
    }

    void initialize(blueprintNode givenblueprintNode)
    {
        mutationAmplitude = defaultMutationAmplitude;
        bias = 0;
        for (int i = 0; i < givenblueprintNode.numberOfConnections; i++)
        {
            weights.push_back(0);
        }
    }
};

class agent
{
public:
    int numberOfNodes;
    vector<blueprintNode> *pointerBlueprintNodes; // hold reference of structure to save space
    vector<dataNode> dataNodes;                   // holds all weights and biases

    void info()
    {
        cout << "---|numberOfNodes: " << numberOfNodes << endl;
        cout << "---|pointerBlueprintNodes: " << pointerBlueprintNodes << endl;
        cout << "---|dataNodes: " << endl;
        cout << "__________" << endl;
        for (int i = 0; i < numberOfNodes; i++)
        {
            dataNodes[i].info();
            cout << "__________" << endl;
        }
    }

    void initialize(vector<blueprintNode> *givenPointerBlueprintNodes)
    {
        // cout << givenPointerBlueprintNodes << endl;
        numberOfNodes = defaultNumberOfInputNodes + defaultNumberOfOutputNodes;
        pointerBlueprintNodes = givenPointerBlueprintNodes;
        vector<blueprintNode> &blueprintNodes = *pointerBlueprintNodes;
        for (int i = 0; i < numberOfNodes; i++)
        {
            dataNode newDataNode;
            dataNodes.push_back(newDataNode);
            dataNodes[i].initialize(blueprintNodes[i]);
        }
    }

    void mutate()
    {
        for (int i = 0; i < numberOfNodes; i++)
        {
            dataNodes[i].bias += (randDouble() * 2 - 1) * dataNodes[i].mutationAmplitude;
            for (int j = 0; j < dataNodes[i].weights.size(); j++)
            {
                dataNodes[i].weights[j] += (randDouble() * 2 - 1) * dataNodes[i].mutationAmplitude;
            }
            dataNodes[i].mutationAmplitude += (randDouble() * 2 - 1) * globalMutationFactor;
        }
    }
};

class specie
{
public:
    int numberOfNodes;                    // used for the number of blueprintNodes as well as agents a specie will have
    vector<blueprintNode> blueprintNodes; // the structure of the architecture
    vector<agent> agents;                 // agents in the specie

    void info()
    {
        cout << "-|numberOfNodes: " << numberOfNodes << endl;
        cout << "-|blueprintNodes: " << endl;
        cout << "__________" << endl;
        for (int i = 0; i < numberOfNodes; i++)
        {
            blueprintNodes[i].info();
            cout << "__________" << endl;
        }
        cout << "-|agents: " << endl;
        cout << "__________" << endl;
        for (int i = 0; i < 1; i++)
        {
            agents[i].info();
            cout << "__________" << endl;
        }
    }

    void initialize()
    {
        numberOfNodes = defaultNumberOfInputNodes + defaultNumberOfOutputNodes;
        vector<int> inputConnections;
        vector<int> outputConnections;
        for (int i = 0; i < defaultNumberOfInputNodes; i++)
        {
            inputConnections.push_back(i);
        }
        for (int i = defaultNumberOfInputNodes; i < numberOfNodes; i++)
        {
            outputConnections.push_back(i);
        }
        for (int i = 0; i < numberOfNodes; i++)
        {
            blueprintNode newblueprintNode;
            blueprintNodes.push_back(newblueprintNode);
            if (i < defaultNumberOfInputNodes)
            {
                blueprintNodes[i].initialize(outputConnections);
            }
            else
            {
                blueprintNodes[i].initialize(inputConnections);
            }
        }
        for (int i = 0; i < numberOfNodes; i++)
        {
            agent newAgent;
            agents.push_back(newAgent);
            agents[i].initialize(&blueprintNodes);
            agents[i].mutate();
        }
    }

    void mutate()
    {
        for (int i = 0; i < numberOfNodes; i++)
        {
            // for (int i = 0; i < numberOfNodes; i++)
            // {
            //     dataNodes[i].weights[i] += (randDouble() * 2 - 1) * dataNodes[i].mutationAmplitude;
            // }
            blueprintNodes[i].mutationRate += (randDouble() * 2 - 1) * globalMutationFactor;
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
        cout << "numberOdSpecies: " << numberOfSpecies << endl;
        cout << "species: " << endl;
        cout << "__________" << endl;
        for (int i = 0; i < numberOfSpecies; i++)
        {
            species[i].info();
            cout << "__________" << endl;
        }
    }

    void initialize()
    {
        numberOfSpecies = defaultNumberOfInputNodes + defaultNumberOfOutputNodes;
        for (int i = 0; i < numberOfSpecies; i++)
        {
            specie newSpecie;
            species.push_back(newSpecie);
            species[i].initialize();
            species[i].mutate();
        }
    }
};

world copy()
{
    /
}

bool compare(world one, world two)
{
    cout << "numberOdSpecies: " << numberOfSpecies << endl;
    cout << "species: " << endl;
    cout << "__________" << endl;
    for (int i = 0; i < numberOfSpecies; i++)
    {
        species[i].info();
        cout << "__________" << endl;
    }

    bool isSame = false;
    isSame = (isSame || one.numberOfSpecies == two.numberOfSpecies);
    for (int i = 0; i < one.numberOfSpecies; i++)
    {
        species[i].info();
        cout << "__________" << endl;
    }
    return isSame;
}

int main()
{
    srand(time(NULL));
    randDouble();

    world newWorld;
    newWorld.initialize();
    newWorld.info();

    int done;
    cout << "Enter Any Key To Exit: " << endl;
    cin >> done;
}
