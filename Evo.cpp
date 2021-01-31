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
public:                       // there is no numberOfWeights int due to it existing in the blueprintNodes given in evaluation
    double mutationAmplitude; // how much a mutation can change the existing value
    double defaultMemory;     // the initial value held in the node when an agent is reset or started
    double memory;            // stores the current value of a node
    bool currentlyActive;     // store if a node is active
    double bias;              // the bias added if node is active
    vector<double> weights;   // list of connection weights

    void info()
    {
        cout << "-----|mutationAmplitude: " << mutationAmplitude << endl;
        cout << "-----|defaultMemory: " << defaultMemory << endl;
        cout << "-----|memory: " << bias << memory;
        cout << "-----|currentlyActive: " << currentlyActive << endl;
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
        defaultMemory = 0;
        memory = 0;
        currentlyActive = true;
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
    vector<dataNode> dataNodes; // holds all weights and biases

    void info()
    {
        cout << "---|numberOfNodes: " << numberOfNodes << endl;
        cout << "---|dataNodes: " << endl;
        cout << "__________" << endl;
        for (int i = 0; i < numberOfNodes; i++)
        {
            dataNodes[i].info();
            cout << "__________" << endl;
        }
    }

    void initialize(vector<blueprintNode> givenBlueprintNodes)
    {
        numberOfNodes = defaultNumberOfInputNodes + defaultNumberOfOutputNodes;
        for (int i = 0; i < numberOfNodes; i++)
        {
            dataNode newDataNode;
            dataNodes.push_back(newDataNode);
            dataNodes[i].initialize(givenBlueprintNodes[i]);
        }
    }

    void mutate()
    {
        for (int i = 0; i < numberOfNodes; i++)
        {
            dataNodes[i].defaultMemory += (randDouble() * 2 - 1) * dataNodes[i].mutationAmplitude;
            dataNodes[i].bias += (randDouble() * 2 - 1) * dataNodes[i].mutationAmplitude;
            for (int j = 0; j < dataNodes[i].weights.size(); j++)
            {
                dataNodes[i].weights[j] += (randDouble() * 2 - 1) * dataNodes[i].mutationAmplitude;
            }
            dataNodes[i].mutationAmplitude += (randDouble() * 2 - 1) * globalMutationFactor;
        }
    }

    void factoryReset()
    {
        for (int i = 0; i < numberOfNodes; i++)
        {
            dataNodes[i].memory = dataNodes[i].defaultMemory; // reset the memory
            dataNodes[i].currentlyActive = true;              // reset active state
        }
    }

    vector<double> evaluate(vector<blueprintNode> givenBlueprintNodes, vector<double> givenInputs)
    {
        vector<double> futureMemory;   // dataNode[i].futureMemory
        vector<bool> perviouslyActive; // dataNode[i].previouslyActive
        for (int i = 0; i < defaultNumberOfInputNodes + defaultNumberOfOutputNodes; i++)
        {
            dataNodes[i].memory += givenInputs[i]; // importing the inputs
        }
        for (int i = 0; i < givenBlueprintNodes.size(); i++)
        {
            futureMemory.push_back(0);                                // placeholder
            perviouslyActive.push_back(dataNodes[i].currentlyActive); // present becomes past
            dataNodes[i].currentlyActive = false;                     // placeholder
        }
        for (int i = 0; i < numberOfNodes; i++)
        {
            if (perviouslyActive[i])
            {
                for (int j = 0; j < givenBlueprintNodes[i].numberOfConnections; j++)
                {
                    double sum = dataNodes[i].memory * dataNodes[i].weights[j];
                    if (sum >= 0)
                    {
                        int connectionNumber = givenBlueprintNodes[i].connections[j];
                        futureMemory[connectionNumber] += sum;
                        dataNodes[connectionNumber].currentlyActive = true;
                    }
                }
            }
        }
        for (int i = 0; i < numberOfNodes; i++)
        {
            if (dataNodes[i].currentlyActive)
            {
                dataNodes[i].memory = futureMemory[i] + dataNodes[i].bias;
            }
        }
        vector<double> output;
        for (int i = defaultNumberOfInputNodes; i < defaultNumberOfInputNodes + defaultNumberOfOutputNodes; i++)
        {
            if (dataNodes[i].currentlyActive)
            {
                output.push_back(dataNodes[i].memory);
            }
            else
            {
                output.push_back(0);
            }
        }
        return output;
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
            agents[i].initialize(blueprintNodes);
            agents[i].mutate();
        }
    }

    void mutate()
    {
        for (int i = 0; i < numberOfNodes; i++)
        {
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

class game
{
public:
    int boarderSize = 10;
    vector<agent> agents;
    vector<vector<blueprintNode>> structures;
    
    void addAgent(agent givenAgent, vector<blueprintNode> givenStructureNodes){
        agents.push_back(givenAgent);
        structures.push_back(givenStructureNodes);
    }
};

int main()
{
    srand(time(NULL));
    randDouble();

    world newWorld;
    newWorld.initialize();
    // newWorld.info();

    int done;
    cout << "Enter Any Key To Exit: " << endl;
    cin >> done;
}
