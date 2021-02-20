#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#define defaultNumberOfInputNodes 1
#define defaultNumberOfOutputNodes 1
#define defaultMutationRate 0.1
#define defaultMutationAmplitude 0.1
#define globalMutationFactor 0.01
#define populationExposurePercent 0.2
#define populationKeptPercent 0.2
// #define startingNumberOfWorlds 1 // manually set to 1 for now
#define startingNumberOfSpecies 3
#define startingNumberOfAgents 10

using namespace std;

// REMINDERS:
// - don't forget to reset/initialize the agent memory after mutation/ before starting a round

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
    int score;                               // the agent's score on an environment, initialized and used in world class during evaluation
    vector<blueprintNode *> *blueprintNodes; // holds reference to its specie's structure
    vector<dataNode *> dataNodes;            // holds all weights and biases

    agent(vector<blueprintNode *> *givenBlueprintNodes) // default initializer
    {
        blueprintNodes = givenBlueprintNodes;
        numberOfNodes = blueprintNodes->size();
        score = 0;
        for (int i = 0; i < numberOfNodes; i++)
        {
            dataNodes.push_back(new dataNode((*givenBlueprintNodes)[i]->numberOfConnections)); // add a dataNode pointer given number of connections to the dataNodes list
        }
    }

    agent(agent *givenAgent) // copies an agent
    {
        numberOfNodes = givenAgent->numberOfNodes;
        score = givenAgent->score;
        blueprintNodes = givenAgent->blueprintNodes;
        for (int i = 0; i < numberOfNodes; i++)
        {
            dataNodes.push_back(new dataNode(givenAgent->dataNodes[i])); // copies each dataNode to dataNodes list
        }
    }

    void info()
    {
        cout << "---|numberOfNodes: " << numberOfNodes << endl;
        cout << "---|score: " << score << endl;
        cout << "---|blueprintNodes: " << blueprintNodes << endl;
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
            dataNodes[i]->memory = dataNodes[i]->defaultMemory; // reset the memory to default value
            dataNodes[i]->currentlyActive = true;               // reset active state to true
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
            dataNodes[i]->currentlyActive = false;                     // placeholder/default value
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

    void evaluate()
    {
        sort(agents.begin(), agents.end(), [](agent *agent1, agent *agent2) { return (agent1->score > agent2->score); });
        for (int i = 0; i < numberOfAgents; i++)
        {
            cout << agents[i]->score << endl;
        }
        cout << endl;
        int topPercent = max(1, int(numberOfAgents * populationKeptPercent));
        for (int i = topPercent; i < numberOfAgents; i++)
        {
            delete agents[i];
            agents[i] = new agent(agents[i % topPercent]);
            agents[i]->mutate();
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

class environment
{
public:
    bool running;
    bool hit;
    int timeLeft;
    int scoreAvailable;
    int numberOfAgents;
    vector<bool> isDead;
    vector<agent *> agents;

    environment()
    {
        running = true;
        hit = false;
        timeLeft = 100;
        scoreAvailable = -1;
        numberOfAgents = 0;
    }

    void info()
    {
        cout << "running: " << running << endl;
        cout << "hit: " << hit << endl;
        cout << "timeLeft: " << timeLeft << endl;
        cout << "scoreAvailable: " << scoreAvailable << endl;
        cout << "numberOfAgents: " << numberOfAgents << endl;
        cout << "isDead: ";
        for (int i = 0; i < numberOfAgents; i++)
        {
            cout << isDead[i] << " ";
        }
        cout << endl
             << "agents: ";
        for (int i = 0; i < numberOfAgents; i++)
        {
            cout << agents[i] << " ";
        }
        cout << endl;
    }

    void addAgent(agent *givenAgent)
    {
        givenAgent->memoryReset();
        isDead.push_back(false);
        agents.push_back(givenAgent);
        numberOfAgents++;
    }

    void start()
    {
        while (running)
        {
            if (hit)
                scoreAvailable /= 2;
            if (!hit && (rand() % (timeLeft - 10)) == 0)
            {
                hit = true;
                scoreAvailable = 10;
            }
            for (int i = 0; i < numberOfAgents; i++)
            {
                if (!isDead[i])
                {
                    if (hit)
                    {
                        if (agents[i]->evaluate({1.0})[0] > 0)
                        {
                            agents[i]->score += scoreAvailable;
                            running = false;
                        }
                    }
                    else
                    {
                        isDead[i] = agents[i]->evaluate({0.0})[0] > 0;
                        if (isDead[i])
                            agents[i]->score += scoreAvailable;
                    }
                }
            }
            bool allDead = true;
            for (int i = 0; i < numberOfAgents; i++)
            {
                if (!isDead[i])
                {
                    allDead = false;
                    break;
                }
            }
            if (allDead || timeLeft == 0)
            {
                running = false;
            }
            timeLeft--;
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
            species.push_back(new specie(startingNumberOfAgents)); // add a specie pointer with given number of agents to the species list
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

    void evaluate()
    {
        int numberOfAgents = 0;
        vector<agent *> agents;
        for (int i = 0; i < numberOfSpecies; i++) // collect all agents in this world
        {
            for (int j = 0; j < species[i]->numberOfAgents; j++)
            {
                species[i]->agents[j]->score = 0;
                agents.push_back(species[i]->agents[j]);
                numberOfAgents++;
            }
        }
        for (int i = 0; i < numberOfAgents; i++) // shuffle list
        {
            int tempAgentIndex = rand() % numberOfAgents;
            agent *tempAgent = agents[tempAgentIndex];
            agents[tempAgentIndex] = agents[i];
            agents[i] = tempAgent;
        }
        for (int i = 0; i < numberOfAgents; i++) // put agents through the environment
        {
            environment newEnvironment;
            int k = i;
            for (int j = 0; j < max(2, int(numberOfAgents * populationExposurePercent)); j++) // decide number of agents in a game
            {
                newEnvironment.addAgent(agents[k]);
                k++;
                if (k == numberOfAgents)
                    k = 0;
            }
            newEnvironment.start();
            // newEnvironment.info();
            // cout << endl;
        }
        // for (int i = 0; i < numberOfAgents; i++)
        // {
        //     cout << agents[i]->score << endl;
        // }
        for (int i = 0; i < numberOfSpecies; i++)
        {
            species[i]->evaluate();
        }
    }
};

vector<world *> worlds;

int main()
{
    srand(unsigned(time(NULL)));
    randDouble();

    worlds.push_back(new world(startingNumberOfSpecies)); // add a world pointer given number of species to the world list
    int i = 10;
    while (i--)
    {
        worlds[0]->evaluate();
    }
    // agent one = new agent(worlds[0]->species[0]->agents[0]);
    // worlds[0]->species[0]->agents[0]->info();
    // one.info();

    // worlds[0]->species[0]->agents[0]->memoryReset();
    // vector<double> output = worlds[0]->species[0]->agents[0]->evaluate({randDouble(), randDouble(), randDouble()});
    // for (int i = 0; i < output.size(); i++)
    // {
    //     cout << output[i] << " ";
    // }
}