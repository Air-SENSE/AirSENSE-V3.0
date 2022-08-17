/**
 * @file Queue.h
 * @author Le Duy Nhat
 * @date 2020-08-26
 * @brief 
 */
#pragma once
#include "Config.h"
#include "string.h"

#define QUEUE_SIZE 1000 /* Size of Circular Queue */


void displayStruct(DataCore s);

class Queue 
{
private:
    DataCore items[QUEUE_SIZE];
    int front, rear;
    static Queue* Instance;
    Queue() {
        front = -1;
        rear = -1;
    }

public:
    static Queue* getInstance()
    {
        if(nullptr == Instance)
        {
            Instance =  new Queue();
        }
        return Instance;
    }
    bool isFull();
    bool isEmpty();
    void enQueue(DataCore element);
    DataCore deQueue();
    void display();
};

