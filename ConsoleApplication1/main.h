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

	void unsubscribe(Node* neighbor) //Function that unsubscribes a node tofrom another one
	{
		if (neighbor == nullptr)
		{
			std::cerr << "Error: null pointer passed to unsubscribe" << std::endl;
			return;
		}
		auto it = subscriptions.find(neighbor);
		if (it != subscriptions.end()) {
			subscriptions.erase(it);

		}
		else
		{
			std::cerr << "Error: neighbor not found in subscriptions" << std::endl;
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

	std::vector<Node*> nodes; //Vector shared pointer to all the nodes

	void addNode(Node* node) //Method to add nodes
	{
		nodes.push_back(node);
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
					Node* neighbor = nodes[rand() % nodes.size()];
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
			for (auto& n : nodes)
			{
				n->unsubscribe(node);
			}
			nodes.erase(std::remove(nodes.begin(), nodes.end(), node), nodes.end());
			delete node;
			std::cout << "destruct end" << std::endl;
		}

	}

};