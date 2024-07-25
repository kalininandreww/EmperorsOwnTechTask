#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <functional>
#include <numeric>
#include <string>


struct Node;

class Network //Класс отвечающий за управление узлами
{
public:

	std::vector<Node*> nodes; //Векторный указатель на ноды

	~Network() //Деструктор
	{
		for (auto node : nodes)
		{
			delete node;
		}
	}

	void addNode(Node* node) //Метод для добавления узлов
	{
		nodes.push_back(node);
	}

	void update() //Метод для обновления узлов
	{
		std::vector<Node*> toDestruct; //Вектор указателй нод без подписок

		for (auto node : nodes)
		{
			int event = rand() % 5;

			switch (event)
			{
				case 0: //Бездействие
					break;
				case 1: //Узел создает событие
					node->createEvent();
					break;
				case 2: //Узел подписывается на соседа
					if (!nodes.empty())
					{
						Node* neighbor = nodes[rand() % nodes.size()];
						node->subscribe(neighbor);
					}
					break;
				case 3: //Узел отписывается от соседа
					if (!node->subscriptions.empty())
					{
						auto it = node->subscriptions.begin();
						std::advance(it, rand() % node->subscriptions.size());
						node->unsubscribe(it->first);
					}
					break;
				case 4: //Узел создает новый узел и подписывается на него
					node->createAndSubscribe(Network & network); //тут ошибка я так понимаю что проблема в вызове инстанса класса в качестве аргумента в классе
					break;

			}
		

			if (node->hasNoSubscribtions()) //Проверка наличия подписок
			{
				toDestruct.push_back(node);
			}
		}
		
		for (auto node : toDestruct) //Удаление
		{
			nodes.erase(std::remove(nodes.begin(), nodes.end(), node), nodes.end()); //
			delete node;
		}

	}
};

struct Node //Структура, представляющая узел в сети
{
	//я запутался в том где мне объявлять соседа это надо исправить
	std::string name; //Имя узла
	std::map<Node*, std::function<void(int)>> subscriptions; //Объект в котором хранятся подписки узла на другие узлы
	std::vector<int> inputData; //Вектор полученных данных от другиъ узлов
	 
	Node(const std::string& name) : name(name) {}
	
	void createEvent() //Метод создания события и рассылки его
	{
		int event = rand() % 1000;
		for (auto &sub : subscriptions)
			{
			sub.second(event);
		}
	}

	void subscribe(Node* neighbor) //Метод подписки
	{

		subscriptions[neighbor] = [this, neighbor](int data)
			{
				if (neighbor == this) return;
				inputData.push_back(data);
				int summ = std::accumulate(inputData.begin(), inputData.end(), 0);
					std::cout << neighbor->name << "->" << this->name << ": S = " << summ << std::endl; //вывод всех отправленных
			};
	}

	void unsubscribe(Node* neighbor) //Метод отписки
	{
		subscriptions.erase(neighbor);
	}

	void createAndSubscribe(Network& network) //Метод созлания нового узла и подписки на него
	{
		Node* newNode = new Node("Узел" + std::to_string(network.nodes.size() + 1));
	}

	bool hasNoSubscribtions() const //Метод проверки наличия подписок
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