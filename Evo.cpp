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

void edit(world &one)
{
    one.numberOfSpecies++;
    for (int i = 0; i < one.species.size(); i++)
    {
        one.species[i].numberOfNodes++;
        for (int j = 0; j < one.species[i].blueprintNodes.size(); j++)
        {
            one.species[i].blueprintNodes[j].numberOfConnections++;
            one.species[i].blueprintNodes[j].mutationRate++;
            for (int k = 0; k < one.species[i].blueprintNodes[j].connections.size(); k++)
            {
                one.species[i].blueprintNodes[j].connections[k]++;
            }
        }

        for (int j = 0; j < one.species[i].agents.size(); j++)
        {
            one.species[i].agents[j].numberOfNodes++;
            one.species[i].agents[j].pointerBlueprintNodes = 0;
            for (int k = 0; k < one.species[i].agents[j].dataNodes.size(); k++)
            {
                one.species[i].agents[j].dataNodes[k].mutationAmplitude++;
                one.species[i].agents[j].dataNodes[k].bias++;
                for (int l = 0; l < one.species[i].agents[j].dataNodes[k].weights.size(); l++)
                {
                    one.species[i].agents[j].dataNodes[k].weights[l]++;
                }
            }
        }
    }
}

void compare(world one, world two)
{
    if (one.numberOfSpecies == two.numberOfSpecies)
        cout << "numberOfSpecies" << endl;
    for (int i = 0; i < one.species.size(); i++)
    {
        if (one.species[i].numberOfNodes == two.species[i].numberOfNodes)
            cout << "numberOfNodes" << endl;
        for (int j = 0; j < one.species[i].blueprintNodes.size(); j++)
        {
            if (one.species[i].blueprintNodes[j].numberOfConnections == two.species[i].blueprintNodes[j].numberOfConnections)
                cout << "numberOfConnections" << endl;
            if (one.species[i].blueprintNodes[j].mutationRate == two.species[i].blueprintNodes[j].mutationRate)
                cout << "mutationRate" << endl;
            for (int k = 0; k < one.species[i].blueprintNodes[j].connections.size(); k++)
            {
                if (one.species[i].blueprintNodes[j].connections[k] == two.species[i].blueprintNodes[j].connections[k])
                    cout << "connections" << endl;
            }
        }
        for (int j = 0; j < one.species[i].agents.size(); j++)
        {
            if (one.species[i].agents[j].numberOfNodes == two.species[i].agents[j].numberOfNodes)
                cout << "numberOfNodes" << endl;
            if (one.species[i].agents[j].pointerBlueprintNodes == two.species[i].agents[j].pointerBlueprintNodes)
                cout << "pointerBlueprintNodes" << endl;
            for (int k = 0; k < one.species[i].agents[j].dataNodes.size(); k++)
            {
                if (one.species[i].agents[j].dataNodes[k].mutationAmplitude == two.species[i].agents[j].dataNodes[k].mutationAmplitude)
                    cout << "mutationAmplitude" << endl;
                if (one.species[i].agents[j].dataNodes[k].bias == two.species[i].agents[j].dataNodes[k].bias)
                    cout << "bias" << endl;
                for (int l = 0; l < one.species[i].agents[j].dataNodes[k].weights.size(); l++)
                {
                    if (one.species[i].agents[j].dataNodes[k].weights[l] == two.species[i].agents[j].dataNodes[k].weights[l])
                        cout << "weights" << endl;
                }
            }
        }
    }
}

int main()
{
    srand(time(NULL));
    randDouble();

    world newWorld;
    newWorld.initialize();
    cout << newWorld.species[0].agents[0].dataNodes[0].weights[0] << endl;
    world world2;
    world2 = newWorld;
    edit(world2);
    cout << world2.species[0].agents[0].dataNodes[0].weights[0] << endl;
    cout << newWorld.species[0].agents[0].dataNodes[0].weights[0] << endl;
    compare(newWorld, world2);
    // newWorld.info();

    int done;
    cout << "Enter Any Key To Exit: " << endl;
    cin >> done;
}
