#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <functional>
#include <numeric>
#include <string>



struct Node;

//Класс отвечающий за управление узлами
class Network
{
public:

	std::vector<Node*> nodes; //Векторный указатель

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
					break;
				case 3: //Узел отписывается от соседа
					break;
				case 4: //Узел создает новый узел и подписывается на него
					node->createAndSubscribe(Network& network); //!!!
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

//Структура, представляющая узел в сети
struct Node
{
	std::string name; //Имя узла
	std::map<Node*, std::function<void(int)>> subscriptions; //Объект в котором хранятся подписки узла на другие узлы
	std::vector<int> inputData; //Вектор полученных данных от другиъ узлов
	 
	Node(const std::string& name) : name(name) {}
	
	void createEvent() //Метод создания события и рассылки его
	{
		int event = rand() % 1000;
		for (auto sub : subscriptions) //найти почему тут ошибка
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

