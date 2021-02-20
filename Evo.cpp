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

// REMINDERS:
// - don't forget to reset/initialize the agent memory after mutation/ before starting a round

// TODO:
// - species mutation

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
        mutationAmplitude += (randDouble() * 2 - 1) * mutationFactor;
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

    vector<double> evaluateData(vector<double> givenInputs)
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
    int timeSinceClimax;                    // time since the max score was updated
    int logIndex;                           // position to replace scoreLog with current score
    double currentScore;                    // holds the current score of the specie, average of the 100 items in scoreLog
    double maxScore;                        // holds the max score of the specie
    double scoreLog[logLength];             // holds 100 of the species best average scores
    vector<blueprintNode *> blueprintNodes; // hold all connections pointing to other connection indexes
    vector<agent *> agents;                 // holds the agents

    specie(int givenNumberOfAgents) // default initializer
    {
        numberOfBlueprintNodes = defaultNumberOfInputNodes + defaultNumberOfOutputNodes; // default structure size
        dataReset();
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

    void evaluateAgents()
    {
        sort(agents.begin(), agents.end(), [](agent *agent1, agent *agent2) {
            return (agent1->score > agent2->score);
        }); // sort from greatest to least
        // for (int i = 0; i < numberOfAgents; i++)
        // {
        //     cout << agents[i]->score << endl;
        // }
        // cout << endl;
        int topPercent = max(1, int(numberOfAgents * agentsKeptPercent)); // get index from where to start replacing agents
        for (int i = topPercent; i < numberOfAgents; i++)
        {
            delete agents[i];                              // delete agent from heap and existence
            agents[i] = new agent(agents[i % topPercent]); // copy over one of the top agents
            agents[i]->mutate();                           // mutate the copied agent
        }
        int sum = 0; // get the average of the top scores
        for (int i = 0; i < topPercent; i++)
        {
            sum += agents[i]->score;
        }
        sum /= topPercent;
        currentScore += sum - scoreLog[logIndex]; // update current score to be the average of the score log
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
        timeSinceClimax++; // count timeSinceClimax
    }

    void mutate()
    {
        for (int i = 0; i < numberOfAgents; i++)
        {
            blueprintNodes[i]->mutate();
        }
    }
};

class environment // this environment tests reaction time
{
public:
    bool running;           // is environment running
    bool hit;               // when the agent should hit
    int timeLeft;           // timer till game ends/ max game length
    int scoreAvailable;     // the score the agents will get if they hit
    int numberOfAgents;     // used for isDead and agents because they are the same size
    vector<bool> isDead;    // is the agent dead
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
            if (hit) // if already hit, decrease the score available by a factor of 2
                scoreAvailable /= 2;
            if (!hit && (rand() % (timeLeft - 10)) == 0)
            /* if not yet hit, then give possibility of hitting,
            the - 10 is so that if it doesn't hit until last minute,
            the score available can decrease to a small amount if the agents don't hit till last minute*/
            {
                hit = true;
                scoreAvailable = 10; // sets the score available to 10 so that the agents will gain score if they hit now
            }
            for (int i = 0; i < numberOfAgents; i++)
            {
                if (!isDead[i])
                {
                    if (hit)
                    {
                        if (agents[i]->evaluateData({1.0})[0] > 0) // if just/already hit and agent hits, give them the score available
                        {
                            agents[i]->score += scoreAvailable;
                            running = false; // end the game after loop so agents get points if its a tie
                        }
                    }
                    else
                    {
                        isDead[i] = agents[i]->evaluateData({0.0})[0] > 0; // if agent hits before hit, the score would be -1 and they die
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

    void evaluateAllAgents()
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
                newEnvironment.addAgent(agents[k]); // add agent to the environment
                k++;
                if (k == numberOfAgents)
                    k = 0;
            }
            newEnvironment.start(); // run the environment until it finishes and update agent scores
            // newEnvironment.info();
            // cout << endl;
        }
        bool speciesReady = true; // check if all the species are past the timeSinceClimax threshold to see if they are ready to evolve
        for (int i = 0; i < numberOfSpecies; i++)
        {
            species[i]->evaluateAgents();
            if (speciesReady && species[i]->timeSinceClimax < timeSinceClimaxThreshold) // first part is for efficiency cuz don't check if false
            {
                speciesReady = false;
            }
        }
        if (speciesReady)
        {
            int topPercent = max(1, int(numberOfSpecies * speciesKeptPercent)); // get index from where to start replacing agents
            for (int i = topPercent; i < numberOfSpecies; i++)
            {
                delete species[i];                                // delete specie from heap and existence
                species[i] = new specie(species[i % topPercent]); // copy over one of the top species
                species[i]->dataReset();                          // reset specie data for new round of evolution
                species[i]->mutate();                             // mutate the copied specie
            }
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
        worlds[0]->evaluateAllAgents();
    }
    // agent one = new agent(worlds[0]->species[0]->agents[0]);
    // worlds[0]->species[0]->agents[0]->info();
    // one.info();

    // worlds[0]->species[0]->agents[0]->memoryReset();
    // vector<double> output = worlds[0]->species[0]->agents[0]->evaluateData({randDouble(), randDouble(), randDouble()});
    // for (int i = 0; i < output.size(); i++)
    // {
    //     cout << output[i] << " ";
    // }
}