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
    vector<int> connections; // list of connections to other structureNodes' indexes

    blueprintNode(vector<int> givenConnections) // default initializer
    {
        numberOfConnections = givenConnections.size();
        mutationRate = defaultMutationRate;
        for (int i = 0; i < numberOfConnections; i++)
        {
            connections.push_back(givenConnections[i]); // copies the given connections
        }
    }

    blueprintNode(blueprintNode *givenBlueprintNode) // copies a blueprintNode
    {
        numberOfConnections = givenBlueprintNode->numberOfConnections;
        mutationRate = givenBlueprintNode->mutationRate;
        for (int i = 0; i < numberOfConnections; i++)
        {
            connections.push_back(givenBlueprintNode->connections[i]);
        }
    }

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

    void mutate()
    {
        mutationRate += (randDouble() * 2 - 1) * globalMutationFactor;
    }
};

class dataNode
{
public:
    int numberOfWeights;
    double mutationAmplitude; // how much a mutation can change the existing value
    double defaultMemory;     // the initial value held in the node when an agent is reset or started
    double memory;            // stores the current value of a node
    bool currentlyActive;     // store if a node is active
    double bias;              // the bias added if node is active
    vector<double> weights;   // list of connection weights

    dataNode(int givenNumberOfWeights) // default initializer
    {
        numberOfWeights = givenNumberOfWeights;
        mutationAmplitude = defaultMutationAmplitude;
        defaultMemory = 0;
        memory = 0;
        currentlyActive = true;
        bias = 0;
        for (int i = 0; i < numberOfWeights; i++)
        {
            weights.push_back(0); // initializes the weights to zero
        }
    }

    dataNode(dataNode *givenDataNode) // copies a dataNode
    {
        numberOfWeights = givenDataNode->numberOfWeights;
        mutationAmplitude = givenDataNode->mutationAmplitude;
        defaultMemory = givenDataNode->defaultMemory;
        memory = givenDataNode->memory;
        currentlyActive = givenDataNode->currentlyActive;
        bias = givenDataNode->bias;
        for (int i = 0; i < numberOfWeights; i++)
        {
            weights.push_back(givenDataNode->weights[i]);
        }
    }

    void info()
    {
        cout << "-----|mutationAmplitude: " << mutationAmplitude << endl;
        cout << "-----|defaultMemory: " << defaultMemory << endl;
        cout << "-----|memory: " << memory << endl;
        cout << "-----|currentlyActive: " << currentlyActive << endl;
        cout << "-----|bias: " << bias << endl;
        cout << "-----|weights: ";
        for (int i = 0; i < weights.size(); i++)
        {
            cout << weights[i] << " ";
        }
        cout << endl;
    }

    void mutate() // change each value by their coresponding mutation value
    {
        defaultMemory += (randDouble() * 2 - 1) * mutationAmplitude;
        bias += (randDouble() * 2 - 1) * mutationAmplitude;
        for (int i = 0; i < numberOfWeights; i++)
        {
            weights[i] += (randDouble() * 2 - 1) * mutationAmplitude;
        }
        mutationAmplitude += (randDouble() * 2 - 1) * globalMutationFactor;
    }
};

class agent
{
public:
    int numberOfNodes;                       // same number of nodes as blueprintNodes because number of weights correlate to number of connections
    vector<dataNode *> dataNodes;            // holds all weights and biases
    vector<blueprintNode *> *blueprintNodes; // holds reference to its specie's structure

    agent(vector<blueprintNode *> *givenBlueprintNodes) // default initializer
    {
        blueprintNodes = givenBlueprintNodes;
        numberOfNodes = blueprintNodes->size();
        for (int i = 0; i < numberOfNodes; i++)
        {
            dataNodes.push_back(new dataNode((*givenBlueprintNodes)[i]->numberOfConnections)); // add a dataNode pointer given number of connections to the dataNodes list
        }
    }

    agent(agent *givenAgent) // copies an agent
    {
        numberOfNodes = givenAgent->numberOfNodes;
        for (int i = 0; i < numberOfNodes; i++)
        {
            dataNodes.push_back(new dataNode(givenAgent->dataNodes[i])); // copies each dataNode to dataNodes list
        }
    }

    void info()
    {
        cout << "---|numberOfNodes: " << numberOfNodes << endl;
        cout << "---|dataNodes: " << endl;
        cout << "__________" << endl;
        for (int i = 0; i < numberOfNodes; i++)
        {
            dataNodes[i]->info();
            cout << "__________" << endl;
        }
    }

    void mutate()
    {
        for (int i = 0; i < numberOfNodes; i++)
        {
            dataNodes[i]->mutate(); // mutate each dataNode
        }
    }

    void memoryReset() // resets/initializes the agent's memory
    {
        for (int i = 0; i < numberOfNodes; i++)
        {
            dataNodes[i]->memory = dataNodes[i]->defaultMemory; // reset the memory
            dataNodes[i]->currentlyActive = true;               // reset active state
        }
    }

    vector<double> evaluate(vector<double> givenInputs)
    {
        vector<double> futureMemory;   // dataNode[i].futureMemory
        vector<bool> perviouslyActive; // dataNode[i].previouslyActive
        vector<blueprintNode *> givenBlueprintNodes = *blueprintNodes;
        for (int i = 0; i < defaultNumberOfInputNodes; i++)
        {
            dataNodes[i]->memory += givenInputs[i]; // importing the inputs
        }
        for (int i = 0; i < numberOfNodes; i++)
        {
            futureMemory.push_back(0);                                 // placeholder
            perviouslyActive.push_back(dataNodes[i]->currentlyActive); // present becomes past
            dataNodes[i]->currentlyActive = false;                     // placeholder
        }
        for (int i = 0; i < numberOfNodes; i++)
        {
            if (perviouslyActive[i])
            {
                for (int j = 0; j < givenBlueprintNodes[i]->numberOfConnections; j++)
                {
                    double sum = dataNodes[i]->memory * dataNodes[i]->weights[j];
                    if (sum >= 0)
                    {
                        int connectionNumber = givenBlueprintNodes[i]->connections[j];
                        futureMemory[connectionNumber] += sum;
                        dataNodes[connectionNumber]->currentlyActive = true;
                    }
                }
            }
        }
        for (int i = 0; i < numberOfNodes; i++)
        {
            if (dataNodes[i]->currentlyActive)
            {
                dataNodes[i]->memory = futureMemory[i] + dataNodes[i]->bias;
            }
            else
            {
                dataNodes[i]->memory = futureMemory[i];
            }
        }
        vector<double> output;
        for (int i = defaultNumberOfInputNodes; i < defaultNumberOfInputNodes + defaultNumberOfOutputNodes; i++)
        {
            if (dataNodes[i]->currentlyActive)
            {
                output.push_back(dataNodes[i]->memory);
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
    int numberOfAgents;
    int numberOfBlueprintNodes;
    vector<blueprintNode *> blueprintNodes; // hold all connections pointing to other connection indexes
    vector<agent *> agents;                 // holds the agents

    specie(int givenNumberOfAgents) // default initializer
    {
        numberOfBlueprintNodes = defaultNumberOfInputNodes + defaultNumberOfOutputNodes; // default structure size
        vector<int> inputConnections;
        vector<int> outputConnections;
        for (int i = 0; i < defaultNumberOfInputNodes; i++)
        {
            inputConnections.push_back(i); // lists the input node indexes
        }
        for (int i = defaultNumberOfInputNodes; i < numberOfBlueprintNodes; i++)
        {
            outputConnections.push_back(i); // lists the output node indexes
        }
        for (int i = 0; i < numberOfBlueprintNodes; i++)
        {
            if (i < defaultNumberOfInputNodes) // add the output indexes to current blueprint node's connections if on input node index and the otherway around
            {
                blueprintNodes.push_back(new blueprintNode(outputConnections)); // add a blueprintNode pointer given connections to the species list
            }
            else
            {
                blueprintNodes.push_back(new blueprintNode(inputConnections));
            }
        }
        numberOfAgents = givenNumberOfAgents;
        for (int i = 0; i < numberOfAgents; i++)
        {
            agents.push_back(new agent(&blueprintNodes)); // add an agent pointer given the blueprint to the species list
            agents[i]->mutate();
        }
    }

    specie(specie *givenSpecie) // copies a specie
    {
        numberOfAgents = givenSpecie->numberOfAgents;
        numberOfBlueprintNodes = givenSpecie->numberOfBlueprintNodes;
        for (int i = 0; i < numberOfBlueprintNodes; i++)
        {
            blueprintNodes.push_back(new blueprintNode(givenSpecie->blueprintNodes[i])); // copies each blueprintNode to blueprintNodes list
        }
        for (int i = 0; i < numberOfAgents; i++)
        {
            agents.push_back(new agent(givenSpecie->agents[i])); // copies each agent to agents list
        }
    }

    void info()
    {
        cout << "-|numberOfAgents: " << numberOfAgents << endl;
        cout << "-|blueprintNodes: " << endl;
        cout << "__________" << endl;
        for (int i = 0; i < numberOfBlueprintNodes; i++)
        {
            blueprintNodes[i]->info();
            cout << "__________" << endl;
        }
        cout << "-|agents: " << endl;
        cout << "__________" << endl;
        for (int i = 0; i < 1; i++)
        {
            agents[i]->info();
            cout << "__________" << endl;
        }
    }

    void mutate()
    {
        for (int i = 0; i < numberOfAgents; i++)
        {
            blueprintNodes[i]->mutate();
        }
    }
};

class world
{
public:
    int numberOfSpecies;
    vector<specie *> species; // holds all specie pointers

    world(int givenNumberOfSpecies) // default initializer
    {
        numberOfSpecies = givenNumberOfSpecies;
        for (int i = 0; i < numberOfSpecies; i++)
        {
            species.push_back(new specie(numberOfSpecies)); // add a specie pointer with given number of agents to the species list
        }
    }

    world(world *givenWorld) // copies a world
    {
        numberOfSpecies = givenWorld->numberOfSpecies;
        for (int i = 0; i < numberOfSpecies; i++)
        {
            species.push_back(new specie(givenWorld->species[i])); // copies each specie to the species list
        }
    }

    void info()
    {
        cout << "numberOdSpecies: " << numberOfSpecies << endl;
        cout << "species: " << endl;
        cout << "__________" << endl;
        for (int i = 0; i < numberOfSpecies; i++)
        {
            species[i]->info();
            cout << "__________" << endl;
        }
    }
};

// class game
// {
// public:
//     int boarderSize = 10;
//     vector<agent> agents;
//     vector<vector<blueprintNode>> structures;

//     void addAgent(agent givenAgent, vector<blueprintNode> givenStructureNodes)
//     {
//         agents.push_back(givenAgent);
//         structures.push_back(givenStructureNodes);
//     }
// };

vector<world *> worlds;
// don't forget to reset/initialize the agent memory after mutation

int main()
{
    srand(time(NULL));
    randDouble();

    worlds.push_back(new world(defaultNumberOfInputNodes + defaultNumberOfOutputNodes)); // add a world pointer given number of species to the world list
    worlds[0]->species[0]->agents[0]->memoryReset();
    vector<double> output = worlds[0]->species[0]->agents[0]->evaluate({randDouble(), randDouble(), randDouble()});
    for (int i = 0; i < output.size(); i++)
    {
        cout << output[i] << " ";
    }
}