#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <functional>
#include <numeric>
#include <string>


struct Node //Structure for nodes
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

		subscriptions[neighbor] = [this, neighbor](int data)
			{
				if (neighbor == this) return;
				inputData.push_back(data);
				int summ = std::accumulate(inputData.begin(), inputData.end(), 0);
				std::cout << neighbor->name << "->" << this->name << ": S = " << summ << std::endl; //вывод всех отправленных
			};
	}

	void unsubscribe(Node* neighbor) //Function that unsubscribes a node to another one
	{
		subscriptions.erase(neighbor);
	}

	void createAndSubscribe(Network& network); //Function that creates a new node and subscribes to it

	bool hasNoSubscribtions() const //Function checking if a node is subscribed to another one
	{
		return subscriptions.empty();
	}
};

class Network //Class that manages nodes
{
public:

	std::vector<Node*> nodes; //Vector pointer to all the nodes

	~Network() //Destructor
	{
		for (auto node : nodes)
		{
			delete node;
		}
	}

	void addNode(Node* node) //Method to add nodes
	{
		nodes.push_back(node);
	}

	void update() //Method that manages actions and destruction of nodes
	{
		std::vector<Node*> toDestruct; //Vector of pointers to nodes without subscriptions

		for (auto node : nodes)
		{
			int event = rand() % 100;

			switch (event)
			{
			case 0: //Node does nothing
				break;
			case 1: //Node creates an event
				node->createEvent();
				break;
			case 2: //Node subscribes to neighbor
				if (!nodes.empty())
				{
					Node* neighbor = nodes[rand() % nodes.size()];
					node->subscribe(neighbor);
				}
				break;
			case 3: //Node unsubscribes from neighbor
				if (!node->subscriptions.empty())
				{
					auto it = node->subscriptions.begin();
					std::advance(it, rand() % node->subscriptions.size());
					node->unsubscribe(it->first);
				}
				break;
			case 4: //Node creates new node and subscribes to it
				node->createAndSubscribe(*this);
				break;

			}


			if (node->hasNoSubscribtions()) //Checking if the node is sobscribed to other nodes
			{
				toDestruct.push_back(node);
			}
		}

		for (auto node : toDestruct) //Destruction of nodes without subscriptions
		{
			nodes.erase(std::remove(nodes.begin(), nodes.end(), node), nodes.end());
			delete node;
		}

	}
	
};

void Node::createAndSubscribe(Network& network) //Function that creates a new node and subscribes to it
{
	Node* newNode = new Node("Узел" + std::to_string(network.nodes.size() + 1));
}


void startNetwork(Network& network) //Method for initializing the Network and getting parameters
{
	int initialNodes;

	double doNothingProbability;
	double eventStartProbability;
	double subscribeProbability;
	double unsubscribeProbability;
	double createAndSubscribeProbability;

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
		
		double totalProbability = doNothingProbability + eventStartProbability + subscribeProbability + subscribeProbability + unsubscribeProbability + createAndSubscribeProbability;

		if (totalProbability == 100)
		{
			hundred = true;
			std::cout << createAndSubscribeProbability; //delete later, this is for testing
		}
		else
		{
			std::cout << "Сумма вероятностей должна быть равна 100, попробуйте снова";
		}
	}

	for (int i = 0; i < initialNodes; i++) //Loop that creates initial nodes
	{
		Node* node = new Node("Узел" + std::to_string(i));
		network.addNode(node);
	}

	for (auto node : network.nodes) //Loop that adds initial random subscriptions
	{
		for (int i = 0; i < rand() % 2; i++)
		{
			Node* neighbor = network.nodes[rand() % network.nodes.size()];
			if (neighbor != node)
			{
				node->subscribe(neighbor);
			}
		}
	}
}

int main() //Main method that runs the programm
{
	Network network;
	startNetwork(network);

	while (network.nodes.size() > 0)
	{
		network.update();
	}

	return 0;
}