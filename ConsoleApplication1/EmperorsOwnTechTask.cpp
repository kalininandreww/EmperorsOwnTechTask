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
#include <Windows.h>
#include "main.h"

double doNothingProbability;
double eventStartProbability;
double subscribeProbability;
double unsubscribeProbability;
double createAndSubscribeProbability;

void Node::createAndSubscribe(Network& network) //Function that creates a new node and subscribes to it
{
	Node* newNode = new Node("Узел" + std::to_string(network.nodes.size()));
	network.addNode(newNode);
	this->subscribe(newNode);
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
		std::cout << "Чтобы остановить программу нажмите 'X'\n";
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
		if (GetKeyState('X'))
		{
			std::cout << "Программа была остановленна вручную";
			exit(0);
		}
		network.update(doNothingProbability, eventStartProbability, subscribeProbability, unsubscribeProbability, createAndSubscribeProbability);
	}
	std::cout << "Не осталось ни одного узла";
	return 0;
}