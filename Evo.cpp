#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#define defaultNumberOfInputNodes 1
#define defaultNumberOfOutputNodes 1
#define defaultMutationRate 0.1
#define defaultMutationAmplitude 0.1
#define mutationFactor 0.01
#define populationExposurePercent 0.2
#define agentsKeptPercent 0.2
#define speciesKeptPercent 0.2
#define startingNumberOfWorlds 1
#define startingNumberOfSpecies 3
#define startingNumberOfAgents 10
#define logLength 10
#define timeSinceClimaxThreshold 100

using namespace std;

double randDouble()
{
    return (double)rand() / RAND_MAX;
}

class blueprintNode
{
public:
    int numberOfConnections;
    double mutationRate;     // the chance that this blueprintNode will mutate
    vector<int> connections; // list of connections to other blueprintNodes' indexes

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

    void mutate() // changes the mutationRate by the global mutationfactor
    {
        mutationRate += (randDouble() * 2 - 1) * mutationFactor;
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
        mutationAmplitude += (randDouble() * 2 - 1) * mutationFactor; // changes the mutationAmplitude by the global mutationfactor
    }
};

class agent
{
public:
    int numberOfNodes;                       // same number of blueprintNodes as dataNodes because number of connections correlate to number of weights
    int score;                               // the agent's overall score, used in species/world/environment class during evaluation
    vector<blueprintNode *> *blueprintNodes; // holds reference to its specie's structure of blueprintNodes
    vector<dataNode *> dataNodes;            // holds all weights and biases

    agent(vector<blueprintNode *> *givenBlueprintNodes) // default initializer
    {
        blueprintNodes = givenBlueprintNodes;
        numberOfNodes = blueprintNodes->size();
        score = 0;
        for (int i = 0; i < numberOfNodes; i++)
        {
            dataNodes.push_back(new dataNode((*givenBlueprintNodes)[i]->numberOfConnections)); // add a dataNode pointer, given number of connections, to the dataNodes list
        }
    }

    agent(agent *givenAgent) // copies an agent
    {
        numberOfNodes = givenAgent->numberOfNodes;
        score = givenAgent->score;
        blueprintNodes = givenAgent->blueprintNodes;
        for (int i = 0; i < numberOfNodes; i++)
        {
            dataNodes.push_back(new dataNode(givenAgent->dataNodes[i])); // copies each dataNode to the dataNodes list
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
            dataNodes[i]->mutate(); // mutate each dataNode, change weights, biases, etc
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

    vector<double> evaluateData(vector<double> givenInputs)
    {
        vector<double> futureMemory;                                   // same thing as if it were dataNode[i].futureMemory, but put here to save heap space
        vector<bool> perviouslyActive;                                 // same thing as if it were dataNode[i].previouslyActive, but put here to save heap space
        vector<blueprintNode *> givenBlueprintNodes = *blueprintNodes; // convert the specie's structure of blueprintNodes
        for (int i = 0; i < defaultNumberOfInputNodes; i++)
        {
            dataNodes[i]->memory += givenInputs[i]; // adds the inputs to their corelating dataNodes memory
        }
        for (int i = 0; i < numberOfNodes; i++)
        {
            futureMemory.push_back(0);                                 // placeholder/default value
            perviouslyActive.push_back(dataNodes[i]->currentlyActive); // present status becomes past status
            dataNodes[i]->currentlyActive = false;                     // placeholder/default value
        }
        for (int i = 0; i < numberOfNodes; i++) // for each node the agent has
        {
            if (perviouslyActive[i]) // if the node is active
            {
                for (int j = 0; j < givenBlueprintNodes[i]->numberOfConnections; j++) // pass the signal through all its connections
                {
                    double sum = dataNodes[i]->memory * dataNodes[i]->weights[j]; // signal is memory * connection weight
                    if (sum >= 0)                                                 // ensure it is a positive number otherwise the target node will remain inactive
                    {
                        int connectionNumber = givenBlueprintNodes[i]->connections[j]; // for efficiency store the connection number
                        futureMemory[connectionNumber] += sum;                         // change the future memory of that node by the signal
                        dataNodes[connectionNumber]->currentlyActive = true;           // since a signal got through, the node is now active
                    }
                }
            }
        }
        for (int i = 0; i < numberOfNodes; i++) // for each node the agent has
        {
            if (dataNodes[i]->currentlyActive) // if the node is active
            {
                dataNodes[i]->memory = futureMemory[i] + dataNodes[i]->bias; // set current memory to future memory plus the bias, the bias allows negative outputs
            }                                                                // if the node isn't active, there is no need to update the memory because it is NULL/0 until it becomes active again by obtaining a signal
        }
        vector<double> output; // package the output
        for (int i = defaultNumberOfInputNodes; i < defaultNumberOfInputNodes + defaultNumberOfOutputNodes; i++)
        {
            if (dataNodes[i]->currentlyActive)
            {
                output.push_back(dataNodes[i]->memory); // give value of output node if active
            }
            else
            {
                output.push_back(0); // give zero if inactive
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
    int timeSinceClimax;                    // time since the max score was updated
    int logIndex;                           // position to replace scoreLog with current score
    double currentScore;                    // holds the current score of the specie, average of the 100 items in scoreLog
    double maxScore;                        // holds the max score of the specie
    double scoreLog[logLength];             // holds 100 of the species top agents average scores for each round/evaluation cycle
    vector<blueprintNode *> blueprintNodes; // hold all connections pointing to other blueprintNodes
    vector<agent *> agents;                 // holds the agents

    specie(int givenNumberOfAgents) // default initializer
    {
        numberOfBlueprintNodes = defaultNumberOfInputNodes + defaultNumberOfOutputNodes; // default structure size
        dataReset();                                                                     // initialize the scoreLog, etc
        vector<int> inputConnections;                                                    // list of the input node indexes
        vector<int> outputConnections;                                                   // list of the output node indexes
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
            if (i < defaultNumberOfInputNodes) // add the output indexes to current blueprint node's connections if on input node index, and works the otherway around
            {
                blueprintNodes.push_back(new blueprintNode(outputConnections)); // add a blueprintNode pointer, given connections, to the species list
            }
            else
            {
                blueprintNodes.push_back(new blueprintNode(inputConnections));
            }
        }
        numberOfAgents = givenNumberOfAgents;
        for (int i = 0; i < numberOfAgents; i++)
        {
            agents.push_back(new agent(&blueprintNodes)); // add an agent pointer, given the blueprint, to the species list
            agents[i]->mutate();                          // mutate the agent so there is diversity once initialized
        }
    }

    specie(specie *givenSpecie) // copies a specie
    {
        numberOfAgents = givenSpecie->numberOfAgents;
        numberOfBlueprintNodes = givenSpecie->numberOfBlueprintNodes;
        timeSinceClimax = givenSpecie->timeSinceClimax;
        logIndex = givenSpecie->logIndex;
        currentScore = givenSpecie->currentScore;
        maxScore = givenSpecie->maxScore;
        for (int i = 0; i < logLength; i++)
        {
            scoreLog[i] = givenSpecie->scoreLog[i];
        }
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
        cout << "-|numberOfBlueprintNodes: " << numberOfBlueprintNodes << endl;
        cout << "-|timeSinceClimax: " << timeSinceClimax << endl;
        cout << "-|logIndex: " << logIndex << endl;
        cout << "-|currentScore: " << currentScore << endl;
        cout << "-|maxScore: " << maxScore << endl;
        cout << "-|scoreLog: ";
        for (int i = 0; i < logLength; i++)
        {
            cout << scoreLog[i] << " ";
        }
        cout << endl;
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

    void dataReset() // resets/initializes the specie's score, etc
    {
        timeSinceClimax = 0;
        logIndex = 0;
        currentScore = 0;
        maxScore = 0;
        for (int i = 0; i < logLength; i++)
        {
            scoreLog[i] = 0;
        }
    }

    void evaluateAgents() // this is the agent selection
    {
        sort(agents.begin(), agents.end(), [](agent *agent1, agent *agent2) { // sort from greatest to least
            return (agent1->score > agent2->score);
        });
        int topPercent = max(1, int(numberOfAgents * agentsKeptPercent)); // get index from where to start replacing agents, keep the top percent
        for (int i = topPercent; i < numberOfAgents; i++)                 // for every agent after the top percent
        {
            delete agents[i];                              // delete agent from heap and existence
            agents[i] = new agent(agents[i % topPercent]); // copy over one of the top agents
            agents[i]->mutate();                           // mutate the copied agent
        }
        int sum = 0;
        for (int i = 0; i < topPercent; i++) // get the average of the top scores
        {
            sum += agents[i]->score;
        }
        sum /= topPercent;
        currentScore += sum - scoreLog[logIndex]; // update the specie's current score to be the average of the score log
        scoreLog[logIndex] = sum;                 // add current score to scoreLog at the log index
        if (currentScore > maxScore)              // set max score and reset timeSinceClimax
        {
            maxScore = currentScore;
            timeSinceClimax = 0;
        }
        logIndex++; // increase logIndex to next index
        if (logIndex == logLength)
        {
            logIndex = 0;
        }
        timeSinceClimax++; // increase the timeSinceClimax
    }

    void mutate()
    {
        for (int i = 0; i < numberOfBlueprintNodes; i++)
        {
            blueprintNodes[i]->mutate(); // mutate each blueprintNode, add/delete nodes/connections
        }
    }
};

class environment // this is the testing environment, currently it tests for reaction time, train an instinct
{
public:
    bool running;           // is environment still running
    bool hit;               // when the agent is allowed to hit
    int timeLeft;           // timer till game ends/max game length
    int scoreAvailable;     // the score the agents will get if they hit at a certain time
    int numberOfAgents;     // used for isDead and agents because they are the same size
    vector<bool> isDead;    // is the agent dead, default is no
    vector<agent *> agents; // holds all agent pointers in the game

    environment() // default initializer
    {
        running = true;
        hit = false;
        timeLeft = 100;
        scoreAvailable = -1;
        numberOfAgents = 0;
    }

    environment(environment *givenEnvironment) // copies an environment
    {
        running = givenEnvironment->running;
        hit = givenEnvironment->hit;
        timeLeft = givenEnvironment->timeLeft;
        scoreAvailable = givenEnvironment->scoreAvailable;
        numberOfAgents = givenEnvironment->numberOfAgents;
        for (int i = 0; i < numberOfAgents; i++)
        {
            isDead[i] = givenEnvironment->isDead[i];                  // copies the isDead list
            agents.push_back(new agent(givenEnvironment->agents[i])); // copies each agent to agents list
        }
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

    void addAgent(agent *givenAgent) // adds an agent pointer to the environment
    {
        givenAgent->memoryReset();
        isDead.push_back(false);
        agents.push_back(givenAgent);
        numberOfAgents++;
    }

    void removeAgent(int givenIndex) // removes an agent pointer from the environment
    {
        //
    }

    void start()
    {
        while (running)
        {
            if (hit) // if already allowed to hit, decrease the score available by a factor of 2
            {
                scoreAvailable /= 2;
            }
            /* if not yet allowed to hit, then give possibility of allowing hitting,
            the - 20 is so that if it doesn't allow hitting until last minute,
            the score available has time to decrease to a small amount if the agents don't hit within the 20 extra ticks*/
            if (!hit && (rand() % (timeLeft - 20)) == 0)
            {
                hit = true;
                scoreAvailable = 10; // sets the score available to 10 so that the agents will gain score if they hit now
            }
            for (int i = 0; i < numberOfAgents; i++) // for each agent
            {
                if (!isDead[i]) // if alive
                {
                    if (hit) // if allowed to hit
                    {
                        if (agents[i]->evaluateData({1.0})[0] > 0) // if agent hits, increase their score by the score available
                        {
                            agents[i]->score += scoreAvailable;
                            running = false; // end the game after the loop so agents get points if it is a tie
                        }
                    }
                    else
                    {
                        isDead[i] = agents[i]->evaluateData({0.0})[0] > 0; // if agent hits, their score changes by -1 and they die
                        if (isDead[i])
                            agents[i]->score += scoreAvailable;
                    }
                }
            }
            bool allDead = true; // check if everyone is dead to end game
            for (int i = 0; i < numberOfAgents; i++)
            {
                if (!isDead[i])
                {
                    allDead = false;
                    break;
                }
            }
            if (allDead || timeLeft == 0) // if timeLeft is 0, end game as well
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

    void evaluateAllAgents() // evaluates all agents in the environment and check is species is ready for selection
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
            int tempAgentIndex = rand() % numberOfAgents; // stored for efficiency
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
                newEnvironment.addAgent(agents[k]); // add agent to the environment
                k++;
                if (k == numberOfAgents)
                    k = 0;
            }
            newEnvironment.start(); // run the environment until it finishes and update agent scores
            // newEnvironment.info();
            // cout << endl;
        }
        bool speciesReady = true; // check if all the species are past the timeSinceClimax threshold to see if they are ready for selection
        for (int i = 0; i < numberOfSpecies; i++)
        {
            species[i]->evaluateAgents();
            if (speciesReady && species[i]->timeSinceClimax < timeSinceClimaxThreshold) // first part is for efficiency cuz don't check if false
            {
                speciesReady = false;
            }
        }
        if (speciesReady) // species selection
        {
            int topPercent = max(1, int(numberOfSpecies * speciesKeptPercent)); // get index from where to start replacing species
            for (int i = topPercent; i < numberOfSpecies; i++)
            {
                delete species[i];                                // delete specie from heap and existence
                species[i] = new specie(species[i % topPercent]); // copy over one of the top species
                species[i]->dataReset();                          // reset specie data for new round of evolution
                species[i]->mutate();                             // mutate the copied specie
            }
            cout << "species evolved" << endl;
        }
    }
};

vector<world *> worlds;

int main()
{
    srand(unsigned((time(NULL) % 11939 + 1) * (time(NULL) % 1009 + 1) * (time(NULL) % 101 + 1) * (time(NULL) % 11 + 1) * (time(NULL) % 2 + 1))); // seed random function

    for (int i = 0; i < startingNumberOfWorlds; i++)
    {
        worlds.push_back(new world(startingNumberOfSpecies)); // add a world pointer, given number of species, to the world list
    }
    while (true) // forever evolve the worlds
    {
        for (int i = 0; i < startingNumberOfWorlds; i++)
        {
            worlds[i]->evaluateAllAgents();
        }
    }
}