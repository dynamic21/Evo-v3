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
    double mutationRate;          // the chance that this structureNode will mutate
    vector<int> connectionVector; // list of connections to other structureNodes

    blueprintNode(vector<int> givenConnection)
    {
        numberOfConnections = givenConnection.size();
        for (int i = 0; i < numberOfConnections; i++)
        {
            mutationRate = defaultMutationRate;
            connectionVector.push_back(givenConnection[i]);
        }
    }

    void info()
    {
        cout << "---|numberOfConnections: " << numberOfConnections << endl;
        cout << "---|mutationRate: " << mutationRate << endl;
        cout << "---|connections: ";
        for (int i = 0; i < numberOfConnections; i++)
        {
            cout << connectionVector[i] << " ";
        }
        cout << endl;
    }
};

class dataNode
{
public:
    double mutationAmplitude = defaultMutationAmplitude; // how much a mutation can change the existing value
    double defaultMemory = 0;                            // the initial value held in the node when an agent is reset or started
    double memory = 0;                                   // stores the current value of a node
    bool currentlyActive = true;                         // store if a node is active
    double bias = 0;                                     // the bias added if node is active
    vector<double> weights;                         // list of connection weights

    dataNode(blueprintNode *givenblueprintNode)
    {
        for (int i = 0; i < givenblueprintNode->numberOfConnections; i++)
        {
            weights.push_back(0); // initializes the weights to zero
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
};

class agent
{
public:
    int numberOfNodes;
    vector<dataNode *> dataNodes; // holds all weights and biases

    agent(vector<blueprintNode *> givenBlueprintNodes)
    {
        numberOfNodes = givenBlueprintNodes.size();
        for (int i = 0; i < numberOfNodes; i++)
        {
            dataNodes.push_back(new dataNode(givenBlueprintNodes[i]));
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

    // void mutate()
    // {
    //     for (int i = 0; i < numberOfNodes; i++)
    //     {
    //         dataNodes[i].defaultMemory += (randDouble() * 2 - 1) * dataNodes[i].mutationAmplitude;
    //         dataNodes[i].bias += (randDouble() * 2 - 1) * dataNodes[i].mutationAmplitude;
    //         for (int j = 0; j < dataNodes[i].weights.size(); j++)
    //         {
    //             dataNodes[i].weights[j] += (randDouble() * 2 - 1) * dataNodes[i].mutationAmplitude;
    //         }
    //         dataNodes[i].mutationAmplitude += (randDouble() * 2 - 1) * globalMutationFactor;
    //     }
    // }

    // void factoryReset()
    // {
    //     for (int i = 0; i < numberOfNodes; i++)
    //     {
    //         dataNodes[i].memory = dataNodes[i].defaultMemory; // reset the memory
    //         dataNodes[i].currentlyActive = true;              // reset active state
    //     }
    // }

    // vector<double> evaluate(vector<blueprintNode> givenBlueprintNodes, vector<double> givenInputs)
    // {
    //     vector<double> futureMemory;   // dataNode[i].futureMemory
    //     vector<bool> perviouslyActive; // dataNode[i].previouslyActive
    //     for (int i = 0; i < defaultNumberOfInputNodes /* + defaultNumberOfOutputNodes*/; i++)
    //     {
    //         dataNodes[i].memory += givenInputs[i]; // importing the inputs
    //     }
    //     for (int i = 0; i < givenBlueprintNodes.size(); i++)
    //     {
    //         futureMemory.push_back(0);                                // placeholder
    //         perviouslyActive.push_back(dataNodes[i].currentlyActive); // present becomes past
    //         dataNodes[i].currentlyActive = false;                     // placeholder
    //     }
    //     for (int i = 0; i < numberOfNodes; i++)
    //     {
    //         if (perviouslyActive[i])
    //         {
    //             for (int j = 0; j < givenBlueprintNodes[i].numberOfConnections; j++)
    //             {
    //                 double sum = dataNodes[i].memory * dataNodes[i].weights[j];
    //                 if (sum >= 0)
    //                 {
    //                     int connectionNumber = givenBlueprintNodes[i].connections[j];
    //                     futureMemory[connectionNumber] += sum;
    //                     dataNodes[connectionNumber].currentlyActive = true;
    //                 }
    //             }
    //         }
    //     }
    //     for (int i = 0; i < numberOfNodes; i++)
    //     {
    //         if (dataNodes[i].currentlyActive)
    //         {
    //             dataNodes[i].memory = futureMemory[i] + dataNodes[i].bias;
    //         }
    //         else
    //         {
    //             dataNodes[i].memory = futureMemory[i];
    //         }
    //     }
    //     vector<double> output;
    //     for (int i = defaultNumberOfInputNodes; i < defaultNumberOfInputNodes + defaultNumberOfOutputNodes; i++)
    //     {
    //         if (dataNodes[i].currentlyActive)
    //         {
    //             output.push_back(dataNodes[i].memory);
    //         }
    //         else
    //         {
    //             output.push_back(0);
    //         }
    //     }
    //     return output;
    // }
};

class specie
{
public:
    int numberOfNodes; // number of blueprintNodes is the same as number of agents
    vector<blueprintNode *> blueprintNodes;
    vector<agent *> agents;

    specie(int givenNumberOfAgents)
    {
        numberOfNodes = givenNumberOfAgents;
        vector<int> inputConnections;
        vector<int> outputConnections;
        for (int i = 0; i < defaultNumberOfInputNodes; i++)
        {
            inputConnections.push_back(i); // list of input node indexes
        }
        for (int i = defaultNumberOfInputNodes; i < numberOfNodes; i++)
        {
            outputConnections.push_back(i); // list of output node indexes
        }
        for (int i = 0; i < numberOfNodes; i++)
        {
            if (i < defaultNumberOfInputNodes) // add the output indexes to current blueprint node connections if in input node position and the otherway around
            {
                blueprintNodes.push_back(new blueprintNode(outputConnections));
            }
            else
            {
                blueprintNodes.push_back(new blueprintNode(inputConnections));
            }
        }
        for (int i = 0; i < numberOfNodes; i++)
        {
            agents.push_back(new agent(blueprintNodes)); // add an agent pointer given the blueprint to the species list
            // agents[i]->mutate();
        }
    }

    void info()
    {
        cout << "-|numberOfNodes: " << numberOfNodes << endl;
        cout << "-|blueprintNodes: " << endl;
        cout << "__________" << endl;
        for (int i = 0; i < numberOfNodes; i++)
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

    // void mutate()
    // {
    //     for (int i = 0; i < numberOfNodes; i++)
    //     {
    //         blueprintNodes[i]->mutationRate += (randDouble() * 2 - 1) * globalMutationFactor;
    //     }
    // }
};

class world
{
public:
    int numberOfSpecies;
    vector<specie *> species;

    world(int givenNumberOfSpecies)
    {
        numberOfSpecies = givenNumberOfSpecies;
        for (int i = 0; i < numberOfSpecies; i++)
        {
            species.push_back(new specie(numberOfSpecies)); // add a specie pointer with input plus output node number of agents to the species list
            // species[i]->mutate();
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

int main()
{
    srand(time(NULL));
    randDouble();

    worlds.push_back(new world(defaultNumberOfInputNodes + defaultNumberOfOutputNodes)); // add a world pointer with input plus output node number of species to the world list
    worlds[0]->info();
}
