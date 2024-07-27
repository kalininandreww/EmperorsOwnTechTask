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
struct Node;

double doNothingProbability;
double eventStartProbability;
double subscribeProbability;
double unsubscribeProbability;
double createAndSubscribeProbability;

struct Node: std::enable_shared_from_this<Node> //Structure for nodes
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
		std::cout << "sub start" << std::endl;
		if (neighbor == nullptr || neighbor == this) return;
		std::cout << "!neighbor == nullptr || !neighbor == this" << std::endl;
		subscriptions[neighbor] = [this, neighbor](int data)
			{
				std::cout << "inside loop" << std::endl;
				inputData.push_back(data);
				std::cout << "push_back" << std::endl;
				int summ = std::accumulate(inputData.begin(), inputData.end(), 0);
				std::cout << neighbor->name << "->" << this->name << ": S = " << summ << std::endl;
				std::cout << neighbor->name << "->" << this->name << ": N = " << inputData.size() << std::endl;
			};
		std::cout << "sub end" << std::endl;
	}

	void unsubscribe(Node* neighbor) //Function that unsubscribes a node from another one
	{
		if (neighbor == nullptr)
		{
			std::cerr << "Error: null pointer passed to unsubscribe" << std::endl;
			return;
		}
		//auto it = subscriptions.find(neighbor);
		//if (it <= subscriptions.end())
		//{
		//	subscriptions.erase(it);
		//}
		//else
		//{
		//	std::cerr << "Error: neighbor not found in subscriptions" << std::endl;
		//	return;
		//}
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

	void update() //Method that manages actions and destruction of nodes
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
			std::cout << "destruct start" << std::endl;
			for (auto n : nodes)
			{
				n->unsubscribe(node);
			}
			nodes.erase(node);
			delete node;
			std::cout << "destruct end" << std::endl;
		}

	}
	
};

void Node::createAndSubscribe(Network& network) //Function that creates a new node and subscribes to it
{
	std::cout << "createAndSubscribe start"<< std::endl;
	Node* newNode = new Node("Узел" + std::to_string(network.nodes.size()));
	network.addNode(newNode);
	this->subscribe(newNode);
	std::cout << "createAndSubscribe end"<<std::endl;
}

void startNetwork(Network& network) //Method for initializing the Network and getting parameters
{
	int initialNodes;

	std::cout << "Введите количество начальных улов: ";
	std::cin >> initialNodes;
	
	bool hundred = false;
	while (hundred == false)
	{
		std::cout << "Введите вероятность бездействия узла в процентах: ";
		std::cin >> doNothingProbability;
		std::cout << "Введите вероятность создания события узлом в процентах: ";
		std::cin >> eventStartProbability;
		std::cout << "Введите вероятность подписки на другой узел в процентах: ";
		std::cin >> subscribeProbability;
		std::cout << "Введите вероятность отписки от другого узла в процентах: ";
		std::cin >> unsubscribeProbability;
		std::cout << "Введите вероятность создания и подписки на новый узел в процентах: ";
		std::cin >> createAndSubscribeProbability;
		
		double totalProbability = doNothingProbability + eventStartProbability + subscribeProbability + unsubscribeProbability + createAndSubscribeProbability;

		if (totalProbability == 100)
		{
			doNothingProbability = doNothingProbability / 100.0;
			eventStartProbability = eventStartProbability / 100.0;
			subscribeProbability = subscribeProbability / 100.0;
			unsubscribeProbability = unsubscribeProbability / 100.0;
			createAndSubscribeProbability = createAndSubscribeProbability / 100.0;
			hundred = true;
		} else
		{
			std::cout << "Сумма вероятностей должна быть равна 100, попробуйте снова\n";
		}
	}

	for (int i = 0; i < initialNodes; ++i) //Loop that creates initial nodes
	{
		Node* node = new Node("Узел" + std::to_string(i));
		network.addNode(node);
	}

	for (auto node : network.nodes) //Loop that adds initial random subscriptions
	{
		for (int i = 0; i < 2; ++i)
		{
			if (!network.nodes.empty())
			{
				Node* neighbor = *std::next(std::begin(network.nodes), (rand() % network.nodes.size() - 1)+1);
				if (neighbor != node)
				{
					node->subscribe(neighbor);
				}
			}
		}
	}
}

int main() //Main method that runs the programm
{
	system("chcp 1251");
	Network network;
	startNetwork(network);

	while (network.nodes.size() > 0)
	{
		network.update();
		std::cout << "1 cycle"<<std::endl;
	}
	std::cout << "Не осталось ни одного узла";
	return 0;
}