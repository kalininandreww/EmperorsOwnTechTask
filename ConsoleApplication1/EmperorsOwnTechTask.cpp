#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <functional>
#include <numeric>
#include <string>


struct Node;

class Network //����� ���������� �� ���������� ������
{
public:

	std::vector<Node*> nodes; //��������� ��������� �� ����

	~Network() //����������
	{
		for (auto node : nodes)
		{
			delete node;
		}
	}

	void addNode(Node* node) //����� ��� ���������� �����
	{
		nodes.push_back(node);
	}

	void update() //����� ��� ���������� �����
	{
		std::vector<Node*> toDestruct; //������ ��������� ��� ��� ��������

		for (auto node : nodes)
		{
			int event = rand() % 5;

			switch (event)
			{
				case 0: //�����������
					break;
				case 1: //���� ������� �������
					node->createEvent();
					break;
				case 2: //���� ������������� �� ������
					if (!nodes.empty())
					{
						Node* neighbor = nodes[rand() % nodes.size()];
						node->subscribe(neighbor);
					}
					break;
				case 3: //���� ������������ �� ������
					if (!node->subscriptions.empty())
					{
						auto it = node->subscriptions.begin();
						std::advance(it, rand() % node->subscriptions.size());
						node->unsubscribe(it->first);
					}
					break;
				case 4: //���� ������� ����� ���� � ������������� �� ����
					node->createAndSubscribe(Network & network); //��� ������ � ��� ������� ��� �������� � ������ �������� ������ � �������� ��������� � ������
					break;

			}
		

			if (node->hasNoSubscribtions()) //�������� ������� ��������
			{
				toDestruct.push_back(node);
			}
		}
		
		for (auto node : toDestruct) //��������
		{
			nodes.erase(std::remove(nodes.begin(), nodes.end(), node), nodes.end()); //
			delete node;
		}

	}
};

struct Node //���������, �������������� ���� � ����
{
	//� ��������� � ��� ��� ��� ��������� ������ ��� ���� ���������
	std::string name; //��� ����
	std::map<Node*, std::function<void(int)>> subscriptions; //������ � ������� �������� �������� ���� �� ������ ����
	std::vector<int> inputData; //������ ���������� ������ �� ������ �����
	 
	Node(const std::string& name) : name(name) {}
	
	void createEvent() //����� �������� ������� � �������� ���
	{
		int event = rand() % 1000;
		for (auto &sub : subscriptions)
			{
			sub.second(event);
		}
	}

	void subscribe(Node* neighbor) //����� ��������
	{

		subscriptions[neighbor] = [this, neighbor](int data)
			{
				if (neighbor == this) return;
				inputData.push_back(data);
				int summ = std::accumulate(inputData.begin(), inputData.end(), 0);
					std::cout << neighbor->name << "->" << this->name << ": S = " << summ << std::endl; //����� ���� ������������
			};
	}

	void unsubscribe(Node* neighbor) //����� �������
	{
		subscriptions.erase(neighbor);
	}

	void createAndSubscribe(Network& network) //����� �������� ������ ���� � �������� �� ����
	{
		Node* newNode = new Node("����" + std::to_string(network.nodes.size() + 1));
	}

	bool hasNoSubscribtions() const //����� �������� ������� ��������
	{
		return subscriptions.empty();
	}
};

int main()
{
	Network network;

	while (network.nodes.size() > 0)
	{
		network.update();
	}

	return 0;
}