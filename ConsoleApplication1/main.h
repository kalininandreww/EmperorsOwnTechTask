#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <functional>
#include <numeric>
#include <string>
#include <random>
#include <memory>
#include <unordered_set>

class Network;

struct Node : std::enable_shared_from_this<Node> //Structure for nodes
{
	std::string name; //Node's name
	std::map<Node*, std::function<void(int)>> subscriptions; //Object that stores data about subscriptions
	std::vector<int> inputData; //Vector of recived data

	Node(const std::string& name) : name(name) {}

	void createEvent() //Function that creates an event
	{
		int event = rand() % 1000;
		for (auto& sub : subscriptions)
		{
			sub.second(event);
		}
	}

	void subscribe(Node* neighbor) //Function that subscribes a node to another one
	{
		if (neighbor == nullptr || neighbor == this) return;
		subscriptions[neighbor] = [this, neighbor](int data)
			{
				inputData.push_back(data);
				int summ = std::accumulate(inputData.begin(), inputData.end(), 0);
				std::cout << neighbor->name << "->" << this->name << ": S = " << summ << std::endl;
				std::cout << neighbor->name << "->" << this->name << ": N = " << inputData.size() << std::endl;
			};
	}

	void unsubscribe(Node* neighbor) //Function that unsubscribes a node from another one
	{
		if (neighbor == nullptr)
		{
			return;
		}
		subscriptions.erase(neighbor);
	}

	void createAndSubscribe(Network& network); //Function that creates a new node and subscribes to it

	bool hasNoSubscriptions() const //Function checking if a node is subscribed to another one
	{
		return subscriptions.empty();
	}
};

class Network //Class that manages nodes
{
public:

	std::unordered_set<Node*> nodes; //Vector shared pointer to all the nodes

	void addNode(Node* node) //Method to add nodes
	{
		nodes.insert(node);
	}

	void update(double doNothingProbability, double eventStartProbability, double subscribeProbability, double unsubscribeProbability, double createAndSubscribeProbability) //Method that manages actions and destruction of nodes
	{
		std::vector<Node*> toDestruct; //Vector of pointers to nodes without subscriptions

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0.0, 1.0);

		for (auto node : nodes)
		{
			double randomNum = dis(gen);
			if (randomNum < eventStartProbability)
			{
				node->createEvent();
			}
			else if (randomNum < (eventStartProbability + subscribeProbability))
			{
				if (!nodes.empty())
				{
					//Node* neighbor = *nodes.begin(rand() % nodes.size());
					Node* neighbor = *std::next(std::begin(nodes), (rand() % nodes.size() - 1) + 1);
					if (neighbor != node)
					{
						node->subscribe(neighbor);
					}
				}
			}
			else if (randomNum < (eventStartProbability + subscribeProbability + unsubscribeProbability))
			{
				if (!node->subscriptions.empty())
				{
					auto it = node->subscriptions.begin();
					std::advance(it, rand() % node->subscriptions.size());
					node->unsubscribe(it->first);
				}
			}
			else if (randomNum < (eventStartProbability + subscribeProbability + unsubscribeProbability + createAndSubscribeProbability))
			{
				node->createAndSubscribe(*this);
			}

			if (node->hasNoSubscriptions()) //Checking if the node is subscribed to other nodes
			{
				toDestruct.push_back(node);
			}
		}
		for (auto node : toDestruct) //Destruction of nodes without subscriptions
		{
			for (auto n : nodes)
			{
				n->unsubscribe(node);
			}
			nodes.erase(node);
			delete node;
		}
	}
};
