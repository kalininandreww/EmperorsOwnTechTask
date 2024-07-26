#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <functional>
#include <numeric>
#include <string>


struct Node;

class Network //Class that manages nodes
{
public:

	std::vector<Node*> nodes; //Vector pointer to nodes

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
			int event = rand() % 5;

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
					node->createAndSubscribe(Network & network); //тут ошибка € так понимаю что проблема в вызове инстанса класса в качестве аргумента в классе
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

struct Node //Structure for nodes
{
	//€ запуталс€ в том где мне объ€вл€ть соседа это надо исправить
	std::string name; //Node's name
	std::map<Node*, std::function<void(int)>> subscriptions; //Object that stores data about subscriptions
	std::vector<int> inputData; //Vector of recived data
	 
	Node(const std::string& name) : name(name) {}
	
	void createEvent() //Function that creates an event
	{
		int event = rand() % 1000;
		for (auto &sub : subscriptions)
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

	void createAndSubscribe(Network& network) //Function that creates a new node and subscribes to it
	{
		Node* newNode = new Node("”зел" + std::to_string(network.nodes.size() + 1));
	}

	bool hasNoSubscribtions() const //Function checking if a node is subscribed to another one
	{
		return subscriptions.empty();
	}
};

int main() //Main method that has the loop in it
{
	Network network;

	while (network.nodes.size() > 0)
	{
		network.update();
	}

	return 0;
}